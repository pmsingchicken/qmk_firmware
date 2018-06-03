#include "laplace.h"
#ifdef RGBLIGHT_ENABLE
#include "rgblight.h"
#endif

#include "pincontrol.h"

#define _BASE 0
#define _FN1 1
#define _FN2 2

#define KC_ KC_TRNS
#define _______ KC_TRNS
#define XXXXXXX KC_NO
#define KC_FN1 TG(_FN1)
#define KC_FN2 MO(_FN2)
#define KC_SPFN1 LT(_FN1, KC_SPACE)
#define KC_SPFN2 LT(_FN2, KC_SPACE)
#define KC_BSFN1 LT(_FN1, KC_BSPC)
#define KC_BSFN2 LT(_FN2, KC_BSPC)
#define KC_RST RESET
#define KC_DBUG DEBUG
#define KC_RTOG RGB_TOG
#define KC_RMOD RGB_MOD
#define KC_RHUI RGB_HUI
#define KC_RHUD RGB_HUD
#define KC_RSAI RGB_SAI
#define KC_RSAD RGB_SAD
#define KC_RVAI RGB_VAI
#define KC_RVAD RGB_VAD
#define KC_CA LCA_T(KC_APP)
#define KC_STOG SOLENOID_TOG
#define KC_SBOF SOLENOID_BUZZ_OFF
#define KC_SBON SOLENOID_BUZZ_ON
#define KC_SDM SOLENOID_DWELL_MINUS
#define KC_SDP SOLENOID_DWELL_PLUS

#define SOLENOID_DEFAULT_DWELL 12 
#define SOLENOID_MAX_DWELL 100
#define SOLENOID_MIN_DWELL 4
#define SOLENOID_PIN D0

bool solenoid_enabled = true;
bool solenoid_on = false;
bool solenoid_buzz = false;
bool solenoid_buzzing = false;
uint16_t solenoid_start = 0;
uint8_t solenoid_dwell = SOLENOID_DEFAULT_DWELL;



void solenoid_buzz_on(void) {
  solenoid_buzz = true;
}

void solenoid_buzz_off(void) {
  solenoid_buzz = false;
}

void solenoid_dwell_minus(void) {
  if (solenoid_dwell > 0) solenoid_dwell--;
}

void solenoid_dwell_plus(void) {
  if (solenoid_dwell < SOLENOID_MAX_DWELL) solenoid_dwell++;
}

void solenoid_toggle(void) {
  solenoid_enabled = !solenoid_enabled;
} 

void solenoid_stop(void) {
  digitalWrite(SOLENOID_PIN, PinLevelLow);
  solenoid_on = false;
  solenoid_buzzing = false;
}

void solenoid_fire(void) {
  if (!solenoid_enabled) return;

  if (!solenoid_buzz && solenoid_on) return;
  if (solenoid_buzz && solenoid_buzzing) return;

  solenoid_on = true;
  solenoid_buzzing = true;
  solenoid_start = timer_read(); 
  digitalWrite(SOLENOID_PIN, PinLevelHigh);
}

void solenoid_check(void) {
  uint16_t elapsed = 0;

  if (!solenoid_on) return;

  elapsed = timer_elapsed(solenoid_start);

  //Check if it's time to finish this solenoid click cycle 
  if (elapsed > solenoid_dwell) {
    solenoid_stop();
    return;
  }

  //Check whether to buzz the solenoid on and off
  if (solenoid_buzz) {
    if (elapsed / SOLENOID_MIN_DWELL % 2 == 0){
      if (!solenoid_buzzing) {
        solenoid_buzzing = true;
        digitalWrite(SOLENOID_PIN, PinLevelHigh);
      }
    }
    else {
      if (solenoid_buzzing) {
        solenoid_buzzing = false;
        digitalWrite(SOLENOID_PIN, PinLevelLow);
      }
    }
  }
  
}

void solenoid_setup(void) {
  pinMode(SOLENOID_PIN, PinDirectionOutput);
}


enum planck_keycodes {
  SOLENOID_TOG,
  SOLENOID_DWELL_MINUS,
  SOLENOID_DWELL_PLUS,
  SOLENOID_BUZZ_ON,
  SOLENOID_BUZZ_OFF,
  EXT_PLV
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
  [_BASE] = KC_KEYMAP(
 //,----+----+----+----+----+----|----+----+----+----+----+----+----.
    GESC, Q  , W  , E  , R  , T  , Y  , U  , I  , O  , P  ,SCLN,QUOT,
 //|----`----`----`----`----`----`----`----`----`----`----`----`----+
    TAB  , A  , S  , D  , F  , G  , H  , J  , K  , L  ,BSLS, ENTER  ,
 //|-----`----`----`----`----`----`----`----`----`----`----`--------|
    LSFT   , Z  , X  , C  , V  , B  , N  , M  ,COMM,DOT ,SLSH, FN2  ,
 //|-------`----`----`----`----`----`----`----`----`----`----`------|
    LCTL ,LGUI,LALT ,SPC , SPC  ,  BSPC ,DEL , CA ,CAPS ,FN1 
 //`-----+----+-----+----+--------`--------+-----+-----+-----+------'
  ),

  [_FN1] = KC_KEYMAP(
 //,----+----+----+----+----+----+----+----+----+----+----+----+----.
    GRV , 1  , 2  , 3  , 4  , 5  , 6  , 7  , 8  , 9  , 0  ,MINS,EQL ,
 //|----`----`----`----`----`----`----`----`----`----`----`----`----+
    TAB , NO ,PSCR,SLCK,PAUS,INS ,HOME,PGUP, NO ,LBRC,RBRC, ENTER   ,
 //|-----`----`----`----`----`----`----`----`----`----`----`--------+
    LSFT   , NO , NO , NO , NO ,DEL ,END,PGDN, NO , UP , NO , FN2   ,
 //|-------`----`----`----`----`----`----`----`----`----`----`------+
    LCTL ,LGUI,LALT,SPC  , SPC   ,  BSPC  ,LEFT ,DOWN ,RGHT , TRNS 
 //`-----+----+-----+----+--------+--------+-----+-----+-----+------'
  ),

  [_FN2] = KC_KEYMAP(
 //,----+----+----+----+----+----+----+----+----+----+----+----+----.
    ESC , F1 , F2 , F3 , F4 , F5 , F6 , F7 , F8 , F9 ,F10 ,F11 ,F12 ,
 //|----`----`----`----`----`----`----`----`----`----`----`----`----+
	RTOG ,RHUI,RSAI,RVAI,VOLU, NO ,NO, NO , SBOF, SBON, NO ,  RMOD  ,
 //|-----`----`----`----`----`----`----`----`----`----`----`--------+
    LSFT ,RHUD,RSAD,RVAD,VOLD, NO , NO , NO , SDM , SDP , STOG, TRNS,
 //|-------`----`----`----`----`----`----`----`----`----`----`------+
	LCTL ,LGUI,LALT ,SPC , SPC  ,  BSPC , NO , NO , NO , TRNS 
 //`-----+----+-----+----+--------+--------+-----+-----+-----+------'
  )
};

void persistent_default_layer_set(uint16_t default_layer) {
  eeconfig_update_default_layer(default_layer);
  default_layer_set(default_layer);
}

void matrix_init_user(void) {
  rgblight_enable();
  solenoid_setup();
}

void matrix_scan_user(void) {
  #ifdef RGBLIGHT_ENABLE

  static uint8_t old_layer = 255;
  uint8_t new_layer = biton32(layer_state);

  if (old_layer != new_layer) {
    switch (new_layer) {
      case _BASE:
        rgblight_sethsv (  0,   0, 255);
        break;
      case _FN1:
        rgblight_sethsv (120, 255, 255);
        break;
      case _FN2:
        rgblight_sethsv (  0, 255, 255);
        break;
    }

    old_layer = new_layer;
  }

  #endif //RGBLIGHT_ENABLE
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
  
  if (record->event.pressed) {
    solenoid_fire(); 
  }

  switch (keycode) {
    case SOLENOID_TOG:
      if (record->event.pressed) {
        solenoid_toggle();
      }
      break;
    case SOLENOID_DWELL_MINUS:
      if (record->event.pressed) {
        solenoid_dwell_minus();
      }
      break;
    case SOLENOID_DWELL_PLUS:
      if (record->event.pressed) {
        solenoid_dwell_plus();
      }
      break;
    case SOLENOID_BUZZ_ON:
      if (record->event.pressed) {
        solenoid_buzz_on();
      }
      break;
    case SOLENOID_BUZZ_OFF:
      if (record->event.pressed) {
        solenoid_buzz_off();
      }
      break;
  }
  return true;
}
