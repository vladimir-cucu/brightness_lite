# Brightness Lite

Lightweight PlayStation Portable plugin that allows the user to change the 4 default brightness preset values on a PSP with CFW.

## Usage

Press the SCREEN button to cycle through the 4 brightness levels.

## Configuration

While using this plugin, the values for the brightness presets are set to `20, 40, 60, 80` by default. The possible value for the screen brightness ranges between `0` and `100`.

To change the presets, modify the comma separated values for `preset` in `brightness_lite.ini`. Here is an example of `brightness_lite.ini` config file:

```
[brightness]
preset = 20, 40, 60, 80
```

The following restriction is forced on the custom presets: `0 < preset[0] < preset[1] < preset[2] < preset[3] <= 100`. If this restriction is not satisfied, the presets will rollback to the previously mentioned default values.

**Important note:** The `brightness_lite.ini` config file should be saved in `ms0:/SEPLUGINS` or `ef0:/SEPLUGINS`. This plugin was tested on my PSP 1000 with 6.61 PRO-C CFW.
