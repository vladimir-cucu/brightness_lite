#include <pspctrl.h>
#include <pspdisplay.h>
#include <pspdisplay_kernel.h>
#include <pspimpose_driver.h>
#include <pspiofilemgr.h>
#include <pspkernel.h>
#include <stdlib.h>
#include <string.h>

#include "include/ini.h"

#define PLUGIN_NAME "brightness_lite"
#define PLUGIN_INI_PATH "ms0:/SEPLUGINS/brightness_lite.ini"
#define PLUGIN_INI_PATH_GO "ef0:/SEPLUGINS/brightness_lite.ini"
#define PLUGIN_MEMORY_PATH "ef0:/SEPLUGINS/brightness_lite.txt"

#define BRIGHTNESS "brightness"
#define DEFAULT_PRESET_COUNT 4
#define MIN_PRESET_COUNT 2
#define MAX_PRESET_COUNT 9
const int DEFAULT_PRESET[4] = {20, 40, 60, 80};

PSP_MODULE_INFO(PLUGIN_NAME, 0x1000, 1, 0);
PSP_MAIN_THREAD_ATTR(0);

typedef struct {
    int presets_count;
    int preset[MAX_PRESET_COUNT];
} configuration;

static int handler(void* config, const char* section, const char* name, const char* value) {
    configuration* brightness = (configuration*)config;

#define MATCH(s, n) strcmp(section, s) == 0 && strcmp(name, n) == 0
    if (MATCH(BRIGHTNESS, "preset")) {
        char* token;
        char value_copy[256];
        strncpy(value_copy, value, sizeof(value_copy) - 1);
        value_copy[sizeof(value_copy) - 1] = '\0';
        int index = 0;
        token = strtok(value_copy, ",");
        while (token != NULL && index < MAX_PRESET_COUNT) {
            brightness->preset[index] = atoi(token);
            index++;
            token = strtok(NULL, ",");
        }
    } else if (MATCH(BRIGHTNESS, "presets_count")) {
        brightness->presets_count = atoi(value);
    } else {
        return 0;
    }
    return 1;
}

void brightness_presets_initialization(configuration* config) {
    int use_default_preset = 0;
    if (ini_parse(PLUGIN_INI_PATH, handler, &config) < 0 && ini_parse(PLUGIN_INI_PATH_GO, handler, &config) < 0) {
        use_default_preset = 1;
    }
    if (config->presets_count < MIN_PRESET_COUNT || config->presets_count > MAX_PRESET_COUNT ||
        config->preset[0] <= 0 || config->preset[config->presets_count - 1] > 100) {
        use_default_preset = 1;
    }
    int i = 0;
    for (i = 0; i < config->presets_count - 1; i++) {
        if (config->preset[i] >= config->preset[i + 1]) {
            use_default_preset = 1;
            break;
        }
    }
    if (use_default_preset) {
        config->presets_count = DEFAULT_PRESET_COUNT;
        int i;
        for (i = 0; i < DEFAULT_PRESET_COUNT; i++) {
            config->preset[i] = DEFAULT_PRESET[i];
        }
    }
}

void set_brightness_level(int brightness_level, configuration config) {
    sceDisplayWaitVblankStart();
    sceDisplaySetBrightness(config.preset[brightness_level], 0);
}

void read_latch_data(SceCtrlLatch* latch_data) {
    sceDisplayWaitVblankStart();
    sceCtrlReadLatch(latch_data);
}

int main_thread(SceSize argc, void* argp) {
    configuration config;
    brightness_presets_initialization(&config);
    int fd = -1, brightness_level = 0, is_screen_on = 1;
    // if (config.presets_count == 4) {
    //     brightness_level = sceImposeGetParam(PSP_IMPOSE_BACKLIGHT_BRIGHTNESS);
    // } else {
    //     char brightness_level_char;
    //     fd = sceIoOpen(PLUGIN_MEMORY_PATH, PSP_O_RDONLY | PSP_O_CREAT, 0777);
    //     int bytes_read = sceIoRead(fd, &brightness_level_char, sizeof(brightness_level_char));
    //     sceIoClose(fd);
    //     if (bytes_read) {
    //         brightness_level = brightness_level_char - '0';
    //     }
    // }
    set_brightness_level(brightness_level, config);

    SceCtrlLatch latch_data;
    sceCtrlSetSamplingCycle(0);
    sceCtrlSetSamplingMode(PSP_CTRL_MODE_DIGITAL);

    while (1) {
        read_latch_data(&latch_data);
        if (is_screen_on && (latch_data.uiBreak & PSP_CTRL_SCREEN)) {
            sceDisplayGetBrightness(&is_screen_on, 0);
            if (is_screen_on) {
                brightness_level = (brightness_level + 1) % config.presets_count;
                set_brightness_level(brightness_level, config);
                // if (config.presets_count != 4) {
                //     fd = sceIoOpen(PLUGIN_MEMORY_PATH, PSP_O_WRONLY | PSP_O_CREAT, 0777);
                //     sceIoWrite(fd, &brightness_level, sizeof(brightness_level));
                //     sceIoClose(fd);
                // }
            }
        } else if (!is_screen_on && (latch_data.uiMake & PSP_CTRL_SCREEN)) {
            is_screen_on = 1;
            set_brightness_level(brightness_level, config);
            while (1) {
                read_latch_data(&latch_data);
                if (latch_data.uiBreak & PSP_CTRL_SCREEN) {
                    break;
                }
            }
        }
    }

    sceKernelExitDeleteThread(0);
    return 0;
}

int module_start(SceSize args, void* argp) {
    int thid = sceKernelCreateThread(PLUGIN_NAME, main_thread, 0x7E, 0x1000, 0, NULL);
    if (thid >= 0) {
        sceKernelStartThread(thid, args, argp);
    }
    return 0;
}
