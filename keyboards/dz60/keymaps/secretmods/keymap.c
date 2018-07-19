#include "dz60.h"
#include <print.h>

#define MODS_CTRL_MASK  (MOD_BIT(KC_LSHIFT)|MOD_BIT(KC_RSHIFT))
#define RGBLIGHT_SLEEP

// custom keycodes
enum custom_keycodes {
  MY_LSHIFT_LBRACE = SAFE_RANGE,
  MY_RSHIFT_RBRACE,
  MY_BACKSPACE_PIPE,
  MY_LSPO
};
bool process_record_user(uint16_t keycode, keyrecord_t *record) {
  static uint8_t mods_pressed;
  static bool lshift_lbrace_interrupted = false;
  static bool rshift_rbrace_interrupted = false;
  static bool lspo_interrupted = false;
  static uint16_t lshift_lbrace_timer = 0;
  static uint16_t rshift_rbrace_timer = 0;
  static uint16_t lspo_timer = 0;
  switch(keycode) {
    case MY_LSPO:
      if (record->event.pressed) {
	lspo_interrupted = false;
        lspo_timer = timer_read();
        register_mods(MOD_BIT(KC_LSHIFT));
      } else {
	if (!lspo_interrupted && timer_elapsed(lspo_timer) < 200) {
	  register_code(KC_9);
	  unregister_code(KC_9);
	}
	unregister_mods(MOD_BIT(KC_LSHIFT));
      }
      return false;
    case MY_LSHIFT_LBRACE:
      if (record->event.pressed) {
        lshift_lbrace_interrupted = false;
        lshift_lbrace_timer = timer_read();
        register_mods(MOD_BIT(KC_LSHIFT));
      } else {
        if (!lshift_lbrace_interrupted && timer_elapsed(lshift_lbrace_timer) < 200) {
          register_code(KC_LBRACKET);
          unregister_code(KC_LBRACKET);
        }
        unregister_mods(MOD_BIT(KC_LSHIFT));
      }
      return false;
    case MY_RSHIFT_RBRACE:
      if (record->event.pressed) {
        rshift_rbrace_interrupted = false;
        rshift_rbrace_timer = timer_read();
        register_mods(MOD_BIT(KC_RSHIFT));
      } else {
        if (!rshift_rbrace_interrupted && timer_elapsed(rshift_rbrace_timer) < 200) {
          register_code(KC_RBRACKET);
          unregister_code(KC_RBRACKET);
        }
        unregister_mods(MOD_BIT(KC_RSHIFT));
      }
      return false;
    case MY_BACKSPACE_PIPE:
      // TODO(rob): all of these 'interrupted' states should be handled in a central place!! christ.
      lspo_interrupted = true;
      if (record->event.pressed) {
	mods_pressed = get_mods();
	if (mods_pressed & MOD_BIT(KC_LSHIFT)) {
	  register_code(KC_BSLASH);
	} else {
	  register_code(KC_BSPACE);
	}
      } else {
	if (mods_pressed & MOD_BIT(KC_LSHIFT)) {
	  unregister_code(KC_BSLASH);
	} else {
	  unregister_code(KC_BSPACE);
	}	
      }
      return false;
  }
  // TODO(rob): should this state be set differently?  currently this won't run for anything checked in the switch statement
  // default:
  lshift_lbrace_interrupted = true;
  rshift_rbrace_interrupted = true;
  lspo_interrupted = true;
  return true;
};

// tapdances
void dance_esc_forcequit_finished(qk_tap_dance_state_t *state, void *user_data) {
  if (state->count == 1) {
    register_code(KC_ESC);
  } else {
    register_code(KC_LALT);
    register_code(KC_LGUI);
    register_code(KC_ESC);
  }
};
void dance_esc_forcequit_reset(qk_tap_dance_state_t *state, void *user_data) {
  if (state->count == 1) {
    unregister_code(KC_ESC);
  } else {
    unregister_code(KC_LALT);
    unregister_code(KC_LGUI);
    unregister_code(KC_ESC);
  }
}
enum {
  TD_ESC_FORCEQUIT = 0
};
qk_tap_dance_action_t tap_dance_actions[] = {
  [TD_ESC_FORCEQUIT] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, dance_esc_forcequit_finished, dance_esc_forcequit_reset)
};


// layers
enum {
  _QWERTY,
  _WASD,
  _FN
};


// usb leds
void led_set_user(uint8_t usb_led) {
  static uint8_t prev_usb_led;
  bool is_num_lock = (usb_led & (1 << USB_LED_NUM_LOCK));
  bool was_num_lock = (prev_usb_led & (1 << USB_LED_NUM_LOCK));
  if (is_num_lock != was_num_lock) {
    layer_invert(_WASD);
  }
  prev_usb_led = usb_led;
};

uint32_t layer_state_set_user(uint32_t state) {
  uint8_t rgb_val = rgblight_get_val();
  switch (biton32(state)) {
    case _QWERTY:
      rgblight_sethsv(179, 48, rgb_val);
      break;
    case _WASD:
      rgblight_sethsv(128, 156, rgb_val);
      break;
    case _FN:
      rgblight_sethsv(0, 186, rgb_val);
      break;
  }
  return state;
};


// keymap
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
  // standard layer
  [_QWERTY] = LAYOUT(
         KC_GRV, KC_1, KC_2, KC_3, KC_4, KC_5, KC_6, KC_7, KC_8, KC_9, KC_0, KC_MINS, KC_EQL, KC_BSLASH, TD(TD_ESC_FORCEQUIT),
         KC_TAB, KC_Q, KC_W, KC_E, KC_R, KC_T, KC_Y, KC_U, KC_I, KC_O, KC_P, KC_LBRC, KC_RBRC, MY_BACKSPACE_PIPE,
         KC_LCTL, KC_A, KC_S, KC_D, KC_F, KC_G, KC_H, KC_J, KC_K, KC_L, KC_SCLN, KC_QUOT, KC_ENT,
         MY_LSPO, KC_NO, KC_Z, KC_X, KC_C, KC_V, KC_B, KC_N, KC_M, KC_COMM, KC_DOT, KC_SLSH, KC_RSPC, KC_NO,
         MO(_FN), KC_LALT, KC_LGUI, KC_SPC, KC_SPC, KC_SPC, KC_LEFT, KC_DOWN, KC_UP, KC_RIGHT, MO(_FN)),
  // standard fn layer
  [_FN] = LAYOUT(
         RESET, KC_F1, KC_F2, KC_F3, KC_F4, KC_F5, KC_F6, KC_F7, KC_F8, KC_F9, KC_F10, KC_F11, KC_F12, KC_AUDIO_VOL_DOWN, KC_AUDIO_VOL_UP,
         KC_TRNS, RGB_TOG, RGB_VAD, RGB_VAI, RGB_MODE_PLAIN, RGB_MODE_BREATHE, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_DELETE,
         KC_CAPS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_MEDIA_PLAY_PAUSE,
         MY_LSHIFT_LBRACE, KC_TRNS, KC_TRNS, KC_TRNS, BL_DEC, BL_TOGG, BL_INC, KC_NUMLOCK, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, MY_RSHIFT_RBRACE, KC_TRNS,
         KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_MEDIA_PREV_TRACK, KC_PGDOWN, KC_PGUP, KC_MEDIA_NEXT_TRACK, TG(_WASD)),

  // wasd layer, activated during numlock
  [_WASD] = LAYOUT(
         KC_F1, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
         KC_TRNS, KC_TRNS, KC_UP, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
         KC_TRNS, KC_LEFT, KC_DOWN, KC_RIGHT, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
         KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
         MO(_FN), KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS),
};
