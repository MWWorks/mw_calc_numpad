/* Copyright 2020 Matthew Whyte
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


#include "mw_calc_numpad.h"

//these should have proper header files, i'm cheaping out
//note including these only seems to work here, not in the above header or keymap - get duplicate definition error
// #include "noled.c"
// #include "calc.c"


// Optional override functions below.
// You can leave any or all of these undefined.
// These are only required if you want to perform custom actions.

// void keyboard_pre_init_kb(void) {
//     // put your keyboard start-up code here
//     // runs once when the firmware starts up

//     // matrix_init_user();

//     // print("ABC");
//     // delay(10000);
//     // print("DEF");
//     // delay(10000);
//     // print("GHI");

// //   noled_command(0b00101000);        //set to single height
// //   noled_command(0b00101100);        //set to double height mode
    
//     _delay_ms(5000);
//     noledBegin();
//     noledClear();

//     calcBegin();
//     calcInput('1');
//     calcInput('+');
//     calcInput('2');
//     calcInput('=');

//   // noledWriteChars(0, 0, "Arse");
//   //   _delay_ms(5000);
//   // noledWriteChars(0, 4, "Arse");
//   //   _delay_ms(5000);
//   // noledWriteChars(0, 8, "Arse");
//   //   _delay_ms(5000);

//     // noled_write_chars('1234567890      1234567890      ');
//     // print(itoa(cursorR));
//     // print(itoa(cursorC));
//     // noled_write_chars("ARSE");
//     // _delay_ms(2000);
//     // noled_clear();
//     // _delay_ms(2000);
//     // noled_write_chars("ARSE");
//     // noled_write_char('R');
//     // noled_write_char('S');
//     // noled_write_char('E');
//     // noled_write_char('r');
//     // noled_write_char('s');
//     // noled_write_char('e');
//     // noled_write_char_at(2, 0, 'r');
//     // noled_write_char_at(4, 0, 's');
//     // noled_write_char_at(6, 0, 'e');
//     // digitalWrite(NOLED_PIN_SCK, HIGH);
//     // digitalWrite(NOLED_PIN_MOSI, LOW);

//     // setPinOutput(NOLED_PIN_SCK);
//     // setPinOutput(NOLED_PIN_MOSI);
//     // writePinHigh(NOLED_PIN_SCK);
//     // writePinLow(NOLED_PIN_MOSI);
// }

// bool process_record_kb(uint16_t keycode, keyrecord_t *record) {

//     /*
//     here's where we handle the calculator keypresses

//     calcInput is self-contained, it handles all calc operations via input of button press
//     i'd hoped to be able to translate button presses by borrowing keycode_to_ascii_lut from qmk
//     but there's no easy map from keycode to ascii for + etc

//     hence therefore casing them out all individually below, which is clunky
//     rather than define custom keycodes, might as well just use the same keypad ones and catch they layer



//     */
//     if(layer_state_is(1)){
//         if (record->event.pressed) {
//             switch (keycode){
//                 case KC_KP_1:
//                     calcInput('1'); return false;
//             }
//         } else {
//         }
//         return false;
//     }

//     return process_record_user(keycode, record);
// }

/*
void matrix_scan_kb(void) {
    // put your looping keyboard code here
    // runs every cycle (a lot)

    matrix_scan_user();
}


bool led_update_kb(led_t led_state) {
    // put your keyboard LED indicator (ex: Caps Lock LED) toggling code here

    return led_update_user(led_state);
}
*/
