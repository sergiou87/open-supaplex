#!/usr/bin/env python3

import argparse
import json
from pathlib import Path
import sys


REPO_ROOT = Path(__file__).resolve().parent.parent
PLATFORM_BUILDS_PATH = REPO_ROOT / ".github" / "platform-builds.json"
PLATFORM_BUILDS_SCHEMA_PATH = REPO_ROOT / ".github" / "platform-builds.schema.json"
OUTPUT_PATH = REPO_ROOT / "PLATFORM_SUPPORT.md"
README_PATH = REPO_ROOT / "README.md"
README_START_MARKER = "<!-- GENERATED_PLATFORM_SUMMARY_START -->"
README_END_MARKER = "<!-- GENERATED_PLATFORM_SUMMARY_END -->"


def detect_newline_style(text):
    return "\r\n" if "\r\n" in text else "\n"


def with_newline_style(text, newline):
    return text.replace("\n", newline)


def load_platforms():
    with PLATFORM_BUILDS_PATH.open("r", encoding="utf-8") as file:
        return json.load(file)


def load_schema():
    with PLATFORM_BUILDS_SCHEMA_PATH.open("r", encoding="utf-8") as file:
        return json.load(file)


def apply_defaults(schema, value):
    schema_type = schema.get("type")

    if schema_type == "array":
        item_schema = schema.get("items", {})
        return [apply_defaults(item_schema, item) for item in value]

    if schema_type == "object":
        result = dict(value)
        properties = schema.get("properties", {})

        for key, property_schema in properties.items():
            if key not in result and "default" in property_schema:
                result[key] = property_schema["default"]

            if key in result:
                result[key] = apply_defaults(property_schema, result[key])

        return result

    return value


def validate_schema(schema, value, path="$"):
    errors = []
    schema_type = schema.get("type")

    if schema_type == "array":
        if not isinstance(value, list):
            return [f"{path} should be an array"]

        item_schema = schema.get("items", {})
        for index, item in enumerate(value):
            errors.extend(validate_schema(item_schema, item, f"{path}[{index}]"))

        return errors

    if schema_type == "object":
        if not isinstance(value, dict):
            return [f"{path} should be an object"]

        properties = schema.get("properties", {})

        for key in schema.get("required", []):
            if key not in value:
                errors.append(f"{path}.{key} is required")

        if schema.get("additionalProperties") is False:
            for key in value:
                if key not in properties:
                    errors.append(f"{path}.{key} is not allowed")

        for key, item in value.items():
            if key in properties:
                errors.extend(validate_schema(properties[key], item, f"{path}.{key}"))

        return errors

    if schema_type == "string" and not isinstance(value, str):
        errors.append(f"{path} should be a string")
    elif schema_type == "integer":
        if isinstance(value, bool) or not isinstance(value, int):
            errors.append(f"{path} should be an integer")
        elif "minimum" in schema and value < schema["minimum"]:
            errors.append(f"{path} should be >= {schema['minimum']}")
    elif schema_type == "boolean" and not isinstance(value, bool):
        errors.append(f"{path} should be a boolean")

    if "enum" in schema and value not in schema["enum"]:
        allowed_values = ", ".join(map(str, schema["enum"]))
        errors.append(f"{path} should be one of: {allowed_values}")

    return errors


def validate(platforms):
    errors = []

    ids = set()
    display_orders = set()

    for platform in platforms:
        platform_id = platform["id"]
        if platform_id in ids:
            errors.append(f"Duplicate platform id: {platform_id}")
        ids.add(platform_id)

        display_order = platform.get("display_order")
        if display_order is None:
            errors.append(f"Platform {platform_id} is missing display_order")
        elif display_order in display_orders:
            errors.append(f"Duplicate display_order: {display_order}")
        else:
            display_orders.add(display_order)

        if platform.get("release"):
            if not platform.get("artifact_name", ""):
                errors.append(f"Release platform {platform_id} is missing artifact_name")

            if not platform.get("release_asset_name", ""):
                errors.append(f"Release platform {platform_id} is missing release_asset_name")

        if platform.get("ci") and not platform.get("runs_on"):
            errors.append(f"CI platform {platform_id} is missing runs_on")

        if platform.get("ci") and not platform.get("build_command"):
            errors.append(f"CI platform {platform_id} is missing build_command")

        if platform.get("release") and not platform.get("artifact_path"):
            errors.append(f"Release platform {platform_id} is missing artifact_path")

        for field_name in ("status_note", "readme_note"):
            field_value = platform.get(field_name, "")
            if not isinstance(field_value, str):
                errors.append(f"Platform {platform_id} has non-string {field_name}")

        if platform.get("supported") and not platform.get("ci"):
            if not platform.get("status_note", "").strip():
                errors.append(f"Supported non-CI platform {platform_id} is missing status_note")

            if not platform.get("readme_note", "").strip():
                errors.append(f"Supported non-CI platform {platform_id} is missing readme_note")

    return errors


def supported_platforms(platforms):
    return [platform for platform in platforms if platform.get("supported")]


def platforms_for_automation(platforms, field_name):
    return [
        {
            "id": platform["id"],
            "name": platform["name"],
            "runs_on": platform["runs_on"],
            "build_command": platform["build_command"],
            "pre_build_command": platform["pre_build_command"],
            "artifact_name": platform["artifact_name"],
            "artifact_path": platform["artifact_path"],
            "setup_msys2": platform["setup_msys2"],
        }
        for platform in sorted(platforms, key=lambda item: item["display_order"])
        if platform.get(field_name)
    ]


def render_matrix(platforms, field_name):
    return json.dumps({"include": platforms_for_automation(platforms, field_name)}, separators=(",", ":"))


def render_release_assets(platforms):
    lines = []
    for platform in sorted(platforms, key=lambda item: item["display_order"]):
        if not platform.get("release"):
            continue

        asset_path = platform["artifact_path"].split("/", 1)[1]
        lines.append(f"{platform['artifact_name']}/{asset_path}|{platform['release_asset_name']}")

    return "\n".join(lines)


def platform_status_notes(platforms):
    notes = []
    for platform in sorted(supported_platforms(platforms), key=lambda item: item["display_order"]):
        note = platform.get("status_note", "").strip()
        if note:
            notes.append(note)

    return notes


def platform_readme_notes(platforms):
    notes = []
    for platform in sorted(supported_platforms(platforms), key=lambda item: item["display_order"]):
        note = platform.get("readme_note", "").strip()
        if note:
            notes.append(note)

    return notes


def render(platforms):
    lines = [
        "# Platform Support",
        "",
        "This document is generated from `.github/platform-builds.json`.",
        "",
        "## Status",
        "",
        "| Platform | Backend | Build System | Port Folder | CI | Release |",
        "| --- | --- | --- | --- | --- | --- |",
    ]

    for platform in sorted(supported_platforms(platforms), key=lambda item: item["display_order"]):
        lines.append(
            "| {name} | {backend} | {build_system} | `{port_dir}` | {ci} | {release} |".format(
                name=platform["name"],
                backend=platform["backend"],
                build_system=platform["build_system"],
                port_dir=platform["port_dir"],
                ci="Yes" if platform.get("ci") else "No",
                release="Yes" if platform.get("release") else "No",
            )
        )

    lines.extend(
        [
            "",
            "## Notes",
            "",
            "- CI and release build metadata live in `.github/platform-builds.json`.",
            "- Release asset naming is defined per platform in `.github/platform-builds.json`.",
            "- There is also a `wasm` folder in the repository, but it is not currently listed as a supported platform in the main documentation or wired into CI/release automation.",
        ]
    )

    for note in platform_status_notes(platforms):
        lines.append(f"- {note}")

    lines.append("")

    return "\n".join(lines)


def render_readme_summary(platforms):
    supported = supported_platforms(platforms)
    released = [platform for platform in platforms if platform.get("release")]

    lines = [
        README_START_MARKER,
        "Other than that, **OpenSupaplex** can be built for any of the following platforms:",
    ]

    for platform in sorted(supported, key=lambda item: item["display_order"]):
        lines.append(f"- {platform['name']}")

    lines.extend(
        [
            "",
            "More platforms (like Android, iOS and tvOS) to come Soon™.",
            "",
            "See [PLATFORM_SUPPORT.md](PLATFORM_SUPPORT.md) for the current support matrix, backend split, and CI/release status.",
            "",
            "The following platforms are currently built from CI and released automatically:",
        ]
    )

    for platform in sorted(released, key=lambda item: item["display_order"]):
        lines.append(f"- {platform['name']}")

    readme_notes = platform_readme_notes(platforms)
    if readme_notes:
        lines.append("")
        lines.extend(readme_notes)

    lines.append(README_END_MARKER)
    return "\n".join(lines)


def replace_marked_section(content, replacement, start_marker, end_marker):
    start = content.find(start_marker)
    end = content.find(end_marker)

    if start == -1 or end == -1 or end < start:
        raise ValueError(f"Could not find marker block {start_marker} ... {end_marker}")

    end += len(end_marker)
    return content[:start] + replacement + content[end:]


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--check", action="store_true", help="Fail if generated output differs from PLATFORM_SUPPORT.md")
    parser.add_argument("--matrix", choices=("ci", "release"), help="Print the requested GitHub Actions build matrix as JSON")
    parser.add_argument("--release-assets", action="store_true", help="Print release asset upload mappings")
    args = parser.parse_args()

    schema = load_schema()
    platforms = apply_defaults(schema, load_platforms())

    errors = validate_schema(schema, platforms)
    errors.extend(validate(platforms))
    if errors:
        for error in errors:
            print(error, file=sys.stderr)
        return 1

    if args.matrix:
        field_name = "ci" if args.matrix == "ci" else "release"
        print(render_matrix(platforms, field_name))
        return 0

    if args.release_assets:
        print(render_release_assets(platforms))
        return 0

    generated = render(platforms)
    generated_readme_summary = render_readme_summary(platforms)
    existing_readme = README_PATH.read_text(encoding="utf-8")
    readme_newline = detect_newline_style(existing_readme)
    updated_readme = replace_marked_section(
        existing_readme,
        with_newline_style(generated_readme_summary, readme_newline),
        README_START_MARKER,
        README_END_MARKER,
    )

    if args.check:
        existing = OUTPUT_PATH.read_text(encoding="utf-8")
        if existing != generated:
            print("PLATFORM_SUPPORT.md is out of date. Run scripts/generate_platform_support.py", file=sys.stderr)
            return 1

        if existing_readme != updated_readme:
            print("README.md platform summary is out of date. Run scripts/generate_platform_support.py", file=sys.stderr)
            return 1
    else:
        OUTPUT_PATH.write_text(generated, encoding="utf-8")
        README_PATH.write_text(updated_readme, encoding="utf-8")

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
