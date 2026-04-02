/**
 * Copyright 2022 Charly Delay <charly@codesink.dev> (@0xcharly)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include QMK_KEYBOARD_H

enum charybdis_keymap_layers {
    LAYER_BASE = 0,
    LAYER_ONE,
    LAYER_TWO,
    LAYER_THREE,
    LAYER_FOUR,
    LAYER_POINTER,
};

// Custom keycodes
enum custom_keycodes {
    SCRNSHT = SAFE_RANGE,
    LCK_SCR,
    ZOOMIN,
    ZOOMOUT,
    ZOOMRST,
    // Spanish layer keycodes (macOS English input)
    ES_ACUT,    // Alt+E (acute accent dead key ´)
    ES_DIAE,    // Alt+U (diaeresis dead key ¨)
    ES_NTIL,    // Alt+N then N (ñ)
    ES_NTIL_S,  // Alt+N then Shift+N (Ñ)
    // Layer 1 keys with shift-dependent behavior
    ES_L1_MINS, // ´ (Alt+E) / ¨ (Alt+U) when shifted
    ES_L1_QUOT, // ' / ? when shifted
    ES_L1_GRV,  // + / * when shifted
    ES_L1_EQL,  // < / > when shifted
    ES_L1_NTIL, // ñ (tap) / Shift (hold) - no uppercase Ñ
    ES_L1_COMM, // , / ; when shifted
    ES_L1_DOT,  // . / : when shifted
};

static uint16_t es_ntil_timer;
static bool es_ntil_pressed = false;
static bool es_ntil_shifted = false;

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    const uint8_t mods = get_mods();
    const bool shifted = mods & MOD_MASK_SHIFT;

    switch (keycode) {
        case ES_L1_NTIL:
            if (record->event.pressed) {
                es_ntil_timer = timer_read();
                es_ntil_pressed = true;
                es_ntil_shifted = false;
            } else {
                if (!es_ntil_shifted && timer_elapsed(es_ntil_timer) < TAPPING_TERM) {
                    SEND_STRING(SS_LALT("n") "n");
                }
                if (es_ntil_shifted) {
                    unregister_mods(MOD_BIT(KC_RSFT));
                }
                es_ntil_pressed = false;
                es_ntil_shifted = false;
            }
            return false;

        case ES_L1_MINS:
            if (record->event.pressed) {
                if (shifted) {
                    del_mods(MOD_MASK_SHIFT);
                    SEND_STRING(SS_LALT("u"));
                    set_mods(mods);
                } else {
                    SEND_STRING(SS_LALT("e"));
                }
            }
            return false;

        case ES_L1_QUOT:
            if (record->event.pressed) {
                if (shifted) {
                    del_mods(MOD_MASK_SHIFT);
                    SEND_STRING("?");
                    set_mods(mods);
                } else {
                    SEND_STRING("'");
                }
            }
            return false;

        case ES_L1_GRV:
            if (record->event.pressed) {
                if (shifted) {
                    del_mods(MOD_MASK_SHIFT);
                    SEND_STRING("*");
                    set_mods(mods);
                } else {
                    SEND_STRING("+");
                }
            }
            return false;

        case ES_L1_EQL:
            if (record->event.pressed) {
                if (shifted) {
                    del_mods(MOD_MASK_SHIFT);
                    SEND_STRING(">");
                    set_mods(mods);
                } else {
                    SEND_STRING("<");
                }
            }
            return false;

        case ES_L1_COMM:
            if (record->event.pressed) {
                if (shifted) {
                    del_mods(MOD_MASK_SHIFT);
                    SEND_STRING(";");
                    set_mods(mods);
                } else {
                    SEND_STRING(",");
                }
            }
            return false;

        case ES_L1_DOT:
            if (record->event.pressed) {
                if (shifted) {
                    del_mods(MOD_MASK_SHIFT);
                    SEND_STRING(":");
                    set_mods(mods);
                } else {
                    SEND_STRING(".");
                }
            }
            return false;

        case ES_ACUT:
            if (record->event.pressed) {
                SEND_STRING(SS_LALT("e"));
            }
            return false;

        case ES_DIAE:
            if (record->event.pressed) {
                SEND_STRING(SS_LALT("u"));
            }
            return false;

        case ES_NTIL:
            if (record->event.pressed) {
                SEND_STRING(SS_LALT("n") "n");
            }
            return false;

        case ES_NTIL_S:
            if (record->event.pressed) {
                SEND_STRING(SS_LALT("n") "N");
            }
            return false;

        case SCRNSHT:
            if (record->event.pressed) {
                SEND_STRING(SS_LGUI(SS_LSFT(SS_LCTL("4"))));
            }
            return false;

        case LCK_SCR:
            if (record->event.pressed) {
                SEND_STRING(SS_LGUI(SS_LCTL("q")));
            }
            return false;

        case ZOOMIN:
            if (record->event.pressed) {
                SEND_STRING(SS_LGUI(SS_LSFT("=")));
            }
            return false;

        case ZOOMOUT:
            if (record->event.pressed) {
                SEND_STRING(SS_LGUI("-"));
            }
            return false;

        case ZOOMRST:
            if (record->event.pressed) {
                SEND_STRING(SS_LGUI("0"));
            }
            return false;
    }

    if (record->event.pressed && keycode != ES_L1_NTIL && es_ntil_pressed) {
        if (!es_ntil_shifted && timer_elapsed(es_ntil_timer) >= TAPPING_TERM) {
            es_ntil_shifted = true;
            register_mods(MOD_BIT(KC_RSFT));
        }
    }

    return true;
}

/** \brief Automatically enable sniping-mode on the pointer layer. */
#define CHARYBDIS_AUTO_SNIPING_ON_LAYER LAYER_POINTER

#ifdef CHARYBDIS_AUTO_POINTER_LAYER_TRIGGER_ENABLE
static uint16_t auto_pointer_layer_timer = 0;

#    ifndef CHARYBDIS_AUTO_POINTER_LAYER_TRIGGER_TIMEOUT_MS
#        define CHARYBDIS_AUTO_POINTER_LAYER_TRIGGER_TIMEOUT_MS 1000
#    endif // CHARYBDIS_AUTO_POINTER_LAYER_TRIGGER_TIMEOUT_MS

#    ifndef CHARYBDIS_AUTO_POINTER_LAYER_TRIGGER_THRESHOLD
#        define CHARYBDIss6_AUTO_POINTER_LAYER_TRIGGER_THRESHOLD 8
#    endif // CHARYBDIS_AUTO_POINTER_LAYER_TRIGGER_THRESHOLD
#endif     // CHARYBDIS_AUTO_POINTER_LAYER_TRIGGER_ENABLE

// layers definition
#define BASE TO(LAYER_BASE)   // qwerty english
#define ONE TG(LAYER_ONE)     // qwerty spanish
#define TWO TG(LAYER_TWO)     // dvorak english
#define THREE MO(LAYER_THREE) // symbols and numpad
#define FOUR MO(LAYER_FOUR)   // media and zoom
#define PT_Z LT(LAYER_POINTER, KC_Z)
#define PT_SLSH LT(LAYER_POINTER, KC_SLSH)
#define PT_SCLN LT(LAYER_POINTER, KC_SCLN)
#define PT_MINS LT(LAYER_POINTER, KC_MINS)
#define NUM_SPC LT(LAYER_FOUR, KC_SPC)

// home row mods qwerty
#define HR_LSFT MT(MOD_LSFT, KC_A)
#define HR_RSFT MT(MOD_RSFT, KC_SCLN)
#define HR_LCTL MT(MOD_LCTL, KC_S)
#define HR_RCTL MT(MOD_RCTL, KC_L)
#define HR_LALT MT(MOD_LALT, KC_D)
#define HR_RALT MT(MOD_RALT, KC_K)
#define HR_LGUI MT(MOD_LGUI, KC_F)
#define HR_RGUI MT(MOD_RGUI, KC_J)

// home row mods dvorak
#define HD_LSFT MT(MOD_LSFT, KC_A)
#define HD_RSFT MT(MOD_RSFT, KC_S)
#define HD_LCTL MT(MOD_LCTL, KC_O)
#define HD_RCTL MT(MOD_RCTL, KC_N)
#define HD_LALT MT(MOD_LALT, KC_E)
#define HD_RALT MT(MOD_RALT, KC_T)
#define HD_LGUI MT(MOD_LGUI, KC_U)
#define HD_RGUI MT(MOD_RGUI, KC_H)

// clang-format off
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
  [LAYER_BASE] = LAYOUT(
  // ╭──────────────────────────────────────────────────────╮ ╭──────────────────────────────────────────────────────╮
       KC_TAB,    KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,       KC_Y,    KC_U,    KC_I,    KC_O,    KC_P, KC_MINS,
  // ├──────────────────────────────────────────────────────┤ ├──────────────────────────────────────────────────────┤
       KC_ESC, HR_LSFT, HR_LCTL, HR_LALT, HR_LGUI,    KC_G,       KC_H, HR_RGUI, HR_RALT, HR_RCTL, HR_RSFT, KC_QUOT,
  // ├──────────────────────────────────────────────────────┤ ├──────────────────────────────────────────────────────┤
       KC_EQL,    PT_Z,    KC_X,    KC_C,    KC_V,    KC_B,       KC_N,    KC_M, KC_COMM,  KC_DOT, PT_SLSH,  KC_GRV,
  // ╰──────────────────────────────────────────────────────┤ ├──────────────────────────────────────────────────────╯
                                   THREE, KC_BSPC,     ONE,     KC_ENT, NUM_SPC
  //                            ╰───────────────────────────╯ ╰──────────────────╯
  ),

  [LAYER_ONE] = LAYOUT(
  // ╭──────────────────────────────────────────────────────╮ ╭──────────────────────────────────────────────────────╮
       _______, _______, _______, _______, _______, _______,    _______, _______, _______, _______, _______, ES_L1_MINS,
  // ├──────────────────────────────────────────────────────┤ ├──────────────────────────────────────────────────────┤
       _______, _______, _______, _______, _______, _______,    _______, _______, _______, _______, ES_L1_NTIL, ES_L1_QUOT,
  // ├──────────────────────────────────────────────────────┤ ├──────────────────────────────────────────────────────┤
       ES_L1_EQL, _______, _______, _______, _______, _______,    _______, _______, ES_L1_COMM, ES_L1_DOT, PT_MINS, ES_L1_GRV,
  // ╰──────────────────────────────────────────────────────┤ ├──────────────────────────────────────────────────────╯
                                  _______, _______,     TWO,    _______, _______
  //                            ╰───────────────────────────╯ ╰──────────────────╯
  ),

  [LAYER_TWO] = LAYOUT(
  // ╭──────────────────────────────────────────────────────╮ ╭──────────────────────────────────────────────────────╮
       _______, KC_QUOT, KC_COMM,  KC_DOT,    KC_P,    KC_Y,       KC_F,    KC_G,    KC_C,    KC_R,    KC_L, KC_BSLS,
  // ├──────────────────────────────────────────────────────┤ ├──────────────────────────────────────────────────────┤
       _______, HD_LSFT, HD_LCTL, HD_LALT, HD_LGUI,    KC_I,       KC_D, HD_RGUI, HD_RALT, HD_RCTL, HD_RSFT,  KC_EQL,
  // ├──────────────────────────────────────────────────────┤ ├──────────────────────────────────────────────────────┤
       KC_MINS, PT_SCLN,    KC_Q,    KC_J,    KC_K,    KC_X,       KC_B,    KC_M,    KC_W,    KC_V,    PT_Z, KC_SLSH,
  // ╰──────────────────────────────────────────────────────┤ ├──────────────────────────────────────────────────────╯
                                  _______, _______,    BASE,    _______, _______
  //                            ╰───────────────────────────╯ ╰──────────────────╯
  ),

  [LAYER_THREE] = LAYOUT(
  // ╭──────────────────────────────────────────────────────╮ ╭──────────────────────────────────────────────────────╮
       LCK_SCR, S(KC_1), S(KC_2), S(KC_3), S(KC_4), S(KC_5),    KC_PMNS,   KC_P7,   KC_P8,   KC_P9, KC_PSLS, XXXXXXX,
  // ├──────────────────────────────────────────────────────┤ ├──────────────────────────────────────────────────────┤
       KC_CAPS, S(KC_6), S(KC_7), S(KC_8), S(KC_9), S(KC_0),    KC_PPLS,   KC_P4,   KC_P5,   KC_P6, KC_PAST, KC_BSPC,
  // ├──────────────────────────────────────────────────────┤ ├──────────────────────────────────────────────────────┤
       SCRNSHT, KC_BSLS, KC_LBRC, KC_RBRC, S(KC_LBRC), S(KC_RBRC),KC_PDOT, KC_P1,   KC_P2,   KC_P3, KC_PEQL,  KC_ENT,
  // ╰──────────────────────────────────────────────────────┤ ├──────────────────────────────────────────────────────╯
                                  _______, _______, _______,    _______,   KC_P0
  //                            ╰───────────────────────────╯ ╰──────────────────╯
  ),

  [LAYER_FOUR] = LAYOUT(
  // ╭──────────────────────────────────────────────────────╮ ╭──────────────────────────────────────────────────────╮
       XXXXXXX, XXXXXXX, XXXXXXX,   KC_F5, XXXXXXX, XXXXXXX,    XXXXXXX, KC_VOLD,   KC_UP, KC_VOLU, KC_MPLY, KC_MUTE,
  // ├──────────────────────────────────────────────────────┤ ├──────────────────────────────────────────────────────┤
       XXXXXXX, XXXXXXX,  KC_F12,  KC_F11,  KC_F10, XXXXXXX,    KC_PGUP, KC_LEFT, KC_DOWN, KC_RGHT, KC_MPRV, KC_MNXT,
  // ├──────────────────────────────────────────────────────┤ ├──────────────────────────────────────────────────────┤
       XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,    KC_PGDN, ZOOMOUT, ZOOMRST,  ZOOMIN, XXXXXXX, XXXXXXX,
  // ╰──────────────────────────────────────────────────────┤ ├──────────────────────────────────────────────────────╯
                                  _______, _______, _______,    _______, XXXXXXX
  //                            ╰───────────────────────────╯ ╰──────────────────╯
  ),

  [LAYER_POINTER] = LAYOUT(
  // ╭──────────────────────────────────────────────────────╮ ╭──────────────────────────────────────────────────────╮
       QK_BOOT,  EE_CLR, XXXXXXX, XXXXXXX,DPI_RMOD,S_D_RMOD,    S_D_MOD, DPI_MOD, XXXXXXX, XXXXXXX,  EE_CLR, QK_BOOT,
  // ├──────────────────────────────────────────────────────┤ ├──────────────────────────────────────────────────────┤
       XXXXXXX, KC_LSFT, KC_LCTL, KC_LALT, KC_LGUI, XXXXXXX,    XXXXXXX, KC_RGUI, KC_RALT, KC_RCTL, KC_RSFT, XXXXXXX,
  // ├──────────────────────────────────────────────────────┤ ├──────────────────────────────────────────────────────┤
       XXXXXXX, _______, DRGSCRL, SNIPING, XXXXXXX, XXXXXXX,    XXXXXXX, XXXXXXX, SNIPING, DRGSCRL, _______, XXXXXXX,
  // ╰──────────────────────────────────────────────────────┤ ├──────────────────────────────────────────────────────╯
                                  KC_BTN2, KC_BTN1, KC_BTN3,    KC_BTN2, KC_BTN1
  //                            ╰───────────────────────────╯ ╰──────────────────╯
  ),
};
// clang-format on

#ifdef POINTING_DEVICE_ENABLE
#    ifdef CHARYBDIS_AUTO_POINTER_LAYER_TRIGGER_ENABLE
report_mouse_t pointing_device_task_user(report_mouse_t mouse_report) {
    if (abs(mouse_report.x) > CHARYBDIS_AUTO_POINTER_LAYER_TRIGGER_THRESHOLD || abs(mouse_report.y) > CHARYBDIS_AUTO_POINTER_LAYER_TRIGGER_THRESHOLD) {
        if (auto_pointer_layer_timer == 0) {
            layer_on(LAYER_POINTER);
#        ifdef RGB_MATRIX_ENABLE
            rgb_matrix_mode_noeeprom(RGB_MATRIX_NONE);
            rgb_matrix_sethsv_noeeprom(HSV_GREEN);
#        endif // RGB_MATRIX_ENABLE
        }
        auto_pointer_layer_timer = timer_read();
    }
    return mouse_report;
}

void matrix_scan_user(void) {
    if (auto_pointer_layer_timer != 0 && TIMER_DIFF_16(timer_read(), auto_pointer_layer_timer) >= CHARYBDIS_AUTO_POINTER_LAYER_TRIGGER_TIMEOUT_MS) {
        auto_pointer_layer_timer = 0;
        layer_off(LAYER_POINTER);
#        ifdef RGB_MATRIX_ENABLE
        rgb_matrix_mode_noeeprom(RGB_MATRIX_DEFAULT_MODE);
#        endif // RGB_MATRIX_ENABLE
    }
}
#    endif // CHARYBDIS_AUTO_POINTER_LAYER_TRIGGER_ENABLE

#    ifdef CHARYBDIS_AUTO_SNIPING_ON_LAYER
layer_state_t layer_state_set_user(layer_state_t state) {
    charybdis_set_pointer_sniping_enabled(layer_state_cmp(state, CHARYBDIS_AUTO_SNIPING_ON_LAYER));
    return state;
}
#    endif // CHARYBDIS_AUTO_SNIPING_ON_LAYER
#endif     // POINTING_DEVICE_ENABLE

#ifdef RGB_MATRIX_ENABLE
// Forward-declare this helper function since it is defined in rgb_matrix.c.
void rgb_matrix_update_pwm_buffers(void);
#endif

