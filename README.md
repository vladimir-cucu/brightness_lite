# Brightness Lite

Lightweight PlayStation Portable plugin that allows the user to change the default brightness preset values on a PSP with CFW.

## Usage

Press the SCREEN button to cycle through the brightness levels.

## Configuration

While using this plugin, the values for the brightness presets are set to `20, 40, 60, 80` by default. The possible value for the screen brightness ranges between `0` and `100`.

To change the presets, modify the comma separated values for `presets` in `brightness_lite.ini`. Here is an example of `brightness_lite.ini` config file:

```
[brightness]
preset = 30, 40, 50, 60, 70
```

The following restriction is forced on the custom presets: `0 < preset[0] < preset[1] < ... < preset[count - 1] <= 100`, where `count` is the number of comma separated values. If this restriction is not satisfied, the presets will rollback to the previously mentioned default values.

## Important note
- If 4 brightness presets are used, the brightness level will sync with the internal brightness level counter of the PSP. If a different number of presets is used, the brightness level will be saved to `brightness_lite.config` file. This is done in order for the plugin to know which brightness level to set on launch.
- The minimum number of brightness presets you can use is 2, while the maximum number is 10.
- The `brightness_lite.ini` config file should be saved in `ms0:/SEPLUGINS` or `ef0:/SEPLUGINS`.
