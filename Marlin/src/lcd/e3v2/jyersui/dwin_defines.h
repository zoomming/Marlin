/**
 * DWIN general defines and data structs
 * Author: LChristophe68 (tititopher68-dev)
 * Version: 1.0
 * Date: 2022/02/03
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as 
 * published by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 */

/**
 * Modify and enhanced by Tititopher68-dev (Christophe L.) since 28/11/2021 !
 * build v3.0.0a was public at 28/05/2022 !
 */

#pragma once

#define EXTJYERSUI 1
//#define DEBUG_DWIN 1

#include "../../../core/types.h"
#include "../common/dwin_color.h"

#if ENABLED(LED_CONTROL_MENU)
  //#include "../../../feature/leds/leds.h"
#endif

#ifndef SET_PROGRESS_MANUALLY
  #define SET_PROGRESS_MANUALLY
#endif
#ifndef STATUS_MESSAGE_SCROLLING
  #define STATUS_MESSAGE_SCROLLING
#endif
#ifndef BAUD_RATE_GCODE
  #define BAUD_RATE_GCODE
#endif
#ifndef HAS_LCD_BRIGHTNESS
  #define HAS_LCD_BRIGHTNESS 1
#endif
#ifndef LCD_BRIGHTNESS_DEFAULT
  #define LCD_BRIGHTNESS_DEFAULT 127
#endif
#ifndef SOUND_MENU_ITEM
  #define SOUND_MENU_ITEM
#endif

#if ANY(PREHEAT_BEFORE_PROBING, PREHEAT_BEFORE_LEVELING, PREHEAT_BEFORE_LEVELING_PROBE_MANUALLY) && HAS_MESH
    #define HAS_LEVELING_HEAT 1
#endif

//#define HAS_ES_DIAG 1

//#define HAS_SHORTCUTS 1

#ifdef BABYSTEP_ZPROBE_OFFSET
  #undef BABYSTEP_ZPROBE_OFFSET
#endif

#if NONE(AQUILA_DISPLAY, DACAI_DISPLAY, TJC_DISPLAY)
  #define DWIN_DISPLAY
#endif

#if ENABLED(TJC_DISPLAY)
  #define HAS_TJC_DISPLAY 1
  #undef DWIN_CREALITY_LCD_CUSTOM_ICONS
  #undef TJC_DISPLAY
#endif

#if DISABLED(REVERSE_ENCODER_DIRECTION) && ENABLED(AQUILA_DISPLAY)
  #define REVERSE_ENCODER_DIRECTION
#endif

#if ENABLED(RC_CPUTYPE)
  #define HAS_RC_CPU 1
  #define STM_CPU "RCT6"
#else
  #define STM_CPU "RET6"
#endif

#ifdef STM32G0B1xx
  #define CPU_STM32G0B1
#endif

#if !EXTJYERSUI
  #undef HAS_LIVEMESH
  #undef HAS_PHYSICAL_MENUS
  #undef PID_GRAPH
  #undef HAS_GRAPHICAL_LEVELMANUAL
#endif

#if !HAS_LEVELING
  #undef HAS_GRAPHICAL_LEVELMANUAL
#endif

#if HAS_NORMAL_GCODE_PREVIEW && HAS_LIGHT_GCODE_PREVIEW
  #error "You can have both Normal and Light Gocode Preview defined"
  #error "Vous ne pouvez pas définir à la fois Normal et Light Gcode Preview"
#endif

#if HAS_NORMAL_GCODE_PREVIEW || HAS_LIGHT_GCODE_PREVIEW
  #define HAS_GCODE_PREVIEW 1
#endif

#if HAS_MAG_MOUNTED_PROBE
  #undef BLTOUCH
#endif

#if ALL(AUTOTEMP, HAS_HOTEND) && !HAS_RC_CPU && EXTJYERSUI
  #define HAS_AUTOTEMP_MENU 1
#endif

#if BOTH(HAS_RC_CPU, AUTO_BED_LEVELING_UBL)
  #define HAS_RC_CPU_AND_UBL 1
  #undef HAS_HOSTACTION_MENUS
  #undef HAS_E_CALIBRATION
  #undef HAS_GRAPHICAL_LEVELMANUAL
#endif

#if !HAS_RC_CPU_AND_UBL || (ENABLED(HAS_RC_CPU_AND_UBL) && !HAS_BED_PROBE)
  #define HAS_LEV_BLINKING
  #if SHOW_REAL_POS
    #define HAS_SHOW_REAL_POS_MENU
  #endif
#endif

#if ENABLED(MPC_AUTOTUNE)
  #ifndef MPC_AUTOTUNE_MENU
    #define MPC_AUTOTUNE_MENU
  #endif
#endif

#if ENABLED(HAS_RC_CPU, MPC_AUTOTUNE, HAS_BED_PROBE)
  #define HAS_RC_CPU_AND_MPC 1
  #undef HAS_E_CALIBRATION
#endif

#if ENABLED(HAS_RC_CPU, HAS_BED_PROBE) && EITHER(INPUT_SHAPING_X, INPUT_SHAPING_Y)
  #define HAS_RC_CPU_AND_SHAPING 1
  #if HAS_RC_CPU_AND_UBL
    #define HAS_RC_CPU_UBL_IS 1
    #define HAS_NO_DWIN_C109 1
    #if ENABLED(LIN_ADVANCE, EXPERIMENTAL_SCURVE)
        #undef EXPERIMENTAL_SCURVE
    #endif
    #if (E0_AUTO_FAN_PIN != -1)
      #undef PID_GRAPH
    #else
      #define HAS_NO_AUTOTILT_AFTERNPRINT 1
      #undef BLTOUCH_VALIDATE_ON_HOMING
    #endif
    #undef HAS_SCREENLOCK
    #undef UBL_TILT_ON_MESH_POINTS
    #undef UBL_TILT_ON_MESH_POINTS_3POINT
  #endif
  #undef HAS_HOSTACTION_MENUS
#endif

#if HAS_NORMAL_GCODE_PREVIEW
  #if HAS_RC_CPU_AND_UBL || HAS_RC_CPU_AND_MPC || HAS_RC_CPU_BILINEAR_EXP_SCURVE || HAS_RC_CPU_AND_SHAPING
    #undef HAS_NORMAL_GCODE_PREVIEW
    #define HAS_LIGHT_GCODE_PREVIEW 1
  #endif
#endif

#if EXTJYERSUI && HAS_BED_PROBE
  #define HAS_LIVEMESH 1
  #if (HAS_RC_CPU_AND_UBL && HAS_JUNCTION_DEVIATION && HAS_LIN_ADVANCE) || HAS_RC_CPU_UBL_IS
    #undef HAS_LIVEMESH
  #endif
#endif

#if ENABLED(HAS_RC_CPU, AUTO_BED_LEVELING_BILINEAR, LIN_ADVANCE, HAS_BED_PROBE)
  #if ENABLED(EXPERIMENTAL_SCURVE)
    #define HAS_RC_CPU_BILINEAR_EXP_SCURVE 1
  #endif
  #undef HAS_E_CALIBRATION
  // Note : Function E_CALIBRATION is disabled with LIN ADVANCED activated...
#endif

#if EXTJYERSUI
  #include "extjyersui.h"
#endif

#if ENABLED(ENDER3V2)
  #define PRINTERNAME "Ender-3 V2"
#elif ENABLED(ENDER3SERIES)
  #define PRINTERNAME "Ender-3 Series"
#elif ENABLED(ENDER3MAX)
  #define PRINTERNAME "Ender-3 Max"
#elif ENABLED(ENDER3S1_F1)
  #define PRINTERNAME "Ender-3 S1-F1"
#elif ENABLED(ENDER3S1_F4)
  #define PRINTERNAME "Ender-3 S1-F4"
#elif ENABLED(ENDER2PRO)
  #define PRINTERNAME "Ender-2 Pro"
#elif ENABLED(CR10SMARTPRO)
  #define PRINTERNAME "CR-10 Smart Pro"
#elif ENABLED(AQUILA)
  #define PRINTERNAME "Aquila"
#endif

#ifndef PRINTERNAME
  #if MB(CREALITY_V4, CREALITY_V422, CREALITY_V431)
    #if ANY(DWIN_DISPLAY, DACAI_DISPLAY, HAS_TJC_DISPLAY)
      #define PRINTERNAME "Ender-3 V2"
    #else
      #define PRINTERNAME "Aquila"
    #endif
  #elif MB(CREALITY_V427)
    #if ANY(DWIN_DISPLAY, DACAI_DISPLAY, HAS_TJC_DISPLAY)
      #define PRINTERNAME "Ender-3 Series"
    #else
      #define PRINTERNAME "Aquila"
    #endif
  #elif MB(CREALITY_V423)
      #define PRINTERNAME "Ender-2 Pro"
  #elif MB(CREALITY_V24S1_301,  CREALITY_V24S1_301F4)
    #define PRINTERNAME "Ender-3 S1"
  #elif MB(CREALITY_V25S1)
    #define PRINTERNAME "CR-10 Smart Pro"
  #else
    #define PRINTERNAME "Ender-3 Series"
  #endif
#endif

#if ENABLED(AUTO_BED_LEVELING_BILINEAR)
  #if ENABLED(BLTOUCH)
    #define MODEFW " BLTouch"
  #elif ENABLED(FIX_MOUNTED_PROBE)
    #define MODEFW " ABL Probe"
  #elif ENABLED(TOUCH_MI_PROBE)
    #define MODEFW " TouchMI"
  #elif ENABLED(PROBE_MANUALLY)
    #define MODEFW " ManualMesh"
  #endif
#elif ENABLED(AUTO_BED_LEVELING_UBL)
  #if NONE(PROBE_MANUALLY, BLTOUCH, FIX_MOUNTED_PROBE, TOUCH_MI_PROBE)
    #define MODEFW " UBL-Noprobe"
  #elif ENABLED(BLTOUCH)
    #define MODEFW " UBL-BLTouch"
  #elif ENABLED(FIX_MOUNTED_PROBE)
    #define MODEFW " UBL-ABL"
  #elif ENABLED(TOUCH_MI_PROBE)
    #define MODEFW " UBL-TouchMI"
  #endif
#else
  #define MODEFW ""
#endif

#if ANY(AUTO_BED_LEVELING_BILINEAR, AUTO_BED_LEVELING_UBL)
  #if !EXTJYERSUI
    #define GRIDFW " " STRINGIFY(GRID_MAX_POINTS_X) "x" STRINGIFY(GRID_MAX_POINTS_Y)
  #else
    #define GRIDFW ""
  #endif
#else
  #define GRIDFW ""
#endif

#ifndef CUSTOM_MACHINE_NAME
  #define CUSTOM_MACHINE_NAME PRINTERNAME MODEFW GRIDFW
#endif

//#define BOOTPERSO

#define Def_Leds_Color  0xFFFFFFFF

#if HAS_SHORTCUTS
  #if HAS_SCREENLOCK
    #define _NB_Shortcuts         10      // (10 shortcuts + Disable => 11)
  #else
    #define _NB_Shortcuts          9      // (9 shortcuts + Disable => 10)
  #endif
#endif
  
typedef struct { 
    bool time_format_textual = false;
    #if ENABLED(AUTO_BED_LEVELING_UBL)
      uint8_t tilt_grid_size : 3;
    #endif
    uint16_t corner_pos : 10;
    uint8_t cursor_color  : 4;
    uint8_t menu_split_line  : 4;
    uint8_t items_menu_text  : 4;
    uint8_t icons_menu_text  : 4;
    uint8_t background  : 4;
    uint8_t menu_top_bg  : 4;
    uint8_t menu_top_txt  : 4;
    uint8_t select_txt  : 4;
    uint8_t select_bg  : 4;
    uint8_t highlight_box  : 4;
    uint8_t popup_highlight  : 4;
    uint8_t popup_txt  : 4;
    uint8_t popup_bg  : 4;
    uint8_t ico_confirm_txt  : 4;
    uint8_t ico_confirm_bg  : 4;
    uint8_t ico_cancel_txt  : 4;
    uint8_t ico_cancel_bg  : 4;
    uint8_t ico_continue_txt  : 4;
    uint8_t ico_continue_bg  : 4;
    uint8_t print_screen_txt  :4;
    uint8_t print_filename  :4;
    uint8_t progress_bar  :4;
    uint8_t progress_percent  : 4;
    uint8_t remain_time  : 4;
    uint8_t elapsed_time  : 4;
    uint8_t status_bar_text  : 4;
    uint8_t status_area_text  : 4;
    uint8_t status_area_percent  : 4;
    uint8_t coordinates_text  : 4;
    uint8_t coordinates_split_line  : 4;
    #if ENABLED(HOST_ACTION_COMMANDS) && HAS_HOSTACTION_MENUS
      uint64_t host_action_label_1 : 48;
      uint64_t host_action_label_2 : 48;
      uint64_t host_action_label_3 : 48;
    #endif
    
    #if BOTH(HAS_BED_PROBE, AUTO_BED_LEVELING_UBL) && !HAS_NO_AUTOTILT_AFTERNPRINT
      uint16_t N_Printed : 8;
    #endif

    #if HAS_SHORTCUTS
      uint8_t shortcut_[_NB_Shortcuts];
      bool shortcuts_ui : 1;
    #endif

    #if HAS_NORMAL_GCODE_PREVIEW
      bool show_gcode_thumbnails : 1;
      //bool dacai_display : 1;
    #endif
    
    bool fan_percent : 1;
    #if ANY(AUTO_BED_LEVELING_BILINEAR, AUTO_BED_LEVELING_UBL, MESH_BED_LEVELING)
      bool leveling_active : 1;
    #endif
    #if ENABLED(BAUD_RATE_GCODE)
      bool baudratemode : 1;
    #endif

    #if HAS_LEVELING_HEAT
      bool ena_LevelingTemp_hotend : 1;
      bool ena_LevelingTemp_bed : 1;
      celsius_t LevelingTemp_hotend = TERN(PREHEAT_BEFORE_PROBING, PROBING_NOZZLE_TEMP, LEVELING_NOZZLE_TEMP);
      celsius_t LevelingTemp_bed = TERN(PREHEAT_BEFORE_PROBING, PROBING_BED_TEMP, LEVELING_BED_TEMP);
    #endif

    #if HAS_HOTEND && BOTH(MPC_AUTOTUNE, MPC_AUTOTUNE_MENU)
      uint8_t MPCtargetmode : 2;
      #if ENABLED(MPC_EDIT_MENU)
        uint8_t filheatcapacity : 3;
      #endif
    #endif

    #if ALL(SDSUPPORT, SDCARD_SORT_ALPHA, SDSORT_GCODE)
      bool sdsort_alpha : 1;
    #endif

    #if ENABLED(DWIN_CREALITY_LCD_CUSTOM_ICONS)
      uint8_t main_icon : 3;
    #endif
    
    bool rev_encoder_dir : 1;
    bool reprint_on : 1;
    bool pack_red = TERN(PACK_ICON_RED, true, false);

    #if ENABLED(BLTOUCH_VALIDATE_ON_HOMING)
      bool bltouch_validate_ui : 1;
    #endif

    #if ENABLED(ADAPTIVE_STEP_SMOOTHING)
      bool ass : 1;
    #endif

    #if HAS_AUTOTEMP_MENU
      bool enable_autotemp : 1;
    #endif

    #if HAS_LEVELING && HAS_LEVELING_HEAT
      uint8_t TempmodeLeveling : 2;
    #endif

    #if ENABLED(HAS_LEV_BLINKING)
      bool lev_blinking : 1;
    #endif

    #if ENABLED(HAS_SHOW_REAL_POS_MENU)
      bool show_real_pos : 1;
    #endif

    //#if ENABLED(HAS_HOTEND, HAS_E_CALIBRATION)
    //  uint8_t e_cal_feedrate_mm_s = DEF_E_CAL_FEEDRATE_MM_S;
    //#endif

    #if EXTJYERSUI

      #if HAS_PHYSICAL_MENUS
        int16_t x_bed_size = DEF_X_BED_SIZE;
        int16_t y_bed_size = DEF_Y_BED_SIZE;
        int16_t x_min_pos = DEF_X_MIN_POS;
        int16_t y_min_pos = DEF_Y_MIN_POS;
        int16_t x_max_pos = DEF_X_MAX_POS;
        int16_t y_max_pos = DEF_Y_MAX_POS;
        int16_t z_max_pos = DEF_Z_MAX_POS;
      #endif
      
      bool invert_dir_extruder = DEF_INVERT_E0_DIR;
      #if ENABLED(NOZZLE_PARK_FEATURE)
        xyz_int_t Park_point = DEF_NOZZLE_PARK_POINT;
      #endif
      #if HAS_BED_PROBE
          uint16_t zprobefeedfast = DEF_Z_PROBE_FEEDRATE_FAST;
          uint16_t zprobefeedslow = DEF_Z_PROBE_FEEDRATE_SLOW;
      #endif
      #if ENABLED(ADVANCED_PAUSE_FEATURE)
        uint8_t fil_unload_feedrate = DEF_FILAMENT_CHANGE_UNLOAD_FEEDRATE;
        uint8_t fil_fast_load_feedrate = DEF_FILAMENT_CHANGE_FAST_LOAD_FEEDRATE;
        #if !HAS_RC_CPU
          uint8_t park_nozzle_timeout = DEF_PAUSE_PARK_NOZZLE_TIMEOUT;
        #endif
      #endif
      #if HAS_MESH
        uint8_t grid_max_points = DEF_GRID_MAX_POINTS;
        float probing_margin = DEF_PROBING_MARGIN;
        float mesh_min_x = DEF_MESH_MIN_X;
        float mesh_max_x = DEF_MESH_MAX_X;
        float mesh_min_y = DEF_MESH_MIN_Y;
        float mesh_max_y = DEF_MESH_MAX_Y;
      #endif

    #endif

    #if ENABLED(CPU_STM32G0B1)
      uint16_t refreshDtime : 12;
    #endif

    #if BOTH(LED_CONTROL_MENU, HAS_COLOR_LEDS)
      uint32_t Led_Color = Def_Leds_Color;
    #endif

    #if (E0_AUTO_FAN_PIN != -1)
      uint8_t extruder_auto_fan_temperature = DEF_EXTRUDER_AUTO_FAN_TEMPERATURE;
    #endif
    
    #if HAS_MAG_MOUNTED_PROBE
      float mag_mounted_posx = DEF_MAG_MOUNTED_POSX;
      float mag_mounted_posy = DEF_MAG_MOUNTED_POSY;
      float mag_mounted_posz = DEF_MAG_MOUNTED_POSZ;
    #endif

  } HMI_datas_t;

  #if BOTH(LED_CONTROL_MENU, HAS_COLOR_LEDS)
    static constexpr size_t eeprom_data_size = 184;
  #else
    static constexpr size_t eeprom_data_size = 156;
  #endif
  extern HMI_datas_t HMI_datas;

//
// Undef :
//
#if ENABLED(LIN_ADVANCE)
  #if DISABLED(EXPERIMENTAL_SCURVE)
    #undef S_CURVE_ACCELERATION
  #endif
#endif


#if EXTJYERSUI
  #if HAS_PHYSICAL_MENUS
    #undef X_BED_SIZE
    #undef Y_BED_SIZE
    #undef X_MIN_POS
    #undef Y_MIN_POS
    #undef X_MAX_POS
    #undef Y_MAX_POS
    #undef Z_MAX_POS
  #endif

  #if ENABLED(NOZZLE_PARK_FEATURE)
    #undef NOZZLE_PARK_POINT
    #if !HAS_RC_CPU
      #undef PAUSE_PARK_NOZZLE_TIMEOUT
    #endif
  #endif

  #if HAS_BED_PROBE
    #undef Z_PROBE_FEEDRATE_FAST
    #undef Z_PROBE_FEEDRATE_SLOW
  #endif

  #if HAS_MESH
    #undef GRID_MAX_POINTS_X
    #undef GRID_MAX_POINTS_Y
    #undef GRID_MAX_POINTS
    #undef PROBING_MARGIN
    #undef MESH_MIN_X
    #undef MESH_MAX_X
    #undef MESH_MIN_Y
    #undef MESH_MAX_Y
  #endif

  #if ENABLED(ADVANCED_PAUSE_FEATURE)
    #undef FILAMENT_CHANGE_UNLOAD_FEEDRATE
    #undef FILAMENT_CHANGE_FAST_LOAD_FEEDRATE
  #endif

  #undef INVERT_E0_DIR

  #if (E0_AUTO_FAN_PIN != -1)
    #undef EXTRUDER_AUTO_FAN_TEMPERATURE
  #endif

  #if HAS_MAG_MOUNTED_PROBE
    #undef MAG_MOUNTED_DEPLOY_1
    #undef MAG_MOUNTED_DEPLOY_2
    #undef MAG_MOUNTED_DEPLOY_3
    #undef MAG_MOUNTED_DEPLOY_4
    #undef MAG_MOUNTED_DEPLOY_5
    #undef MAG_MOUNTED_STOW_1
    #undef MAG_MOUNTED_STOW_2
    #undef MAG_MOUNTED_STOW_3
    #undef MAG_MOUNTED_STOW_4
    #undef MAG_MOUNTED_STOW_5
  #endif
  
  //
  // New Defines :
  //
  #if HAS_PHYSICAL_MENUS
    #define X_BED_SIZE (float)HMI_datas.x_bed_size
    #define Y_BED_SIZE (float)HMI_datas.y_bed_size
    #define X_MIN_POS  (float)HMI_datas.x_min_pos
    #define Y_MIN_POS  (float)HMI_datas.y_min_pos
    #define X_MAX_POS  (float)HMI_datas.x_max_pos
    #define Y_MAX_POS  (float)HMI_datas.y_max_pos
    #define Z_MAX_POS  (float)HMI_datas.z_max_pos
  #endif

  #if HAS_BED_PROBE
    #define Z_PROBE_FEEDRATE_FAST HMI_datas.zprobefeedfast
    #define Z_PROBE_FEEDRATE_SLOW HMI_datas.zprobefeedslow
  #endif

  #if HAS_MESH
    #define GRID_MAX_POINTS_X HMI_datas.grid_max_points
    #define GRID_MAX_POINTS_Y HMI_datas.grid_max_points
    #define GRID_MAX_POINTS (HMI_datas.grid_max_points * HMI_datas.grid_max_points)
    #define PROBING_MARGIN HMI_datas.probing_margin
    #define MESH_MIN_X HMI_datas.mesh_min_x
    #define MESH_MAX_X HMI_datas.mesh_max_x
    #define MESH_MIN_Y HMI_datas.mesh_min_y
    #define MESH_MAX_Y HMI_datas.mesh_max_y
  #endif
  
  #define INVERT_E0_DIR HMI_datas.invert_dir_extruder
  #if ENABLED(NOZZLE_PARK_FEATURE)
    #define NOZZLE_PARK_POINT {(float)HMI_datas.Park_point.x, (float)HMI_datas.Park_point.y, (float)HMI_datas.Park_point.z}
  #endif
  #if ENABLED(ADVANCED_PAUSE_FEATURE)
    #define FILAMENT_CHANGE_UNLOAD_FEEDRATE (float)HMI_datas.fil_unload_feedrate
    #define FILAMENT_CHANGE_FAST_LOAD_FEEDRATE (float)HMI_datas.fil_fast_load_feedrate
    #if !HAS_RC_CPU
      #define PAUSE_PARK_NOZZLE_TIMEOUT HMI_datas.park_nozzle_timeout
      #define MIN_PARK_NOZZLE_TIMEOUT 30
      #define MAX_PARK_NOZZLE_TIMEOUT 180
    #endif
  #endif

  #if (E0_AUTO_FAN_PIN != -1)
      #define EXTRUDER_AUTO_FAN_TEMPERATURE HMI_datas.extruder_auto_fan_temperature
  #endif

  #if HAS_MAG_MOUNTED_PROBE
    #define MAG_MOUNTED_DEPLOY_1 { PROBE_DEPLOY_FEEDRATE, { HMI_datas.mag_mounted_posx, HMI_datas.mag_mounted_posy, HMI_datas.mag_mounted_posz } }  // Move to side Dock & Attach probe
    #define MAG_MOUNTED_DEPLOY_2 { PROBE_DEPLOY_FEEDRATE, { (HMI_datas.mag_mounted_posx - 45), HMI_datas.mag_mounted_posy, HMI_datas.mag_mounted_posz } }  // Move probe off dock
    #define MAG_MOUNTED_STOW_1   { PROBE_STOW_FEEDRATE,   { HMI_datas.mag_mounted_posx, HMI_datas.mag_mounted_posy, HMI_datas.mag_mounted_posz } }  // Move to dock
    #define MAG_MOUNTED_STOW_2   { PROBE_STOW_FEEDRATE,   { HMI_datas.mag_mounted_posx, HMI_datas.mag_mounted_posy, 0 } }  // Place probe beside remover
    #define MAG_MOUNTED_STOW_3   { PROBE_STOW_FEEDRATE,   { (HMI_datas.mag_mounted_posx - 5), HMI_datas.mag_mounted_posy, 0 } }  // Side move to remove probe
    #define MAG_MOUNTED_STOW_4   { PROBE_STOW_FEEDRATE,   { (HMI_datas.mag_mounted_posx - 5), HMI_datas.mag_mounted_posy, HMI_datas.mag_mounted_posz } }  // Ensure probe is off
  #endif

#endif


