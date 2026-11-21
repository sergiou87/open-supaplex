# hqx

hq4x pixel-art scaler by Maxim Stepin, from the C library by Cameron Zemek:
https://github.com/grom358/hqx (commit `a1c7d415f4bb7b947c4c2e47faf220b0148f4bac`).

Licensed under the GNU LGPL 2.1 or later, see [COPYING](COPYING).

Only `common.h` and `hq4x.c` are included. Changes from upstream, also marked
with `OpenSupaplex:` comments in the files:

- `common.h`: colors are converted to YUV on demand, with a small cache, instead
  of through the 64 MB `RGBtoYUV` table that `hqxInit()` fills. The U and V
  terms are truncated to `int` before 128 is added: upstream casts negative
  doubles to `uint32_t`, which is undefined behavior and gives wrong results on
  ARM. Output matches upstream on x86. `yuv_diff` casts its differences to
  `int` explicitly, which doesn't change behavior.
- `hq4x.c`: `hq4x_32_rb` is `static`, `hqx.h` is not used, and the `hq4x_32`
  wrapper was removed. The file is `#include`d by `src/sdl2/video.c`, its only
  user, so the ports' build files don't list it.
