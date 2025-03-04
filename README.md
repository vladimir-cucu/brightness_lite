# Brightness Lite

Lightweight PlayStation Portable plugin that allows the user to change the 4 default brightness preset values on a PSP with CFW.

## Usage

Press the SCREEN button to cycle through the 4 brightness levels.

## Configuration

While using this plugin, the values for the brightness presets are set to `20, 40, 60, 80` by default. The possible value for the screen brightness ranges between `0` and `100`.

To change the presets, modify the values for `preset_1`, `preset_2`, `preset_3` and `preset_4` in `brightness_lite.ini`. The following restriction is forced on the custom presets: `0 < preset_1 < preset_2 < preset_3 < preset_4 <= 100`. If this restriction is not satisfied, the presets will rollback to the previously mentioned default values.

**Important note:** The `brightness_lite.ini` file should be saved in `ms0:/SEPLUGINS` or `ef0:/SEPLUGINS`. This plugin was tested on my PSP 1000 with 6.61 PRO-C CFW.
