/**
 * Bed Level Tools
 * Extended by: Christophe LEVEQUE (Tititopher68-dev)
 * Version: 1.0.0
 * 
 * Based on the original work of: Henri-J-Norden
 * https://github.com/Jyers/Marlin/pull/126
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

#include "../../../inc/MarlinConfigPre.h"

#if BOTH(DWIN_CREALITY_LCD_JYERSUI, HAS_LEVELING)

#include "../../marlinui.h"
#include "../../../core/types.h"
#include "../../../feature/bedlevel/bedlevel.h"
#include "../../../gcode/gcode.h"
#include "../../../module/planner.h"

#if ENABLED(AUTO_BED_LEVELING_UBL)
  #include "../../../libs/least_squares_fit.h"
  #include "../../../libs/vector_3.h"
#endif

#if HAS_GRAPHICAL_LEVELMANUAL
  #include "dwin_lcd.h"
  #include "jyersui.h"
#endif

#include "dwin.h"

#include "bedlevel_tools.h"

BedLevelToolsClass BedLevelTools;

bool BedLevelToolsClass::viewer_asymmetric_range = false;
bool BedLevelToolsClass::viewer_print_value = false;
bool BedLevelToolsClass::goto_mesh_value = false;
uint8_t BedLevelToolsClass::mesh_x = 0;
uint8_t BedLevelToolsClass::mesh_y = 0;

#if HAS_GRAPHICAL_LEVELMANUAL
  bool BedLevelToolsClass::LevelManual_flag = true;
#endif

constexpr uint16_t padding_x = 8;
constexpr uint16_t total_width_px = DWIN_WIDTH - padding_x - padding_x;

#if HAS_LIVEMESH || HAS_GRAPHICAL_LEVELMANUAL
  uint8_t _lastx, _lasty = 0;
#endif

uint16_t cell_width_px;
uint16_t cell_height_px;
uint16_t padding_y_top;

uint16_t start_x_px;
uint16_t end_x_px;
uint16_t start_y_px;
uint16_t end_y_px;

float v_max, v_min, range;

char _cmd[MAX_CMD_SIZE+16], _str_1[16], _str_2[16];

const uint16_t color_bg = CrealityDWIN.GetColor(HMI_datas.popup_bg, Color_Bg_Window);
const uint16_t color_popup_txt = CrealityDWIN.GetColor(HMI_datas.popup_txt, Popup_Text_Color);
const uint16_t color_popup_highlight = CrealityDWIN.GetColor(HMI_datas.popup_highlight, Color_White);

#if ENABLED(AUTO_BED_LEVELING_UBL)
  uint8_t BedLevelToolsClass::tilt_grid = 1;

  void BedLevelToolsClass::manual_value_update(bool undefined/*=false*/) {
    sprintf_P(_cmd, PSTR("M421 I%i J%i Z%s %s"), mesh_x, mesh_y, dtostrf(current_position.z, 1, 3, _str_1), undefined ? "N" : "");
    gcode.process_subcommands_now(_cmd);
    planner.synchronize();
  }

  bool BedLevelToolsClass::create_plane_from_mesh() {
    struct linear_fit_data lsf_results;
    incremental_LSF_reset(&lsf_results);
    GRID_LOOP(x, y) {
      if (!isnan(bedlevel.z_values[x][y])) {
        xy_pos_t rpos = { bedlevel.get_mesh_x(x), bedlevel.get_mesh_y(y) };
        incremental_LSF(&lsf_results, rpos, bedlevel.z_values[x][y]);
      }
    }

    if (finish_incremental_LSF(&lsf_results)) {
      SERIAL_ECHOPGM("Could not complete LSF!");
      return true;
    }

    bedlevel.set_all_mesh_points_to_value(0);

    matrix_3x3 rotation = matrix_3x3::create_look_at(vector_3(lsf_results.A, lsf_results.B, 1));
    GRID_LOOP(i, j) {
      float mx = bedlevel.get_mesh_x(i),
            my = bedlevel.get_mesh_y(j),
            mz = bedlevel.z_values[i][j];

      if (DEBUGGING(LEVELING)) {
        DEBUG_ECHOPAIR_F("before rotation = [", mx, 7);
        DEBUG_CHAR(',');
        DEBUG_ECHO_F(my, 7);
        DEBUG_CHAR(',');
        DEBUG_ECHO_F(mz, 7);
        DEBUG_ECHOPGM("]   ---> ");
        DEBUG_DELAY(20);
      }

      rotation.apply_rotation_xyz(mx, my, mz);

      if (DEBUGGING(LEVELING)) {
        DEBUG_ECHOPAIR_F("after rotation = [", mx, 7);
        DEBUG_CHAR(',');
        DEBUG_ECHO_F(my, 7);
        DEBUG_CHAR(',');
        DEBUG_ECHO_F(mz, 7);
        DEBUG_ECHOLNPGM("]");
        DEBUG_DELAY(20);
      }

      bedlevel.z_values[i][j] = mz - lsf_results.D;
    }
    return false;
  }

#else

  void BedLevelToolsClass::manual_value_update() {
    sprintf_P(_cmd, PSTR("G29 I%i J%i Z%s"), mesh_x, mesh_y, dtostrf(current_position.z, 1, 3, _str_1));
    gcode.process_subcommands_now(_cmd);
    planner.synchronize();
  }

#endif


void BedLevelToolsClass::manual_mesh_move(const bool zmove/*=false*/) {
  if (zmove) {
    planner.synchronize();
    current_position.z = goto_mesh_value ? bedlevel.z_values[mesh_x][mesh_y] : Z_CLEARANCE_BETWEEN_PROBES;
    planner.buffer_line(current_position, homing_feedrate(Z_AXIS), active_extruder);
    planner.synchronize();
  }
  else {
    TERN_(HAS_GRAPHICAL_LEVELMANUAL, if (LevelManual_flag) ) CrealityDWIN.Popup_Handler(MoveWait);
    sprintf_P(_cmd, PSTR("G0 F300 Z%s"), dtostrf(Z_CLEARANCE_BETWEEN_PROBES, 1, 3, _str_1));
    gcode.process_subcommands_now(_cmd);
    sprintf_P(_cmd, PSTR("G42 F4000 I%i J%i"), mesh_x, mesh_y);
    gcode.process_subcommands_now(_cmd);
    planner.synchronize();
    current_position.z = goto_mesh_value ? bedlevel.z_values[mesh_x][mesh_y] : Z_CLEARANCE_BETWEEN_PROBES;
    planner.buffer_line(current_position, homing_feedrate(Z_AXIS), active_extruder);
    planner.synchronize();
    TERN_(HAS_GRAPHICAL_LEVELMANUAL, if (LevelManual_flag) ) CrealityDWIN.Redraw_Menu();
  }
}

void BedLevelToolsClass::next_point(const bool graph_edit/*=false*/) {
  if ((mesh_x != (GRID_MAX_POINTS_X - 1) && (!temp_val.evenvalue)) || mesh_y != (GRID_MAX_POINTS_Y - 1)) {
    if ((mesh_x == (GRID_MAX_POINTS_X - 1) && mesh_y % 2 == 0) || (mesh_x == 0 && mesh_y % 2 == 1))
      mesh_y++;
    else if (mesh_y % 2 == 0)
      mesh_x++;
    else
      mesh_x--;
    #if HAS_GRAPHICAL_LEVELMANUAL
      LevelManual_flag = !graph_edit;
      if (graph_edit) DrawSelectPoint(mesh_x, mesh_y, false, true);
      else manual_mesh_move();
    #else
      manual_mesh_move();
    #endif
  }
  else if (temp_val.evenvalue && mesh_x != 0) { 
    mesh_x--;
    #if HAS_GRAPHICAL_LEVELMANUAL
      LevelManual_flag = !graph_edit;
      if (graph_edit) DrawSelectPoint(mesh_x, mesh_y, false, true);
      else manual_mesh_move();
    #else
      manual_mesh_move();
    #endif
  }
  #if ENABLED(AUTO_BED_LEVELING_UBL) && !HAS_BED_PROBE
  else {
    if (temp_val.active_menu == UBLMesh) {
    temp_val.flag_noprobe = true;
    CrealityDWIN.Viewmesh();
    }
  }
  #endif
}

void BedLevelToolsClass::prev_point(const bool graph_edit/*=false*/) {
  if (mesh_x != 0 || mesh_y != 0) {
    if ((mesh_x == (GRID_MAX_POINTS_X - 1) && mesh_y % 2 == 1) || (mesh_x == 0 && mesh_y % 2 == 0)) {
      mesh_y--;
    }
    else if (mesh_y % 2 == 0) {
      mesh_x--;
    }
    else {
      mesh_x++;
    }
    #if HAS_GRAPHICAL_LEVELMANUAL
      LevelManual_flag = !graph_edit;
      if (graph_edit) DrawSelectPoint(mesh_x, mesh_y, false, true);
      else manual_mesh_move();
    #else
      manual_mesh_move();
    #endif
  }
}


float BedLevelToolsClass::get_max_value() {
  float max = __FLT_MAX__ * -1;
  LOOP_L_N(x, GRID_MAX_POINTS_X) LOOP_L_N(y, GRID_MAX_POINTS_Y) {
    if (!isnan(bedlevel.z_values[x][y]) && bedlevel.z_values[x][y] > max)
      max = bedlevel.z_values[x][y];
  }
  return max;
}


float BedLevelToolsClass::get_min_value() {
  float min = __FLT_MAX__;
  LOOP_L_N(x, GRID_MAX_POINTS_X) LOOP_L_N(y, GRID_MAX_POINTS_Y) {
    if (!isnan(bedlevel.z_values[x][y]) && bedlevel.z_values[x][y] < min)
      min = bedlevel.z_values[x][y];
  }
  return min;
}


void BedLevelToolsClass::Init_Grid_Mesh(uint16_t _padding_y_top /*= 86*/) {
  padding_y_top = _padding_y_top;
  cell_width_px  = total_width_px / (GRID_MAX_POINTS_X);
  cell_height_px = total_width_px / (GRID_MAX_POINTS_Y);
}


void BedLevelToolsClass::Cell_Grid_Mesh(const uint8_t px, const uint8_t py) {
  start_x_px = padding_x + 2 + px * cell_width_px;
  end_x_px   = start_x_px + cell_width_px - 2;
  start_y_px = padding_y_top + ((GRID_MAX_POINTS_Y) - py - 1) * cell_height_px;
  end_y_px   = start_y_px + cell_height_px - 2;
}


#if HAS_LIVEMESH
  void BedLevelToolsClass::Draw_Grid_Mesh() {
    Init_Grid_Mesh(10 + TITLE_HEIGHT);
    GRID_LOOP(x, y) {
      Cell_Grid_Mesh(x, y);
      DWIN_Draw_Rectangle(0, CrealityDWIN.GetColor(HMI_datas.menu_split_line, Line_Color, true), 
            start_x_px - 1 , start_y_px - 1 , end_x_px + 1, end_y_px + 1);
      DWIN_Draw_Rectangle(1, Color_Grey, 
            start_x_px, start_y_px, end_x_px, end_y_px);
    }
  }
#endif


#if HAS_LIVEMESH || HAS_GRAPHICAL_LEVELMANUAL
  void BedLevelToolsClass::DrawSelectPoint(const uint8_t x, const uint8_t y, const bool startprobing/*=false*/, const bool edition/*=false*/) {
    if (!startprobing) {
      Cell_Grid_Mesh(_lastx, _lasty);
      #if HAS_GRAPHICAL_LEVELMANUAL
        if (edition) DWIN_Draw_Rectangle(0, Color_Bg_Black, 
              start_x_px - 1 , start_y_px - 1 , end_x_px + 1, end_y_px + 1);
        else
      #endif
      DWIN_Draw_Rectangle(0, CrealityDWIN.GetColor(HMI_datas.menu_split_line, Line_Color, true), 
            start_x_px - 1 , start_y_px - 1 , end_x_px + 1, end_y_px + 1);
    }
    Cell_Grid_Mesh(x, y);
    DWIN_Draw_Rectangle(0, (HMI_datas.pack_red) ? Red_Select_Color : Select_Color, 
            start_x_px - 1 , start_y_px - 1 , end_x_px + 1, end_y_px + 1);
    _lastx = x;
    _lasty = y;
  }
#endif


#if HAS_GRAPHICAL_LEVELMANUAL
  void BedLevelToolsClass::Draw_Bed_Mesh(const bool editpoint/*=false*/, const bool start/*=false*/) {
    Init_Grid_Mesh(editpoint ? 37 + TITLE_HEIGHT : 86); // 18 -> 37
    goto_mesh_value = false;
    if (editpoint && start) { mesh_x = 0; mesh_y = 0; }
#else
  void BedLevelToolsClass::Draw_Bed_Mesh() {
    Init_Grid_Mesh();
#endif
  temp_val.drawing_mesh = true;
  v_max = abs(get_max_value()), v_min = abs(get_min_value()), range = _MAX(v_min, v_max);

  // Clear background from previous selection and select new square, _MAX(0, padding_x - 1) -> _MAX(0, padding_x), padding_y_top + total_width_px -> padding_y_top + total_width_px - 1
  //DWIN_Draw_Rectangle(1, Color_Bg_Black, _MAX(0, padding_x - 1), _MAX(0, padding_y_top - 1), padding_x + total_width_px, padding_y_top + total_width_px);
  const uint16_t bg_start_x_px = padding_x + 2 + ((GRID_MAX_POINTS_X) - 1) * cell_width_px;
  const uint16_t bg_start_y_px = padding_y_top + ((GRID_MAX_POINTS_Y) - 1) * cell_height_px;
  DWIN_Draw_Rectangle(1, Color_Bg_Black, _MAX(0, padding_x), _MAX(0, padding_y_top - 2), bg_start_x_px + cell_width_px, bg_start_y_px + cell_height_px);
  GRID_LOOP(x, y) {
    #if HAS_GRAPHICAL_LEVELMANUAL
      DrawMeshPoint(x, y, bedlevel.z_values[x][y], false, editpoint);
    #else
      DrawMeshPoint(x, y, bedlevel.z_values[x][y]);
    #endif
  }
}  


#if HAS_GRAPHICAL_LEVELMANUAL
  void BedLevelToolsClass::DrawMeshPoint(const uint8_t x, const uint8_t y, const float v, const bool livemesh/*=false*/, const bool edition/*=false*/) {
#else
  void BedLevelToolsClass::DrawMeshPoint(const uint8_t x, const uint8_t y, const float v, const bool livemesh/*=false*/) {
#endif
    char buf[8];
    Cell_Grid_Mesh(x, y);
    #if HAS_LIVEMESH
      if (livemesh) { v_max = 0.25; v_min = 0.25; }       // Absolute value !
    #endif
    DWIN_Draw_Rectangle(1,                                                                                 // RGB565 colors: http://www.barth-dev.de/online/rgb565-color-picker/
      isnan(v) ? Color_Grey : (                                                           // gray if undefined
        (v < 0 ?
          (uint16_t)round(0x1F * -v / (((!viewer_asymmetric_range) && (!temp_val.flag_viewmesh) && (!livemesh)) ? range : v_min)) << 11 : // red if mesh point value is negative
          (uint16_t)round(0x3F *  v / (((!viewer_asymmetric_range) && (!temp_val.flag_viewmesh) && (!livemesh)) ? range : v_max)) << 5) | // green if mesh point value is positive
            _MIN(0x1F, (((uint8_t)abs(v) / 10) * 4))),                                    // + blue stepping for every mm
      start_x_px, start_y_px, end_x_px, end_y_px
    );

    safe_delay(10);
    LCD_SERIAL.flushTX();

    // Draw value text on
    if ((viewer_print_value || livemesh || temp_val.flag_viewmesh || TERN0(HAS_GRAPHICAL_LEVELMANUAL, edition))) {
      int8_t offset_x, offset_y = cell_height_px / 2 - fontsize;
      if (isnan(v)) {  // undefined
        DWIN_Draw_String(false, meshfont, Color_White, Color_Bg_Blue, start_x_px + cell_width_px / 2 - 5 - char_xoffset, start_y_px + offset_y, F("X"));
      }
      else {                          // has value
        if TERN(HAS_TJC_DISPLAY, (GRID_MAX_POINTS_X < 8), (GRID_MAX_POINTS_X < 10))
          sprintf_P(buf, PSTR("%s"), dtostrf(abs(v), 1, 2, _str_1));
        else
          sprintf_P(buf, PSTR("%02i"), (uint16_t)(abs(v- (int16_t)v) * 100));
        offset_x = cell_width_px / 2 - 3 * (strlen(buf)) - 2 - (2 * char_xoffset);
        if TERN(HAS_TJC_DISPLAY,(!(GRID_MAX_POINTS_X < 8)), (!(GRID_MAX_POINTS_X < 10))) {
          #if HAS_TJC_DISPLAY
           if ((GRID_MAX_POINTS_X >= 8) && (GRID_MAX_POINTS_X <= 10))
          #endif
            DWIN_Draw_String(false, meshfont, Color_White, Color_Bg_Blue, start_x_px - 2 + offset_x, start_y_px + offset_y /*+ square / 2 - 6*/, F("."));
        }
        DWIN_Draw_String(false, meshfont, Color_White, Color_Bg_Blue, start_x_px + 1 + offset_x, start_y_px + offset_y /*+ square / 2 - 6*/, buf);
      }
      safe_delay(10);
      LCD_SERIAL.flushTX();
    }
  }


void BedLevelToolsClass::Set_Mesh_Viewer_Status() { // TODO: draw gradient with values as a legend instead
  if (v_min > 3e+10F) v_min = 0.0000001;
  if (v_max > 3e+10F) v_max = 0.0000001;
  if (range > 3e+10F) range = 0.0000001;

  if (viewer_asymmetric_range || temp_val.flag_viewmesh) {
    dtostrf(-v_min, 1, 3, _str_1);
    dtostrf( v_max, 1, 3, _str_2);
  }
  else {
    dtostrf(-range, 1, 3, _str_1);
    dtostrf( range, 1, 3, _str_2);
  }
  sprintf_P(_cmd, PSTR("Red %s..0..%s Green"), _str_1, _str_2);
  ui.set_status(_cmd);
  temp_val.drawing_mesh = false;
}


#if HAS_GRAPHICAL_LEVELMANUAL
  void BedLevelToolsClass::value_up() {
    if (bedlevel.z_values[mesh_x][mesh_y] < MAX_Z_OFFSET) {
      bedlevel.z_values[mesh_x][mesh_y] += 0.01;
      gcode.process_subcommands_now(F("M290 Z0.01"));
      planner.synchronize();
      current_position.z += 0.01f;
      sync_plan_position();
      update_popup_editvalue();
    }
  }


  void BedLevelToolsClass::value_down() {
    if (bedlevel.z_values[mesh_x][mesh_y] > MIN_Z_OFFSET) {
      bedlevel.z_values[mesh_x][mesh_y] -= 0.01;
      gcode.process_subcommands_now(F("M290 Z-0.01"));
      planner.synchronize();
      current_position.z -= 0.01f;
      sync_plan_position();
      update_popup_editvalue();
    }
  }


  void BedLevelToolsClass::update_popup_editvalue() {
    if (isnan(bedlevel.z_values[mesh_x][mesh_y]))
                    bedlevel.z_values[mesh_x][mesh_y] = 0;
    const float editvalue_z = bedlevel.z_values[mesh_x][mesh_y];
    sprintf_P(_cmd, PSTR(" %s%s "), (editvalue_z >= 0)? "+" : "-" , dtostrf(abs(editvalue_z), 2, 2, _str_1));
    DWIN_Draw_String(true, DWIN_FONT_STAT, color_popup_txt, color_bg, 42 + 131, 235, F(_cmd));
  }


  void BedLevelToolsClass::onEncoderS(EncoderState encoder_diffState) {
    switch (encoder_diffState) {
      case ENCODER_DIFF_CW:    next_point(true); break;
      case ENCODER_DIFF_CCW:   prev_point(true); break;
      case ENCODER_DIFF_ENTER: manual_mesh_move(); popup_editvalue(); break;
      default: break;
    }
  }


  void BedLevelToolsClass::popup_editvalue() {
    CrealityDWIN.Confirm_Handler(Update_Editvalue);
    DWIN_Draw_String(false, DWIN_FONT_STAT, color_popup_txt, color_bg, 42, 235, F("Mesh Z value:"));
    
    update_popup_editvalue();
    goto_mesh_value = true;
    current_position.z = 0;
    manual_mesh_move(true);

    DWIN_UpdateLCD();
  }


  void BedLevelToolsClass::onEncoderV(EncoderState encoder_diffState) {
    switch (encoder_diffState) {
      case ENCODER_DIFF_CW:    value_up(); break;
      case ENCODER_DIFF_CCW:   value_down(); break;
      case ENCODER_DIFF_ENTER:
        JYERSUI::backcolor = JYERSUI::last_backcolor;
        goto_mesh_value = false;
        manual_mesh_move(true);
        CrealityDWIN.Popup_Handler(Finish_Editvalue);
        break;
      default: break;
    }
  }
#endif

#endif