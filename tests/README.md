# Tests

This folder contains "tests" for OpenSupaplex. They're actually solutions for many levels recorded by users and shared
with the community.

Some of these solutions were collected through the years by [Elmer Productions](http://www.elmerproductions.com/), and the files
in this folder were downloaded from their website: http://www.elmerproductions.com/sp/solutions.html

Other solutions were collected by Greg from [Supaplex Online](https://www.supaplex.online/) who kindly shared them with me to
improve the quality of this reimplementation.

Each solution belongs to its author.

## How do they work

OpenSupaplex, as the SpeedFix modification of the original game, can record and play demos. Using specific command line
options, these demos can be run quickly with certain parameters:
 - `-q <path_to_demo_file>` will run the demo quickly, skipping the intro and with no game speed limit.
 - `-u <path_to_demo_file>` will run the demo without any graphical interface, just the logic of the game.

With any of these parameters, the game will just output whether the demo succeded (Murphy reached the exit with after
collecting all the Infotrons) or not.

The premise here is that in all these solutions Murphy is able to reach the exit with all the Infotrons, however…

## Regression replays

`run-tests.rb` also runs the synthetic replays in `regressions/solves` and `regressions/failures`.
The former must finish successfully (exit code 0); the latter must report a failed demo (exit code 1).
Crashes and other exit codes fail the test in either directory.

The downward-infotron regression uses one infotron directly below Murphy and an exit to its right.
Its inputs wait one frame, move down for eight frames, then move right toward the exit:

- `regressions/failures/falling-infotron-down.sp` leaves space below the infotron, so it immediately
  starts falling. Moving down into it must kill Murphy, not collect it.
- `regressions/solves/stationary-infotron-down.sp` puts hardware below the infotron instead. The
  stationary infotron must remain collectible and the replay must succeed.

These files differ by just that supporting tile. Both outcomes were verified against the original
SpeedFix v6.3 executable under DOSBox. The original comparison is `cmp ax, 4` at `loc_4E1A1`
in `asm/SUPAPLEX.ASM`: both the infotron tile and its zero state are required for collection.

The `debug-base-tests` executable runs the normal game with debug mode enabled and presses B
at frame 34, during downward infotron collection. Debug keys are not part of the recorded input
format, so this test adapter supplies that single keyboard event without changing game logic.
It is built with the main executable and copied beside it by `ci-build.sh`.

- `regressions/debug-failures/vacated-infotron-space.sp` must fail. Murphy's vacated cell remains
  reserved Space, not Base; B must not clear it early. Otherwise a zonk falls into the cell too
  soon and is erased when Murphy finishes moving, incorrectly opening the route to the exit.
- `regressions/debug-solves/base-removal-push-control.sp` changes only one hardware tile to real
  Base. B removes that Base, creating room to push the surviving zonk aside and reach the exit.

Both outcomes were verified in SpeedFix v6.3 with emulated B-key input during the same movement.
The Space assignment is at `loc_4E5BA` in `asm/SUPAPLEX.ASM`.

The zonk right-roll regression checks entry into a `0x9999` reservation beneath a falling zonk:

- `regressions/solves/zonk-roll-under-falling-zonk.sp` has two diagonally placed zonks falling
  together. The lower zonk must roll right into the upper zonk's reserved destination, even while
  the upper zonk is moving. Murphy waits four frames, moves right for 32 frames, then waits;
  rejecting the roll delays the lower zonk and prevents this replay from reaching the exit.
- `regressions/solves/zonk-roll-right-control.sp` changes only the upper zonk to hardware. The
  lower zonk rolls into empty space instead, and the same inputs must still solve the level.

Both replays succeed in SpeedFix v6.3. Its unpacked instruction at `CS:1400` is
`cmp byte [si+0x17be], 1` (bytes `80 BC BE 17 01`), corresponding to `loc_48011` in
`asm/SUPAPLEX.ASM`. The executable checks the upper zonk's tile byte, not its animation state.
This executable evidence is recorded here without modifying the read-only assembly reference.

The special-port coordinate regression checks that records match exact byte offsets:

- `regressions/solves/special-port-odd-position.sp` contains a downward special port at tile 610
  and a record with the odd offset 1221. That malformed record must be ignored, not rounded down
  to the port's actual offset 1220. With gravity unchanged, Murphy can move right into the exit.
- `regressions/failures/special-port-matching-position.sp` changes only the record's low byte to
  make the offset exactly 1220. The record then enables gravity and pulls Murphy below the exit;
  the same recorded inputs must fail.

Both outcomes were verified in SpeedFix v6.3 under DOSBox. `sub_4F2AF` in `asm/SUPAPLEX.ASM`
swaps the stored position's bytes and compares the full 16-bit offset with `cmp bx, si`.
Converting the lookup position to a 16-bit byte offset preserves that exact comparison.

The red-disk border regression uses the existing padding before the playfield:

- `regressions/failures/red-disk-above-top-border.sp` collects a red disk, moves through an empty
  top border, and plants it at tile index -49. Murphy returns to the playfield and snaps upward,
  then presses Right for one frame. The snap must keep Murphy busy, so the brief Right input
  cannot enter the exit and the replay fails.
- `regressions/solves/red-disk-above-top-border-control.sp` uses the same level and extends that
  final Right input to nine frames, allowing the snap to finish and Murphy to reach the exit.

Both outcomes were verified in SpeedFix v6.3 under DOSBox. The original stores the disk's byte
offset as `0xFF9E`, which addresses tile -49 through 16-bit address arithmetic. The C tile index
must remain signed like Murphy's position; an unsigned index instead accesses tile 65487,
leaves the off-field red disk missing, and incorrectly lets the shorter replay succeed.

## "Failed" solutions

…some of these solutions don't seem to be solutions at all in the original game with the SpeedFix v6.3 ([the last official
release of the SpeedFix](http://www.elmerproductions.com/sp/software.html#speedfix)).

I left them in the `failures` subfolder here for completeness as I find they fail both in OpenSupaplex and the SpeedFix 
version 6.3. This legacy collection is not run by `run-tests.rb`; expected-failure regression tests belong
in `regressions/failures`.

OpenSupaplex takes SpeedFix v6.3 as the reference baseline in terms of behavior and aims to preserve the very same logic.
Even some bugs in the original game might be preserved, but that's something I will think on a case-by-case basis.
