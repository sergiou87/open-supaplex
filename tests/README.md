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

The hardware-immunity regression reaches a nonzero-state hardware cell in the same padding:

- `regressions/solves/nonzero-state-hardware-immunity.sp` starts Murphy at tile 0, moves up for
  33 frames, back down for 32, then right into the exit. The extra upward input bumps padding
  tile -300, whose existing word is `0x8E06`: hardware tile 6 with state `0x8E`. Hardware must
  remain immune to explosions regardless of its state, so Murphy survives and reaches the exit.
- `regressions/solves/nonzero-state-hardware-control.sp` moves up for only 32 frames and never
  bumps the hardware. The remaining movement is identical and must also succeed.

Both replays succeed in SpeedFix v6.3 under DOSBox, and both engines already contain that padding
word before playback; the test does not inject tile states. `sub_4A61F` in `asm/SUPAPLEX.ASM`
returns immediately when the explosion center's tile byte is 6, without checking its state byte.

The timer-alias regression checks the original shared playfield/timer memory layout:

- `regressions/solves/explosion-timer-field-alias.sp` starts Murphy near the bottom border and
  moves into the extra level-file padding. A terminal there detonates a yellow disk beside an
  orange disk at tile 0. When explosion timer 0 reaches 7, the two timer bytes also represent
  tile 1536 as an exit with state zero. Murphy moves down at that frame and must complete the level.
- `regressions/failures/explosion-timer-field-alias-control.sp` changes only the orange disk to
  hardware, so timer 0 stays zero and the same recorded movement cannot find an exit.

Both outcomes were verified in SpeedFix v6.3 under DOSBox. Its level words begin at `0x1834`,
and its explosion timers begin immediately after the 1536 level words at `0x2434`. The C views
share storage while keeping the saved level-state and explosion-timer arrays at their existing
sizes, so older savegames remain compatible.

The downward-exit state regression uses the same timer-backed tile:

- `regressions/failures/downward-exit-nonzero-state.sp` detonates a yellow disk beside two orange
  disks, making timers 0 and 1 both reach 7. Tile 1536 is then word `0x0707`: an exit with state 7.
  Moving down must reject that nonzero-state exit, rather than mark the replay successful.
- `regressions/solves/downward-exit-zero-state-control.sp` changes only the second orange disk
  to hardware. Timer 1 stays zero, so the target becomes word `0x0007` and must remain usable.

Both outcomes were verified in SpeedFix v6.3 under DOSBox. At `loc_4E1A9`, the original
`cmp ax, 7` checks both the exit tile and its zero state, just like movement in other directions.

Menu autorepeat uses CSV input traces because mouse events are not encoded in `.SP` demos.
`menu-throttle-tests` replays the recorded frame and button state, checking the original level
selection and repeat-counter values before each event. It also checks the identical throttle
logic in level/player/ranking scrolling and level-set rotation. The CI build copies this adapter
beside the game, and `run-tests.rb` runs `regressions/menu-solves/*.csv`.

- `regressions/menu-solves/frame-counter-wrap.csv` was recorded from unmodified SpeedFix v6.3
  under DOSBox using a virtual mouse driver. The frame counter advances naturally from `0xFF80`
  through `0xFFFF` to `0x0030`; the driver does not change game state. After the wrap, the original
  advances the selected level again at frame 13, but unbounded C subtraction would stall.
- `regressions/menu-solves/frame-counter-control.csv` is the first 65 frames of the same recording,
  covering a separate button hold/release before the wrap. Its behavior must remain unchanged.

The CSV columns are frame counter, mouse button state (0=released, 1=left, 2=right), selected level,
repeat interval, and previous-repeat frame. Original menu routines subtract in 16-bit `AX` before
an unsigned comparison; the C subtraction must retain that same 16-bit wraparound.

`ranking-tests` loads the player records in `regressions/ranking-solves/*.csv` and compares
the real ranking routine's order, displayed progress/time fields, and current-player rank with
expected ranks captured from unmodified SpeedFix v6.3. The original loaded equivalent binary
`PLAYER.LST` files and opened its menu; the input driver only closed the menu, without changing
player data or game state.

- `elapsed-time-ties.csv` covers equal-progress hour/minute/second tie-breaks, stable equal-time
  ordering, completed players, and the original signed-byte ordering of 127/128/255-hour totals.
- `progress-priority-control.csv` gives every player different progress, verifying that progress
  still takes precedence over time.

The columns are player index, name, next level, hours, minutes, seconds, and expected zero-based
rank. Original `sub_4BF8D` compares progress descending, then hours/minutes/seconds ascending,
using signed byte comparisons. Equal sort keys retain file order. Time fields are still displayed
as unsigned bytes, even when their signed sort keys are negative.

`hall-of-fame-tests` checks `regressions/hall-of-fame-solves/*.csv` against results saved by
unmodified SpeedFix v6.3 after actually completing the final level. Each case supplies the current
player's time and all three Hall of Fame entries before and after insertion. The adapter calls
the real update routine with all levels completed, then checks every entry and the completion flag.

Cases exercise the 127/128-hour boundary and the original signed-byte ordering of 200- and
255-hour totals. Controls cover faster insertion, hour/minute/second comparisons, and leaving the
table unchanged for a slower player. Original `sub_4D1B6` uses signed `jl` comparisons for all three time fields;
the stored/displayed byte values remain unsigned.

## "Failed" solutions

…some of these solutions don't seem to be solutions at all in the original game with the SpeedFix v6.3 ([the last official
release of the SpeedFix](http://www.elmerproductions.com/sp/software.html#speedfix)).

I left them in the `failures` subfolder here for completeness as I find they fail both in OpenSupaplex and the SpeedFix 
version 6.3. This legacy collection is not run by `run-tests.rb`; expected-failure regression tests belong
in `regressions/failures`.

OpenSupaplex takes SpeedFix v6.3 as the reference baseline in terms of behavior and aims to preserve the very same logic.
Even some bugs in the original game might be preserved, but that's something I will think on a case-by-case basis.
