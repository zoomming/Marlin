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
#pragma once

/**
 * lcd/e3v2/jyersui/dwin.h
 */

/**
 * Modify and enhanced by Tititopher68-dev (Christophe L.) since 28/11/2021 !
 * build v3.0.0a was public at 28/05/2022 !
 */


#include "dwin_defines.h"
#include "dwin_lcd.h"
#include "jyersui.h"

#include "../common/encoder.h"
#include "../common/limits.h"
#include "../../../libs/BL24CXX.h"

#include "../../../inc/MarlinConfig.h"

#if EITHER(PIDTEMP, PIDTEMPBED)
  #include "../../../module/temperature.h"
#endif

#if HAS_BED_PROBE
  #include "../../../feature/bedlevel/bedlevel.h"
#endif

#if ANY(BABYSTEPPING, HAS_BED_PROBE, HAS_WORKSPACE_OFFSET)
  #define HAS_ZOFFSET_ITEM 1
#endif

#if HAS_ZOFFSET_ITEM
  #define MAX_Z_OFFSET 9.99
  #if HAS_BED_PROBE
    #define MIN_Z_OFFSET -9.99
  #else
    #define MIN_Z_OFFSET -1
  #endif
#endif

enum processID : uint8_t {
  Main, Print, Menu, Value, Option, File, Popup, Confirm, Keyboard, Wait, Locked, Short_cuts, 
  #if HAS_GRAPHICAL_LEVELMANUAL && EXTJYERSUI
    Selectvalue_Mesh, Editvalue_Mesh,
  #endif
  eCalibration
};

enum PopupID : uint8_t {
  Pause, Stop, Resume, SaveLevel, ETemp, ConfFilChange, PurgeMore, MeshSlot,
  Level, Home, MoveWait, Heating,  FilLoad, FilChange, TempWarn, Runout, PIDWait, PIDWaitH, PIDWaitB, MPCWaitH, Resuming, ManualProbing,
  FilInsert, HeaterTime, UserInput, LevelError, InvalidMesh, NocreatePlane, UI, Complete, ConfirmStartPrint, BadextruderNumber,
  TemptooHigh, PIDTimeout, PIDDone, MPC_Temperror, MPC_Interrupted, MPC_Done, viewmesh, Level2, Level3, endsdiag, Reprint, EC_Stage1, EC_Stage2, EC_Stage3, EC_Stage4, Bed_T_Wizard, Wizard_Bed_T, Custom,
  Plottemp, 
  #if HAS_GRAPHICAL_LEVELMANUAL && EXTJYERSUI
    EditMesh, Update_Editvalue, Finish_Editvalue,
  #endif
  MPCWait
};


enum menuID : uint8_t {
  MainMenu,
    Prepare,
      Move,
      #if HAS_HOTEND && HAS_E_CALIBRATION
        MoveE100,
      #endif
      HomeMenu,
      ManualLevel,
      ZOffset,
      Preheat,
      ChangeFilament,
      MenuCustom,
      HostActions,
    Control,
      TempMenu,
        #if HAS_AUTOTEMP_MENU
          AUTOTEMP_menu,
        #endif
        #if (HAS_HOTEND || HAS_HEATED_BED) && ANY(PIDTEMP, PIDTEMPBED)
          PID,
            HotendPID,
            BedPID,
        #endif
        #if BOTH(MPC_AUTOTUNE, MPC_AUTOTUNE_MENU)
          MPC,
          #if ENABLED(MPC_EDIT_MENU)
            MPCFilc,
          #endif
        #endif
        #if HAS_PREHEAT
          #define _PREHEAT_ID(N) Preheat##N,
          REPEAT_1(PREHEAT_COUNT, _PREHEAT_ID)
        #endif
      Motion,
        HomeOffsets,
        MaxSpeed,
        MaxAcceleration,
        #if HAS_CLASSIC_JERK
          MaxJerk,
        #endif
        #if HAS_JUNCTION_DEVIATION
          JDmenu,
        #endif
        Steps,
      #if ENABLED(FWRETRACT)
        FwRetraction,
      #endif
      Parkmenu,
      #if ANY(CASE_LIGHT_MENU, LED_CONTROL_MENU)
        Ledsmenu,
        #if BOTH(CASE_LIGHT_MENU, CASELIGHT_USES_BRIGHTNESS)
          CaseLightmenu,
        #endif
        #if ENABLED(LED_CONTROL_MENU)
          LedControlmenu,
          #if HAS_COLOR_LEDS
            #if ENABLED(LED_COLOR_PRESETS)
              LedControlpresets,
            #endif
            LedControlcustom,
          #endif
        #endif
      #endif
      Visual,
        ColorSettings,
        #if HAS_SHORTCUTS
          ShortcutSettings,
        #endif
      HostSettings,
        ActionCommands,
      Advanced,
        #if EXTJYERSUI && HAS_PHYSICAL_MENUS
          PhySetMenu,
        #endif
        #if HAS_BED_PROBE
          ProbeMenu,
          #if HAS_MAG_MOUNTED_PROBE
            MagMountedProbeMenu,
          #endif
        #endif
        #if HAS_TRINAMIC_CONFIG && !HAS_RC_CPU
          TMCMenu,
        #endif
        #if HAS_SHAPING
          ISmenu,
        #endif
        Filmenu,
      Info,
    #if HAS_MESH
      Leveling,
        LevelManual,
        LevelView,
        MeshViewer,
        LevelSettings,
        #if EXTJYERSUI
          MeshInsetMenu,
        #endif
        #if ENABLED(PROBE_MANUALLY)
          ManualMesh,
        #endif
        #if ENABLED(AUTO_BED_LEVELING_UBL) && !HAS_BED_PROBE
          UBLMesh,
        #endif
    #endif
    InfoMain,
  Tune,
    #if HAS_CANCEL_OBJECTS
      CancelObjects_menu,
    #endif
  PreheatHotend
};

#if HAS_PID_HEATING || ENABLED(MPC_AUTOTUNE)
  enum pidresult_t : uint8_t { 
    #if HAS_PID_HEATING 
      PID_BAD_HEATER_ID = 0, 
      PID_TEMP_TOO_HIGH, 
      PID_TUNING_TIMEOUT,
      PID_STARTED, 
      PID_EXTR_START, 
      PID_BED_START, 
      PID_DONE,
    #endif
    #if ENABLED(MPC_AUTOTUNE)
      MPCTEMP_START = 0,
      MPC_TEMP_ERROR,
      MPC_INTERRUPTED,
      MPC_DONE,
    #endif
  };
#endif

#if !HAS_RC_CPU
  enum multID : uint8_t {
    MULT_HOTEND,
    MULT_MPC,
    MULT_FILTYPE,
    MULT_BED,
    MULT_MAXACC,
    MULT_JERK,
    MULT_STEPS,
    MULT_AUTOTEMP
  };
#endif

typedef struct {
  // Flags
  bool flag_selection = false;
  bool flag_viewmesh = false;
  bool flag_leveling_m = false;
  #if !HAS_BED_PROBE
    bool flag_noprobe = false;
  #endif
  
  bool flag_shortcut = false;
  bool flag_chg_fil = false;
  bool auto_fw_retract = false;
  bool printing = false;
  bool paused = false;
  bool sdprint = false;
  bool livemove =  TERN(USER_MOVE_LIVE, true, false);
  bool liveadjust = TERN(USER_MOVE_LIVE, true, false);
  bool probe_deployed = false;
  bool flag_Dfloat = false;
  bool flag_tune_menu = false;
  bool flag_tune_submenu = false;

  #if HAS_SHORTCUTS
    bool flagdir = false;
    bool shortcutsmode = TERN(Def_Mode_Shortcut, Def_Mode_Shortcut, false);
  #endif

  bool flag_resetstatus = true;
  bool flag_stop_print = false;

  #if HAS_BED_PROBE
   bool flag_unreachable = false;
  #endif

  bool flag_busy = false;

  #if HAS_FAN
    bool force_restore_fan = false;
  #endif

  #if HAS_GCODE_PREVIEW
    bool file_preview = false;
  #endif

  #if !HAS_RC_CPU
    bool flag_QuickHome = false;
    bool flag_multiplicator = false;
  #endif

  #if HAS_MESH
    bool evenvalue = false;
    bool drawing_mesh = false;
  #endif

  #if EXTJYERSUI && PID_GRAPH && HAS_TEMP_SENSOR && !HAS_RC_CPU
    bool htemp = false;
  #endif

  #if HAS_AUTOTEMP_MENU
    bool autotemp_enable = false;
  #endif

  #if HAS_CANCEL_OBJECTS
    bool menucancelobjects = false;
  #endif

  // Auxiliary values
  AxisEnum axis = X_AXIS;    // Axis Select
  int16_t pausetemp = 0;
  int16_t pausebed = 0;
  int16_t pausefan = 0;
  uint8_t preheatmode = 0;
  uint8_t zoffsetmode = TERN(USER_MOVE_LIVE, 2, 0);
  uint8_t nummode = 0;
  uint16_t unitmultip = 1;
  
  #if !HAS_RC_CPU
    uint8_t nummult = 0;
    uint8_t factmultip = 0;
    uint8_t maxaccmult = 0;
    uint8_t jerkmult = 0;
    uint8_t stepsmult = 0;
    uint8_t mpcmult = 0;
    uint8_t fil_typemult = 0;
    uint8_t hotendmult = 0;
    uint8_t bedmult = 0;
    uint8_t autotempmult = 0;
  #endif

  #if HAS_FAN
    uint8_t fan_speed = 0;
  #endif

  uint8_t scrollpos = 0;
  uint8_t active_menu = MainMenu, last_menu = MainMenu;
  uint8_t selection = 0, last_selection = 0, last_pos_selection = 0;
  uint8_t process = Main, last_process = Main;
  PopupID popup, last_popup;
  float zoffsetvalue = 0;
  uint8_t gridpoint;
  float tempvalue;
  float valuemin;
  float valuemax;
  uint16_t valueunit;
  uint8_t valuetype;
  float corner_avg;
  float corner_pos;
  float zval;

  #if HAS_SHORTCUTS
    #define NB_Shortcuts  _NB_Shortcuts      // defined in dwin_defines.h
    //uint8_t shortcutsmode = 0;
    uint8_t activ_shortcut = 0;
    uint8_t old_sel_shortcut = 4;
    uint8_t _selection;
    uint8_t shortcut[NB_Shortcuts] = { 0 };
    uint8_t current_shortcut_0 = 0;
    uint8_t current_shortcut_1 = 0;
    uint8_t Available_Shortcut_Count = 0;
  #endif

  #if HAS_LEVELING && HAS_LEVELING_HEAT
    uint8_t LevelingTempmode = 0;
  #endif

  #if HAS_PID_HEATING
    uint16_t PID_e_temp = 180;
    uint16_t PID_bed_temp = 60;
    uint8_t PID_cycles = 5;
    pidresult_t pidresult = PID_DONE;
  #endif

  #if ENABLED(HAS_HOTEND, MPCTEMP) 
    #if ENABLED(MPC_INCLUDE_FAN)
      float editable_value;
    #endif
    uint8_t filheatcap;
    uint8_t MPC_targetmode = 0;
    celsius_t MPCtargettemp = 200;
  #endif
  uint8_t tune_item = 0;

  #if HAS_HOSTACTION_MENUS
    bool keyboard_restrict, reset_keyboard, numeric_keyboard = false;
    uint8_t maxstringlen;
  #endif

  #if HAS_FILAMENT_SENSOR
    bool State_runoutenable = false;
    uint8_t rsensormode = 0;
    float editable_distance;
  #endif

  #if ENABLED(BAUD_RATE_GCODE)
    uint8_t BAUD_PORT = 0;
  #endif

  #if HAS_HOTEND && HAS_E_CALIBRATION
    bool flag_wait = false;
    PopupID popup_EC;
    uint8_t old_process, old_last_selection;
    float control_value, real_value;
  #endif

  #if ENABLED(HAS_BED_PROBE, AUTO_BED_LEVELING_UBL) && !HAS_NO_AUTOTILT_AFTERNPRINT
    uint16_t NPrinted = 0;
  #endif

  bool sd_item_flag = false;

  #if ALL(SDSUPPORT, SDCARD_SORT_ALPHA, SDSORT_GCODE)
    bool old_sdsort;
    bool SDremoved = false;
  #endif

  #if ENABLED(DWIN_CREALITY_LCD_CUSTOM_ICONS)
    uint8_t iconmain = 0;
  #endif

  #if HAS_AUTOTEMP_MENU
    uint8_t autotemp_item = 0;
  #endif

  #if HAS_SHAPING
    bool flag_freq = true;
    #if ENABLED(INPUT_SHAPING_X)
      bool flag_IS_X = true;
    #endif
    #if ENABLED(INPUT_SHAPING_X)
      bool flag_IS_Y = true;
    #endif
    float editable_decimal;
  #endif

  #if HAS_BACKLIGHT_TIMEOUT
    bool backlight_timeout_disable = false;
  #endif

  #if EXTJYERSUI
    bool cancel_lev = false;       // Cancel leveling
    #if ENABLED(NOZZLE_PARK_FEATURE)
      int16_t last_pos = 0;
    #endif
    #if HAS_MESH
      float last_meshinset_value;
    #endif
  #endif

  #if HAS_CANCEL_OBJECTS
    int8_t index_nb = -1;
    uint8_t count_canceled;
  #endif

  #if HAS_TRINAMIC_CONFIG && !HAS_RC_CPU
    #if AXIS_IS_TMC(X)
      float stepper_current_x;
    #endif
    #if AXIS_IS_TMC(Y)
      float stepper_current_y;
    #endif
    #if AXIS_IS_TMC(Z)
      float stepper_current_z;
    #endif
    #if AXIS_IS_TMC(E0)
      float stepper_current_e;
    #endif
  #endif

  #if ENABLED(CPU_STM32G0B1)
    uint16_t refreshtimevalue;
  #endif
} temp_val_t;
extern temp_val_t temp_val;

enum colorID : uint8_t {
  Default, White, Light_White, Blue, Yellow, Orange, Red, Light_Red, Green, Light_Green, Magenta, Light_Magenta, Cyan, Light_Cyan, Brown, Black
};

enum shortcutID : uint8_t {
  Disable, Preheat_menu, Cooldown, Disable_stepper, Autohome, ZOffsetmenu , M_Tramming_menu, Change_Fil, Move_rel_Z, ScreenL, Save_set
};

constexpr float min_frequency = TERN(__AVR__, float(uint32_t(STEPPER_TIMER_RATE) / 2) / shaping_time_t(-2), 1.0f);
constexpr float max_freq_edit = 200.0f;

extern char Hostfilename[66];

#define Color_Shortcut_0     0x10E4
#define Color_Shortcut_1     0x29A6
#define Color_Red_Shortcut_0 0x2104
#define Color_Red_Shortcut_1 0xC0A5
#define Red_Select_Color     0xF26C  // Red Selected color if pack_red Display flashed by user


#define Custom_Colors_no_Black 14
#define Custom_Colors       15
#define Color_Aqua          RGB(0x00,0x3F,0x1F)
#define Color_Light_White   0xBDD7
#define Color_Green         0x07E0
#define Color_Light_Green   0x3460
#define Color_Cyan          0x07FF
#define Color_Light_Cyan    0x04F3
#define Color_Blue          0x015F
#define Color_Light_Blue    0x3A6A
#define Color_Magenta       0xF81F
#define Color_Light_Magenta 0x9813
#define Color_Light_Red     0x8800
#define Color_Orange        0xFA20
#define Color_Light_Orange  0xFBC0
#define Color_Light_Yellow  0x8BE0
#define Color_Brown         0xCC27
#define Color_Light_Brown   0x6204
#define Color_Black         0x0000
#define Color_Grey          0x18E3
#define Check_Color         0x4E5C  // Check-box check color
#define Confirm_Color       0x34B9
#define Cancel_Color        0x3186
// Custom icons



class CrealityDWINClass {
public:
  
  #if EXTJYERSUI && HAS_MESH
    static uint8_t _GridxGrid;
  #endif
  static constexpr const char * const color_names[Custom_Colors + 1] = {"Default","  White","L_White","   Blue"," Yellow"," Orange","    Red","  L_Red","  Green","L_Green","Magenta","L_Magen","   Cyan"," L_Cyan","  Brown","  Black"};
  static constexpr const char * const preheat_modes[3] = { "   Both", " Hotend", "    Bed" };
  static constexpr const char * const zoffset_modes[3] = { "No Live" , "OnClick", "   Live" };
  static constexpr const char * const num_modes[3] = { " 0.1 mm", "   1 mm", "  10 mm" };
  #if !HAS_RC_CPU
    static constexpr const char * const num_mult[3] = { "     x1", "    x10", "   x100" };
  #endif
  #if HAS_FILAMENT_SENSOR
   static constexpr const char * const runoutsensor_modes[4] = { "   NONE" , "   HIGH" , "    LOW", " MOTION" };
  #endif
  #if HAS_SHORTCUTS
    #if HAS_SCREENLOCK
      static constexpr const char * const shortcut_list[NB_Shortcuts + 1] = { "Disable", "Preheat" , " Coold." , "D. Step" , "HomeXYZ" , "ZOffset" , "M.Tram." , "Chg Fil" , " Move Z", "ScreenL", "   Save" };
      static constexpr const char * const _shortcut_list[NB_Shortcuts + 1] = { GET_TEXT(MSG_DISABLE), GET_TEXT(MSG_PREHEAT) , GET_TEXT(MSG_COOLDOWN) , GET_TEXT(MSG_DIS_STEPS) , GET_TEXT(MSG_AUTO_HOME) , GET_TEXT(MSG_OFFSET_Z) , GET_TEXT(MSG_M_TRAMMING) , GET_TEXT(MSG_CHGFIL) , GET_TEXT(MSG_MOVE_Z) , GET_TEXT(MSG_SCREENLOCK), GET_TEXT(MSG_BUTTON_SAVE) };
    #else
      static constexpr const char * const shortcut_list[NB_Shortcuts + 1] = { "Disable", "Preheat" , " Coold." , "D. Step" , "HomeXYZ" , "ZOffset" , "M.Tram." , "Chg Fil" , " Move Z", "   Save" };
      static constexpr const char * const _shortcut_list[NB_Shortcuts + 1] = { GET_TEXT(MSG_DISABLE), GET_TEXT(MSG_PREHEAT) , GET_TEXT(MSG_COOLDOWN) , GET_TEXT(MSG_DIS_STEPS) , GET_TEXT(MSG_AUTO_HOME) , GET_TEXT(MSG_OFFSET_Z) , GET_TEXT(MSG_M_TRAMMING) , GET_TEXT(MSG_CHGFIL) , GET_TEXT(MSG_MOVE_Z) , GET_TEXT(MSG_BUTTON_SAVE) };
    #endif
  #endif
  #if ENABLED(DWIN_CREALITY_LCD_CUSTOM_ICONS)
    static constexpr const char * const icon_main[4] = { "  Stock", " AleSan", " Gotcha", " Giadej" };
  #endif

  #if EXTJYERSUI && HAS_MESH
    #if ENABLED(AUTO_BED_LEVELING_BILINEAR)
      #if HAS_BED_PROBE
        static constexpr const char * const GridxGrid[13] = { "    3x3" , "    4x4" , "    5x5" , "    6x6" , "    7x7" , "    8x8" , "    9x9", "  10x10", "  11x11", "  12x12", "  13x13", "  14x14", "  15x15" };
      #else
        static constexpr const char * const GridxGrid[3] = { "    3x3" , "    4x4" , "    5x5" };
      #endif
    #elif ENABLED(AUTO_BED_LEVELING_UBL)
      #if HAS_BED_PROBE
        static constexpr const char * const GridxGrid[9] = { "    7x7", "    8x8", "    9x9", "  10x10", "  11x11", "  12x12", "  13x13", "  14x14", "  15x15" };
      #else
        static constexpr const char * const GridxGrid[3] = { "    3x3" , "    4x4" , "    5x5" };
      #endif
    #endif
  #endif

  #if HAS_LEVELING && HAS_LEVELING_HEAT
    static constexpr const char * const LevelingTemp_modes[4] = { "   Both", " Hotend", "    Bed", "   None" };
  #endif

  #if HAS_HOTEND && BOTH(MPC_AUTOTUNE, MPC_AUTOTUNE_MENU)
    #if ENABLED(MPC_EDIT_MENU)
      static constexpr const char * const Filament_heat_capacity[4] = { "   None", "    PLA", "   PETG", " Custom" };
    #endif
      static constexpr const char * const MPC_target[2] = { "  200 C", "  220 C" };
  #endif

  
  static void Clear_Screen(uint8_t e=3);
  static void Draw_Float(float value, uint8_t row, bool selected=false, uint16_t minunit=10);
  static void Draw_Option(uint8_t value, const char * const * options, uint8_t row, bool selected=false, bool color=false);
  #if HAS_HOSTACTION_MENUS
    static void Draw_String(char * string, uint8_t row, bool selected=false, bool below=false);
    static const uint64_t Encode_String(const char * string);
    static void Decode_String(const uint64_t num, char string[8]);
  #endif
  static uint16_t GetColor(uint8_t color, uint16_t original, bool light=false);
  static void Draw_Checkbox(uint8_t row, bool value);
  static void Draw_Title(const char * title);
  static void Draw_Title(FSTR_P const title);
  static void Draw_Menu_Item(uint16_t row, uint8_t icon=0, const char * const label1=nullptr, const char * const label2=nullptr, bool more=false, bool centered=false, bool onlyCachedFileIcon=false);
  static void Draw_Menu_Item(uint8_t row, uint8_t icon=0, FSTR_P const flabel1=nullptr, FSTR_P const flabel2=nullptr, bool more=false, bool centered=false, bool onlyCachedFileIcon=false);
  static void Draw_Menu(uint8_t menu, uint8_t select=0, uint8_t scroll=0);
  static void Redraw_Menu(bool lastprocess=true, bool lastselection=false, bool lastmenu=false, bool flag_scroll=false);
  static void Redraw_Screen();
  #if !HAS_RC_CPU
    static void Draw_Quick_Home();
    static void Get_fact_multiplicator(multID select);
  #endif
  static void Keep_selection();
  static void Return_to_tune();
 
  static void Main_Menu_Icons();
  static void Draw_Main_Menu(uint8_t select=0);
  static void Print_Screen_Icons();
  static void Draw_Print_Screen();
  static void Draw_Print_Filename(const bool reset=false);
  static void Draw_Print_ProgressBar();
  #if ENABLED(SET_REMAINING_TIME)
    static void Draw_Print_ProgressRemain();
  #endif
  static void Draw_Print_ProgressElapsed();
  static void Draw_Print_confirm();
  static void Draw_SD_Item(uint8_t item, uint8_t row, bool onlyCachedFileIcon=false);
  static void Draw_SD_List(bool removed=false, uint8_t select=0, uint8_t scroll=0, bool onlyCachedFileIcon=false);
  #if ALL(SDSUPPORT, SDCARD_SORT_ALPHA, SDSORT_GCODE)
    static void DWIN_Sort_SD(bool isSDMounted=false);
  #endif
  static void Draw_Status_Area(bool icons=false);
  static void Draw_Popup(FSTR_P const line1, FSTR_P const line2, FSTR_P const line3, uint8_t mode, uint8_t icon=0);
  static void Popup_Select(bool scal=false);
  static void Update_Status_Bar(bool refresh=false);
  #if HAS_HOSTACTION_MENUS
    static void Draw_Keyboard(bool restrict, bool numeric, uint8_t selected=0, bool uppercase=false, bool lock=false);
    static void Draw_Keys(uint8_t index, bool selected, bool uppercase=false, bool lock=false);
  #endif

  #if HAS_FAN
    static void Draw_item_fan_speed(const bool draw, const uint8_t row);
    static uint8_t Set_fan_speed_value();
  #endif
  
  #if HAS_LIGHT_GCODE_PREVIEW
    static void Preview_DrawFromSD();
    static void Get_Value(char *buf, const char * const key, float &value);
    static bool Has_Preview();
  #endif

  #if HAS_NORMAL_GCODE_PREVIEW
      static bool find_and_decode_gcode_preview(char *name, uint8_t preview_type, uint16_t *address, bool onlyCachedFileIcon=false);
      static bool find_and_decode_gcode_header(char *name, uint8_t header_type);
  #endif

  #if HAS_BED_PROBE
    static void Bed_Tramming_init_P();
    static void Bed_Tramming_wizard(bool wizard = false);
  #endif

  static void ZOffset_Init();
  static void ManualLevel_Init();
  static void Bed_Tramming_init();
  static void Bed_Tramming(uint8_t point, float z_pos, bool pb = false);

  #if BOTH(HAS_BED_PROBE, AUTO_BED_LEVELING_UBL)
    static void Autotilt_Common();
    #if !HAS_NO_AUTOTILT_AFTERNPRINT
      static void Autotilt_AfterNPrint(uint16_t NPrints);
    #endif
  #endif

  static FSTR_P Get_Menu_Title(uint8_t menu);
  static uint8_t Get_Menu_Size(uint8_t menu);
  static void Menu_Item_Handler(uint8_t menu, uint8_t item, bool draw=true);

  static void Popup_Handler(PopupID popupid, bool option = false);
  static void Confirm_Handler(PopupID popupid, bool option = false);

  static void Main_Menu_Control();
  static void Menu_Control();
  static void Value_Control();
  static void Option_Control();
  static void File_Control();
  static void Print_Screen_Control();
  static void Popup_Control();
  static void Confirm_Control();
  #if HAS_HOSTACTION_MENUS
    static void Keyboard_Control();
  #endif

  static void Setup_Value(float value, float min, float max, float unit, uint8_t type);
  static void Modify_Value(float &value, float min, float max, float unit, void (*f)()=nullptr);
  static void Modify_Value(uint8_t &value, float min, float max, float unit, void (*f)()=nullptr);
  static void Modify_Value(uint16_t &value, float min, float max, float unit, void (*f)()=nullptr);
  static void Modify_Value(int16_t &value, float min, float max, float unit, void (*f)()=nullptr);
  static void Modify_Value(uint32_t &value, float min, float max, float unit, void (*f)()=nullptr);
  static void Modify_Value(int8_t &value, float min, float max, float unit, void (*f)()=nullptr);
  static void Modify_Option(uint8_t value, const char * const * options, uint8_t max);
  
  #if HAS_HOSTACTION_MENUS
    static void Modify_String(char * string, uint8_t maxlength, bool restrict);
  #endif

  static void Update_Status(const char * const text);
  static void Start_Print(bool sd);
  static void Stop_Print();
  static void Update();
  static void State_Update();
  static void Screen_Update();
  static void AudioFeedback(const bool success=true);
  static void Save_Settings(char *buff);
  static void Load_Settings(const char *buff);
  static void Reset_Settings();

  static void Viewmesh();
  static void RebootPrinter();
  static void DWIN_RebootScreen();
  #if HAS_SCREENLOCK
    static void DWIN_ScreenLock();
    static void DWIN_ScreenUnLock();
    static void HMI_ScreenLock();
  #endif
  static void DWIN_Hostheader(const char *text);
  #if HAS_ES_DIAG
    static void DWIN_Init_diag_endstops();
  #endif  
  //static bool DWIN_iSprinting() { return temp_val.printing; }

  #if HAS_SHORTCUTS
    static void DWIN_Move_Z();
    static void DWIN_QuitMove_Z();
    static void HMI_Move_Z();
    static void Apply_shortcut(uint8_t shortcut);
    static uint8_t Get_Shortcut_Icon(uint8_t shortcut);
    static uint8_t Get_Available_Shortcut(uint8_t index);
    static uint8_t Get_Available_Shortcut_Count();
    static void Set_Current_Shortcuts();
    static void Init_Main();
  #endif

  #if HAS_HOTEND
    static void Check_E_Move_Mintemp(const bool calib = false);
  #endif

  #if HAS_HOTEND && HAS_E_CALIBRATION
    static void DWIN_Init_EC_Stage1();
    static void DWIN_Init_EC_Stage2();
    static void DWIN_Init_EC_Stage3();
    static void DWIN_Init_EC_Stage4();

    static void DWIN_E_Calibration();
    static void DWIN_E_Calibration_Done();
    static void HMI_E_Calibration();
  #endif

  #if ENABLED(BAUD_RATE_GCODE)
      static void SetBaudRate();
  #endif

  #if HAS_FILAMENT_SENSOR
    static void DWIN_Filament_Runout(const uint8_t extruder);
    static void Get_Rsensormode(uint8_t Rsmode);
  #endif

  #if ENABLED(FILAMENT_LOAD_UNLOAD_GCODES)
    static void DWIN_Load_Unload(const bool load = false);
  #endif

  #if EXTJYERSUI && HAS_MESH
    static uint8_t Get_Grid_Max_XY(uint8_t valuegrid);
    static uint8_t Get_GridxGrid(uint8_t datavalue);
    static void MaxMeshArea();
    static void CenterMeshArea();
    static void ApplyMeshLimits();
  #endif

  #if HAS_LEVELING
    static void Dis_steppers_and_cooldown();
  #endif
  
  #if HAS_LEVELING_HEAT
    static void HeatBeforeLeveling();
  #endif

  #if BOTH(LED_CONTROL_MENU, HAS_COLOR_LEDS)
    static void ApplyLEDColor();
  #endif
  
  static void DWIN_Invert_Extruder();

  static void DWIN_Gcode(const int16_t codenum);
  static void DWIN_CError();
  static void DWIN_C12();
  static void DWIN_C108();
  #if HAS_LEVELING && HAS_LEVELING_HEAT
    static void DWIN_C109();
    static void DWIN_C109_report();
  #endif
  static void DWIN_C510();
  static void DWIN_C997();

  #if EITHER(PIDTEMP, PIDTEMPBED)
    static void SetPID(celsius_t t, heater_id_t h);
    #if ENABLED(PIDTEMP)
      static void Do_HotendPID() { SetPID(temp_val.PID_e_temp, H_E0); }
    #endif
    #if ENABLED(PIDTEMPBED)
      static void Do_BedPID() { SetPID(temp_val.PID_bed_temp, H_BED); }
    #endif
  #endif

  #if PID_GRAPH && ANY(HAS_PID_HEATING, MPC_AUTOTUNE)
    static void DWIN_Draw_PID_MPC_Popup();
  #endif
  #if HAS_PID_HEATING
    static void PidTuning(pidresult_t result);
  #endif
  #if BOTH(MPC_AUTOTUNE, MPC_AUTOTUNE_MENU)
    static void ApplyMPCTarget();
    static void MPCTuning(pidresult_t result);
    #if ENABLED(MPC_EDIT_MENU)
      static void ApplyFilHeatCapacity();
    #endif
  #endif

  #if EXTJYERSUI && PID_GRAPH && (HAS_TEMP_SENSOR || HAS_HEATED_BED) && !HAS_RC_CPU
    static void DWIN_Draw_Plot_Nozzle_Bed(const bool htemp = true);
  #endif

  #if HAS_LIVEMESH
    static void Draw_LiveMesh();
  #endif

  #if HAS_GRAPHICAL_LEVELMANUAL && EXTJYERSUI
    static void Draw_EditMesh(const bool init = false);
    static void Selectvalue_Control();
    static void Editvalue_Control();
  #endif

  #if HAS_CANCEL_OBJECTS
    static uint8_t Get_nb_canceled_objects();
    static void DWIN_Cancel_object(int8_t object);
  #endif

};

extern CrealityDWINClass CrealityDWIN;
