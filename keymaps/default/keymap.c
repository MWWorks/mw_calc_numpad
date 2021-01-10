/* Copyright 2020 https://mwworks.uk mw@mwworks.uk
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
#include "noled.c"
#include "calc.c"
#include "raw_hid.h"


//hid currently work in progress
void raw_hid_receive(uint8_t *data, uint8_t length) {
    for(uint8_t i=0; i<length; i++){
        noledWriteChar(0, i, (char)data[i]);
    }
}


//borrowed this from process_terminal, we'll use it below
const char keycode_to_ascii_lut[58] = {0, 0, 0, 0, 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', 0, 0, 0, '\t', ' ', '-', '=', '[', ']', '\\', 0, ';', '\'', '`', ',', '.', '/'};

// Defines names for use in layer keycodes and the keymap
enum layer_names {
    _KP,
    _CL,
    _FN,
};

// Defines the keycodes used by our macros in process_record_user
enum custom_keycodes {
    QMKBEST = SAFE_RANGE,
    QMKURL,
    CL_PLUS,
    CL_STAR,
    CL_TYPE,
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    /* Base */
    [_KP] = LAYOUT(
        MO(_FN), KC_D   , TG(_CL), CL_TYPE,
        KC_ESC , KC_PSLS, KC_PAST, KC_PMNS,
        KC_KP_7, KC_KP_8, KC_KP_9, KC_PPLS,
        KC_KP_4, KC_KP_5, KC_KP_6,
        KC_KP_1, KC_KP_2, KC_KP_3,
        KC_KP_0,          KC_PDOT, LCTL_T(KC_PENT)
    ),
    [_CL] = LAYOUT(
        MO(_FN), _______, _______, _______,
        KC_C   , KC_SLSH, CL_STAR, KC_MINS,
        KC_7   , KC_8   , KC_9   , CL_PLUS,
        KC_4   , KC_5   , KC_6   ,
        KC_1   , KC_2   , KC_3   ,
        KC_0   ,          KC_DOT , KC_EQL
    ),
    [_FN] = LAYOUT(
        _______, KC_R   , KC_S   , KC_N   ,
        RESET  , _______, _______, _______,
        _______, _______, _______, _______,
        _______, RESET  , _______,
        _______, _______, _______,
        _______,          KC_X   , KC_TAB
    ),
};

void keyboard_pre_init_user(void) {
    noledBegin();
    noledClear();

    calcBegin();
    noledWriteChars(0, 0, "K");
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {

    /*
    here's where we handle the calculator keypresses

    calcInput is self-contained, it handles all calc operations via input of button press - see file for functions
    see above for borrow of keycode > ascii map - it doesn't handle everything, but does most - just + and * to case out
    so otherwise the calc layer is standard for 0-9 a-z etc that get hijacked below for the calculator

    */
    if (record->event.pressed) { //handle on press, else case is keyup
        //the above map handles most
        if(keycode<58 && keycode != KC_TAB){
            calcInput(keycode_to_ascii_lut[(uint8_t)keycode]); return false;
        }else if(keycode == CL_PLUS){
            calcInput('+'); return false;
        }else if(keycode == CL_STAR){
            calcInput('*'); return false;
        }else if(keycode == CL_TYPE){
            send_string(calc_result); return false;
        }
    }

    return true;
}

layer_state_t layer_state_set_user(layer_state_t state) {
    switch (get_highest_layer(state)) {
    case _CL:
          noledWriteChars(0, 0, "C");
        break;
    case _KP:
          noledWriteChars(0, 0, "K");
        break;
    }
  return state;
}

/*
void matrix_init_user(void) {

}

void matrix_scan_user(void) {

}

bool led_update_user(led_t led_state) {
    return true;
}
*/
