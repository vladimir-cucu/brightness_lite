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
#define PLUGIN_CONFIG_PATH "ms0:/SEPLUGINS/brightness_lite.config"
#define PLUGIN_CONFIG_PATH_GO "ef0:/SEPLUGINS/brightness_lite.config"

#define MIN_BRIGHTNESS_PRESETS_COUNT 2
#define MAX_BRIGHTNESS_PRESETS_COUNT 10
#define DEFAULT_BRIGHTNESS_PRESETS_COUNT 4
const int DEFAULT_BRIGHTNESS_PRESET[4] = {20, 40, 60, 80};

PSP_MODULE_INFO(PLUGIN_NAME, 0x1000, 1, 0);
PSP_MAIN_THREAD_ATTR(0);

typedef struct {
    int count;
    int preset[MAX_BRIGHTNESS_PRESETS_COUNT];
} configuration;

static int handler(void* config, const char* section, const char* name, const char* value) {
    configuration* brightness = (configuration*)config;

#define MATCH(s, n) strcmp(section, s) == 0 && strcmp(name, n) == 0
    if (MATCH("brightness", "presets")) {
        char* token;
        char value_copy[256];
        strncpy(value_copy, value, sizeof(value_copy) - 1);
        value_copy[sizeof(value_copy) - 1] = '\0';
        int count = 0;
        token = strtok(value_copy, ",");
        while (token != NULL && count < MAX_BRIGHTNESS_PRESETS_COUNT) {
            brightness->preset[count] = atoi(token);
            count++;
            token = strtok(NULL, ",");
        }
        brightness->count = count;
    } else {
        return 0;
    }
    return 1;
}

void initialize_brightness_presets(configuration* brightness) {
    int use_default_presets = 0, i;
    if (ini_parse(PLUGIN_INI_PATH, handler, brightness) < 0 && ini_parse(PLUGIN_INI_PATH_GO, handler, brightness) < 0) {
        use_default_presets = 1;
    } else if (brightness->count < MIN_BRIGHTNESS_PRESETS_COUNT || 0 >= brightness->preset[0] ||
               brightness->preset[brightness->count - 1] > 100) {
        use_default_presets = 1;
    } else {
        for (i = 0; i < brightness->count - 1; i++) {
            if (brightness->preset[i] >= brightness->preset[i + 1]) {
                use_default_presets = 1;
                break;
            }
        }
    }
    if (use_default_presets) {
        brightness->count = DEFAULT_BRIGHTNESS_PRESETS_COUNT;
        for (i = 0; i < DEFAULT_BRIGHTNESS_PRESETS_COUNT; i++) {
            brightness->preset[i] = DEFAULT_BRIGHTNESS_PRESET[i];
        }
    }
}

int open_config_file() {
    int fd = sceIoOpen(PLUGIN_CONFIG_PATH, PSP_O_RDONLY | PSP_O_WRONLY | PSP_O_CREAT, 0777);
    if (fd < 0) {
        fd = sceIoOpen(PLUGIN_CONFIG_PATH_GO, PSP_O_RDONLY | PSP_O_WRONLY | PSP_O_CREAT, 0777);
    }
    return fd;
}

int get_initial_brightness_level(int brightness_count) {
    int brightness_level = 0;
    if (brightness_count == DEFAULT_BRIGHTNESS_PRESETS_COUNT) {
        brightness_level = sceImposeGetParam(PSP_IMPOSE_BACKLIGHT_BRIGHTNESS);
    } else {
        char brightness_level_char[1];
        int fd = open_config_file();
        if (fd >= 0) {
            int bytes_read = sceIoRead(fd, &brightness_level_char, sizeof(brightness_level_char));
            sceIoClose(fd);
            if (bytes_read) {
                int new_brightness_level = brightness_level_char[0] - '0';
                if (0 <= new_brightness_level && new_brightness_level < brightness_count) {
                    brightness_level = new_brightness_level;
                }
            }
        }
    }
    return brightness_level;
}

void save_brightness_level_to_config(int brightness_level) {
    int fd = open_config_file();
    char brightness_level_char[1] = {brightness_level + '0'};
    if (fd >= 0) {
        sceIoWrite(fd, &brightness_level_char, sizeof(brightness_level_char));
        sceIoClose(fd);
    }
}

void set_brightness_level(int brightness_level, configuration brightness) {
    sceDisplayWaitVblankStart();
    sceDisplaySetBrightness(brightness.preset[brightness_level], 0);
}

void read_latch_data(SceCtrlLatch* latch_data) {
    sceDisplayWaitVblankStart();
    sceCtrlReadLatch(latch_data);
}

int main_thread(SceSize argc, void* argp) {
    configuration brightness;
    int brightness_level, is_screen_on = 1;
    SceCtrlLatch latch_data;

    initialize_brightness_presets(&brightness);
    brightness_level = get_initial_brightness_level(brightness.count);
    set_brightness_level(brightness_level, brightness);
    sceCtrlSetSamplingCycle(0);
    sceCtrlSetSamplingMode(PSP_CTRL_MODE_DIGITAL);

    while (1) {
        read_latch_data(&latch_data);
        if (is_screen_on && (latch_data.uiBreak & PSP_CTRL_SCREEN)) {
            sceDisplayGetBrightness(&is_screen_on, 0);
            if (is_screen_on) {
                brightness_level = (brightness_level + 1) % brightness.count;
                set_brightness_level(brightness_level, brightness);
                if (brightness.count != DEFAULT_BRIGHTNESS_PRESETS_COUNT) {
                    save_brightness_level_to_config(brightness_level);
                }
            }
        } else if (!is_screen_on && (latch_data.uiMake & PSP_CTRL_SCREEN)) {
            is_screen_on = 1;
            set_brightness_level(brightness_level, brightness);
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
