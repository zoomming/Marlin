/**
 * Marlin 3D Printer Firmware
 * Copyright (c) 2020 MarlinFirmware [https://github.com/MarlinFirmware/Marlin]
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
#pragma once

/**
 * User_Config.h
 *
 * Basic settings such as:
 *
 * - 
 *
 * Advanced settings can be found in Configuration.h / Configuration_adv.h
 */

#if Ext_Config_JyersUI

//===========================================================================
//
// ****************************  PRINTER MODEL ******************************
//
//===========================================================================

#define ENDER3V2
//#define ENDER3SERIES
//#define ENDER3MAX
//#define ENDER3S1_F1
//#define ENDER3S1_F4
//#define ENDER2PRO
//#define CR10SMARTPRO
//#define AQUILA

// ******************************  CPU Type ********************************
/**
 * CPU-Type :
 * You can have STM32F103RET6, STM32F103RCT6, STM32F401RCT6, GD32F303RET6, 
 * STM32G0B1RET6 CPUs
 * For Family RCT6, uncomment #define RC_CPUTYPE
 * WARNING ! UBL mode and RCT6 Type CPU disable E CALIBRATION WIZZARD and
 *           HOST ACTIONS MENUS
 * 
 */

//#define RC_CPUTYPE




// ***************************** User BED SIZE ****************************
/**
 * Here you can define your own bed size X and Y
 * 
 */
//#define X_BED_SIZE 230
//#define Y_BED_SIZE 230


// **************************** HOME XY OFFSET ****************************
/**
 * Here you can set your HOME XY OFFSET, this is the offset of the nozzle 
 * from the Bed at {0,0}. If the nozzle is outside the Bed, the value should 
 * be negative. If the nozzle is inside the Bed, the value will be positive. 
 * To be defined in X and in Y.
 * 
 */

//#define HOME_X_OFFSET 0
//#define HOME_Y_OFFSET 0


// ***************************** User PREHEAT *****************************
//
// Without Linear Advanced actived, Preheat Constants -> Up to 7 are supported without changes
// With Linear Advanced actived, Preheat Constants -> Up to 5 are supported without changes
//
#define PREHEAT_1_LABEL       "Warmup"
#define PREHEAT_1_TEMP_HOTEND  210
#define PREHEAT_1_TEMP_BED      70
#define PREHEAT_1_FAN_SPEED      0 // Value from 0 to 255

#define PREHEAT_2_LABEL       "PLA"
#define PREHEAT_2_TEMP_HOTEND  205
#define PREHEAT_2_TEMP_BED      60
#define PREHEAT_2_FAN_SPEED      0 // Value from 0 to 255

#define PREHEAT_3_LABEL       "ABS"
#define PREHEAT_3_TEMP_HOTEND  240
#define PREHEAT_3_TEMP_BED      90
#define PREHEAT_3_FAN_SPEED      0 // Value from 0 to 255

#define PREHEAT_4_LABEL       "PETG"
#define PREHEAT_4_TEMP_HOTEND  235
#define PREHEAT_4_TEMP_BED      80
#define PREHEAT_4_FAN_SPEED      0 // Value from 0 to 255

#define PREHEAT_5_LABEL       "TPU"
#define PREHEAT_5_TEMP_HOTEND  230
#define PREHEAT_5_TEMP_BED      60
#define PREHEAT_5_FAN_SPEED      0 // Value from 0 to 255

//#define PREHEAT_6_LABEL       "Name"
//#define PREHEAT_6_TEMP_HOTEND 205
//#define PREHEAT_6_TEMP_BED     60
//#define PREHEAT_6_FAN_SPEED     0 // Value from 0 to 255

//#define PREHEAT_7_LABEL       "Name"
//#define PREHEAT_7_TEMP_HOTEND 205
//#define PREHEAT_7_TEMP_BED     60
//#define PREHEAT_7_FAN_SPEED     0 // Value from 0 to 255


//===========================================================================
//===========================================================================
//
// **********************  OPTIONAL FEATURE SETTINGS   **********************
//
//===========================================================================
//===========================================================================

// *************************  IMAGE BOOT CUSTOM  ****************************
#define BOOTPERSO

// **************************  LCD_CUSTOM_ICONS  ****************************
// If you have flashed your Display with my Custom Pack Perso ->
// uncomment #define DWIN_CREALITY_LCD_CUSTOM_ICONS 
#define DWIN_CREALITY_LCD_CUSTOM_ICONS

// *********************  THUMBNAILS GCODE PREVIEW  *************************
// You can choose betweeb Normal or Light Gcode Preview fonctionality
// With Light, you don't have littles Thumbnails in file lists
// in the Print menu. 
#define HAS_NORMAL_GCODE_PREVIEW 1
//#define HAS_LIGHT_GCODE_PREVIEW 1

// **********************  PHYSICAL SETTING MENUS  **************************
#define HAS_PHYSICAL_MENUS 1

// ************************  HOST ACTIONS MENUS  ****************************
#define HAS_HOSTACTION_MENUS 1

// ************************  ENDSTOPS DIAG DEBUG  ***************************
#define HAS_ES_DIAG 1

// **************************  EXTRA SHORTCUTS  *****************************
// Extra 1: Move Z
#define HAS_SHORTCUTS 1

// ***********************  E CALIBRATION WIZZARD  **************************
#define HAS_E_CALIBRATION 1

// ********************** PID Graph during Autotune  ************************
#define PID_GRAPH 1

// ************************** JUNCTION DEVIATION ****************************
// By default, CLASSIC_JERK is defined. Define HAS_JUNCTION_DEVIATION replace
// CLASSIC_JERK 
//#define HAS_JUNCTION_DEVIATION 1

// **************************** LINEAR ADVANCE ******************************
#define HAS_LIN_ADVANCE 1
//#define HAS_EXPERIMENTAL_SCURVE 1

// **************** MPC Model Predictive Control for Hotend *****************
//#define MPCCTRL
#define HAS_MPC_EDIT_MENU 1         // Disable to save some memory flash

// ************************** ZV Input Shaping ******************************
//#define HAS_INPUT_SHAPING 1

// ************************* SHOW REAL Z Position ***************************
#define SHOW_REAL_POS 1

// **************************** HAS SCREENLOCK  *****************************
#define HAS_SCREENLOCK 1

// ****************** HAS GRAPHICAL LEVELMANUAL EDITION  ********************
#define HAS_GRAPHICAL_LEVELMANUAL 1

// ********************** HAS UBL TILT ON MESH POINTS  **********************
// * Use nearest mesh points with G29 J for better Z reference
//#define HAS_UBL_TILT_ON_MESH_POINTS 1

// ************************* HAS MAG MOUNTED PROBE  *************************
/**
 * If you use a Magnetically Mounted Probe
 * type probe Klackender : see https://github.com/kevinakasam/KlackEnder-Probe
 * instead of BLTouch/CRTouch/3DTouch probe
 */
//#define HAS_MAG_MOUNTED_PROBE 1

// ************************ BLTOUCH VALIDATION CHECK  ***********************
// Process to detect a) unplugged BLTouch probe pins, 
// and b) reversed BLTouch probe pins, before homing.
//#define HAS_BLTOUCH_CHECK 1

// ******************** M486 CANCEL OBJECTS FUNCTIONALITY*********************
// IMPORTANT : Can be activated if the capacity of your flash memory allows it.
// The post-processing script according your slicer must be installed from the
// Cancel_Objects_Scripts_post_processings_Slicers folder.
//#define HAS_CANCEL_OBJECTS 1

// **************************** HAS SPRITE PRO  *****************************
// Uncomment if you have a Creality SPRITE PRO - 300°C
// These parameters define Thermistor, Temp MAX, E_Steps, Probe X and Y Offsets
//#define HAS_SPRITE_PRO 1
//#define TYPE_SPRITE_SENSOR 13    // can be type 1, type 5, type 13 or type 20.

// ************************** HAS CUSTOM EXTRUDER  ***************************
// Uncomment then define your own thermistance type number and Temp Max
// cf @section temperature in Configuration.h to determine the thermistance type
// (don't forget to take into account the overshoot of +15°C)
//#define CUSTOM_TEMP_SENSOR 1    // Number sensor type
//#define CUSTOM_TEMP_MAX 315     // Temp Max + 15°C
//#define CUSTOM_E_STEPS 415

//===========================================================================
//===========================================================================
//
// **********************  LCD and COLOR UI SETTINGS   **********************
//
//===========================================================================
//===========================================================================

// *********************  LCD DISPLAY TYPE SETTINGS  ************************
/**
 * LCD DISPLAY TYPE :
 * With this firmware, you can use 3 types of LCD Display :
 * - DWIN Display -> Stock Ender3V2 display (printers sold since their launch and
 *                   until December 2022 (subject to change)).
 * - DACAI Display -> New Ender3V2 / Ender3S1 display (printers sold since December 2021 (subject to change)).
 * - AQUILA Display -> LCD Display (sold by Voxelab) - 100% Compatible with DWIN Display.
 * 
 * If you haven't a DWIN Display, uncomment your type #define DACAI_DISPLAY 
 *    or #define AQUILA_DISPLAY (in this case, only if you have a Voxelab Aquila Printer!)
 */

//#define DACAI_DISPLAY
//#define AQUILA_DISPLAY
//#define TJC_DISPLAY

 /** 
  *    Shortcut Defaults
  *    0: Disable
  *    1: Preheat -> Preheat menu
  *    2: Coold. -> Cooldown action
  *    3: D. Step -> Disable Steps motors
  *    4: HomeXYZ -> AutoHome
  *    5: ZOffset -> Z-Offset menu
  *    6: M.Tram. -> Manual Tramming menu
  *    7: Chg Fil -> Changement Filament
  *    8: Move Z -> Moze Live Z action
  *    9: ScreenL -> ScreenLock action (only if '#define HAS_SCREENLOCK' is uncommented above)
  *    10: (or 9 if '#define HAS_SCREENLOCK' is commented above) Save -> Store Settings
  */

#if HAS_SHORTCUTS
  #define Def_Shortcut_0    1
  #define Def_Shortcut_1    2
  #define Def_Shortcut_2    0
  #define Def_Shortcut_3    0
  #define Def_Shortcut_4    0
  #define Def_Shortcut_5    0
  #define Def_Shortcut_6    0
  #define Def_Shortcut_7    0
  #define Def_Shortcut_8    0
  #define Def_Shortcut_9    0
  #define Def_Mode_Shortcut false  // false: Disable, true: Enable
#endif
  
/**
 * LCD Brightness default :
 * Default = 127
 * 
 */

#if ENABLED(DACAI_DISPLAY)
  #define Def_brightness_default          32
#else
  #define Def_brightness_default          127
#endif

/**
 * Apply a timeout to low-priority status messages :
 * Default = 60 seconds
 * 
 */
#define STATUS_MESSAGE_TIMEOUT_SEC 60 // (seconds)
  
/**
 * User Mode LIVE (move axes, Z-Offset settings) :
 * desactived (commented)
 * activated (uncommented)
 * 
 */

#define USER_MOVE_LIVE
  



/**
 * Color list number :
 * Default(0), White(1), Light_White(2), Blue(3), Yellow(4), Orange(5), Red(6), Light_Red(7), Green(8), Light_Green(9), Magenta(10), Light_Magenta(11), Cyan(12), Light_Cyan(13), Brown(14), Black(15)
 * 
 */

  //#define PACK_ICON_RED // unComment 'delete the //' if your have choice a pack Red for the icon's display


  #define Def_cursor_color               0
  #define Def_menu_split_line            0
  #define Def_items_menu_text            0
  #define Def_icons_menu_text            0
  #define Def_background                 0      // This parameter is disabled with TJC_DISPLAY
  #define Def_menu_top_bg                0
  #define Def_menu_top_txt               0
  #define Def_select_txt                 0
  #define Def_select_bg                  0
  #define Def_highlight_box              0
  #define Def_popup_highlight            0
  #define Def_popup_txt                  0
  #define Def_popup_bg                   0
  #define Def_ico_confirm_txt            0
  #define Def_ico_confirm_bg             0
  #define Def_ico_cancel_txt             0
  #define Def_ico_cancel_bg              0
  #define Def_ico_continue_txt           0
  #define Def_ico_continue_bg            0
  #define Def_print_screen_txt           0
  #define Def_print_filename             0
  #define Def_progress_bar               0
  #define Def_progress_percent           0
  #define Def_remain_time                0
  #define Def_elapsed_time               0
  #define Def_status_bar_text            0
  #define Def_status_area_text           0
  #define Def_status_area_percent        0
  #define Def_coordinates_text           0
  #define Def_coordinates_split_line     0



  #endif // End User_config.h