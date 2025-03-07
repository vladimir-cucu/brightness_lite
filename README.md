# Brightness Lite

Lightweight PlayStation Portable plugin that allows the user to change the default brightness preset values on a PSP with CFW.

## Why use this plugin instead of the alternatives?

This plugin fixes the following issues present in other brightness plugins:
- the issue in Brightness Control v1.5/1.6 plugin, where the PSP crashes in some games (e.g. GTA: Librety City Stories),
- the issue in bright3 plugin, where the brightness level resets to the first level when the plugin restarts (e.g. entering/exiting a game),
- the issue in both aforementioned plugins, where long pressing the SCREEN button doesn't fully dim the display and pressing the SCREEN button afterward skips brightness levels.

## Usage

Press the SCREEN button to cycle through the brightness levels.

## Configuration

While using this plugin, the values for the brightness presets are set to `20, 40, 60, 80` by default. The possible value for the screen brightness ranges between `0` and `100`.

To change the default presets, modify the comma separated values for `presets` in `brightness_lite.ini` file, which should be saved in `ms0:/SEPLUGINS` or `ef0:/SEPLUGINS`.

Here is an example of `brightness_lite.ini` config file that would use 5 custom presets:

```
[brightness]
preset = 30, 40, 50, 60, 70
```

The following restrictions are forced on the custom presets:
- `0 < preset[1] < preset[2] < ... < preset[number_of_presets] <= 100`,
- 2 is the minimum number of presets that can be used,
- 10 is the maximum number of presets that can be used and only up to the first 10 comma separated values will be considered, and any extra will be ignored.

If these restrictions are not satisfied, the presets will rollback to the 4 default values mentioned at the beginning of this section.

**Important technical note:** If 4 brightness presets are used, the brightness level will sync with the internal brightness level counter of the PSP. If a different number of presets is used, the brightness level will be saved to `brightness_lite.config` file. This is done in order for the plugin to know which brightness level to set when started.
