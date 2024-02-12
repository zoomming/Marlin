/**
 * Marlin 3D Printer Firmware
 * Copyright (c) 2021 MarlinFirmware [https://github.com/MarlinFirmware/Marlin]
 *
 * Based on Sprinter and grbl.
 * Copyright (c) 2011 Camiel Gubbels / Erik van der Zalm
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 */

/**
 * Based on part of original sources provided by Prusa under GPL
 * However, this code exists in others sources where the declared author does not respect
 * the GPL license by using parts of source code without citing the original authors
 * or by hiding code that must remain open.
 */

/**
 * Modify and enhanced by Tititopher68-dev (Christophe L.) since 28/11/2021 !
 * build v3.0.0a was public at 28/05/2022 !
 */

//#include "../../../inc/MarlinConfigPre.h"
#include "dwin_defines.h"

#if BOTH(DWIN_CREALITY_LCD_JYERSUI, HAS_ES_DIAG)

#include "diag_endstops.h"

#include "../../../core/types.h"

#if ENABLED(BD_SENSOR)
  #include "../../../module/endstops.h"
#endif

#include "../../marlinui.h"
#include "dwin_lcd.h"

#include "jyersui.h"
#include "dwin.h"




#if HAS_FILAMENT_SENSOR
  #include "../../../feature/runout.h"
#endif

#if HAS_BED_PROBE
  #include "../../../module/probe.h"
#endif

EndSDiagClass EndSDiag;


void draw_ends_label(FSTR_P const flabel=nullptr) {
  JYERSUI::cursor.x = 40;
  if (flabel) JYERSUI::Draw_String(F(flabel));
  JYERSUI::Draw_String(F(": "));
  JYERSUI::MoveBy(0, 25);
}

void draw_ends_state(const bool is_hit) {
  const uint8_t LM = 130;
  JYERSUI::cursor.x = LM;
  DWIN_Draw_Rectangle(1, Color_Bg_Window, LM, JYERSUI::cursor.y, LM + 100, JYERSUI::cursor.y + 20);
  is_hit ? JYERSUI::Draw_String(RGB(31,31,16), F(STR_ENDSTOP_HIT)) : JYERSUI::Draw_String(RGB(16,63,16), F(STR_ENDSTOP_OPEN));
  JYERSUI::MoveBy(0, 25);
}

void EndSDiagClass::Draw_ends_diag() {
  char str[20];
  sprintf_P(str, PSTR("%s %s"), GET_TEXT(MSG_LCD_ENDSTOPS), GET_TEXT(MSG_DEBUG_MENU));
  CrealityDWINClass::Clear_Screen(1);
  CrealityDWINClass::Draw_Title(str);
  JYERSUI::ClearMenuArea();
  DWIN_Draw_Rectangle(0, Color_White, 13, 59, 259, 351);
  CrealityDWINClass::DWIN_Init_diag_endstops();
  DWIN_Draw_Rectangle(1, (HMI_datas.pack_red)? Color_Red : Confirm_Color, 87, 280, 186, 317);
  DWIN_Draw_Rectangle(0, Color_White, 86, 279, 187, 318);
  DWIN_Draw_Rectangle(0, Color_White, 85, 278, 188, 319);
  sprintf_P(str, PSTR(" %s "), GET_TEXT(MSG_BUTTON_CANCEL));
  DWIN_Draw_String(false, DWIN_FONT_STAT, Color_White, Color_Bg_Window, 96, 290, F(str));
  JYERSUI::cursor.y = 80;
  #define ES_LABEL(S) draw_ends_label(F(STR_##S))
  #if HAS_X_MIN
    ES_LABEL(X_MIN);
  #endif
  #if HAS_Y_MIN
    ES_LABEL(Y_MIN);
  #endif
  #if HAS_Z_MIN
    ES_LABEL(Z_MIN);
  #endif
  #if HAS_FILAMENT_SENSOR
    draw_ends_label(F(STR_FILAMENT));
  #endif
  Update_ends_diag();
}

void EndSDiagClass::Update_ends_diag() {
  JYERSUI::cursor.y = 80;
  #if ENABLED(BD_SENSOR)
    bool Endstops::bdp_state; // = false
    #define READ_ES(P) ((P == Z_MIN_PIN) ? bdp_state : READ(P))
  #else
    #define READ_ES(P) READ(P)
  #endif
  #define ES_REPORT(S) draw_ends_state(READ_ES(S##_PIN) == S##_ENDSTOP_HIT_STATE)
  #if HAS_X_MIN
    ES_REPORT(X_MIN);
  #endif
  #if HAS_Y_MIN
    ES_REPORT(Y_MIN);
  #endif
  #if HAS_Z_MIN
    ES_REPORT(Z_MIN);
  #endif
  #if HAS_FILAMENT_SENSOR
    draw_ends_state(READ(FIL_RUNOUT1_PIN) != runout.out_state());
  #endif
  DWIN_UpdateLCD();
}

#endif
