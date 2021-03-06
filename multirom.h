/*
 * This file is part of MultiROM.
 *
 * MultiROM is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * MultiROM is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with MultiROM.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef MULTIROM_H
#define MULTIROM_H

#include <pthread.h>
#include <stdio.h>

#include "boot_img_hdr.h"
#include "util.h"

enum
{
    ROM_DEFAULT           = 0,

    ROM_ANDROID_INTERNAL  = 1,
    ROM_ANDROID_USB_IMG   = 2,
    ROM_ANDROID_USB_DIR   = 3,

    ROM_LINUX_INTERNAL    = 4,
    ROM_LINUX_USB         = 5,

    ROM_UNSUPPORTED_INT   = 6,
    ROM_UNSUPPORTED_USB   = 7,
    ROM_UNKNOWN           = 8
};

#define M(x) (1 << x)
#define MASK_INTERNAL (M(ROM_DEFAULT) | M(ROM_ANDROID_INTERNAL) | M(ROM_UNSUPPORTED_INT) | M(ROM_LINUX_INTERNAL))
#define MASK_USB_ROMS (M(ROM_ANDROID_USB_IMG) | M(ROM_ANDROID_USB_DIR) | M(ROM_UNSUPPORTED_USB) | M(ROM_LINUX_USB))
#define MASK_ANDROID (M(ROM_ANDROID_USB_DIR) | M(ROM_ANDROID_USB_IMG) | M(ROM_ANDROID_INTERNAL))
#define MASK_UNSUPPORTED (M(ROM_UNSUPPORTED_USB) | M(ROM_UNSUPPORTED_INT))
#define MASK_LINUX (M(ROM_LINUX_INTERNAL) | M(ROM_LINUX_USB))
#define MASK_KEXEC (MASK_LINUX)

enum 
{
    EXIT_REBOOT              = 0x01,
    EXIT_UMOUNT              = 0x02,
    EXIT_REBOOT_RECOVERY     = 0x04,
    EXIT_REBOOT_BOOTLOADER   = 0x08,
    EXIT_SHUTDOWN            = 0x10,
    EXIT_KEXEC               = 0x20,

    EXIT_REBOOT_MASK         = (EXIT_REBOOT | EXIT_REBOOT_RECOVERY | EXIT_REBOOT_BOOTLOADER | EXIT_SHUTDOWN),
};

struct usb_partition
{
    char *name;
    char *mount_path;
    char *uuid;
    char *fs;
    int keep_mounted;
};

struct rom_info {
    // for future vals?
    map *str_vals;
};

struct multirom_rom
{
    int id;
    char *name;
    char *base_path;
    int type;
    int has_bootimg;
    struct usb_partition *partition;
};

struct multirom_status
{
    int is_second_boot;
    int auto_boot_seconds;
    int colors;
    int brightness;
    int enable_adb;
    int hide_internal;
    char *int_display_name;
    int rotation;
    struct multirom_rom *auto_boot_rom;
    struct multirom_rom *current_rom;
    struct multirom_rom **roms;
    struct usb_partition **partitions;
    char *curr_rom_part;
};

extern char busybox_path[64];
extern char adbd_path[64];

int multirom(void);
int multirom_find_base_dir(void);
void multirom_emergency_reboot(void);
int multirom_default_status(struct multirom_status *s);
void multirom_find_usb_roms(struct multirom_status *s);
int multirom_generate_rom_id(void);
struct multirom_rom *multirom_get_internal(struct multirom_status *s);
struct multirom_rom *multirom_get_rom(struct multirom_status *s, const char *name, const char *part_uuid);
struct multirom_rom *multirom_get_rom_by_id(struct multirom_status *s, int id);
int multirom_load_status(struct multirom_status *s);
int multirom_import_internal(void);
void multirom_dump_status(struct multirom_status *s);
int multirom_save_status(struct multirom_status *s);
int multirom_prepare_for_boot(struct multirom_status *s, struct multirom_rom *to_boot);
int multirom_dump_boot(const char *dest);
void multirom_free_status(struct multirom_status *s);
void multirom_free_rom(void *rom);
int multirom_init_fb(int rotation);
int multirom_prep_android_mounts(struct multirom_rom *rom);
int multirom_create_media_link(void);
int multirom_process_android_fstab(void);
int multirom_get_api_level(const char *path);
int multirom_get_rom_type(struct multirom_rom *rom);
void multirom_take_screenshot(void);
int multirom_get_trampoline_ver(void);
int multirom_has_kexec(void);
int multirom_load_kexec(struct multirom_status *s, struct multirom_rom *rom);
int multirom_get_cmdline(char *str, size_t size);
int multirom_find_file(char *res, const char *name_part, const char *path);
int multirom_fill_kexec_linux(struct multirom_status *s, struct multirom_rom *rom, char **cmd);
int multirom_fill_kexec_android(struct multirom_rom *rom, char **cmd);
int multirom_extract_bytes(const char *dst, FILE *src, size_t size);
int multirom_update_partitions(struct multirom_status *s);
void multirom_destroy_partition(void *part);
void multirom_set_usb_refresh_thread(struct multirom_status *s, int run);
void multirom_set_usb_refresh_handler(void (*handler)(void));
int multirom_mount_usb(struct usb_partition *part);
int multirom_mount_loop(const char *src, const char *dst, const char *fs, int flags);
int multirom_copy_log(char *klog);
int multirom_scan_partition_for_roms(struct multirom_status *s, struct usb_partition *p);
struct usb_partition *multirom_get_partition(struct multirom_status *s, char *uuid);
struct usb_partition *multirom_get_data_partition(struct multirom_status *s);
int multirom_path_exists(char *base, char *filename);
int multirom_search_last_kmsg(const char *expr);
struct rom_info *multirom_parse_rom_info(struct multirom_status *s, struct multirom_rom *rom);
void multirom_destroy_rom_info(struct rom_info *info);
char **multirom_get_rom_info_str(struct rom_info *info, char *key);
int multirom_replace_aliases_cmdline(char **s, struct rom_info *i, struct multirom_status *status, struct multirom_rom *rom);
int multirom_replace_aliases_root_path(char **s, struct multirom_rom *rom);
char *multirom_get_klog(void);
int multirom_get_battery(void);
void multirom_set_brightness(int val);

#endif
