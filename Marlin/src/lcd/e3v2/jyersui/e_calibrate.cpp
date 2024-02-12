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
 * Modify and enhanced by Tititopher68-dev (Christophe L.) since 28/11/2021 !
 * build v3.0.0a was public at 28/05/2022 !
 */

#include "../../../inc/MarlinConfigPre.h"

#if HAS_HOTEND && HAS_E_CALIBRATION

#include "../../../core/types.h"
#include "dwin_lcd.h"
#include "jyersui.h"
#include "dwin.h"
#include "e_calibrate.h"
#include "../../../gcode/gcode.h"
#include "../../../module/planner.h"
#include "../../../module/temperature.h"

E_calibrateClass e_calibrate;

bool E_calibrateClass::done = false;
//float E_calibrateClass::real_value = 0;
//float E_calibrateClass::control_value = 0;
//float E_calibrateClass::temp_value = 0;

void E_calibrateClass::initStage1() {
  done = false;
  temp_val.real_value = 0;
  temp_val.control_value = 0;
  //temp_value = 0;
  Draw_Stage1();
}

void E_calibrateClass::Draw_Stage1() {
  CrealityDWINClass::DWIN_Init_EC_Stage1();
  CrealityDWINClass::Clear_Screen(1);
  CrealityDWINClass::Draw_Title(GET_TEXT(MSG_E_CALIBRATION_TOOL));
  JYERSUI::ClearMenuArea();
  DWIN_Draw_Rectangle(0, CrealityDWINClass::GetColor(HMI_datas.highlight_box, Color_White), 13, 59, 259, 351);
  //
  //JYERSUI::Draw_CenteredString(DWIN_FONT_STAT, Color_White, 80, GET_TEXT_F(MSG_EC_STAGE1_1));
  //JYERSUI::Draw_CenteredString(DWIN_FONT_STAT, Color_White, 120, GET_TEXT_F(MSG_EC_STAGE1_2));
  //
  #if !HAS_TJC_DISPLAY
    DRAW_IconWB(1, ICON_ECalTool_S, 81, 70);
  #endif
  JYERSUI::Draw_CenteredString(DWIN_FONT_STAT, Color_White, 180, GET_TEXT_F(MSG_EC_STAGE1_1));
  JYERSUI::Draw_CenteredString(DWIN_FONT_STAT, Color_White, 210, GET_TEXT_F(MSG_EC_STAGE1_2));
  JYERSUI::Draw_CenteredString(DWIN_FONT_STAT, Color_White, 240, GET_TEXT_F(MSG_EC_STAGE1_4));
  //
  DWIN_Draw_Rectangle(1, CrealityDWINClass::GetColor(HMI_datas.ico_confirm_bg, (HMI_datas.pack_red)? Color_Red : Confirm_Color), 26, 280, 125, 317);
  DWIN_Draw_Rectangle(1, CrealityDWINClass::GetColor(HMI_datas.ico_cancel_bg , Cancel_Color), 146, 280, 245, 317);
  DWIN_Draw_String(false, DWIN_FONT_STAT, CrealityDWINClass::GetColor(HMI_datas.ico_confirm_txt, Color_White), CrealityDWINClass::GetColor(HMI_datas.ico_confirm_bg, (HMI_datas.pack_red)? Color_Red : Confirm_Color), 45, 290, GET_TEXT_F(MSG_BUTTON_START));
  DWIN_Draw_String(false, DWIN_FONT_STAT, CrealityDWINClass::GetColor(HMI_datas.ico_cancel_txt, Color_White), CrealityDWINClass::GetColor(HMI_datas.ico_cancel_bg , Cancel_Color), 165, 290, GET_TEXT_F(MSG_BUTTON_CANCEL));
  CrealityDWINClass::Popup_Select();
  DWIN_UpdateLCD();
}

void E_calibrateClass::Draw_Stage2() {
  CrealityDWINClass::DWIN_Init_EC_Stage2();
  CrealityDWINClass::Clear_Screen(1);
  CrealityDWINClass::Draw_Title(GET_TEXT(MSG_E_CALIBRATION_TOOL));
  JYERSUI::ClearMenuArea();
  DWIN_Draw_Rectangle(0, CrealityDWINClass::GetColor(HMI_datas.highlight_box, Color_White), 13, 59, 259, 351);
  //
  JYERSUI::Draw_CenteredString(DWIN_FONT_STAT, Color_White, 130, GET_TEXT_F(MSG_EC_STAGE2_1));
  JYERSUI::Draw_CenteredString(DWIN_FONT_STAT, Color_White, 160, GET_TEXT_F(MSG_OF));
  JYERSUI::Draw_CenteredString(DWIN_FONT_STAT, Color_White, 190, GET_TEXT_F(MSG_EC_STAGE2_3));
  //
  if (thermalManager.temp_hotend[0].target < thermalManager.extrude_min_temp) 
        CrealityDWINClass::Popup_Handler(ETemp);
  else {
      if (thermalManager.temp_hotend[0].is_below_target(-2)) {
        CrealityDWINClass::Popup_Handler(Heating);
        CrealityDWINClass::Update_Status(GET_TEXT(MSG_HEATING));
        thermalManager.wait_for_hotend(0);
        CrealityDWINClass::Update_Status("");
      }
      JYERSUI::ClearMenuArea();
      DWIN_Draw_Rectangle(0, CrealityDWINClass::GetColor(HMI_datas.highlight_box, Color_White), 13, 59, 259, 351);
      //
      JYERSUI::Draw_CenteredString(DWIN_FONT_STAT, Color_White, 130, GET_TEXT_F(MSG_EC_STAGE2_1));
      JYERSUI::Draw_CenteredString(DWIN_FONT_STAT, Color_White, 160, GET_TEXT_F(MSG_OF));
      JYERSUI::Draw_CenteredString(DWIN_FONT_STAT, Color_White, 190, GET_TEXT_F(MSG_EC_STAGE2_3));
      //char str[25];
      //sprintf_P(str, PSTR("M83\nG1 E100 F%i"), (HMI_datas.e_cal_feedrate_mm_s * 60));
      //gcode.process_subcommands_now(str);
      //planner.synchronize();
      current_position.e = 0;
      sync_plan_position();
      current_position.e = 100;
      planner.synchronize();
      planner.buffer_line(current_position, manual_feedrate_mm_s[E_AXIS], active_extruder);
      planner.synchronize();
      Draw_Stage3();
      DWIN_UpdateLCD();
  }
}

void E_calibrateClass::Draw_Stage3() {
  CrealityDWINClass::DWIN_Init_EC_Stage3();
  CrealityDWINClass::Clear_Screen(1);
  CrealityDWINClass::Draw_Title(GET_TEXT(MSG_E_CALIBRATION_TOOL));
  JYERSUI::ClearMenuArea();
  DWIN_Draw_Rectangle(0, CrealityDWINClass::GetColor(HMI_datas.highlight_box, Color_White), 13, 59, 259, 351);
  //
  #if !HAS_TJC_DISPLAY
    DRAW_IconWB(1, ICON_ECalTool_E, 81, 70);
  #endif
  JYERSUI::Draw_CenteredString(DWIN_FONT_STAT, Color_White, 180, GET_TEXT_F(MSG_EC_STAGE3_1));
  //JYERSUI::Draw_CenteredString(DWIN_FONT_STAT, Color_White, 120, GET_TEXT_F(MSG_EC_STAGE3_2));
  //JYERSUI::Draw_CenteredString(DWIN_FONT_STAT, Color_White, 160, GET_TEXT_F(MSG_EC_STAGE3_3));
  JYERSUI::Draw_CenteredString(DWIN_FONT_STAT, Color_White, 210, GET_TEXT_F(MSG_EC_STAGE3_4));
  //
  DWIN_Draw_Rectangle(1, (HMI_datas.pack_red)? Color_Red : Confirm_Color, 87, 280, 186, 317);
  DWIN_Draw_Rectangle(0, Color_White, 86, 279, 187, 318);
  DWIN_Draw_Rectangle(0, Color_White, 85, 278, 188, 319);  
  //
  JYERSUI::Draw_CenteredString(DWIN_FONT_STAT, Color_White, 290, GET_TEXT_F(MSG_BUTTON_CONFIRM));
  //
  const uint8_t LM = 20;
  JYERSUI::cursor.x = LM;
  JYERSUI::cursor.y = 240;
  char nstr[10];
  char str[20];
  sprintf_P(str, PSTR(" %s%s "), (temp_val.control_value >= 0)? "+" : "-" , dtostrf(abs(temp_val.control_value), 4, 1, nstr));
    DWIN_Draw_String(false, DWIN_FONT_STAT, Color_White, JYERSUI::backcolor, LM, JYERSUI::cursor.y, GET_TEXT_F(MSG_E_MEASURED_VALUE));
    DWIN_Draw_String(true, DWIN_FONT_STAT, Color_White, JYERSUI::backcolor, LM + (1 + STAT_CHR_W * strlen(GET_TEXT(MSG_E_MEASURED_VALUE))), JYERSUI::cursor.y, F(str));

  DWIN_UpdateLCD();
}

void E_calibrateClass::Draw_Stage4() {
  CrealityDWINClass::DWIN_Init_EC_Stage4();
  CrealityDWINClass::Clear_Screen(1);
  CrealityDWINClass::Draw_Title(GET_TEXT(MSG_E_CALIBRATION_TOOL));
  JYERSUI::ClearMenuArea();
  DWIN_Draw_Rectangle(0, CrealityDWINClass::GetColor(HMI_datas.highlight_box, Color_White), 13, 59, 259, 351);
  //
  temp_val.real_value = (planner.settings.axis_steps_per_mm[E_AXIS] * 100) / (120 - temp_val.control_value);
  planner.settings.axis_steps_per_mm[E_AXIS] = temp_val.real_value;
  planner.synchronize();
  char nstr[16];
  char str[25];
  sprintf_P(str, PSTR("%s steps/mm"), dtostrf(temp_val.real_value, 4, 2, nstr));
  JYERSUI::Draw_CenteredString(DWIN_FONT_STAT, Color_White, 160, GET_TEXT_F(MSG_EC_FINAL));
  JYERSUI::Draw_CenteredString(DWIN_FONT_STAT, Color_White, 200, F(str));
  //
  DWIN_Draw_Rectangle(1, (HMI_datas.pack_red)? Color_Red : Confirm_Color, 87, 280, 186, 317);
  DWIN_Draw_Rectangle(0, Color_White, 86, 279, 187, 318);
  DWIN_Draw_Rectangle(0, Color_White, 85, 278, 188, 319);  
  //
  JYERSUI::Draw_CenteredString(DWIN_FONT_STAT, Color_White, 290, GET_TEXT_F(MSG_BUTTON_EXIT));
  //

  DWIN_UpdateLCD();
}

void E_calibrateClass::onEncoderE(EncoderState encoder_diffState) {
  switch (encoder_diffState) {
    case ENCODER_DIFF_CW:
      //temp_value += 0.1;
      if (abs(temp_val.control_value) <= 100)  temp_val.control_value += 0.1 ; 
      break;
    case ENCODER_DIFF_CCW:
      //temp_value -= 0.1;
      if (abs(temp_val.control_value) <= 100) temp_val.control_value -= 0.1 ;
      break;
      
    case ENCODER_DIFF_ENTER: done = true; break;
    default: break;
  }
  const uint8_t LM = 20;
  JYERSUI::cursor.x = LM;
  JYERSUI::cursor.y = 240;
  char nstr[10];
  char str[20];
  sprintf_P(str, PSTR(" %s%s "), (temp_val.control_value >= 0)? "+" : "-" ,dtostrf(abs(temp_val.control_value), 4, 1, nstr));
    DWIN_Draw_String(true, DWIN_FONT_STAT, Color_White, JYERSUI::backcolor, LM + (1 + STAT_CHR_W * strlen(GET_TEXT(MSG_E_MEASURED_VALUE))), JYERSUI::cursor.y, F(str));

  DWIN_UpdateLCD();
}

#endif
