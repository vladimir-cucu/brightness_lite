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
#define BRIGHTNESS "brightness"
#define BRIGHTNESS_COUNT 4

PSP_MODULE_INFO(PLUGIN_NAME, 0x1000, 1, 0);
PSP_MAIN_THREAD_ATTR(0);

int BRIGHTNESS_PRESET[4] = {20, 40, 60, 80};

typedef struct {
    int preset[BRIGHTNESS_COUNT];
} configuration;

static int handler(void* config, const char* section, const char* name,
                   const char* value) {
    configuration* pconfig = (configuration*)config;

#define MATCH(s, n) strcmp(section, s) == 0 && strcmp(name, n) == 0
    if (MATCH(BRIGHTNESS, "preset")) {
        char* token;
        char value_copy[256];
        strncpy(value_copy, value, sizeof(value_copy) - 1);
        value_copy[sizeof(value_copy) - 1] = '\0';
        int index = 0;
        token = strtok(value_copy, ",");
        while (token != NULL && index < BRIGHTNESS_COUNT) {
            pconfig->preset[index] = atoi(token);
            index++;
            token = strtok(NULL, ",");
        }
    } else {
        return 0;
    }
    return 1;
}

void brightness_presets_initialization() {
    configuration config;
    if (ini_parse(PLUGIN_INI_PATH, handler, &config) < 0 &&
        ini_parse(PLUGIN_INI_PATH_GO, handler, &config) < 0) {
        return;
    }
    if (0 < config.preset[0] && config.preset[0] < config.preset[1] &&
        config.preset[1] < config.preset[2] &&
        config.preset[2] < config.preset[3] && config.preset[3] <= 100) {
        int i;
        for (i = 0; i < BRIGHTNESS_COUNT; i++) {
            BRIGHTNESS_PRESET[i] = config.preset[i];
        }
    }
}

void set_brightness_level(int brightness_level) {
    sceDisplayWaitVblankStart();
    sceDisplaySetBrightness(BRIGHTNESS_PRESET[brightness_level], 0);
}

void read_latch_data(SceCtrlLatch* latch_data) {
    sceDisplayWaitVblankStart();
    sceCtrlReadLatch(latch_data);
}

int main_thread(SceSize argc, void* argp) {
    int brightness_level = sceImposeGetParam(PSP_IMPOSE_BACKLIGHT_BRIGHTNESS),
        is_screen_on = 1;
    set_brightness_level(brightness_level);

    SceCtrlLatch latch_data;
    sceCtrlSetSamplingCycle(0);
    sceCtrlSetSamplingMode(PSP_CTRL_MODE_DIGITAL);

    while (1) {
        read_latch_data(&latch_data);
        if (is_screen_on && (latch_data.uiBreak & PSP_CTRL_SCREEN)) {
            sceDisplayGetBrightness(&is_screen_on, 0);
            if (is_screen_on) {
                brightness_level = (brightness_level + 1) % BRIGHTNESS_COUNT;
                set_brightness_level(brightness_level);
            }
        } else if (!is_screen_on && (latch_data.uiMake & PSP_CTRL_SCREEN)) {
            is_screen_on = 1;
            set_brightness_level(brightness_level);
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
    brightness_presets_initialization();
    int thid =
        sceKernelCreateThread(PLUGIN_NAME, main_thread, 0x7E, 0x1000, 0, NULL);
    if (thid >= 0) {
        sceKernelStartThread(thid, args, argp);
    }
    return 0;
}
