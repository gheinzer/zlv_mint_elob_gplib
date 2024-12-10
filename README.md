# ELOB GPL
General-purpose library for the ZLV-Mint ELO-Board.

Please refer to the documentation on [GitHub Pages](https://gheinzer.github.io/zlv_mint_elob_gplib/index.html) for more information.

## Highlights
- Error handling with `try { } catch { }`
- Native `printf` and `scanf` capabilities over USB-UART
- Scientific unit handling library
- `DS1307` Real-Time-Clock driver
- Synchrounous, buffered UART driver
- Terminal emulator customization (e.g. text color customization)

## Including this in a project
Add this library as a submodule to your project using git:

```sh
git submodule add https://github.com/gheinzer/zlv_mint_elob_gplib.git elob_gplib
```