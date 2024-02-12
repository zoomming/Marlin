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
 * (at your option) any later version. Need Configuration Files v3.x Unlimited
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
 * lcd/e3v2/jyersui/dwin.cpp
 * Initial Base Jyers (Jacob Myers) v1.3.5b
 * 
 * Modify and enhanced by Tititopher68-dev (Christophe L.) since 28/11/2021 !
 * build v3.0.0a was public at 28/05/2022 ! -> actual build is v3.x Unlimited 
 */

#include "../../../inc/MarlinConfigPre.h"

#if ENABLED(DWIN_CREALITY_LCD_JYERSUI)

  #include "dwin.h"

  #if EXTJYERSUI
    #include "extjyersui.h"
    #if PID_GRAPH
     #include "plot.h"
    #endif
  #else
    #undef PID_GRAPH
  #endif

  #if HAS_SCREENLOCK
    #include "screenlock.h"
  #endif

  #include "../../marlinui.h"
  #include "../../../MarlinCore.h"

  #include "../../../gcode/gcode.h"
  #include "../../../module/temperature.h"
  #include "../../../module/planner.h"

  #if HAS_SHAPING
    #include "../../../module/stepper.h"
  #endif

  #include "../../../module/settings.h"
  #include "../../../libs/buzzer.h"
  #include "../../../inc/Conditionals_post.h"

  //#define DEBUG_OUT 1
  #include "../../../core/debug_out.h"

  #if ENABLED(ADVANCED_PAUSE_FEATURE)
    #include "../../../feature/pause.h"
  #endif

  #if HAS_FILAMENT_SENSOR
    #include "../../../feature/runout.h"
    //editable_distance;
  #endif

  #if ENABLED(HOST_ACTION_COMMANDS)
    #include "../../../feature/host_actions.h"
  #endif

  //#if ANY(BABYSTEPPING, HAS_BED_PROBE, HAS_WORKSPACE_OFFSET)
  //  #define HAS_ZOFFSET_ITEM 1
  //#endif

  #ifndef strcasecmp_P
    #define strcasecmp_P(a, b) strcasecmp((a), (b))
  #endif

  #ifdef BLTOUCH_HS_MODE
    #include "../../../feature/bltouch.h"
  #endif

  #if HAS_LEVELING
    #include "../../../feature/bedlevel/bedlevel.h"
    #include "bedlevel_tools.h"
  #endif

  #if HAS_BED_PROBE
    #include "../../../module/probe.h"
  #endif

  #if ENABLED(POWER_LOSS_RECOVERY)
    #include "../../../feature/powerloss.h"
  #endif

  #include "../../../module/stepper.h"

  #if HAS_TRINAMIC_CONFIG && !HAS_RC_CPU
    #include "../../../module/stepper/trinamic.h"
  #endif

  #if HAS_ES_DIAG
    #include "diag_endstops.h"
  #endif

  #if HAS_SHORTCUTS
    #include "shortcuts.h"
  #endif

  #if HAS_MESH
    #include "trammingtool.h"
  #endif

  #if HAS_CANCEL_OBJECTS
    #include "../../../feature/cancel_object.h"
    //int8_t index_nb = -1;
    //uint8_t count_canceled;
  #endif

  #if ENABLED(CASE_LIGHT_MENU)
    #include "../../../feature/caselight.h"
  #endif

  #if ENABLED(LED_CONTROL_MENU)
    #include "../../../feature/leds/leds.h"
  #endif

  #if HAS_BACKLIGHT_TIMEOUT && defined(NEOPIXEL_BKGD_INDEX_FIRST)
    #include "../../../feature/leds/leds.h"
  #endif
  
  #if HAS_HOTEND && HAS_E_CALIBRATION
    #include "e_calibrate.h"
    //PopupID popup_EC;
    //uint8_t old_process, old_last_selection;
  #endif

  #include <stdio.h>

  //bool temp_val.sd_item_flag = false;
  
  #if HAS_GCODE_PREVIEW
    #include "../../../libs/base64.hpp"
    #include <map>
    #if HAS_LIGHT_GCODE_PREVIEW

      typedef struct {
        char name[13] = "";   //8.3 + null
        uint32_t thumbstart = 0;
        int thumbsize = 0;
        int thumbheight = 0;
        int thumbwidth = 0;
        uint8_t *thumbdata = nullptr;
        float time = 0;
        float filament = 0;
        float layer = 0;
        float width = 0;
        float height = 0;
        float length = 0;
        void setname(const char * const fn);
        void clear();
      } fileprop_t;
      fileprop_t fileprop;

      void fileprop_t::setname(const char * const fn) {
        const uint8_t len = _MIN(sizeof(name) - 1, strlen(fn));
        memcpy(&name[0], fn, len);
        name[len] = '\0';
      }

      void fileprop_t::clear() {
        fileprop.name[0] = '\0';
        fileprop.thumbstart = 0;
        fileprop.thumbsize = 0;
        fileprop.thumbheight = 0;
        fileprop.thumbwidth = 0;
        fileprop.thumbdata = nullptr;
        fileprop.time = 0;
        fileprop.filament = 0;
        fileprop.layer = 0;
      }

    #endif //end HAS_LIGHT_GCODE_PREVIEW

  #endif
  #include <string>
  using namespace std;

  #if !HAS_PHYSICAL_MENUS
    #define MACHINE_SIZE STRINGIFY(X_BED_SIZE) "x" STRINGIFY(Y_BED_SIZE) "x" STRINGIFY(Z_MAX_POS)
  #endif

  #ifndef CORP_WEBSITE_E
    #if ENABLED(BOOTPERSO)
      #define CORP_WEBSITE1 "Build date: " STRING_DISTRIBUTION_DATE
      #define CORP_WEBSITE2 "Author: tititopher68-dev"
    #else
      #define CORP_WEBSITE1 "Github.com/"
      #define CORP_WEBSITE2 "Jyers/Marlin/"
    #endif
  #endif

  #if ENABLED(BOOTPERSO)
    #define BUILD_NUMBER "Build v3.x Unlimited"
  #else
    #define BUILD_NUMBER "Build v2.6 Final"
  #endif

  #define MENU_CHAR_LIMIT  24
  #define STATUS_CHAR_LIMIT  30

  #define MAX_PRINT_SPEED   500
  #define MIN_PRINT_SPEED   10

  #if HAS_FAN
    #define MAX_FAN_SPEED     255
    #define MIN_FAN_SPEED     0
  #endif

  #define MAX_XY_OFFSET 100

  /**
  #if HAS_ZOFFSET_ITEM
    #define MAX_Z_OFFSET 9.99
    #if HAS_BED_PROBE
      #define MIN_Z_OFFSET -9.99
    #else
      #define MIN_Z_OFFSET -1
    #endif
  #endif
  */

  #if HAS_BED_PROBE
   	static float mlev_cycles = 1; // ims - default number of cycles for tramming
  #endif

  #if HAS_HOTEND
    #define MAX_FLOW_RATE   200
    #define MIN_FLOW_RATE   10

    #define MAX_E_TEMP    (HEATER_0_MAXTEMP - HOTEND_OVERSHOOT)
    #define MIN_E_TEMP    0
  #endif

  #if HAS_HEATED_BED
    #define MAX_BED_TEMP  (BED_MAXTEMP - BED_OVERSHOOT)
    #define MIN_BED_TEMP  0
  #endif

  #define KEY_WIDTH 26
  #define KEY_HEIGHT 30
  #define KEY_INSET 5
  #define KEY_PADDING 3
  #define KEY_Y_START DWIN_HEIGHT-(KEY_HEIGHT*4+2*(KEY_INSET+1))

  #define FEEDRATE_UNIT 1
  #define ACCELERATION_UNIT 1
  #define JERK_UNIT 10
  #define STEPS_UNIT 100

  /**
 * Custom menu items with jyersLCD
 */
#if ENABLED(CUSTOM_MENU_CONFIG)
  #ifdef CONFIG_MENU_ITEM_5_DESC
    #define CUSTOM_MENU_COUNT 5
  #elif defined(CONFIG_MENU_ITEM_4_DESC)
    #define CUSTOM_MENU_COUNT 4
  #elif defined(CONFIG_MENU_ITEM_3_DESC)
    #define CUSTOM_MENU_COUNT 3
  #elif defined(CONFIG_MENU_ITEM_2_DESC)
    #define CUSTOM_MENU_COUNT 2
  #elif defined(CONFIG_MENU_ITEM_1_DESC)
    #define CUSTOM_MENU_COUNT 1
  #endif
  #if CUSTOM_MENU_COUNT
    #define HAS_CUSTOM_MENU 1
  #endif
#endif


  #define MBASE(L) (49 + MLINE * (L))
  
  /**
  constexpr float default_max_feedrate[]        = DEFAULT_MAX_FEEDRATE;
  constexpr float default_max_acceleration[]    = DEFAULT_MAX_ACCELERATION;
  constexpr float default_steps[]               = DEFAULT_AXIS_STEPS_PER_UNIT;
  #if HAS_CLASSIC_JERK
    constexpr float default_max_jerk[]            = { DEFAULT_XJERK, DEFAULT_YJERK, DEFAULT_ZJERK, DEFAULT_EJERK };
  #endif
  */

  #if HAS_JUNCTION_DEVIATION
    #define MIN_JD_MM  0.01
    #define MAX_JD_MM  0.99
  #endif


  enum SelectItem : uint8_t {
    PAGE_PRINT = 0,
    PAGE_PREPARE,
    PAGE_CONTROL,
    PAGE_INFO_LEVELING,
    #if HAS_SHORTCUTS
      PAGE_SHORTCUT0,
      PAGE_SHORTCUT1,
    #endif
    PAGE_COUNT,

    PRINT_SETUP = 0,
    PRINT_PAUSE_RESUME,
    PRINT_STOP,
    PRINT_COUNT
  };

  //uint8_t scrollpos = 0;
  //uint8_t active_menu = MainMenu, last_menu = MainMenu;
  //uint8_t temp_val.selection = 0, last_selection = 0, last_pos_selection = 0;
  //uint8_t process = Main, last_process = Main;
  //PopupID popup, last_popup;
  #if HAS_HOSTACTION_MENUS
    //bool keyboard_restrict, reset_keyboard, numeric_keyboard = false;
    //uint8_t maxstringlen;
    char *stringpointer = nullptr;
  #endif

  void (*funcpointer)() = nullptr;
  void *valuepointer = nullptr;
  //float tempvalue;
  //float valuemin;
  //float valuemax;
  //uint16_t valueunit;
  //uint8_t valuetype;

  char cmd[MAX_CMD_SIZE+32], str_1[16], str_2[16], str_3[16];
  char statusmsg[128];
  char filename[LONG_FILENAME_LENGTH];
  char Hostfilename[LONG_FILENAME_LENGTH];
  char reprint_filename[13];

  /**
  #if HAS_FILAMENT_SENSOR
    bool State_runoutenable = false;
    uint8_t temp_val.rsensormode = 0;
  #endif
  */

  #if ENABLED(HOST_ACTION_COMMANDS, HAS_HOSTACTION_MENUS)
    char action1[9];
    char action2[9];
    char action3[9];
  #endif

  /**
  #if ENABLED(BAUD_RATE_GCODE)
    uint8_t BAUD_PORT = 0;
  #endif
  */

  #if HAS_GCODE_PREVIEW
    //bool file_preview = false;
    char header1[40], header2[40], header3[40];
    #if !HAS_LIGHT_GCODE_PREVIEW
      std::map<string, int> image_cache;
      uint16_t next_available_address = 1;
      static millis_t thumbtime = 0;
      static millis_t name_scroll_time = 0;
      #define SCROLL_WAIT 1000
      uint16_t file_preview_image_address = 1;
      uint16_t header_time_s = 0;
    #endif
  #endif

  #if HAS_LEVELING
    static bool level_state;
  #endif

  /**
  #if ENABLED(HAS_BED_PROBE, AUTO_BED_LEVELING_UBL) && !HAS_NO_AUTOTILT_AFTERNPRINT
    uint16_t NPrinted = 0;
  #endif

  #if ALL(SDSUPPORT, SDCARD_SORT_ALPHA, SDSORT_GCODE)
    bool old_sdsort;
    bool SDremoved = false;
  #endif

  #if ENABLED(DWIN_CREALITY_LCD_CUSTOM_ICONS)
    uint8_t iconmain = 0;
  #endif
  */
  
  //struct
  temp_val_t temp_val;
  HMI_datas_t HMI_datas;
  CrealityDWINClass CrealityDWIN;

  template <unsigned N, unsigned S = N>
  class TextScroller {
    public:
      static const unsigned SIZE = N;
      static const unsigned SPACE = S;
      typedef char Buffer[SIZE + 1];

      inline TextScroller()
        : scrollpos(0)
      { }

      inline void reset() {
        scrollpos = 0;
      }

      const char* scroll(size_t& pos, Buffer &buf, const char * text, bool *updated = nullptr) {
        const size_t len = strlen(text);
        if (len > SIZE) {
          if (updated) *updated = true;
          if (scrollpos >= len + SPACE) scrollpos = 0;
          pos = 0;
          if (scrollpos < len) {
            const size_t n = min(len - scrollpos, SIZE);
            memcpy(buf, text + scrollpos, n);
            pos += n;
          }
          if (pos < SIZE) {
            const size_t n = min(len + SPACE - scrollpos, SIZE - pos);
            memset(buf + pos, ' ', n);
            pos += n;
          }
          if (pos < SIZE) {
            const size_t n = SIZE - pos;
            memcpy(buf + pos, text, n);
            pos += n;
          }
          buf[pos] = '\0';
          ++scrollpos;
          return buf;
        } else {
          pos = len;
          return text;
        }
      }

    private:
      uint16_t scrollpos;
  };


  #if EXTJYERSUI && HAS_MESH
    uint8_t CrealityDWINClass::_GridxGrid;
  #endif

  #if HAS_BED_PROBE
    #define PROBE_X_MIN _MAX(0 + temp_val.corner_pos, X_MIN_POS + probe.offset.x, X_MIN_POS + PROBING_MARGIN) - probe.offset.x
    #define PROBE_X_MAX _MIN((X_BED_SIZE) - temp_val.corner_pos, X_MAX_POS + probe.offset.x, X_MAX_POS - PROBING_MARGIN) - probe.offset.x
    #define PROBE_Y_MIN _MAX(0 + temp_val.corner_pos, Y_MIN_POS + probe.offset.y, Y_MIN_POS + PROBING_MARGIN) - probe.offset.y
    #define PROBE_Y_MAX _MIN((Y_BED_SIZE) - temp_val.corner_pos, Y_MAX_POS + probe.offset.y, Y_MAX_POS - PROBING_MARGIN) - probe.offset.y
    #define TRAM_POINT_XY_P { { PROBE_X_MIN, PROBE_Y_MIN }, { PROBE_X_MIN, PROBE_Y_MAX }, { PROBE_X_MAX, PROBE_Y_MAX }, { PROBE_X_MAX, PROBE_Y_MIN }, { (X_BED_SIZE) / 2.0f - probe.offset.x, (Y_BED_SIZE) / 2.0f - probe.offset.y } }
    static xy_pos_t tram_points_P[] = TRAM_POINT_XY_P;
  #endif
  #define TRAM_POINT_XY { { temp_val.corner_pos, temp_val.corner_pos }, { temp_val.corner_pos, (Y_BED_SIZE) - temp_val.corner_pos }, { (X_BED_SIZE) - temp_val.corner_pos, (Y_BED_SIZE) - temp_val.corner_pos }, { (X_BED_SIZE) - temp_val.corner_pos, temp_val.corner_pos }, { (X_BED_SIZE) / 2.0f, (Y_BED_SIZE) / 2.0f} }
  static xy_pos_t tram_points[] = TRAM_POINT_XY;

  
  constexpr const char * const CrealityDWINClass::color_names[16];
  constexpr const char * const CrealityDWINClass::preheat_modes[3];
  constexpr const char * const CrealityDWINClass::zoffset_modes[3];
  constexpr const char * const CrealityDWINClass::num_modes[3];

  #if !HAS_RC_CPU
    constexpr const char * const CrealityDWINClass::num_mult[3];
  #endif

  #if HAS_FILAMENT_SENSOR
    constexpr const char * const CrealityDWINClass::runoutsensor_modes[4];
  #endif

  #if HAS_SHORTCUTS
    constexpr const char * const CrealityDWINClass::shortcut_list[NB_Shortcuts + 1];
    constexpr const char * const CrealityDWINClass::_shortcut_list[NB_Shortcuts + 1];
  #endif

  #if HAS_LEVELING && HAS_LEVELING_HEAT
    constexpr const char * const CrealityDWINClass::LevelingTemp_modes[4];
  #endif

  #if HAS_HOTEND && BOTH(MPC_AUTOTUNE, MPC_AUTOTUNE_MENU)
    #if ENABLED(MPC_EDIT_MENU)
      constexpr const char * const CrealityDWINClass::Filament_heat_capacity[4];
    #endif
      constexpr const char * const CrealityDWINClass::MPC_target[2];
    MPC_t &mpc = thermalManager.temp_hotend[0].mpc;
  #endif

  #if ENABLED(DWIN_CREALITY_LCD_CUSTOM_ICONS)
    constexpr const char * const CrealityDWINClass::icon_main[4];
  #endif

  #if EXTJYERSUI && HAS_MESH
    #if ENABLED(AUTO_BED_LEVELING_BILINEAR)
      constexpr uint8_t NB_Grids = GRID_LIMIT - 2;
      constexpr const char * const CrealityDWINClass::GridxGrid[NB_Grids];
    #elif ENABLED(AUTO_BED_LEVELING_UBL)
      #if HAS_BED_PROBE
        constexpr uint8_t NB_Grids = GRID_LIMIT - 6;
      #else
        constexpr uint8_t NB_Grids = GRID_LIMIT - 2;
      #endif
      constexpr const char * const CrealityDWINClass::GridxGrid[NB_Grids];
    #endif
  #endif

  /* General Display Functions */

  // Clear a part of the screen
  //  4=Entire screen
  //  3=Title bar and Menu area (default)
  //  2=Menu area
  //  1=Title bar
  void CrealityDWINClass::Clear_Screen(uint8_t e/*=3*/) {
    const uint16_t color_background = TERN(HAS_TJC_DISPLAY, Color_Bg_Black, GetColor(HMI_datas.background, Color_Bg_Black));
    if (e == 1 || e == 3 || e == 4) DWIN_Draw_Rectangle(1, GetColor(HMI_datas.menu_top_bg, Color_Bg_Blue, false), 0, 0, DWIN_WIDTH, TITLE_HEIGHT); // Clear Title Bar
    if (e == 2 || e == 3) DWIN_Draw_Rectangle(1, color_background, 0, 31, DWIN_WIDTH, STATUS_Y); // Clear Menu Area
    if (e == 4) DWIN_Draw_Rectangle(1, color_background, 0, 31, DWIN_WIDTH, DWIN_HEIGHT); // Clear Popup Area
  }


  void CrealityDWINClass::Draw_Float(float value, uint8_t row, bool selected/*=false*/, uint16_t minunit/*=10*/) {
    if ((abs(value) < 10) && minunit == 10000)  temp_val.flag_Dfloat = true;
    else  temp_val.flag_Dfloat = false;
    const uint16_t color_background = TERN(HAS_TJC_DISPLAY, Color_Bg_Black, GetColor(HMI_datas.background, Color_Bg_Black));
    const uint8_t digits = temp_val.flag_Dfloat ? 6 : (uint8_t)floor(log10(abs(value))) + log10(minunit) + (minunit > 1) ; 
    const uint16_t bColor = (selected) ? GetColor(HMI_datas.select_bg, (HMI_datas.pack_red) ? Red_Select_Color : Select_Color) : color_background;
    const uint16_t xpos = 240 - (digits * 8);
    // modif 192 -> 184
    DWIN_Draw_Rectangle(1, color_background, 184, MBASE(row), xpos - 8, MBASE(row) + 16);
    if (isnan(value))
      DWIN_Draw_String(true, DWIN_FONT_MENU, GetColor(HMI_datas.items_menu_text, Color_White), bColor, xpos - 8, MBASE(row), F(" NaN"));
    else {
      DWIN_Draw_FloatValue(true, true, 0, DWIN_FONT_MENU, Color_White, bColor, temp_val.flag_Dfloat ? 3 : digits - log10(minunit) + 1, log10(minunit), xpos, MBASE(row), (value < 0 ? -value : value));
      DWIN_Draw_String(true, DWIN_FONT_MENU, GetColor(HMI_datas.select_txt, Color_White), bColor, xpos - 8, MBASE(row), value < 0 ? F("-") : F(" "));
    }
  }

  void CrealityDWINClass::Draw_Option(uint8_t value, const char * const * options, uint8_t row, bool selected/*=false*/, bool color/*=false*/) {
    const uint16_t color_background = TERN(HAS_TJC_DISPLAY, Color_Bg_Black, GetColor(HMI_datas.background, Color_Bg_Black));
    uint16_t sColor = GetColor(HMI_datas.select_txt, Color_White),
             bColor = (selected) ? GetColor(HMI_datas.select_bg, (HMI_datas.pack_red) ? Red_Select_Color : Select_Color): color_background,
             tColor = (color) ? GetColor(value, sColor, false) : GetColor(HMI_datas.items_menu_text, Color_White);
    DWIN_Draw_Rectangle(1, bColor, 202, MBASE(row) + 14, 258, MBASE(row) - 2);
    DWIN_Draw_String(false, DWIN_FONT_MENU, ((tColor == color_background) || ((tColor == Color_Black) && (HMI_datas.background == 0))) ? GetColor(HMI_datas.items_menu_text, Color_White) : tColor, bColor, 202, MBASE(row) - 1, options[value]);
  }

  #if HAS_HOSTACTION_MENUS
    void CrealityDWINClass::Draw_String(char * string, uint8_t row, bool selected/*=false*/, bool below/*=false*/) {
      const uint16_t color_background = TERN(HAS_TJC_DISPLAY, Color_Bg_Black, GetColor(HMI_datas.background, Color_Bg_Black));
      if (!string) string[0] = '\0';
      const uint8_t offset_x = DWIN_WIDTH-strlen(string)*8 - 20;
      const uint8_t offset_y = (below) ? MENU_CHR_H * 3 / 5 : 0;
      DWIN_Draw_Rectangle(1, color_background, offset_x - 10, MBASE(row)+offset_y-1, offset_x, MBASE(row)+16+offset_y);
      DWIN_Draw_String(true, DWIN_FONT_MENU, GetColor(HMI_datas.items_menu_text, Color_White), (selected) ? ((HMI_datas.pack_red) ? Red_Select_Color : Select_Color) : color_background, offset_x, MBASE(row)-1+offset_y, string);
    }

    const uint64_t CrealityDWINClass::Encode_String(const char * string) {
      const char table[65] = "-0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ_abcdefghijklmnopqrstuvwxyz";
      uint64_t output = 0;
      LOOP_L_N(i, strlen(string)) {
        uint8_t upper_bound = 63, lower_bound = 0;
        uint8_t midpoint;
        LOOP_L_N(x, 6) {
          midpoint = (uint8_t)(0.5*(upper_bound+lower_bound));
          if (string[i] > table[midpoint]) lower_bound = midpoint;
          else if (string[i] < table[midpoint]) upper_bound = midpoint;
          else break;
        }
        output += midpoint*pow(64,i);
      }
      return output;
    }

    void CrealityDWINClass::Decode_String(uint64_t num, char * string) {
      const char table[65] = "-0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ_abcdefghijklmnopqrstuvwxyz";
      LOOP_L_N(i, 30) {
        string[i] = table[num%64];
        num /= 64;
        if (num==0) {
          string[i+1] = '\0';
          break;
        }
      }
    }
  #endif

  #if HAS_SHORTCUTS
    void CrealityDWINClass::Apply_shortcut(uint8_t shortcut) {
      switch (shortcut) {
        case Preheat_menu: temp_val.flag_shortcut = true; Draw_Menu(Preheat); break;
        case Cooldown: thermalManager.cooldown(); break;
        case Disable_stepper: queue.inject(F("M84")); break;
        case Autohome: Popup_Handler(Home); gcode.home_all_axes(true); Draw_Main_Menu(temp_val._selection); break;
        #if HAS_ZOFFSET_ITEM
          case ZOffsetmenu:
            temp_val.flag_shortcut = true;
            ZOffset_Init();
            break;
        #endif
        case M_Tramming_menu:
          temp_val.flag_shortcut = true;
          ManualLevel_Init();
          break;
        #if ENABLED(ADVANCED_PAUSE_FEATURE)
          case Change_Fil:
            #if ENABLED(FILAMENT_LOAD_UNLOAD_GCODES)
              temp_val.flag_shortcut = true;
              Draw_Menu(ChangeFilament);
            #else
              Draw_Menu(Prepare, PREPARE_CHANGEFIL);
            #endif
          break;
        #endif
        case Move_rel_Z:
          DWIN_Move_Z();
          break;
        #if HAS_SCREENLOCK
          case ScreenL:
            DWIN_ScreenLock();
            break;
        #endif
        case Save_set:
          AudioFeedback(settings.save());
        break;
        default : break;
      }
    }


    void CrealityDWINClass::Set_Current_Shortcuts() {
      temp_val.Available_Shortcut_Count = Get_Available_Shortcut_Count();

      if (temp_val.Available_Shortcut_Count > 0) {
        uint8_t valid_shortcut_index = 0;

        LOOP_L_N(i, NB_Shortcuts) {
           if (temp_val.shortcut[i] > 0) {
            valid_shortcut_index++;

            if (valid_shortcut_index == 1) {
              temp_val.current_shortcut_0 = temp_val.shortcut[i];
              if (temp_val.Available_Shortcut_Count == 1) break;
            } else {
              temp_val.current_shortcut_1 = temp_val.shortcut[i];
              break;
            }
          }
        }
      }
    }

    uint8_t CrealityDWINClass::Get_Available_Shortcut_Count() {
      uint8_t shortcuts_count = 0;
      LOOP_L_N(i, NB_Shortcuts) { if (temp_val.shortcut[i] > 0) shortcuts_count++; }
      return shortcuts_count;
    }

    uint8_t CrealityDWINClass::Get_Available_Shortcut(uint8_t index) {
      uint8_t shortcut = temp_val.shortcut[index];
      uint8_t valid_shortcut_index = 0;

        LOOP_L_N(i, NB_Shortcuts) { 
        if (temp_val.shortcut[i] > 0) {   
          valid_shortcut_index++;

          if (valid_shortcut_index == (index + 1)) {
            shortcut = temp_val.shortcut[i]; 
            break;
          }
        }
      }
      return shortcut;
    }

    uint8_t CrealityDWINClass::Get_Shortcut_Icon(uint8_t shortcut) {
      switch (shortcut) {

        case Preheat_menu:  return ICON_Temperature; break;
        case Cooldown: return ICON_Cool; break;
        case Disable_stepper: return ICON_CloseMotor; break;
        case Autohome: return ICON_SetHome; break;
        case ZOffsetmenu: return ICON_Zoffset; break;
        case M_Tramming_menu: return ICON_PrintSize; break;
        #if ENABLED(ADVANCED_PAUSE_FEATURE)
          case Change_Fil: return ICON_ResumeEEPROM; break;
        #endif
        case Move_rel_Z: return ICON_MoveZ; break;
        #if HAS_SCREENLOCK
          case ScreenL: return ICON_Lock; break;
        #endif
        case Save_set: return ICON_WriteEEPROM; break;
        default : break;
      }
      return ICON_Info;
    }
  #endif

  uint16_t CrealityDWINClass::GetColor(uint8_t color, uint16_t original, bool light/*=false*/) {
    switch (color) {
      case Default:
        return original;
        break;
      case White:
        return (light) ? Color_Light_White : Color_White;
        break;
      case Light_White:
        return Color_Light_White;
        break;
      case Green:
        return (light) ? Color_Light_Green : Color_Green;
        break;
      case Light_Green:
        return Color_Light_Green;
        break;
      case Blue:
        return (light) ? Color_Light_Blue : Color_Blue;
        break;
      case Magenta:
        return (light) ? Color_Light_Magenta : Color_Magenta;
        break;
      case Light_Magenta:
        return Color_Light_Magenta;
        break;
      case Red:
        return (light) ? Color_Light_Red : Color_Red;
        break;
      case Light_Red:
        return Color_Light_Red;
        break;
      case Orange:
        return (light) ? Color_Light_Orange : Color_Orange;
        break;
      case Yellow:
        return (light) ? Color_Light_Yellow : Color_Yellow;
        break;
      case Brown:
        return (light) ? Color_Light_Brown : Color_Brown;
        break;
      case Cyan:
        return (light) ? Color_Light_Cyan : Color_Cyan;
        break;
      case Light_Cyan:
        return Color_Light_Cyan;
        break;
      case Black:
        return Color_Black;
        break;           
    }
    return Color_White;
  }

  void CrealityDWINClass::Draw_Title(const char * ctitle) {
    DWIN_Draw_String(false, DWIN_FONT_HEAD, GetColor(HMI_datas.menu_top_txt, Color_White, false), Color_Bg_Blue, (DWIN_WIDTH - strlen(ctitle) * STAT_CHR_W) / 2, 5, ctitle);
  }
  void CrealityDWINClass::Draw_Title(FSTR_P const ftitle) {
    DWIN_Draw_String(false, DWIN_FONT_HEAD, GetColor(HMI_datas.menu_top_txt, Color_White, false), Color_Bg_Blue, (DWIN_WIDTH - strlen_P(FTOP(ftitle)) * STAT_CHR_W) / 2, 5, ftitle);
  }

  void _Decorate_Menu_Item(uint8_t row, uint8_t icon, bool more) {
    if (icon) DRAW_IconWTB(ICON, icon, 26, MBASE(row) - 3);   //Draw Menu Icon
    if (more) DRAW_IconWTB(ICON, ICON_More, 226, MBASE(row) - 3); // Draw More Arrow
    DWIN_Draw_Line(CrealityDWIN.GetColor(HMI_datas.menu_split_line, Line_Color, true), 16, MBASE(row) + 33, 256, MBASE(row) + 33); // Draw Menu Line
  }

  uint16_t image_address;
  void CrealityDWINClass::Draw_Menu_Item(uint16_t row, uint8_t icon/*=0*/, const char * label1, const char * label2, bool more/*=false*/, bool centered/*=false*/, bool onlyCachedFileIcon/*=false*/) {
    const uint8_t label_offset_y = (label1 && label2) ? MENU_CHR_H * 3 / 5 : 0,
                  label1_offset_x = !centered ? LBLX : LBLX * 4/5 + _MAX(LBLX * 1U/5, (DWIN_WIDTH - LBLX - (label1 ? strlen(label1) : 0) * MENU_CHR_W) / 2),
                  label2_offset_x = !centered ? LBLX : LBLX * 4/5 + _MAX(LBLX * 1U/5, (DWIN_WIDTH - LBLX - (label2 ? strlen(label2) : 0) * MENU_CHR_W) / 2);
    if (label1) DWIN_Draw_String(false, DWIN_FONT_MENU, GetColor(HMI_datas.items_menu_text, Color_White), TERN(HAS_TJC_DISPLAY, Color_Bg_Black, GetColor(HMI_datas.background, Color_Bg_Black)), label1_offset_x, MBASE(row) - 1 - label_offset_y, label1); // Draw Label
    if (label2) DWIN_Draw_String(false, DWIN_FONT_MENU, GetColor(HMI_datas.items_menu_text, Color_White), TERN(HAS_TJC_DISPLAY, Color_Bg_Black, GetColor(HMI_datas.background, Color_Bg_Black)), label2_offset_x, MBASE(row) - 1 + label_offset_y, label2); // Draw Label

    #if HAS_GCODE_PREVIEW && HAS_NORMAL_GCODE_PREVIEW
      if ((HMI_datas.show_gcode_thumbnails) && (temp_val.sd_item_flag) && (icon == ICON_File) && find_and_decode_gcode_preview(card.filename, Thumnail_Icon, &image_address, onlyCachedFileIcon))
        DWIN_SRAM_Memory_Icon_Display(9, MBASE(row) - 18, image_address);
      else 
        if (icon) DRAW_IconWTB(ICON, icon, 26, MBASE(row) - 3);   //Draw Menu Icon
    #else
      if (icon) DRAW_IconWTB(ICON, icon, 26, MBASE(row) - 3);   //Draw Menu Icon
    #endif
    if (more) DRAW_IconWTB(ICON, ICON_More, 226, MBASE(row) - 3); // Draw More Arrow
    DWIN_Draw_Line(GetColor(HMI_datas.menu_split_line, Line_Color, true), 16, MBASE(row) + 33, 256, MBASE(row) + 33); // Draw Menu Line
    }

  void CrealityDWINClass::Draw_Menu_Item(uint8_t row, uint8_t icon/*=0*/, FSTR_P const flabel1, FSTR_P const flabel2, bool more/*=false*/, bool centered/*=false*/, bool onlyCachedFileIcon/*=false*/) {
    const uint8_t label_offset_y = (flabel1 && flabel2) ? MENU_CHR_H * 3 / 5 : 0,
                  label1_offset_x = !centered ? LBLX : LBLX * 4/5 + _MAX(LBLX * 1U/5, (DWIN_WIDTH - LBLX - (flabel1 ? strlen_P(FTOP(flabel1)) : 0) * MENU_CHR_W) / 2),
                  label2_offset_x = !centered ? LBLX : LBLX * 4/5 + _MAX(LBLX * 1U/5, (DWIN_WIDTH - LBLX - (flabel2 ? strlen_P(FTOP(flabel2)) : 0) * MENU_CHR_W) / 2);
    if (flabel1) DWIN_Draw_String(false, DWIN_FONT_MENU, GetColor(HMI_datas.items_menu_text, Color_White), TERN(HAS_TJC_DISPLAY, Color_Bg_Black, GetColor(HMI_datas.background, Color_Bg_Black)), label1_offset_x, MBASE(row) - 1 - label_offset_y, flabel1); // Draw Label
    if (flabel2) DWIN_Draw_String(false, DWIN_FONT_MENU, GetColor(HMI_datas.items_menu_text, Color_White), TERN(HAS_TJC_DISPLAY, Color_Bg_Black, GetColor(HMI_datas.background, Color_Bg_Black)), label2_offset_x, MBASE(row) - 1 + label_offset_y, flabel2); // Draw Label

    #if HAS_GCODE_PREVIEW && HAS_NORMAL_GCODE_PREVIEW
      if ((HMI_datas.show_gcode_thumbnails) && (temp_val.sd_item_flag) && (icon == ICON_File) && find_and_decode_gcode_preview(card.filename, Thumnail_Icon, &image_address, onlyCachedFileIcon))
      DWIN_SRAM_Memory_Icon_Display(9, MBASE(row) - 18, image_address);
    else 
      if (icon) DRAW_IconWTB(ICON, icon, 26, MBASE(row) - 3);   //Draw Menu Icon
    #else
      if (icon) DRAW_IconWTB(ICON, icon, 26, MBASE(row) - 3);   //Draw Menu Icon
    #endif
    if (more) DRAW_IconWTB(ICON, ICON_More, 226, MBASE(row) - 3); // Draw More Arrow
    DWIN_Draw_Line(GetColor(HMI_datas.menu_split_line, Line_Color, true), 16, MBASE(row) + 33, 256, MBASE(row) + 33); // Draw Menu Line
  }

  void CrealityDWINClass::Draw_Checkbox(uint8_t row, bool value) {
    #if ENABLED(DWIN_CREALITY_LCD_CUSTOM_ICONS) // Draw appropriate checkbox icon
      DWIN_Draw_Rectangle(1, TERN(HAS_TJC_DISPLAY, Color_Bg_Black, GetColor(HMI_datas.background, Color_Bg_Black)), 226, MBASE(row) - 3, 226 + 40, MBASE(row) - 3 + 20);
      DRAW_IconWTB(ICON, (value ? ICON_Checkbox_T : ICON_Checkbox_F), 226, MBASE(row) - 3);
      // Draw a basic checkbox using rectangles and lines
    #elif EITHER(DACAI_DISPLAY, HAS_TJC_DISPLAY)
        DWIN_Draw_Rectangle(1, Color_Bg_Black, 226, MBASE(row), 226 + 17, MBASE(row) + 17);
        DWIN_Draw_Rectangle(0, Color_White, 226, MBASE(row), 226 + 17, MBASE(row) + 17);
        DWIN_Draw_String(true, DWIN_FONT_MENU, (HMI_datas.pack_red) ? Color_Red : Check_Color, Color_Bg_Black, 226 + 2, MBASE(row) - 1, value ? F("x") : F(" "));
    #else
        DWIN_Draw_Rectangle(1, Color_Bg_Black, 226, MBASE(row) - 3, 226 + 20, MBASE(row) - 3 + 20);
        DWIN_Draw_Rectangle(0, Color_White, 226, MBASE(row) - 3, 226 + 20, MBASE(row) - 3 + 20);
        if (value) {
          DWIN_Draw_Line((HMI_datas.pack_red) ? Color_Red : Check_Color, 227, MBASE(row) - 3 + 11, 226 + 8, MBASE(row) - 3 + 17);
          DWIN_Draw_Line((HMI_datas.pack_red) ? Color_Red : Check_Color, 227 + 8, MBASE(row) - 3 + 17, 226 + 19, MBASE(row) - 3 + 1);
          DWIN_Draw_Line((HMI_datas.pack_red) ? Color_Red : Check_Color, 227, MBASE(row) - 3 + 12, 226 + 8, MBASE(row) - 3 + 18);
          DWIN_Draw_Line((HMI_datas.pack_red) ? Color_Red : Check_Color, 227 + 8, MBASE(row) - 3 + 18, 226 + 19, MBASE(row) - 3 + 2);
          DWIN_Draw_Line((HMI_datas.pack_red) ? Color_Red : Check_Color, 227, MBASE(row) - 3 + 13, 226 + 8, MBASE(row) - 3 + 19);
          DWIN_Draw_Line((HMI_datas.pack_red) ? Color_Red : Check_Color, 227 + 8, MBASE(row) - 3 + 19, 226 + 19, MBASE(row) - 3 + 3);
        }
    #endif
  }

  void CrealityDWINClass::Draw_Menu(uint8_t menu, uint8_t select/*=0*/, uint8_t scroll/*=0*/) {
    uint16_t cColor = GetColor(HMI_datas.cursor_color, Rectangle_Color);
    if (temp_val.active_menu != menu) {
      temp_val.last_menu = temp_val.active_menu;
      if (temp_val.process == Menu) temp_val.last_selection = temp_val.selection;
    }
    temp_val.selection = _MIN(select, Get_Menu_Size(menu));
    temp_val.scrollpos = scroll;
    if (temp_val.selection - temp_val.scrollpos > MROWS)
      temp_val.scrollpos = temp_val.selection - MROWS;
    temp_val.process = Menu;
    temp_val.active_menu = menu;
    Clear_Screen();
    Draw_Title(Get_Menu_Title(menu));
    LOOP_L_N(i, TROWS) Menu_Item_Handler(menu, i + temp_val.scrollpos);
    if ((cColor == TERN(HAS_TJC_DISPLAY, Color_Bg_Black, GetColor(HMI_datas.background, Color_Bg_Black))) || ((cColor == Color_Black) && (HMI_datas.background == 0)))
        DWIN_Draw_Rectangle(0, GetColor(HMI_datas.items_menu_text, Color_White), 0, MBASE(temp_val.selection - temp_val.scrollpos) - 18, 14, MBASE(temp_val.selection - temp_val.scrollpos) + 31);
    else
        DWIN_Draw_Rectangle(1, cColor, 0, MBASE(temp_val.selection - temp_val.scrollpos) - 18, 14, MBASE(temp_val.selection - temp_val.scrollpos) + 31);
  }

  void CrealityDWINClass::Redraw_Menu(bool lastprocess/*=true*/, bool lastselection/*=false*/, bool lastmenu/*=false*/, bool flag_scroll/*=false*/) {
    switch ((lastprocess) ? temp_val.last_process : temp_val.process) {
      case Menu:
        if (temp_val.flag_selection) { temp_val.last_selection = temp_val.last_pos_selection; temp_val.flag_selection = false; }
        Draw_Menu((lastmenu) ? temp_val.last_menu : temp_val.active_menu, (lastselection) ? temp_val.last_selection : temp_val.selection, (flag_scroll) ? 0 : temp_val.scrollpos);
        break;
      case Main:  Draw_Main_Menu((lastselection) ? temp_val.last_selection : temp_val.selection); break;
      case Print:
        if (temp_val.flag_tune_submenu) { Draw_Menu(temp_val.active_menu); break; }
        else if (temp_val.flag_tune_menu) { Draw_Menu(Tune); break; }
             else Draw_Print_Screen(); 
        break;
      case File:
        #if ALL(SDSUPPORT, SDCARD_SORT_ALPHA, SDSORT_GCODE)
          DWIN_Sort_SD(card.isMounted());
        #endif
        Draw_SD_List(false, (lastselection) ? temp_val.last_selection : 0, (lastselection) ? temp_val.scrollpos : 0, true);
        break;
      default: break;
    }
  }

  void CrealityDWINClass::Redraw_Screen() {
    if (printingIsActive()) Draw_Print_Screen();
    else Redraw_Menu(false);
    Draw_Status_Area(true);
    Update_Status_Bar(true);
    #if HAS_FAN
      temp_val.force_restore_fan = false;
    #endif
  }

  #if !HAS_RC_CPU
    void CrealityDWINClass::Draw_Quick_Home() {
      #if HAS_SHORTCUTS
        if (temp_val.flag_shortcut) {
          temp_val.flag_shortcut = false;
          Init_Main();
          }

      #endif
      temp_val.flag_QuickHome = false;
      if (temp_val.flag_tune_submenu) {
        Return_to_tune();
        //Redraw_Menu(false, true, true); 
      } 
      else Draw_Main_Menu();
    }

    void CrealityDWINClass::Get_fact_multiplicator(multID select) {
      switch (select) {
        case MULT_HOTEND: temp_val.factmultip = POW(10, temp_val.hotendmult); break;
        case MULT_MPC: temp_val.factmultip = POW(10, temp_val.mpcmult); break;
        case MULT_FILTYPE: temp_val.factmultip = POW(10, temp_val.fil_typemult); break;
        case MULT_BED: temp_val.factmultip = POW(10, temp_val.bedmult); break;
        case MULT_MAXACC: temp_val.factmultip = POW(10, temp_val.maxaccmult); break;
        case MULT_JERK: temp_val.factmultip = POW(10, temp_val.jerkmult); break;
        case MULT_STEPS: temp_val.factmultip = POW(10, temp_val.stepsmult); break;
        case MULT_AUTOTEMP: temp_val.factmultip = POW(10, temp_val.autotempmult); break;
      }

    }
  #endif

  void CrealityDWINClass::Keep_selection() { 
      temp_val.flag_selection = true;
      temp_val.last_pos_selection = temp_val.selection;
    }
  
  void CrealityDWINClass::Return_to_tune() {
    temp_val.flag_tune_submenu = false;
    Draw_Menu(Tune, temp_val.tune_item);
  }

  /* Primary Menus and Screen Elements */

  void CrealityDWINClass::Main_Menu_Icons() {
    uint16_t Ystart0                = 130;
    uint16_t Yend0                  = 229;
    uint16_t Ystart1                = 246;
    uint16_t Yend1                  = 345;

    uint16_t xEnd_0                  = 126;
    uint16_t xStart_Icon_0           = 62;
    uint16_t xStart_Shortcut_Text_0  = 17;

    #if HAS_SHORTCUTS
      temp_val.Available_Shortcut_Count = Get_Available_Shortcut_Count();
    #endif

    DRAW_IconWTB(TERN(DWIN_CREALITY_LCD_CUSTOM_ICONS, ((temp_val.iconmain * 2) + 1), ICON), ICON_LOGO, 71, 
      #if HAS_SHORTCUTS
        ((temp_val.shortcutsmode) && (temp_val.Available_Shortcut_Count > 0))? TERN(AQUILA, 34, 41) : TERN(AQUILA, 65, 72)
      #else
        TERN(AQUILA, 65, 72)
      #endif
      );

    #if HAS_SHORTCUTS
        if (temp_val.Available_Shortcut_Count > 0) {
          Ystart0 = temp_val.shortcutsmode ? 68 : 130;
          Yend0   = temp_val.shortcutsmode ? 167 : 229;
          Ystart1 = temp_val.shortcutsmode ? 184 : 246;
          Yend1   = temp_val.shortcutsmode ? 283 : 345;
    
          if (temp_val.Available_Shortcut_Count == 1) {
            xEnd_0                 = 254;
            xStart_Icon_0          = 126;
            xStart_Shortcut_Text_0 = 84;
          }
      }
    #endif

    uint8_t icon = TERN(DWIN_CREALITY_LCD_CUSTOM_ICONS, ((temp_val.iconmain * 2) + 1), ICON);
    uint16_t main_color_box = GetColor(HMI_datas.highlight_box, Color_White);
    uint16_t main_color_text = GetColor(HMI_datas.icons_menu_text, Color_White);

    if (temp_val.selection == 0) {
      DRAW_IconWB(icon, ICON_Print_1, 17, Ystart0);
      DWIN_Draw_Rectangle(0, main_color_box, 17, Ystart0, 126, Yend0);
    }
    else {
      DRAW_IconWB(icon, ICON_Print_0, 17, Ystart0);
    }
    DWIN_Draw_String(false, DWIN_FONT_MENU, main_color_text, Color_Bg_Blue, 52, Ystart0 + 70, GET_TEXT_F(MSG_BUTTON_PRINT));
    if (temp_val.selection == 1) {
      DRAW_IconWB(icon, ICON_Prepare_1, 145, Ystart0);
      DWIN_Draw_Rectangle(0, main_color_box, 145, Ystart0, 254, Yend0);
    }
    else {
      DRAW_IconWB(icon, ICON_Prepare_0, 145, Ystart0);
    }
    DWIN_Draw_String(false, DWIN_FONT_MENU, main_color_text, Color_Bg_Blue, 170,  Ystart0 + 70, GET_TEXT_F(MSG_PREPARE));
    if (temp_val.selection == 2) {
      DRAW_IconWB(icon, ICON_Control_1, 17, Ystart1);
      DWIN_Draw_Rectangle(0, main_color_box, 17, Ystart1, 126, Yend1);
    }
    else {
      DRAW_IconWB(icon, ICON_Control_0, 17, Ystart1);
    }
    DWIN_Draw_String(false, DWIN_FONT_MENU, main_color_text, Color_Bg_Blue, 43, Ystart1 + 70, GET_TEXT_F(MSG_CONTROL));
    #if HAS_ABL_OR_UBL
      if (temp_val.selection == 3) {
        DRAW_IconWB(icon, ICON_Leveling_1, 145, Ystart1);
        DWIN_Draw_Rectangle(0, main_color_box, 145, Ystart1, 254, Yend1);
      }
      else {
        DRAW_IconWB(icon, ICON_Leveling_0, 145, Ystart1);
      }
      DWIN_Draw_String(false, DWIN_FONT_MENU, main_color_text, Color_Bg_Blue, 179, Ystart1 + 70, GET_TEXT_F(MSG_BUTTON_LEVEL));
    #else
      if (temp_val.selection == 3) {
        DRAW_IconWB(icon, ICON_Info_1, 145, Ystart1);
        DWIN_Draw_Rectangle(0, main_color_box, 145, Ystart1, 254, Yend1);
      }
      else {
        DRAW_IconWB(icon, ICON_Info_0, 145, Ystart1);
      }
      DWIN_Draw_String(false, DWIN_FONT_MENU, main_color_text, Color_Bg_Blue, 181, Ystart1 + 70, GET_TEXT_F(MSG_BUTTON_INFO));
    #endif
    #if HAS_SHORTCUTS
      if (temp_val.shortcutsmode) {
        if (temp_val.Available_Shortcut_Count > 0) {
          if ((temp_val.selection >= 4) && (temp_val.activ_shortcut == 4)) {
            #if ENABLED(DWIN_CREALITY_LCD_CUSTOM_ICONS)
              DWIN_Draw_Rectangle(1, ((temp_val.iconmain == 0) && ENABLED(AQUILA)) ? Color_Red_Shortcut_1 : Color_Shortcut_1, 17, 300, xEnd_0, 347);
            #else
              DWIN_Draw_Rectangle(1, Color_Shortcut_1, 17, 300, xEnd_0, 347);
            #endif
            DWIN_Draw_Rectangle(0, main_color_box, 17, 300, xEnd_0, 347);
          }
          else {
            #if ENABLED(DWIN_CREALITY_LCD_CUSTOM_ICONS)
              DWIN_Draw_Rectangle(1, ((temp_val.iconmain == 0) && ENABLED(AQUILA)) ? Color_Red_Shortcut_0 : Color_Shortcut_0, 17, 300, xEnd_0, 347);
            #else
              DWIN_Draw_Rectangle(1, Color_Shortcut_0, 17, 300, xEnd_0, 347);
            #endif
          }
          DRAW_IconWTB(ICON, Get_Shortcut_Icon(temp_val.current_shortcut_0), xStart_Icon_0, 304);
          DWIN_Draw_String(false, DWIN_FONT_MENU, main_color_text, Color_Bg_Blue, xStart_Shortcut_Text_0 + ((109 - strlen(_shortcut_list[temp_val.current_shortcut_0]) * MENU_CHR_W) / 2), 327, F(_shortcut_list[temp_val.current_shortcut_0]));
        }
        if (Get_Available_Shortcut_Count() > 1) {
          if ((temp_val.selection >= 4) && (temp_val.activ_shortcut == 5)) {
            #if ENABLED(DWIN_CREALITY_LCD_CUSTOM_ICONS)
              DWIN_Draw_Rectangle(1, ((temp_val.iconmain == 0) && ENABLED(AQUILA)) ? Color_Red_Shortcut_1 : Color_Shortcut_1, 145, 300, 254, 347);
            #else
              DWIN_Draw_Rectangle(1, Color_Shortcut_1, 145, 300, 254, 347);
            #endif
            DWIN_Draw_Rectangle(0, main_color_box, 145, 300, 254, 347);
          }
          else {
            #if ENABLED(DWIN_CREALITY_LCD_CUSTOM_ICONS)
              DWIN_Draw_Rectangle(1, ((temp_val.iconmain == 0) && ENABLED(AQUILA)) ? Color_Red_Shortcut_0 : Color_Shortcut_0, 145, 300, 254, 347);
            #else
              DWIN_Draw_Rectangle(1, Color_Shortcut_0, 145, 300, 254, 347);
            #endif
          }
          DRAW_IconWTB(ICON, Get_Shortcut_Icon(temp_val.current_shortcut_1), 190, 304);
          DWIN_Draw_String(false, DWIN_FONT_MENU, main_color_text, Color_Bg_Blue, 145 + ((109 - strlen(_shortcut_list[temp_val.current_shortcut_1]) * MENU_CHR_W) / 2), 327, F(_shortcut_list[temp_val.current_shortcut_1]));
        }
      }
    #endif
    DWIN_UpdateLCD();
    delay(40);
    
  }


  void CrealityDWINClass::Draw_Main_Menu(uint8_t select/*=0*/) {
    temp_val.process = Main;
    temp_val.active_menu = MainMenu;
    temp_val.selection = select;
    Clear_Screen();
    Draw_Title(Get_Menu_Title(MainMenu));
    SERIAL_ECHOPGM("\nDWIN handshake ");
    Main_Menu_Icons();
  }

  void CrealityDWINClass::Print_Screen_Icons() {
    uint8_t icon = TERN(DWIN_CREALITY_LCD_CUSTOM_ICONS, ((temp_val.iconmain * 2) + 1), ICON);
    uint16_t print_color_text = GetColor(HMI_datas.icons_menu_text, Color_White);
    uint16_t print_color_box = GetColor(HMI_datas.highlight_box, Color_White);

    if (temp_val.selection == 0) {
      DRAW_IconWB(icon, ICON_Setup_1, 8, 252);
      DWIN_Draw_Rectangle(0, print_color_box, 8, 252, 87, 351);
    }
    else {
      DRAW_IconWB(icon, ICON_Setup_0, 8, 252);
    }
    DWIN_Draw_String(false, DWIN_FONT_MENU, print_color_text, Color_Bg_Blue, 30, 322, GET_TEXT_F(MSG_TUNE));
    if (temp_val.selection == 2) {
      DRAW_IconWB(icon, ICON_Stop_1, 184, 252);
      DWIN_Draw_Rectangle(0, print_color_box, 184, 252, 263, 351);
    }
    else {
      DRAW_IconWB(icon, ICON_Stop_0, 184, 252);
    }
    DWIN_Draw_String(false, DWIN_FONT_MENU, print_color_text, Color_Bg_Blue, 205, 322, GET_TEXT_F(MSG_BUTTON_STOP));
    if (temp_val.paused) {
      if (temp_val.selection == 1) {
        DRAW_IconWB(icon, ICON_Continue_1, 96, 252);
        DWIN_Draw_Rectangle(0, print_color_box, 96, 252, 175, 351);
      }
      else {
        DRAW_IconWB(icon, ICON_Continue_0, 96, 252);
      }
      DWIN_Draw_String(false, DWIN_FONT_MENU, print_color_text, Color_Bg_Blue, 114, 322, GET_TEXT_F(MSG_BUTTON_PRINT));
    }
    else {
      if (temp_val.selection == 1) {
        DRAW_IconWB(icon, ICON_Pause_1, 96, 252);
        DWIN_Draw_Rectangle(0, print_color_box, 96, 252, 175, 351);
      }
      else {
        DRAW_IconWB(icon, ICON_Pause_0, 96, 252);
      }
      DWIN_Draw_String(false, DWIN_FONT_MENU, print_color_text, Color_Bg_Blue, 114, 322, GET_TEXT_F(MSG_BUTTON_PAUSE));
    }
  }

  void CrealityDWINClass::Draw_Print_Screen() {
    temp_val.process = Print;
    temp_val.selection = 0;
    const uint16_t color_background = TERN(HAS_TJC_DISPLAY, Color_Bg_Black, GetColor(HMI_datas.background, Color_Bg_Black));
    const uint16_t color_print_screen_text = GetColor(HMI_datas.print_screen_txt, Color_White);
    Clear_Screen();
    DWIN_Draw_Rectangle(1, color_background, 8, 352, DWIN_WIDTH - 8, 376);
    Draw_Title(GET_TEXT(MSG_PRINTING));
    Print_Screen_Icons();
    DRAW_IconWTB(ICON, ICON_PrintTime, 14, 171);
    DRAW_IconWTB(ICON, ICON_RemainTime, 147, 169);
    DWIN_Draw_String(false, DWIN_FONT_MENU, color_print_screen_text, color_background, 41, 163, GET_TEXT_F(MSG_ELAPSED_TIME));
    DWIN_Draw_String(false, DWIN_FONT_MENU, color_print_screen_text, color_background, 176, 163, GET_TEXT_F(MSG_REMAINING_TIME));
    Update_Status_Bar(true);
    Draw_Print_ProgressBar();
    Draw_Print_ProgressElapsed();
    TERN_(SET_REMAINING_TIME, Draw_Print_ProgressRemain());
    Draw_Print_Filename(true);
  }

  void CrealityDWINClass::Draw_Print_Filename(const bool reset/*=false*/) {
    typedef TextScroller<STATUS_CHAR_LIMIT> Scroller;
    static Scroller scroller;
    if (reset) scroller.reset();
    if (temp_val.process == Print) {
      Scroller::Buffer buf;
      size_t outlen = 0;
      const uint16_t color_background = TERN(HAS_TJC_DISPLAY, Color_Bg_Black, GetColor(HMI_datas.background, Color_Bg_Black));
      const char* outstr = scroller.scroll(outlen, buf, filename);
      DWIN_Draw_Rectangle(1, color_background, 8, 50, DWIN_WIDTH - 8, 80);
      const int8_t npos = (DWIN_WIDTH - outlen * MENU_CHR_W) / 2;
      DWIN_Draw_String(false, DWIN_FONT_MENU, GetColor(HMI_datas.print_filename, Color_White), color_background, npos, 60, outstr);
    }
  }

  void CrealityDWINClass::Draw_Print_ProgressBar() {
    uint8_t printpercent = temp_val.sdprint ? card.percentDone() : (ui._get_progress() / 100);
    const uint16_t color_background = TERN(HAS_TJC_DISPLAY, Color_Bg_Black, GetColor(HMI_datas.background, Color_Bg_Black));
    const uint16_t color_percent = GetColor(HMI_datas.progress_percent, Percent_Color);
    DRAW_IconWB(ICON, ICON_Bar, 15, 93);
    DWIN_Draw_Rectangle(1, GetColor(HMI_datas.progress_bar, BarFill_Color), 16 + printpercent * 240 / 100, 93, 256, 113);
    DWIN_Draw_IntValue(true, true, 0, DWIN_FONT_MENU, color_percent, color_background, 3, 109, 133, printpercent);
    DWIN_Draw_String(false, DWIN_FONT_MENU, color_percent, color_background, 133, 133, F("%"));
  }

  #if ENABLED(SET_REMAINING_TIME)

    void CrealityDWINClass::Draw_Print_ProgressRemain() {
      uint16_t remainingtime = ui.get_remaining_time();
      const uint16_t color_background = TERN(HAS_TJC_DISPLAY, Color_Bg_Black, GetColor(HMI_datas.background, Color_Bg_Black));
      const uint16_t color_remain_time = GetColor(HMI_datas.remain_time, Color_White);
      DWIN_Draw_IntValue(true, true, 1, DWIN_FONT_MENU, color_remain_time, color_background, 2, 176, 187, remainingtime / 3600);
      DWIN_Draw_IntValue(true, true, 1, DWIN_FONT_MENU, color_remain_time, color_background, 2, 200, 187, (remainingtime % 3600) / 60);
      if (HMI_datas.time_format_textual) {
        DWIN_Draw_String(false, DWIN_FONT_MENU, color_remain_time, color_background, 192, 187, GET_TEXT_F(MSG_SHORT_HOUR));
        DWIN_Draw_String(false, DWIN_FONT_MENU, color_remain_time, color_background, 216, 187, GET_TEXT_F(MSG_SHORT_MINUTE));
      }
      else
        DWIN_Draw_String(false, DWIN_FONT_MENU, GetColor(HMI_datas.remain_time, Color_White), color_background, 192, 187, F(":"));
    }

  #endif

  void CrealityDWINClass::Draw_Print_ProgressElapsed() {
    duration_t elapsed = print_job_timer.duration();
    const uint16_t color_background = TERN(HAS_TJC_DISPLAY, Color_Bg_Black, GetColor(HMI_datas.background, Color_Bg_Black));
    const uint16_t color_elapsed_time = GetColor(HMI_datas.elapsed_time, Color_White);
    DWIN_Draw_IntValue(true, true, 1, DWIN_FONT_MENU, color_elapsed_time, color_background, 2, 42, 187, elapsed.value / 3600);
    DWIN_Draw_IntValue(true, true, 1, DWIN_FONT_MENU, color_elapsed_time, color_background, 2, 66, 187, (elapsed.value % 3600) / 60);
    if (HMI_datas.time_format_textual) {
      DWIN_Draw_String(false, DWIN_FONT_MENU, color_elapsed_time, color_background, 58, 187, GET_TEXT_F(MSG_SHORT_HOUR));
      DWIN_Draw_String(false, DWIN_FONT_MENU, color_elapsed_time, color_background, 82, 187, GET_TEXT_F(MSG_SHORT_MINUTE));
    }
    else
      DWIN_Draw_String(false, DWIN_FONT_MENU, color_elapsed_time, color_background, 58, 187, F(":"));
  }

  void CrealityDWINClass::Draw_Print_confirm() {
    #if HAS_GCODE_PREVIEW
      if (!temp_val.file_preview) Draw_Print_Screen(); 
      else {Clear_Screen(); Draw_Title(GET_TEXT(MSG_PRINTING));}
    #else
      Draw_Print_Screen();
    #endif
    temp_val.process = Confirm;
    temp_val.popup = Complete;
    temp_val.flag_stop_print = true;
    const uint16_t color_background = TERN(HAS_TJC_DISPLAY, Color_Bg_Black, GetColor(HMI_datas.background, Color_Bg_Black));
    const uint16_t color_popup_highlight = GetColor(HMI_datas.popup_highlight, Color_White);
    ui.refresh_backlight_timeout();
    DWIN_Draw_Rectangle(1, color_background, 8, 252, 263, 351);
    #if HAS_GCODE_PREVIEW
          if (temp_val.file_preview) {
            DWIN_Draw_Rectangle(1, color_background, 45, 75, 231, 261);
            DWIN_Draw_Rectangle(0, GetColor(HMI_datas.highlight_box, Color_White), 45, 75, 231, 261);
            DWIN_SRAM_Memory_Icon_Display(48 ,78 , 
              #if HAS_LIGHT_GCODE_PREVIEW
                0x0000
              #else
                file_preview_image_address
              #endif
              );
            Update_Status(cmd);
          }
    #endif
    DWIN_Draw_Rectangle(1, GetColor(HMI_datas.ico_confirm_bg , (HMI_datas.pack_red)? Color_Red : Confirm_Color), 87, 288, 186, 335);
    DWIN_Draw_String(false, DWIN_FONT_HEAD, GetColor(HMI_datas.ico_confirm_txt, Color_White), GetColor(HMI_datas.ico_confirm_bg, (HMI_datas.pack_red)? Color_Red : Confirm_Color), 87 + ((99 - 7 * STAT_CHR_W) / 2), 304, GET_TEXT_F(MSG_BUTTON_CONFIRM));
    
    DWIN_Draw_Rectangle(0, color_popup_highlight, 86, 287, 187, 336);
    DWIN_Draw_Rectangle(0, color_popup_highlight, 85, 286, 188, 337);
  }

  void CrealityDWINClass::Draw_SD_Item(uint8_t item, uint8_t row, bool onlyCachedFileIcon/*=false*/) {
    if (item == 0)
      Draw_Menu_Item(0, ICON_Back, card.flag.workDirIsRoot ? GET_TEXT_F(MSG_BACK) : F(".."));
    else {
      card.getfilename_sorted(SD_ORDER(item - 1, card.get_num_Files()));
      char * const filename = card.longest_filename();
      size_t max = MENU_CHAR_LIMIT;
      size_t pos = strlen(filename), len = pos;
      if (!card.flag.filenameIsDir)
        while (pos && filename[pos] != '.') pos--;
      len = pos;
      if (len > max) len = max;
      char name[len + 1];
      LOOP_L_N(i, len) name[i] = filename[i];
      if (pos > max)
        LOOP_S_L_N(i, len - 3, len) name[i] = '.';
      name[len] = '\0';
      Draw_Menu_Item(row, card.flag.filenameIsDir ? ICON_More : ICON_File, name, NULL, NULL, false, onlyCachedFileIcon);
    }
  }

  void CrealityDWINClass::Draw_SD_List(bool removed/*=false*/, uint8_t select/*=0*/, uint8_t scroll/*=0*/, bool onlyCachedFileIcon/*=false*/) {
    uint16_t cColor = GetColor(HMI_datas.cursor_color, Rectangle_Color);
    Clear_Screen();
    Draw_Title(GET_TEXT(MSG_FILE_SELECTION));
    temp_val.selection = min((int)select, card.get_num_Files() + 1);
    temp_val.scrollpos = scroll;
    if (temp_val.selection - temp_val.scrollpos > MROWS)
      temp_val.scrollpos = temp_val.selection - MROWS;
    temp_val.process = File;
    if (card.isMounted() && !removed) {
      temp_val.sd_item_flag = true;
      LOOP_L_N(i, _MIN(card.get_num_Files() + 1, TROWS))
        Draw_SD_Item(i + temp_val.scrollpos, i, onlyCachedFileIcon);
    }
    else {
      Draw_Menu_Item(0, ICON_Back, GET_TEXT_F(MSG_BACK));
      temp_val.sd_item_flag = false;
      #if ALL(SDSUPPORT, SDCARD_SORT_ALPHA, SDSORT_GCODE)
        temp_val.SDremoved = true;
      #endif
      DWIN_Draw_Rectangle(1, Color_Bg_Red, 10, MBASE(3) - 10, DWIN_WIDTH - 10, MBASE(4));
      DWIN_Draw_String(false, font16x32, Color_Yellow, Color_Bg_Red, ((DWIN_WIDTH) - 8 * 16) / 2, MBASE(3), GET_TEXT_F(MSG_NO_MEDIA));
    }
    if ((cColor == TERN(HAS_TJC_DISPLAY, Color_Bg_Black, GetColor(HMI_datas.background, Color_Bg_Black))) || ((cColor == Color_Black) && (HMI_datas.background == 0)))
        DWIN_Draw_Rectangle(0, GetColor(HMI_datas.items_menu_text, Color_White), 0, MBASE(temp_val.selection-temp_val.scrollpos) - 18, 8, MBASE(temp_val.selection-temp_val.scrollpos) + 31);
    else
        DWIN_Draw_Rectangle(1, cColor, 0, MBASE(temp_val.selection-temp_val.scrollpos) - 18, 8, MBASE(temp_val.selection-temp_val.scrollpos) + 31);
  }

  #if ALL(SDSUPPORT, SDCARD_SORT_ALPHA, SDSORT_GCODE)
    void CrealityDWINClass::DWIN_Sort_SD(bool isSDMounted/*=false*/) {
      if (isSDMounted) {
        if ((temp_val.old_sdsort != HMI_datas.sdsort_alpha) || (temp_val.SDremoved) || (!card.flag.workDirIsRoot)) {
          temp_val.SDremoved = false;
          temp_val.old_sdsort = HMI_datas.sdsort_alpha;
          card.setSortOn(true);  // To force to clear the RAM!
          card.setSortOn(HMI_datas.sdsort_alpha); 
          }
      }   
    }
  #endif
  
  void CrealityDWINClass::Draw_Status_Area(bool icons/*=false*/) {
    const uint16_t color_background = TERN(HAS_TJC_DISPLAY, Color_Bg_Black, GetColor(HMI_datas.background, Color_Bg_Black));
    const uint16_t color_status_area_text = GetColor(HMI_datas.status_area_text, Color_White);
    const uint16_t color_status_area_percent = GetColor(HMI_datas.status_area_percent, Color_White);
    const uint16_t color_coordinates_text = GetColor(HMI_datas.coordinates_text, Color_White);

    if (icons) DWIN_Draw_Rectangle(1, color_background, 0, STATUS_Y, DWIN_WIDTH, DWIN_HEIGHT - 1);

    #if HAS_HOTEND
      static float hotend = -1;
      static int16_t hotendtarget = -1, _flow = -1;
      if (icons) {
        hotend = -1;
        hotendtarget = -1;
        DRAW_IconWTB(ICON, ICON_HotendTemp, 10, 383);
        DWIN_Draw_String(false, DWIN_FONT_STAT, color_status_area_text, color_background, 25 + 3 * STAT_CHR_W + 5, 384, F("/"));
      }
      if (thermalManager.temp_hotend[0].celsius != hotend) {
        hotend = thermalManager.temp_hotend[0].celsius;
        DWIN_Draw_IntValue(true, true, 0, DWIN_FONT_STAT, color_status_area_text, color_background, 3, 28, 384, thermalManager.temp_hotend[0].celsius);
        DWIN_Draw_DegreeSymbol(color_status_area_text, 25 + 3 * STAT_CHR_W + 5, 386);
      }
      if (thermalManager.temp_hotend[0].target != hotendtarget) {
        hotendtarget = thermalManager.temp_hotend[0].target;
        DWIN_Draw_IntValue(true, true, 0, DWIN_FONT_STAT, color_status_area_percent, color_background, 3, 25 + 4 * STAT_CHR_W + 6, 384, thermalManager.temp_hotend[0].target);
        DWIN_Draw_DegreeSymbol(color_status_area_percent, 25 + 4 * STAT_CHR_W + 39, 386);
      }
      if (icons) {
        _flow = -1;
        DRAW_IconWTB(ICON, ICON_StepE, 112, 417);
        DWIN_Draw_String(false, DWIN_FONT_STAT, color_status_area_percent, color_background, 116 + 5 * STAT_CHR_W + 2, 417, F("%"));
      }
      if (planner.flow_percentage[0] != _flow) {
        _flow = planner.flow_percentage[0];
        DWIN_Draw_IntValue(true, true, 0, DWIN_FONT_STAT, color_status_area_percent, color_background, 3, 116 + 2 * STAT_CHR_W, 417, planner.flow_percentage[0]);
      }
    #endif

    #if HAS_HEATED_BED
      static float bed = -1;
      static int16_t bedtarget = -1;
      if (icons) {
        bed = -1;
        bedtarget = -1;
        DRAW_IconWTB(ICON, ICON_BedTemp, 10, 416);
        DWIN_Draw_String(false, DWIN_FONT_STAT, color_status_area_text, color_background, 25 + 3 * STAT_CHR_W + 5, 417, F("/"));
      }
      if (thermalManager.temp_bed.celsius != bed) {
        bed = thermalManager.temp_bed.celsius;
        DWIN_Draw_IntValue(true, true, 0, DWIN_FONT_STAT, color_status_area_text, color_background, 3, 28, 417, thermalManager.temp_bed.celsius);
        DWIN_Draw_DegreeSymbol(color_status_area_text, 25 + 3 * STAT_CHR_W + 5, 419);
      }
      if (thermalManager.temp_bed.target != bedtarget) {
        bedtarget = thermalManager.temp_bed.target;
        DWIN_Draw_IntValue(true, true, 0, DWIN_FONT_STAT, color_status_area_percent, color_background, 3, 25 + 4 * STAT_CHR_W + 6, 417, thermalManager.temp_bed.target);
        DWIN_Draw_DegreeSymbol(color_status_area_percent, 25 + 4 * STAT_CHR_W + 39, 419);
      }
    #endif

    #if HAS_FAN
      static uint8_t fan = -1;
      if (icons) {
        fan = -1;
        DRAW_IconWTB(ICON, ICON_FanSpeed, 187, 383);
        DWIN_Draw_String((HMI_datas.fan_percent) ? false : true, DWIN_FONT_STAT, color_status_area_text, color_background, 195 + 5 * STAT_CHR_W + 2, 384, (HMI_datas.fan_percent) ? F("%") : F(" "));
      }
      if ((thermalManager.fan_speed[0] != fan) || (TERN0(HAS_FAN, temp_val.force_restore_fan))) {
        fan = thermalManager.fan_speed[0];
        DWIN_Draw_IntValue(true, true, 0, DWIN_FONT_STAT, color_status_area_text, color_background, 3, 195 + 2 * STAT_CHR_W, 384, Set_fan_speed_value()) ;
      
      }
    #endif

    #if HAS_ZOFFSET_ITEM
      static float offset = -1;

      if (icons) {
        offset = -1;
        DRAW_IconWTB(ICON, ICON_Zoffset, 187, 416);
      }
      if (temp_val.zoffsetvalue != offset) {
        offset = temp_val.zoffsetvalue;
        TERN_(HAS_TJC_DISPLAY, DWIN_Draw_Rectangle(1, color_background, 205, 417, 247, 437));
        DWIN_Draw_FloatValue(true, true, 0, DWIN_FONT_STAT, color_status_area_text, color_background, 2, 2, 207, 417, (temp_val.zoffsetvalue < 0 ? -temp_val.zoffsetvalue : temp_val.zoffsetvalue));
        DWIN_Draw_String(true, DWIN_FONT_MENU, color_status_area_text, color_background, 205, 419, temp_val.zoffsetvalue < 0 ? F("-") : F(" "));
      }
      #if HAS_MESH
        static bool _leveling_active = false;
        static bool _printing_leveling_active = false;
        if (printingIsActive()) {
          TERN_(HAS_LEV_BLINKING, _printing_leveling_active = ((planner.leveling_active && planner.leveling_active_at_z(current_position.z)) || _printing_leveling_active));   
          if ((_printing_leveling_active = (planner.leveling_active && planner.leveling_active_at_z(current_position.z)) && TERN1(HAS_LEV_BLINKING, ((HMI_datas.lev_blinking && ui.get_blink()) || !HMI_datas.lev_blinking))))
            DWIN_Draw_Rectangle(0, color_status_area_text, 187, 415, 204, 435);
          else 
            DWIN_Draw_Rectangle(0, color_background, 187, 415, 204, 435);
        }
        else {
          TERN_(HAS_LEV_BLINKING, _leveling_active = (planner.leveling_active || _leveling_active));
          if ((_leveling_active = planner.leveling_active && TERN1(HAS_LEV_BLINKING, ((HMI_datas.lev_blinking && ui.get_blink()) || !HMI_datas.lev_blinking))))
            DWIN_Draw_Rectangle(0, color_status_area_text, 187, 415, 204, 435);
          else 
            DWIN_Draw_Rectangle(0, color_background, 187, 415, 204, 435);
        }  
      #endif
    #endif

    static int16_t feedrate = -1;
    if (icons) {
      feedrate = -1;
      DRAW_IconWTB(ICON, ICON_Speed, 113, 383);
      DWIN_Draw_String(false, DWIN_FONT_STAT, color_status_area_percent, color_background, 116 + 5 * STAT_CHR_W + 2, 384, F("%"));
    }
    if (feedrate_percentage != feedrate) {
      feedrate = feedrate_percentage;
      DWIN_Draw_IntValue(true, true, 0, DWIN_FONT_STAT, color_status_area_percent, color_background, 3, 116 + 2 * STAT_CHR_W, 384, feedrate_percentage);
    }

    static float x = -1, y = -1, z = -1;
    static bool update_x = false, update_y = false, update_z = false;
    #if ENABLED(SHOW_REAL_POS)
      #if ENABLED(HAS_SHOW_REAL_POS_MENU)
        float _px, _py, _pz;
        if (HMI_datas.show_real_pos) {
      #endif
        TERN(HAS_SHOW_REAL_POS_MENU,,const float) _px = stepper.position(X_AXIS) / planner.settings.axis_steps_per_mm[X_AXIS];
        TERN(HAS_SHOW_REAL_POS_MENU,,const float) _py = stepper.position(Y_AXIS) / planner.settings.axis_steps_per_mm[Y_AXIS];
	      TERN(HAS_SHOW_REAL_POS_MENU,,const float) _pz = stepper.position(Z_AXIS) / planner.settings.axis_steps_per_mm[Z_AXIS];
      #if ENABLED(HAS_SHOW_REAL_POS_MENU)
        }
        else {
          _px = current_position.x;
          _py = current_position.y;
	        _pz = current_position.z;
        }
      #endif
	  #else
      const float _px = current_position.x;
      const float _py = current_position.y;
	    const float _pz = current_position.z;
	  #endif

    update_x = (_px != x || axis_should_home(X_AXIS) || update_x);
    update_y = (_py != y || axis_should_home(Y_AXIS) || update_y);
    update_z = (_pz != z || axis_should_home(Z_AXIS) || update_z);
    if (icons) {
      x = y = z = -1;
      DWIN_Draw_Line(GetColor(HMI_datas.coordinates_split_line, Line_Color, true), 16, 450, 256, 450);
      DRAW_IconWTB(ICON, ICON_MaxSpeedX,  10, 456);
      DRAW_IconWTB(ICON, ICON_MaxSpeedY,  95, 456);
      DRAW_IconWTB(ICON, ICON_MaxSpeedZ, 180, 456);
    }
    if (update_x) {
      x = _px;
      TERN_(HAS_TJC_DISPLAY, DWIN_Draw_Rectangle(1, color_background, 35, 417, 247, 437));
      if ((update_x = axis_should_home(X_AXIS) && ui.get_blink()))
        DWIN_Draw_String(true, DWIN_FONT_MENU, color_coordinates_text, color_background, 35, 459, F("  -?-  "));
      else
        JYERSUI::Draw_Float(true, true, DWIN_FONT_MENU, color_coordinates_text, color_background, 3, 1, 35, 459, _px);
    }
    if (update_y) {
      y = _py;
      if ((update_y = axis_should_home(Y_AXIS) && ui.get_blink()))
        DWIN_Draw_String(true, DWIN_FONT_MENU, color_coordinates_text, color_background, 120, 459, F("  -?-  "));
      else
        JYERSUI::Draw_Float(true, true, DWIN_FONT_MENU, color_coordinates_text, color_background, 3, 1, 120, 459, _py);
    }
    if (update_z) {
      z = _pz;
      if ((update_z = axis_should_home(Z_AXIS) && ui.get_blink()))
        DWIN_Draw_String(true, DWIN_FONT_MENU, color_coordinates_text, color_background, 205, 459, F("  -?-  "));
      else
        #if ENABLED(SHOW_REAL_POS)
          JYERSUI::Draw_Float(true, true, DWIN_FONT_MENU, color_coordinates_text, color_background, 3, 2, 205, 459, _pz);
        #else
          TERN_(HAS_TJC_DISPLAY, DWIN_Draw_Rectangle(1, color_background, 205, 459, 253, 475));
          DWIN_Draw_FloatValue(true, true, 0, DWIN_FONT_MENU, color_coordinates_text, color_background, 3, 2, 205, 459, (_pz >= 0) ? ((DISABLED(HAS_BED_PROBE) && temp_val.printing) ? (_pz - temp_val.zoffsetvalue) : _pz) : 0);
          //DWIN_Draw_FloatValue(true, true, 0, DWIN_FONT_MENU, color_coordinates_text, color_background, 3, 2, 205, 459, (_pz >= 0) ? _pz : 0);
        #endif
    }
    DWIN_UpdateLCD();
  }

  void CrealityDWINClass::Draw_Popup(FSTR_P const line1, FSTR_P const line2, FSTR_P const line3, uint8_t mode, uint8_t icon/*=0*/) {
    if (temp_val.process != Confirm && temp_val.process != Popup && temp_val.process != Wait) temp_val.last_process = temp_val.process;
    if ((temp_val.process == Menu || temp_val.process == Wait || temp_val.process == File) && mode == Popup) temp_val.last_selection = temp_val.selection;
    temp_val.process = mode;
    Clear_Screen();
    #if HAS_GRAPHICAL_LEVELMANUAL && EXTJYERSUI
      if (mode == Editvalue_Mesh)  Draw_Title(F("Edit Value Mesh"));
    #endif
    const uint16_t color_bg = GetColor(HMI_datas.popup_bg, Color_Bg_Window);
    const uint16_t color_popup_txt = GetColor(HMI_datas.popup_txt, Popup_Text_Color);
    const uint16_t color_popup_highlight = GetColor(HMI_datas.popup_highlight, Color_White);
    DWIN_Draw_Rectangle(0, color_popup_highlight, 13, 59, 259, 351);
    DWIN_Draw_Rectangle(1, color_bg, 14, 60, 258, 350);
    const uint8_t ypos = (mode == Popup || mode == Confirm || TERN0(HAS_GRAPHICAL_LEVELMANUAL, mode == Editvalue_Mesh)) ? 180 : 230; // 150 -> 180
    const uint8_t ypos_icon = (mode == Popup || mode == Confirm || TERN0(HAS_GRAPHICAL_LEVELMANUAL, mode == Editvalue_Mesh)) ? 89 : 105; // 74 -> 89
    if (icon > 0) DRAW_IconWTB(ICON, icon, 101, ypos_icon);
    DWIN_Draw_String(true, DWIN_FONT_MENU, color_popup_txt, color_bg, (272 - 8 * strlen_P(FTOP(line1))) / 2, ypos, line1);
    DWIN_Draw_String(true, DWIN_FONT_MENU, color_popup_txt, color_bg, (272 - 8 * strlen_P(FTOP(line2))) / 2, ypos + 30, line2);
    DWIN_Draw_String(true, DWIN_FONT_MENU, color_popup_txt, color_bg, (272 - 8 * strlen_P(FTOP(line3))) / 2, ypos + 60, line3);
    if (mode == Popup) {
      temp_val.selection = 0;
      DWIN_Draw_Rectangle(1, GetColor(HMI_datas.ico_confirm_bg, (HMI_datas.pack_red)? Color_Red : Confirm_Color), 26, 280, 125, 317);
      DWIN_Draw_Rectangle(1, GetColor(HMI_datas.ico_cancel_bg , Cancel_Color), 146, 280, 245, 317);
      DWIN_Draw_String(false, DWIN_FONT_STAT, GetColor(HMI_datas.ico_confirm_txt, Color_White), GetColor(HMI_datas.ico_confirm_bg, (HMI_datas.pack_red)? Color_Red : Confirm_Color), 39, 290, GET_TEXT_F(MSG_BUTTON_CONFIRM));
      DWIN_Draw_String(false, DWIN_FONT_STAT, GetColor(HMI_datas.ico_cancel_txt, Color_White), GetColor(HMI_datas.ico_cancel_bg , Cancel_Color), 165, 290, GET_TEXT_F(MSG_BUTTON_CANCEL));
      Popup_Select();
    }
    else if ((mode == Confirm) || TERN0(HAS_GRAPHICAL_LEVELMANUAL, (mode == Editvalue_Mesh))) {
      DWIN_Draw_Rectangle(1, GetColor(HMI_datas.ico_continue_bg, (HMI_datas.pack_red)? Color_Red : Confirm_Color), 87, 280, 186, 317);
      DWIN_Draw_Rectangle(0, color_popup_highlight, 86, 279, 187, 318);
      DWIN_Draw_Rectangle(0, color_popup_highlight, 85, 278, 188, 319);
      DWIN_Draw_String(false, DWIN_FONT_STAT, GetColor(HMI_datas.ico_continue_txt, Color_White), GetColor(HMI_datas.ico_continue_bg, (HMI_datas.pack_red)? Color_Red : Confirm_Color), 
          #if EXTJYERSUI
            #if HAS_LIVEMESH
              96, 290, GET_TEXT_F(MSG_BUTTON_CONTINUE)
            #else
              (temp_val.popup == Level2) ? 107 : 96, 
              290,
              (temp_val.popup == Level2) ? GET_TEXT_F(MSG_BUTTON_CANCEL) : GET_TEXT_F(MSG_BUTTON_CONTINUE)
            #endif
          #else
            96,
            290,
            GET_TEXT_F(MSG_BUTTON_CONTINUE)
          #endif
          );
    }
  }

#if HAS_FAN
  void CrealityDWINClass::Draw_item_fan_speed(const bool draw, const uint8_t row) {
    temp_val.fan_speed = Set_fan_speed_value();
    if (draw) {
      Draw_Menu_Item(row, ICON_FanSpeed, HMI_datas.fan_percent ? F("Fan Speed %") : F("Fan Speed"));
      Draw_Float(temp_val.fan_speed, row, false, 1);
    }
    else
      Modify_Value(temp_val.fan_speed, MIN_FAN_SPEED, HMI_datas.fan_percent ? 100 : MAX_FAN_SPEED, 1);
  }

  uint8_t CrealityDWINClass::Set_fan_speed_value() {
    if (HMI_datas.fan_percent) {
      if (thermalManager.fan_speed[0] == 255) return 100;
      else return int((thermalManager.fan_speed[0] * 392) / 1000);
    }
    else return thermalManager.fan_speed[0];
  }
#endif

  #if HAS_HOTEND && HAS_E_CALIBRATION
    void CrealityDWINClass::DWIN_E_Calibration() {
      if  (!e_calibrate.done) {
        e_calibrate.initStage1();
      }
    }

    void CrealityDWINClass::DWIN_E_Calibration_Done() {
      if (e_calibrate.done) {
        e_calibrate.done = false;
        e_calibrate.Draw_Stage4();
      }
    }

    void CrealityDWINClass::HMI_E_Calibration() {
      EncoderState encoder_diffState = Encoder_ReceiveAnalyze();
      if (encoder_diffState == ENCODER_DIFF_NO) return;
      e_calibrate.onEncoderE(encoder_diffState);
      if (e_calibrate.isDone_E()) DWIN_E_Calibration_Done();
    }
  #endif

  #if HAS_SCREENLOCK
    void CrealityDWINClass::DWIN_ScreenLock() {
      temp_val.process = Locked;
      if  (!screenLock.unlocked) {
        screenLock.init();
      }
    }

    void CrealityDWINClass::DWIN_ScreenUnLock() {
      if (screenLock.unlocked) {
        screenLock.unlocked = false;
        if (!temp_val.printing) {
          #if HAS_SHORTCUTS
            Init_Main();
          #endif 
          Draw_Main_Menu(); }
        else Draw_Print_Screen();
      }
    }

    void CrealityDWINClass::HMI_ScreenLock() {
      EncoderState encoder_diffState = Encoder_ReceiveAnalyze();
      if (encoder_diffState == ENCODER_DIFF_NO) return;
      screenLock.onEncoder(encoder_diffState);
      if (screenLock.isUnlocked()) DWIN_ScreenUnLock();
    }
  #endif

  #if HAS_SHORTCUTS
    void CrealityDWINClass::DWIN_Move_Z() {
      temp_val.process = Short_cuts;
      if  (!shortcuts.quitmenu) {
        shortcuts.initZ();
      }
    }

    void CrealityDWINClass::DWIN_QuitMove_Z() {
      if (shortcuts.quitmenu) {
        shortcuts.quitmenu = false;
        queue.inject(F("M84"));
        Draw_Main_Menu(temp_val._selection);
      }
    }

    void CrealityDWINClass::HMI_Move_Z() {
      EncoderState encoder_diffState = Encoder_ReceiveAnalyze();
      if (encoder_diffState == ENCODER_DIFF_NO) return;
      shortcuts.onEncoderZ(encoder_diffState);
      if (shortcuts.isQuitedZ()) DWIN_QuitMove_Z();
    }
  #endif

  #if HAS_MESH
    void CrealityDWINClass::Viewmesh() {
      Clear_Screen(4);
      Draw_Title(GET_TEXT_F(MSG_MESH_VIEW));
      temp_val.flag_viewmesh = true;
      temp_val.last_process = temp_val.process;
      temp_val.last_selection = temp_val.selection;
      temp_val.process = Confirm;
      temp_val.popup = viewmesh;
      if (planner.leveling_active) {
        DWIN_Draw_Rectangle(1, TERN(HAS_TJC_DISPLAY, Color_Bg_Black, GetColor(HMI_datas.background, Color_Bg_Black)), 0, KEY_Y_START, DWIN_WIDTH-2, DWIN_HEIGHT-2);
        BedLevelTools.Draw_Bed_Mesh();
        BedLevelTools.Set_Mesh_Viewer_Status();
        DWIN_Draw_Rectangle(1, (HMI_datas.pack_red)? Color_Red : Confirm_Color, 87, 406, 186, 443);
        DWIN_Draw_Rectangle(0, Color_White, 86, 405, 187, 444);
        DWIN_Draw_Rectangle(0, Color_White, 85, 404, 188, 445);
        DWIN_Draw_String(false, DWIN_FONT_STAT, Color_White, Color_Bg_Window, 96, 416, GET_TEXT_F(MSG_BUTTON_CONTINUE));
      }
      else Confirm_Handler(LevelError);
    }
  
    #if HAS_LIVEMESH
      void CrealityDWINClass::Draw_LiveMesh() {
        Popup_Handler(Level2);
        safe_delay(3000);
        Clear_Screen(3);
        Draw_Title(F("View Live Mesh"));
        temp_val.last_process = Menu;
        temp_val.last_selection = temp_val.selection;
        temp_val.process = Confirm;
        temp_val.popup = Level3;
        ExtJyersui.Draw_LiveMeshGrid();
        DWIN_Draw_Rectangle(1, (HMI_datas.pack_red)? Color_Red : Confirm_Color, 87, 305, 186, 342);
        DWIN_Draw_Rectangle(0, Color_White, 86, 304, 187, 343);
        DWIN_Draw_Rectangle(0, Color_White, 85, 303, 188, 344);
        DWIN_Draw_String(false, DWIN_FONT_STAT, Color_White, Color_Bg_Window, 107, 315, GET_TEXT_F(MSG_BUTTON_CANCEL));
      }
    #endif

  #endif


  void MarlinUI::kill_screen(FSTR_P const error, FSTR_P const component) {
    CrealityDWIN.Draw_Popup(GET_TEXT_F(MSG_KILLED), error, GET_TEXT_F(MSG_SWITCH_PS_OFF), Wait, ICON_BLTouch);
  }

  void CrealityDWINClass::Popup_Select(bool scal/*=false*/) {
    const uint16_t c1 = (temp_val.selection == 0) ? GetColor(HMI_datas.popup_highlight, Color_White) : GetColor(HMI_datas.popup_bg, Color_Bg_Window),
                  c2 = (temp_val.selection == 0) ? GetColor(HMI_datas.popup_bg, Color_Bg_Window) : GetColor(HMI_datas.popup_highlight, Color_White);
    DWIN_Draw_Rectangle(0, c1, 25, scal ? 304 : 279, 126, scal ? 343 : 318);
    DWIN_Draw_Rectangle(0, c1, 24, scal ? 303 : 278, 127, scal ? 344 : 319);
    DWIN_Draw_Rectangle(0, c2, 145, scal ? 304 : 279, 246, scal ? 343 : 318);
    DWIN_Draw_Rectangle(0, c2, 144, scal ? 303 : 278, 247, scal ? 344 : 319);
  }

  void CrealityDWINClass::Update_Status_Bar(bool refresh/*=false*/) {
    //add
    typedef TextScroller<STATUS_CHAR_LIMIT> Scroller;
    static bool new_msg;

    static Scroller scroller;
    static char lastmsg[128];
    if (strcmp(lastmsg, statusmsg) != 0 || refresh) {
      strcpy(lastmsg, statusmsg);
      scroller.reset();
      new_msg = true;
    }
    //add
    Scroller::Buffer buf;
    size_t len = 0;
    const char* dispmsg = scroller.scroll(len, buf, statusmsg, &new_msg);
    if (new_msg) {
      new_msg = false;
      const int8_t npos = (DWIN_WIDTH - len * MENU_CHR_W) / 2;
      DWIN_Draw_Rectangle(1, (temp_val.process == Print) ? Color_Grey : TERN(HAS_TJC_DISPLAY, Color_Bg_Black, GetColor(HMI_datas.background, Color_Bg_Black)), 8, (temp_val.process == Print) ? 214 : 352, DWIN_WIDTH - 8, (temp_val.process == Print) ? 238 : 376);
      DWIN_Draw_String(false, DWIN_FONT_MENU, GetColor(HMI_datas.status_bar_text, Color_White), TERN(HAS_TJC_DISPLAY, Color_Bg_Black, GetColor(HMI_datas.background, Color_Bg_Black)), npos, (temp_val.process == Print) ? 219 : 357, dispmsg);
    }
  }

  #if HAS_HOSTACTION_MENUS
    void CrealityDWINClass::Draw_Keyboard(bool restrict, bool numeric, uint8_t selected, bool uppercase/*=false*/, bool lock/*=false*/) {
      temp_val.process = Keyboard;
      temp_val.keyboard_restrict = restrict;
      temp_val.numeric_keyboard = numeric;
      DWIN_Draw_Rectangle(0, Color_White, 0, KEY_Y_START, DWIN_WIDTH-2, DWIN_HEIGHT-2);
      DWIN_Draw_Rectangle(1, Color_Bg_Black, 1, KEY_Y_START+1, DWIN_WIDTH-3, DWIN_HEIGHT-3);
      LOOP_L_N(i, 36) Draw_Keys(i, (i == selected), uppercase, lock);
    }

    void CrealityDWINClass::Draw_Keys(uint8_t index, bool selected, bool uppercase/*=false*/, bool lock/*=false*/) {
      const char *keys;
      if (temp_val.numeric_keyboard) keys = "1234567890&<>() {}[]*\"\':;!?";
      else keys = (uppercase) ? "QWERTYUIOPASDFGHJKLZXCVBNM" : "qwertyuiopasdfghjklzxcvbnm";
      #define KEY_X1(x) x*KEY_WIDTH+KEY_INSET+KEY_PADDING
      #define KEY_X2(x) (x+1)*KEY_WIDTH+KEY_INSET-KEY_PADDING
      #define KEY_Y1(y) KEY_Y_START+KEY_INSET+KEY_PADDING+y*KEY_HEIGHT
      #define KEY_Y2(y) KEY_Y_START+KEY_INSET-KEY_PADDING+(y+1)*KEY_HEIGHT

      const uint8_t rowCount[3] = {10, 9, 7};
      const float xOffset[3] = {0, 0.5f*KEY_WIDTH, 1.5f*KEY_WIDTH};

      if (index < 28) {
        if (index == 19) {
          DWIN_Draw_Rectangle(0, Color_Light_Blue, KEY_X1(0), KEY_Y1(2), KEY_X2(0)+xOffset[1], KEY_Y2(2));
          DWIN_Draw_Rectangle(0, (selected) ? Select_Color : Color_Bg_Black, KEY_X1(0)+1, KEY_Y1(2)+1, KEY_X2(0)+xOffset[1]-1, KEY_Y2(2)-1);
          if (!temp_val.numeric_keyboard) {
            if (lock) {
              DWIN_Draw_Line(Select_Color, KEY_X1(0)+17, KEY_Y1(2)+16, KEY_X1(0)+25, KEY_Y1(2)+8);
              DWIN_Draw_Line(Select_Color, KEY_X1(0)+17, KEY_Y1(2)+16, KEY_X1(0)+9, KEY_Y1(2)+8);
            }
            else {
              DWIN_Draw_Line((uppercase) ? Select_Color : Color_White, KEY_X1(0)+17, KEY_Y1(2)+8, KEY_X1(0)+25, KEY_Y1(2)+16);
              DWIN_Draw_Line((uppercase) ? Select_Color : Color_White, KEY_X1(0)+17, KEY_Y1(2)+8, KEY_X1(0)+9, KEY_Y1(2)+16);
            }
          }
        }
        else if (index == 27) {
          DWIN_Draw_Rectangle(0, Color_Light_Blue, KEY_X1(7)+xOffset[2], KEY_Y1(2), KEY_X2(9), KEY_Y2(2));
          DWIN_Draw_Rectangle(0, (selected) ? Select_Color : Color_Bg_Black, KEY_X1(7)+xOffset[2]+1, KEY_Y1(2)+1, KEY_X2(9)-1, KEY_Y2(2)-1);
          DWIN_Draw_String(true, DWIN_FONT_MENU, Color_Red, Color_Bg_Black, KEY_X1(7)+xOffset[2]+3, KEY_Y1(2)+5, F("<--"));
        }
        else {
          if (index > 19) index--;
          if (index > 27) index--;
          uint8_t y, x;
          if (index < rowCount[0]) y = 0, x = index;
          else if (index < (rowCount[0]+rowCount[1])) y = 1, x = index-rowCount[0];
          else y = 2, x = index-(rowCount[0]+rowCount[1]);
          const char keyStr[2] = {keys[(y>0)*rowCount[0]+(y>1)*rowCount[1]+x], '\0'};
          DWIN_Draw_Rectangle(0, Color_Light_Blue, KEY_X1(x)+xOffset[y], KEY_Y1(y), KEY_X2(x)+xOffset[y], KEY_Y2(y));
          DWIN_Draw_Rectangle(0, (selected) ? Select_Color : Color_Bg_Black, KEY_X1(x)+xOffset[y]+1, KEY_Y1(y)+1, KEY_X2(x)+xOffset[y]-1, KEY_Y2(y)-1);
          DWIN_Draw_String(false, DWIN_FONT_MENU, Color_White, Color_Bg_Black, KEY_X1(x)+xOffset[y]+5, KEY_Y1(y)+5, keyStr);
          if (temp_val.keyboard_restrict && temp_val.numeric_keyboard && index > 9) {
            DWIN_Draw_Line(Color_Light_Red, KEY_X1(x)+xOffset[y]+1, KEY_Y1(y)+1, KEY_X2(x)+xOffset[y]-1, KEY_Y2(y)-1);
            DWIN_Draw_Line(Color_Light_Red, KEY_X1(x)+xOffset[y]+1, KEY_Y2(y)-1, KEY_X2(x)+xOffset[y]-1, KEY_Y1(y)+1);
          }
        }
      }
      else {
        switch (index) {
          case 28:
            DWIN_Draw_Rectangle(0, Color_Light_Blue, KEY_X1(0), KEY_Y1(3), KEY_X2(0)+xOffset[1], KEY_Y2(3));
            DWIN_Draw_Rectangle(0, (selected) ? Select_Color : Color_Bg_Black, KEY_X1(0)+1, KEY_Y1(3)+1, KEY_X2(0)+xOffset[1]-1, KEY_Y2(3)-1);
            DWIN_Draw_String(false, DWIN_FONT_MENU, Color_White, Color_Bg_Black, KEY_X1(0)-1, KEY_Y1(3)+5, F("?123"));
            break;
          case 29:
            DWIN_Draw_Rectangle(0, Color_Light_Blue, KEY_X1(1)+xOffset[1], KEY_Y1(3), KEY_X2(1)+xOffset[1], KEY_Y2(3));
            DWIN_Draw_Rectangle(0, (selected) ? Select_Color : Color_Bg_Black, KEY_X1(1)+xOffset[1]+1, KEY_Y1(3)+1, KEY_X2(1)+xOffset[1]-1, KEY_Y2(3)-1);
            DWIN_Draw_String(false, DWIN_FONT_MENU, Color_White, Color_Bg_Black, KEY_X1(1)+xOffset[1]+5, KEY_Y1(3)+5, F("-"));
            break;
          case 30:
            DWIN_Draw_Rectangle(0, Color_Light_Blue, KEY_X1(2)+xOffset[1], KEY_Y1(3), KEY_X2(2)+xOffset[1], KEY_Y2(3));
            DWIN_Draw_Rectangle(0, (selected) ? Select_Color : Color_Bg_Black, KEY_X1(2)+xOffset[1]+1, KEY_Y1(3)+1, KEY_X2(2)+xOffset[1]-1, KEY_Y2(3)-1);
            DWIN_Draw_String(false, DWIN_FONT_MENU, Color_White, Color_Bg_Black, KEY_X1(2)+xOffset[1]+5, KEY_Y1(3)+5, F("_"));
            break;
          case 31:
            DWIN_Draw_Rectangle(0, Color_Light_Blue, KEY_X1(3)+xOffset[1], KEY_Y1(3), KEY_X2(5)+xOffset[1], KEY_Y2(3));
            DWIN_Draw_Rectangle(0, (selected) ? Select_Color : Color_Bg_Black, KEY_X1(3)+xOffset[1]+1, KEY_Y1(3)+1, KEY_X2(5)+xOffset[1]-1, KEY_Y2(3)-1);
            DWIN_Draw_String(false, DWIN_FONT_MENU, Color_White, Color_Bg_Black, KEY_X1(3)+xOffset[1]+14, KEY_Y1(3)+5, F("Space"));
            if (temp_val.keyboard_restrict) {
              DWIN_Draw_Line(Color_Light_Red, KEY_X1(3)+xOffset[1]+1, KEY_Y1(3)+1, KEY_X2(5)+xOffset[1]-1, KEY_Y2(3)-1);
              DWIN_Draw_Line(Color_Light_Red, KEY_X1(3)+xOffset[1]+1, KEY_Y2(3)-1, KEY_X2(5)+xOffset[1]-1, KEY_Y1(3)+1);
            }
            break;
          case 32:
            DWIN_Draw_Rectangle(0, Color_Light_Blue, KEY_X1(6)+xOffset[1], KEY_Y1(3), KEY_X2(6)+xOffset[1], KEY_Y2(3));
            DWIN_Draw_Rectangle(0, (selected) ? Select_Color : Color_Bg_Black, KEY_X1(6)+xOffset[1]+1, KEY_Y1(3)+1, KEY_X2(6)+xOffset[1]-1, KEY_Y2(3)-1);
            DWIN_Draw_String(false, DWIN_FONT_MENU, Color_White, Color_Bg_Black, KEY_X1(6)+xOffset[1]+7, KEY_Y1(3)+5, F("."));
            if (temp_val.keyboard_restrict) {
              DWIN_Draw_Line(Color_Light_Red, KEY_X1(6)+xOffset[1]+1, KEY_Y1(3)+1, KEY_X2(6)+xOffset[1]-1, KEY_Y2(3)-1);
              DWIN_Draw_Line(Color_Light_Red, KEY_X1(6)+xOffset[1]+1, KEY_Y2(3)-1, KEY_X2(6)+xOffset[1]-1, KEY_Y1(3)+1);
            }
            break;
          case 33:
            DWIN_Draw_Rectangle(0, Color_Light_Blue, KEY_X1(7)+xOffset[1], KEY_Y1(3), KEY_X2(7)+xOffset[1], KEY_Y2(3));
            DWIN_Draw_Rectangle(0, (selected) ? Select_Color : Color_Bg_Black, KEY_X1(7)+xOffset[1]+1, KEY_Y1(3)+1, KEY_X2(7)+xOffset[1]-1, KEY_Y2(3)-1);
            DWIN_Draw_String(false, DWIN_FONT_MENU, Color_White, Color_Bg_Black, KEY_X1(7)+xOffset[1]+4, KEY_Y1(3)+5, F("/"));
            if (temp_val.keyboard_restrict) {
              DWIN_Draw_Line(Color_Light_Red, KEY_X1(7)+xOffset[1]+1, KEY_Y1(3)+1, KEY_X2(7)+xOffset[1]-1, KEY_Y2(3)-1);
              DWIN_Draw_Line(Color_Light_Red, KEY_X1(7)+xOffset[1]+1, KEY_Y2(3)-1, KEY_X2(7)+xOffset[1]-1, KEY_Y1(3)+1);
            }
            break;
          case 34:
            DWIN_Draw_Rectangle(0, Color_Light_Blue, KEY_X1(7)+xOffset[2], KEY_Y1(3), KEY_X2(9), KEY_Y2(3));
            DWIN_Draw_Rectangle(0, (selected) ? Select_Color : Color_Bg_Black, KEY_X1(7)+xOffset[2]+1, KEY_Y1(3)+1, KEY_X2(9)-1, KEY_Y2(3)-1);
            DWIN_Draw_String(true, DWIN_FONT_MENU, Color_Cyan, Color_Bg_Black, KEY_X1(7)+xOffset[2]+3, KEY_Y1(3)+5, F("-->"));
            break;
        }
      }
    }
  #endif


  /* Menu Item Config */

  void CrealityDWINClass::Menu_Item_Handler(uint8_t menu, uint8_t item, bool draw/*=true*/) {
    const uint8_t row = item - temp_val.scrollpos;
    string buf;

  #if HAS_PREHEAT

    #define PREHEAT_BACK 0
    #define PREHEAT_SUBMENU_HOME (PREHEAT_BACK + DISABLED(HAS_RC_CPU))
    #define PREHEAT_SUBMENU_HOTEND (PREHEAT_SUBMENU_HOME + ENABLED(HAS_HOTEND))
    #define PREHEAT_SUBMENU_BED (PREHEAT_SUBMENU_HOTEND + ENABLED(HAS_HEATED_BED))
    #define PREHEAT_SUBMENU_FAN (PREHEAT_SUBMENU_BED + ENABLED(HAS_FAN))
    #define PREHEAT_SUBMENU_TOTAL PREHEAT_SUBMENU_FAN

    auto preheat_submenu = [&](const int index, const uint8_t item, const uint8_t sel) {
      switch (item) {
        case PREHEAT_BACK:
          if (draw)
            Draw_Menu_Item(row, ICON_Back, GET_TEXT_F(MSG_BACK));
          else
            Draw_Menu(TempMenu, sel);
          break;
        #if !HAS_RC_CPU
          case PREHEAT_SUBMENU_HOME:
            if (draw) Draw_Menu_Item(row, ICON_Back, GET_TEXT_F(MSG_MAIN));
            else Draw_Quick_Home();
            break;
        #endif
        #if HAS_HOTEND        
          case PREHEAT_SUBMENU_HOTEND:
            if (draw) {
              Draw_Menu_Item(row, ICON_SetEndTemp, GET_TEXT_F(MSG_HOTEND_TEMPERATURE));
              Draw_Float(ui.material_preset[index].hotend_temp, row, false, 1);
            }
            else
              Modify_Value(ui.material_preset[index].hotend_temp, MIN_E_TEMP, MAX_E_TEMP, 1);
            break;
        #endif
        #if HAS_HEATED_BED
          case PREHEAT_SUBMENU_BED:
            if (draw) {
              Draw_Menu_Item(row, ICON_SetBedTemp, GET_TEXT_F(MSG_BED_TEMPERATURE));
              Draw_Float(ui.material_preset[index].bed_temp, row, false, 1);
            }
            else
              Modify_Value(ui.material_preset[index].bed_temp, MIN_BED_TEMP, MAX_BED_TEMP, 1);
            break;
        #endif
        #if HAS_FAN
          case PREHEAT_SUBMENU_FAN:
            if (draw) {
              Draw_Menu_Item(row, ICON_FanSpeed, GET_TEXT_F(MSG_FAN_SPEED));
              Draw_Float(ui.material_preset[index].fan_speed, row, false, 1);
            }
            else
              Modify_Value(ui.material_preset[index].fan_speed, MIN_FAN_SPEED, MAX_FAN_SPEED, 1);
            break;
        #endif
      }
    };

  #endif

    switch (menu) {
      case Prepare:

        #define PREPARE_BACK 0
        #define PREPARE_MOVE (PREPARE_BACK + 1)
        #define PREPARE_DISABLE (PREPARE_MOVE + 1)
        #define PREPARE_HOME (PREPARE_DISABLE + 1)
        #define PREPARE_MANUALLEVEL (PREPARE_HOME + 1)
        #define PREPARE_ZOFFSET (PREPARE_MANUALLEVEL + ENABLED(HAS_ZOFFSET_ITEM))
        #define PREPARE_PREHEAT (PREPARE_ZOFFSET + ENABLED(HAS_PREHEAT))
        #define PREPARE_COOLDOWN (PREPARE_PREHEAT + EITHER(HAS_HOTEND, HAS_HEATED_BED))
        #define PREPARE_CHANGEFIL (PREPARE_COOLDOWN + ENABLED(ADVANCED_PAUSE_FEATURE))
        #define PREPARE_CUSTOM_MENU (PREPARE_CHANGEFIL + ENABLED(HAS_CUSTOM_MENU))
        #define PREPARE_ACTIONCOMMANDS (PREPARE_CUSTOM_MENU + ENABLED(HOST_ACTION_COMMANDS, HAS_HOSTACTION_MENUS))
        #define PREPARE_TOTAL PREPARE_ACTIONCOMMANDS

        switch (item) {
          case PREPARE_BACK:
            if (draw)
              Draw_Menu_Item(row, ICON_Back, GET_TEXT_F(MSG_BACK));
            else
              Draw_Main_Menu(1);
            break;
          case PREPARE_MOVE:
            if (draw)
              Draw_Menu_Item(row, ICON_Axis, GET_TEXT_F(MSG_MOVE_AXIS), nullptr, true);
            else
              Draw_Menu(Move);
            break;
          case PREPARE_DISABLE:
            if (draw)
              Draw_Menu_Item(row, ICON_CloseMotor, GET_TEXT_F(MSG_DISABLE_STEPPERS));
            else
              queue.inject(F("M84"));
            break;
          case PREPARE_HOME:
            if (draw)
              Draw_Menu_Item(row, ICON_SetHome, GET_TEXT_F(MSG_HOMING), nullptr, true);
            else
              Draw_Menu(HomeMenu);
            break;
          case PREPARE_MANUALLEVEL:
            if (draw)
              Draw_Menu_Item(row, ICON_PrintSize, GET_TEXT_F(MSG_BED_TRAMMING_MANUAL), nullptr, true);
            else {
              ManualLevel_Init();
            }
            break;

          #if HAS_ZOFFSET_ITEM
            case PREPARE_ZOFFSET:
              if (draw)
                Draw_Menu_Item(row, ICON_Zoffset, GET_TEXT_F(MSG_OFFSET_Z), nullptr, true);
              else {
                ZOffset_Init();
              }
              break;
          #endif

          #if HAS_PREHEAT
            case PREPARE_PREHEAT:
              if (draw)
                Draw_Menu_Item(row, ICON_Temperature, GET_TEXT_F(MSG_PREHEAT), nullptr, true);
              else
                Draw_Menu(Preheat);
              break;
          #endif

          #if HAS_HOTEND || HAS_HEATED_BED
            case PREPARE_COOLDOWN:
              if (draw)
                Draw_Menu_Item(row, ICON_Cool, GET_TEXT_F(MSG_COOLDOWN));
              else {
                thermalManager.cooldown();
                Update_Status(GET_TEXT(MSG_COOLDOWN));
              }
              break;
          #endif

          #if HAS_CUSTOM_MENU
            case PREPARE_CUSTOM_MENU:
              #ifndef CUSTOM_MENU_CONFIG_TITLE
                #define CUSTOM_MENU_CONFIG_TITLE "Custom Commands"
              #endif
              if (draw)
                Draw_Menu_Item(row, ICON_Version, F(CUSTOM_MENU_CONFIG_TITLE));
              else
                Draw_Menu(MenuCustom);
              break;
          #endif

          #if ENABLED(HOST_ACTION_COMMANDS, HAS_HOSTACTION_MENUS)
            case PREPARE_ACTIONCOMMANDS:
            if (draw)
              Draw_Menu_Item(row, ICON_SetHome, GET_TEXT_F(MSG_HOST_ACTIONS), nullptr, true);
            else 
              Draw_Menu(HostActions);
            break;
          #endif

          #if ENABLED(ADVANCED_PAUSE_FEATURE)
            case PREPARE_CHANGEFIL:
              if (draw) {
                Draw_Menu_Item(row, ICON_ResumeEEPROM, GET_TEXT_F(MSG_FILAMENTCHANGE)
                  #if ENABLED(FILAMENT_LOAD_UNLOAD_GCODES)
                    , nullptr, true
                  #endif
                );
              }
              else {
                #if ENABLED(FILAMENT_LOAD_UNLOAD_GCODES)
                  Draw_Menu(ChangeFilament);
                #else
                  Update_Status(GET_TEXT(MSG_FILAMENT_PARK_ENABLED));
                  #if ENABLED(NOZZLE_PARK_FEATURE)
                    queue.inject(F("G28O\nG27 P2"));
                  #else
                    sprintf_P(cmd, PSTR("G28O\nG0 F4000 X%i Y%i\nG0 F3000 Z%i"), TERN(EXTJYERSUI, HMI_datas.Park_point.x, 240) , TERN(EXTJYERSUI, HMI_datas.Park_point.y, 220), TERN(EXTJYERSUI, HMI_datas.Park_point.z, 20));
                    queue.inject(cmd);
                  #endif
                  if (thermalManager.temp_hotend[0].target < thermalManager.extrude_min_temp)
                    Popup_Handler(ETemp);
                  else {
                    temp_val.flag_chg_fil = true;
                    if (thermalManager.temp_hotend[0].is_below_target(-2)) {
                      Popup_Handler(Heating);
                      Update_Status(GET_TEXT(MSG_HEATING));
                      thermalManager.wait_for_hotend(0);
                    }
                    Popup_Handler(FilChange);
                    Update_Status(GET_TEXT(MSG_FILAMENT_CHANGE_INIT));
                    sprintf_P(cmd, PSTR("M600 B1 R%i"), thermalManager.temp_hotend[0].target);
                    gcode.process_subcommands_now(cmd);
                    temp_val.flag_chg_fil = false;
                    Draw_Menu(Prepare, PREPARE_CHANGEFIL);
                  }
                #endif
              }
              break;
          #endif
        }
        break;

      case HomeMenu:

        #define HOME_BACK  0
        #define HOME_ALL   (HOME_BACK + 1)
        #define HOME_X     (HOME_ALL + 1)
        #define HOME_Y     (HOME_X + 1)
        #define HOME_Z     (HOME_Y + 1)
        #define HOME_SET   (HOME_Z + 1)
        #define HOME_TOTAL HOME_SET

        switch (item) {
          case HOME_BACK:
            if (draw)
              Draw_Menu_Item(row, ICON_Back, GET_TEXT_F(MSG_BACK));
            else
              Draw_Menu(Prepare, PREPARE_HOME);
            break;
          case HOME_ALL:
            if (draw)
              Draw_Menu_Item(row, ICON_Homing, GET_TEXT_F(MSG_AUTO_HOME));
            else {
              Popup_Handler(Home);
              gcode.home_all_axes(true);
              Redraw_Menu();
            }
            break;
          case HOME_X:
            if (draw)
              Draw_Menu_Item(row, ICON_MoveX, GET_TEXT_F(MSG_AUTO_HOME_X));
            else {
              Popup_Handler(Home);
              gcode.process_subcommands_now(F("G28 X"));
              planner.synchronize();
              Redraw_Menu();
            }
            break;
          case HOME_Y:
            if (draw)
              Draw_Menu_Item(row, ICON_MoveY, GET_TEXT_F(MSG_AUTO_HOME_Y));
            else {
              Popup_Handler(Home);
              gcode.process_subcommands_now(F("G28 Y"));
              planner.synchronize();
              Redraw_Menu();
            }
            break;
          case HOME_Z:
            if (draw)
              Draw_Menu_Item(row, ICON_MoveZ, GET_TEXT_F(MSG_AUTO_HOME_Z));
            else {
              Popup_Handler(Home);
              gcode.process_subcommands_now(F("G28 Z"));
              planner.synchronize();
              Redraw_Menu();
            }
            break;
          case HOME_SET:
            if (draw)
              Draw_Menu_Item(row, ICON_SetHome, GET_TEXT_F(MSG_SET_HOME_OFFSETS));
            else {
              gcode.process_subcommands_now(F("G92 X0 Y0 Z0"));
              AudioFeedback();
            }
            break;
        }
        break;

      case Move:

        #define MOVE_BACK 0
        #define MOVE_X (MOVE_BACK + 1)
        #define MOVE_Y (MOVE_X + 1)
        #define MOVE_Z (MOVE_Y + 1)
        #define MOVE_E (MOVE_Z + ENABLED(HAS_HOTEND))
        #define MOVE_E100 (MOVE_E + ENABLED(HAS_HOTEND, HAS_E_CALIBRATION))
        #define MOVE_P (MOVE_E100 + ENABLED(HAS_BED_PROBE))
        #define MOVE_UNIT (MOVE_P + 1)
        #define MOVE_LIVE (MOVE_UNIT + 1)
        #define MOVE_TOTAL MOVE_LIVE

        switch (item) {
          case MOVE_BACK:
            if (draw)
              Draw_Menu_Item(row, ICON_Back, GET_TEXT_F(MSG_BACK));
            else {
              temp_val.livemove = TERN(USER_MOVE_LIVE, true, false);
              #if HAS_BED_PROBE
                temp_val.probe_deployed = false;
                probe.set_deployed(temp_val.probe_deployed);
              #endif
              Draw_Menu(Prepare, PREPARE_MOVE);
            }
            break;
          case MOVE_X:
            if (draw) {
              Draw_Menu_Item(row, ICON_MoveX, GET_TEXT_F(MSG_MOVE_X));
              Draw_Float(current_position.x, row, false);
            }
            else
              Modify_Value(current_position.x, X_MIN_POS, X_MAX_POS, 10);
            break;
          case MOVE_Y:
            if (draw) {
              Draw_Menu_Item(row, ICON_MoveY, GET_TEXT_F(MSG_MOVE_Y));
              Draw_Float(current_position.y, row);
            }
            else
              Modify_Value(current_position.y, Y_MIN_POS, Y_MAX_POS, 10);
            break;
          case MOVE_Z:
            if (draw) {
              Draw_Menu_Item(row, ICON_MoveZ, GET_TEXT_F(MSG_MOVE_Z));
              Draw_Float(current_position.z, row);
            }
            else
              Modify_Value(current_position.z, Z_MIN_POS, Z_MAX_POS, 10);
            break;

          #if HAS_HOTEND
            case MOVE_E:
              if (draw) {
                Draw_Menu_Item(row, ICON_Extruder, GET_TEXT_F(MSG_MOVE_E));
                current_position.e = 0;
                sync_plan_position();
                Draw_Float(current_position.e, row);
              }
              else Check_E_Move_Mintemp();
              break;

            #if HAS_E_CALIBRATION
              case MOVE_E100:
                if (draw)
                  Draw_Menu_Item(row, ICON_Extruder, F("E-Tool Calib."), nullptr, true);
                else
                  Check_E_Move_Mintemp(true);
                break;
            #endif
          #endif // HAS_HOTEND

          #if HAS_BED_PROBE
            case MOVE_P:
              if (draw) {
                Draw_Menu_Item(row, ICON_Probe, GET_TEXT_F(MSG_MANUAL_DEPLOY));
                Draw_Checkbox(row, temp_val.probe_deployed);
              }
              else {
                temp_val.probe_deployed = !temp_val.probe_deployed;
                probe.set_deployed(temp_val.probe_deployed);
                Draw_Checkbox(row, temp_val.probe_deployed);
              }
              break;
          #endif
          case MOVE_UNIT:
            if (draw) {
              Draw_Menu_Item(row, ICON_Homing, F("Basic unit"));
              Draw_Option(temp_val.nummode, num_modes, row);
            }
            else 
              Modify_Option(temp_val.nummode, num_modes, 2);
            break;
          case MOVE_LIVE:
            if (draw) {
              Draw_Menu_Item(row, ICON_Axis, GET_TEXT_F(MSG_LIVE_ADJUSTMENT));
              Draw_Checkbox(row, temp_val.livemove);
            }
            else {
              temp_val.livemove = !temp_val.livemove;
              Draw_Checkbox(row, temp_val.livemove);
            }
            break;
        }
        break;
        
      #if HAS_HOTEND && HAS_E_CALIBRATION
        case MoveE100:

          #define MOVE_E100_BACK 0
          #define MOVE_E100_EXTR (MOVE_E100_BACK + 1)
          #define MOVE_E100_DIST (MOVE_E100_EXTR + 1)
          #define MOVE_E100_ESTEP (MOVE_E100_DIST + 1)
          #define MOVE_E100_TOTAL MOVE_E100_ESTEP

          temp_val.control_value = 20;
          current_position.e = 0;
          sync_plan_position();

          switch (item) {
            case MOVE_E100_BACK:
              if (draw)
                Draw_Menu_Item(row, ICON_Back, GET_TEXT_F(MSG_BACK));
              else
                Draw_Menu(Move, MOVE_E100);
              break;
            case MOVE_E100_EXTR:
              if (draw)
                Draw_Menu_Item(row, ICON_Extruder, F("Move Ext. 100mm"));
              else {
                temp_val.flag_wait = true;
                current_position.e += 100;
                planner.synchronize();
                planner.buffer_line(current_position, manual_feedrate_mm_s[E_AXIS], active_extruder);
                planner.synchronize();
                temp_val.flag_wait = false;
              }
              break;
            case MOVE_E100_DIST:
              if (draw) {
                Draw_Menu_Item(row, ICON_Extruder, F("Dist. measured:"));
                Draw_Float(temp_val.control_value, row, false, 10);
                }
              else {
                Modify_Value(temp_val.control_value, -100, 100, 10);
              }
              break;
            case MOVE_E100_ESTEP:
              Draw_Menu_Item(row, ICON_Extruder, F("E-steps:"));
              Draw_Float(planner.settings.axis_steps_per_mm[E_AXIS], row, false, 100);
              break;
          }
          break;
      #endif

      case ManualLevel:

        #define MLEVEL_BACK 0
        #define MLEVEL_HOME  (MLEVEL_BACK + DISABLED(HAS_RC_CPU))
        #define MLEVEL_PROBE (MLEVEL_HOME + ENABLED(HAS_BED_PROBE))
        #define MLEVEL_BL (MLEVEL_PROBE + 1)
        #define MLEVEL_TL (MLEVEL_BL + 1)
        #define MLEVEL_TR (MLEVEL_TL + 1)
        #define MLEVEL_BR (MLEVEL_TR + 1)
        #define MLEVEL_C (MLEVEL_BR + 1)
        #define MLEVEL_ZPOS (MLEVEL_C + 1)
        #define MLEVEL_CYCLES (MLEVEL_ZPOS + ENABLED(HAS_BED_PROBE)) // ims - option for number of cycles
        #define MLEVEL_TOTAL MLEVEL_CYCLES

        static float mlev_z_pos = 0;
        static bool use_probe = false;
   
        switch (item) {
          case MLEVEL_BACK:
            if (draw)
              Draw_Menu_Item(row, ICON_Back, GET_TEXT_F(MSG_BACK));
            else {
              TERN_(HAS_LEVELING, set_bed_leveling_enabled(level_state));
              TERN_(HAS_BED_PROBE, use_probe = false);
              temp_val.flag_busy = false;
              #if HAS_SHORTCUTS
                if (temp_val.flag_shortcut) { temp_val.flag_shortcut = false; Draw_Main_Menu(temp_val._selection); }
              else
              #endif 
                Draw_Menu(Prepare, PREPARE_MANUALLEVEL);
            }
            break;
          #if !HAS_RC_CPU
            case MLEVEL_HOME:
              if (draw) Draw_Menu_Item(row, ICON_Back, GET_TEXT_F(MSG_MAIN));
              else {
                temp_val.flag_busy = false;
                TERN_(HAS_BED_PROBE, use_probe = false);
                TERN_(HAS_LEVELING, set_bed_leveling_enabled(level_state));
                Draw_Quick_Home();
                }
              break;
          #endif
          #if HAS_BED_PROBE
            case MLEVEL_PROBE:
              if (draw) {
                Draw_Menu_Item(row, ICON_Zoffset, GET_TEXT_F(MSG_ZPROBE_ENABLE));
                Draw_Checkbox(row, use_probe);
              }
              else {
                use_probe = !use_probe;
                Draw_Checkbox(row, use_probe);
                if (use_probe) {
                  Keep_selection();
                  if (axes_should_home()) { Popup_Handler(Home); TERN_(HAS_BED_PROBE, do_z_clearance(Z_HOMING_HEIGHT)); gcode.home_all_axes(true); }
                  Popup_Handler(Bed_T_Wizard);
                }
              }
              break;
          #endif
          case MLEVEL_BL:
            if (draw)
              Draw_Menu_Item(row, ICON_AxisBL, GET_TEXT_F(MSG_LEVBED_FL));
            else {
              Bed_Tramming(0, mlev_z_pos, use_probe);
            }
            break;
          case MLEVEL_TL:
            if (draw)
              Draw_Menu_Item(row, ICON_AxisTL, GET_TEXT_F(MSG_LEVBED_BL));
            else {
              Bed_Tramming(1, mlev_z_pos, use_probe);
            }
            break;
          case MLEVEL_TR:
            if (draw)
              Draw_Menu_Item(row, ICON_AxisTR, GET_TEXT_F(MSG_LEVBED_BR));
            else {
              Bed_Tramming(2, mlev_z_pos, use_probe);
            }
            break;
          case MLEVEL_BR:
            if (draw)
              Draw_Menu_Item(row, ICON_AxisBR, GET_TEXT_F(MSG_LEVBED_FR));
            else {
              Bed_Tramming(3, mlev_z_pos, use_probe);
            }
            break;
          case MLEVEL_C:
            if (draw)
              Draw_Menu_Item(row, ICON_AxisC, GET_TEXT_F(MSG_LEVBED_C));
            else {
              Bed_Tramming(4, mlev_z_pos, use_probe);
            }
            break;
          case MLEVEL_ZPOS:
            if (draw) {
              Draw_Menu_Item(row, ICON_SetZOffset, GET_TEXT_F(MSG_MOVE_Z));
              Draw_Float(mlev_z_pos, row, false, 100);
            }
            else
              Modify_Value(mlev_z_pos, 0, MAX_Z_OFFSET, 100);
            break;
          #if HAS_BED_PROBE
            case MLEVEL_CYCLES: // ims
            if (draw) {
              Draw_Menu_Item(row, ICON_ResumeEEPROM, F("Nbr of cycles"));
              Draw_Float(mlev_cycles, row, false, 1);
            }
            else
              Modify_Value(mlev_cycles, 1, 50, 1);
            break;
          #endif
        }
        break;

      #if HAS_ZOFFSET_ITEM
        case ZOffset:

          #define ZOFFSET_BACK 0
          #define ZOFFSET_MAIN (ZOFFSET_BACK + DISABLED(HAS_RC_CPU))
          #define ZOFFSET_HOME (ZOFFSET_MAIN + 1)
          #define ZOFFSET_MODE (ZOFFSET_HOME + 1)
          #define ZOFFSET_OFFSET (ZOFFSET_MODE + 1)
          #define ZOFFSET_UP (ZOFFSET_OFFSET + 1)
          #define ZOFFSET_DOWN (ZOFFSET_UP + 1)
          #define ZOFFSET_SAVE (ZOFFSET_DOWN + ENABLED(EEPROM_SETTINGS))
          #define ZOFFSET_TOTAL ZOFFSET_SAVE

          switch (item) {
            case ZOFFSET_BACK:
              if (draw)
                Draw_Menu_Item(row, ICON_Back, GET_TEXT_F(MSG_BACK));
              else {
                temp_val.zoffsetmode = TERN(USER_MOVE_LIVE, 2, 0);
                #if !HAS_BED_PROBE
                  gcode.process_subcommands_now(F("M211 S1"));
                #endif
                TERN_(HAS_LEVELING, set_bed_leveling_enabled(level_state));
                #if HAS_SHORTCUTS
                  if (temp_val.flag_shortcut) { temp_val.flag_shortcut = false; Draw_Main_Menu(temp_val._selection); }
                  else 
                #endif
                  Draw_Menu(Prepare, PREPARE_ZOFFSET);
              }
              break;
            #if !HAS_RC_CPU
              case ZOFFSET_MAIN:
                if (draw) Draw_Menu_Item(row, ICON_Back, GET_TEXT_F(MSG_MAIN));
                else {
                  temp_val.zoffsetmode = TERN(USER_MOVE_LIVE, 2, 0);
                  #if !HAS_BED_PROBE
                    gcode.process_subcommands_now(F("M211 S1"));
                  #endif
                  TERN_(HAS_LEVELING, set_bed_leveling_enabled(level_state));
                  TERN_(HAS_LEVELING, Dis_steppers_and_cooldown());
                  Draw_Quick_Home(); 
                  }
                break;
            #endif
            case ZOFFSET_HOME:
              if (draw)
                Draw_Menu_Item(row, ICON_Homing, GET_TEXT_F(MSG_AUTO_HOME_Z));
              else {
                Popup_Handler(Home);
                gcode.process_subcommands_now(F("G28 Z"));
                Popup_Handler(MoveWait);
                #if ENABLED(Z_SAFE_HOMING)
                  planner.synchronize();
                  sprintf_P(cmd, PSTR("G0 F4000 X%s Y%s"), dtostrf(Z_SAFE_HOMING_X_POINT, 1, 3, str_1), dtostrf(Z_SAFE_HOMING_Y_POINT, 1, 3, str_2));
                  gcode.process_subcommands_now(cmd);
                #else
                  sprintf_P(cmd, PSTR("G0 F4000 X%s Y%s"), dtostrf((X_BED_SIZE + X_MIN_POS) / 2.0f, 1, 3, str_1), dtostrf((Y_BED_SIZE + Y_MIN_POS) / 2.0f, 1, 3, str_2));
                  gcode.process_subcommands_now(cmd);
                #endif
                gcode.process_subcommands_now(F("G0 F300 Z0"));
                planner.synchronize();
                Redraw_Menu();
              }
              break;
              case ZOFFSET_MODE:
                if (draw) {
                  Draw_Menu_Item(row, ICON_Zoffset, GET_TEXT_F(MSG_LIVE_ADJUSTMENT));
                  Draw_Option(temp_val.zoffsetmode, zoffset_modes, row);
                }
                else 
                  Modify_Option(temp_val.zoffsetmode, zoffset_modes, 2);
                break;
            case ZOFFSET_OFFSET:
              if (draw) {
                Draw_Menu_Item(row, ICON_SetZOffset, GET_TEXT_F(MSG_OFFSET_Z));
                Draw_Float(temp_val.zoffsetvalue, row, false, 100);
              }
              else {
                Modify_Value(temp_val.zoffsetvalue, MIN_Z_OFFSET, MAX_Z_OFFSET, 100);
              }
              break;
            case ZOFFSET_UP:
              if (draw) {
                sprintf_P(cmd, PSTR("%s %s"), GET_TEXT(MSG_BABYSTEP_Z), GET_TEXT(MSG_UP));
                Draw_Menu_Item(row, ICON_Axis, F(cmd));
                }
               else {
                if (temp_val.zoffsetvalue < MAX_Z_OFFSET) {
                  if (temp_val.zoffsetmode != 0) {
                    gcode.process_subcommands_now(F("M290 Z0.01"));
                    planner.synchronize();
                  }
                  temp_val.zoffsetvalue += 0.01;
                  Draw_Float(temp_val.zoffsetvalue, row - 1, false, 100);
                }
              }
              break;
            case ZOFFSET_DOWN:
              if (draw) {
                sprintf_P(cmd, PSTR("%s %s"), GET_TEXT(MSG_BABYSTEP_Z), GET_TEXT(MSG_DOWN));
                Draw_Menu_Item(row, ICON_AxisD, F(cmd));
                }
               else {
                if (temp_val.zoffsetvalue > MIN_Z_OFFSET) {
                  if (temp_val.zoffsetmode != 0) {
                    gcode.process_subcommands_now(F("M290 Z-0.01"));
                    planner.synchronize();
                  }
                  temp_val.zoffsetvalue -= 0.01;
                  Draw_Float(temp_val.zoffsetvalue, row - 2, false, 100);
                }
              }
              break;  
            #if ENABLED(EEPROM_SETTINGS)
              case ZOFFSET_SAVE:
                if (draw)
                  Draw_Menu_Item(row, ICON_WriteEEPROM, GET_TEXT_F(MSG_BUTTON_SAVE));
                else
                  AudioFeedback(settings.save());
                break;
            #endif
          }
          break;
      #endif

      #if HAS_PREHEAT
        case Preheat: {
        
          #define PREHEAT_BACK 0
          #define PREHEAT_HOME (PREHEAT_BACK + DISABLED(HAS_RC_CPU))
          #define PREHEAT_MODE (PREHEAT_HOME + 1)
          #define PREHEAT_1 (PREHEAT_MODE + 1)
          #define PREHEAT_2 (PREHEAT_1 + (PREHEAT_COUNT >= 2))
          #define PREHEAT_3 (PREHEAT_2 + (PREHEAT_COUNT >= 3))
          #define PREHEAT_4 (PREHEAT_3 + (PREHEAT_COUNT >= 4))
          #define PREHEAT_5 (PREHEAT_4 + (PREHEAT_COUNT >= 5))
          #define PREHEAT_6 (PREHEAT_5 + (PREHEAT_COUNT >= 6))
          #define PREHEAT_7 (PREHEAT_6 + (PREHEAT_COUNT >= 7))
          #define PREHEAT_8 (PREHEAT_7 + (PREHEAT_COUNT >= 8))
          #define PREHEAT_9 (PREHEAT_8 + (PREHEAT_COUNT >= 9))
          #define PREHEAT_10 (PREHEAT_9 + (PREHEAT_COUNT >= 10))
          #define PREHEAT_TOTAL PREHEAT_10

          auto do_preheat = [](const uint8_t m) {
            thermalManager.cooldown();
            if (temp_val.preheatmode == 0 || temp_val.preheatmode == 1) { ui.preheat_hotend_and_fan(m); }
            if (temp_val.preheatmode == 0 || temp_val.preheatmode == 2) ui.preheat_bed(m);
          };

          switch (item) {
            case PREHEAT_BACK:
              if (draw)
                Draw_Menu_Item(row, ICON_Back, GET_TEXT_F(MSG_BACK));
              else {
                #if HAS_SHORTCUTS
                  if (temp_val.flag_shortcut) { temp_val.flag_shortcut = false;  Draw_Main_Menu(temp_val._selection); }
                  else 
                #endif
                  Draw_Menu(Prepare, PREPARE_PREHEAT); }
              break;
            #if !HAS_RC_CPU
              case PREHEAT_HOME:
                if (draw) Draw_Menu_Item(row, ICON_Back, GET_TEXT_F(MSG_MAIN));
                else Draw_Quick_Home();
                break;
            #endif

            case PREHEAT_MODE:
              if (draw) {
                Draw_Menu_Item(row, ICON_Homing, GET_TEXT_F(MSG_CONFIGURATION));
                Draw_Option(temp_val.preheatmode, preheat_modes, row);
              }
              else
                Modify_Option(temp_val.preheatmode, preheat_modes, 2);
              break;

            #define _PREHEAT_CASE(N) \
            case PREHEAT_##N: { \
              if (draw) Draw_Menu_Item(row, ICON_Temperature, F(PREHEAT_## N ##_LABEL)); \
              else do_preheat((N) - 1); \
            } break;
            REPEAT_1(PREHEAT_COUNT, _PREHEAT_CASE)
          }
        } break;
      #endif // HAS_PREHEAT

      #if ENABLED(FILAMENT_LOAD_UNLOAD_GCODES)
        case ChangeFilament:

          #define CHANGEFIL_BACK 0
          #define CHANGEFIL_HOME (CHANGEFIL_BACK + DISABLED(HAS_RC_CPU))
          #define CHANGEFIL_PARKHEAD (CHANGEFIL_HOME + 1)
          #define CHANGEFIL_LOAD (CHANGEFIL_PARKHEAD + 1)
          #define CHANGEFIL_UNLOAD (CHANGEFIL_LOAD + 1)
          #define CHANGEFIL_CHANGE (CHANGEFIL_UNLOAD + 1)
          #define CHANGEFIL_TOTAL CHANGEFIL_CHANGE

          switch (item) {
            case CHANGEFIL_BACK:
              if (draw)
                Draw_Menu_Item(row, ICON_Back, GET_TEXT_F(MSG_BACK));
              else {
                #if HAS_SHORTCUTS
                  if (temp_val.flag_shortcut) { temp_val.flag_shortcut = false; Draw_Main_Menu(temp_val._selection); }
                  else
                #endif
                  Draw_Menu(Prepare, PREPARE_CHANGEFIL);
              }
              break;
            #if !HAS_RC_CPU
              case CHANGEFIL_HOME:
                if (draw) Draw_Menu_Item(row, ICON_Back, GET_TEXT_F(MSG_MAIN));
                else Draw_Quick_Home();
                break;
            #endif
            case CHANGEFIL_PARKHEAD:
              if (draw)
                Draw_Menu_Item(row, ICON_ParkPos, GET_TEXT_F(MSG_FILAMENT_PARK_ENABLED));
              else {
                #if ENABLED(NOZZLE_PARK_FEATURE)
                    queue.inject(F("G28O\nG27 P2"));
                  #else
                    sprintf_P(cmd, PSTR("G28O\nG0 F4000 X%i Y%i\nG0 F3000 Z%i"), TERN(EXTJYERSUI, HMI_datas.Park_point.x, 240) , TERN(EXTJYERSUI, HMI_datas.Park_point.y, 220), TERN(EXTJYERSUI, HMI_datas.Park_point.z, 20));
                    queue.inject(cmd);
                  #endif
              }
              break;
            case CHANGEFIL_LOAD:
              if (draw)
                Draw_Menu_Item(row, ICON_WriteEEPROM, GET_TEXT_F(MSG_FILAMENTLOAD));
              else {
                if (thermalManager.temp_hotend[0].target < thermalManager.extrude_min_temp)
                  Popup_Handler(ETemp);
                else {
                  DWIN_Load_Unload();
                  temp_val.flag_chg_fil = false;
                  Draw_Menu(ChangeFilament, CHANGEFIL_LOAD);
                }
              }
              break;
            case CHANGEFIL_UNLOAD:
              if (draw)
                Draw_Menu_Item(row, ICON_ReadEEPROM, GET_TEXT_F(MSG_FILAMENTUNLOAD));
              else {
                if (thermalManager.temp_hotend[0].target < thermalManager.extrude_min_temp)
                  Popup_Handler(ETemp);
                else {
                  DWIN_Load_Unload(true);
                  Draw_Menu(ChangeFilament, CHANGEFIL_UNLOAD);
                }
              }
              break;
            case CHANGEFIL_CHANGE:
              if (draw)
                Draw_Menu_Item(row, ICON_ResumeEEPROM, GET_TEXT_F(MSG_FILAMENTCHANGE));
              else {
                if (thermalManager.temp_hotend[0].target < thermalManager.extrude_min_temp) {
                  Popup_Handler(ETemp);
                }
                else {
                  temp_val.flag_chg_fil = true;
                  if (thermalManager.temp_hotend[0].is_below_target(-2)) {
                    Update_Status(GET_TEXT(MSG_FILAMENT_CHANGE_HEATING));
                    Popup_Handler(Heating);
                    thermalManager.wait_for_hotend(0);
                  }
                  Popup_Handler(FilChange);
                  Update_Status(GET_TEXT(MSG_FILAMENT_CHANGE_HEADER));
                  sprintf_P(cmd, PSTR("M600 B1 R%i"), thermalManager.temp_hotend[0].target);
                  gcode.process_subcommands_now(cmd);
                  temp_val.flag_chg_fil = false;
                  Draw_Menu(ChangeFilament, CHANGEFIL_CHANGE);
                }
              }
              break;
          }
          break;
      #endif // FILAMENT_LOAD_UNLOAD_GCODES

      #if HAS_CUSTOM_MENU
        case MenuCustom:

          #define CUSTOM_MENU_BACK 0
          #define CUSTOM_MENU_1 1
          #define CUSTOM_MENU_2 2
          #define CUSTOM_MENU_3 3
          #define CUSTOM_MENU_4 4
          #define CUSTOM_MENU_5 5
          #define CUSTOM_MENU_TOTAL CUSTOM_MENU_COUNT

          switch (item) {
            case CUSTOM_MENU_BACK:
              if (draw)
                Draw_Menu_Item(row, ICON_Back, GET_TEXT_F(MSG_BACK));
              else
                Draw_Menu(Prepare, PREPARE_CUSTOM_MENU);
              break;

            #if CUSTOM_MENU_COUNT >= 1
              case CUSTOM_MENU_1:
                if (draw)
                  Draw_Menu_Item(row, ICON_Info, F(CONFIG_MENU_ITEM_1_DESC));
                else {
                  Popup_Handler(Custom);
                  gcode.process_subcommands_now(F(CONFIG_MENU_ITEM_1_GCODE));
                  planner.synchronize();
                  Redraw_Menu();
                  #if ENABLED(CUSTOM_MENU_CONFIG_SCRIPT_AUDIBLE_FEEDBACK)
                    AudioFeedback();
                  #endif
                  #ifdef CUSTOM_MENU_CONFIG_SCRIPT_RETURN
                    queue.inject(F(CUSTOM_MENU_CONFIG_SCRIPT_DONE));
                  #endif
                }
                break;
            #endif

            #if CUSTOM_MENU_COUNT >= 2
              case CUSTOM_MENU_2:
                if (draw)
                  Draw_Menu_Item(row, ICON_Info, F(CONFIG_MENU_ITEM_2_DESC));
                else {
                  Popup_Handler(Custom);
                  gcode.process_subcommands_now(F(CONFIG_MENU_ITEM_2_GCODE));
                  planner.synchronize();
                  Redraw_Menu();
                  #if ENABLED(CUSTOM_MENU_CONFIG_SCRIPT_AUDIBLE_FEEDBACK)
                    AudioFeedback();
                  #endif
                  #ifdef CUSTOM_MENU_CONFIG_SCRIPT_RETURN
                    queue.inject(F(CUSTOM_MENU_CONFIG_SCRIPT_DONE));
                  #endif
                }
                break;
            #endif

            #if CUSTOM_MENU_COUNT >= 3
              case CUSTOM_MENU_3:
                if (draw)
                  Draw_Menu_Item(row, ICON_Info, F(CONFIG_MENU_ITEM_3_DESC));
                else {
                  Popup_Handler(Custom);
                  gcode.process_subcommands_now(F(CONFIG_MENU_ITEM_3_GCODE));
                  planner.synchronize();
                  Redraw_Menu();
                  #if ENABLED(CUSTOM_MENU_CONFIG_SCRIPT_AUDIBLE_FEEDBACK)
                    AudioFeedback();
                  #endif
                  #ifdef CUSTOM_MENU_CONFIG_SCRIPT_RETURN
                    queue.inject(F(CUSTOM_MENU_CONFIG_SCRIPT_DONE));
                  #endif
                }
                break;
            #endif

            #if CUSTOM_MENU_COUNT >= 4
              case CUSTOM_MENU_4:
                if (draw)
                  Draw_Menu_Item(row, ICON_Info, F(CONFIG_MENU_ITEM_4_DESC));
                else {
                  Popup_Handler(Custom);
                  gcode.process_subcommands_now(F(CONFIG_MENU_ITEM_4_GCODE));
                  planner.synchronize();
                  Redraw_Menu();
                  #if ENABLED(CUSTOM_MENU_CONFIG_SCRIPT_AUDIBLE_FEEDBACK)
                    AudioFeedback();
                  #endif
                  #ifdef CUSTOM_MENU_CONFIG_SCRIPT_RETURN
                    queue.inject(F(CUSTOM_MENU_CONFIG_SCRIPT_DONE));
                  #endif
                }
                break;
            #endif

            #if CUSTOM_MENU_COUNT >= 5
              case CUSTOM_MENU_5:
                if (draw)
                  Draw_Menu_Item(row, ICON_Info, F(CONFIG_MENU_ITEM_5_DESC));
                else {
                  Popup_Handler(Custom);
                  gcode.process_subcommands_now(F(CONFIG_MENU_ITEM_5_GCODE));
                  planner.synchronize();
                  Redraw_Menu();
                  #if ENABLED(CUSTOM_MENU_CONFIG_SCRIPT_AUDIBLE_FEEDBACK)
                    AudioFeedback();
                  #endif
                  #ifdef CUSTOM_MENU_CONFIG_SCRIPT_RETURN
                    queue.inject(F(CUSTOM_MENU_CONFIG_SCRIPT_DONE));
                  #endif
                }
                break;
          #endif // Custom Menu
        }
        break;
    #endif // HAS_CUSTOM_MENU

      #if ENABLED(HOST_ACTION_COMMANDS, HAS_HOSTACTION_MENUS)
        case HostActions:

          #define HOSTACTIONS_BACK 0
          #define HOSTACTIONS_HOME (HOSTACTIONS_BACK + DISABLED(HAS_RC_CPU))
          #define HOSTACTIONS_1 (HOSTACTIONS_HOME + 1)
          #define HOSTACTIONS_2 (HOSTACTIONS_1 + 1)
          #define HOSTACTIONS_3 (HOSTACTIONS_2 + 1)
          #define HOSTACTIONS_TOTAL HOSTACTIONS_3

          temp_val.sd_item_flag = false;

          switch(item) {
            case HOSTACTIONS_BACK:
              if (draw) 
                Draw_Menu_Item(row, ICON_Back, GET_TEXT_F(MSG_BACK));
              else {
                if (temp_val.flag_tune_submenu) {
                  Return_to_tune();
                  //Redraw_Menu(false, true, true);
                }
                else
                  Draw_Menu(Prepare, PREPARE_ACTIONCOMMANDS);
              }  
              break;
            #if !HAS_RC_CPU
              case HOSTACTIONS_HOME:
                if (draw) Draw_Menu_Item(row, ICON_Back, GET_TEXT_F(MSG_MAIN));
                else Draw_Quick_Home();
                break;
            #endif          

            #define _HOSTACTIONS_CASE(N) \
              case HOSTACTIONS_##N: { \
                if (draw) { \
                Draw_Menu_Item(row, ICON_File, action##N); } \
                else { if (!strcmp(action##N, "-") == 0) hostui.action(F(action##N)); } \
              } break ;
            REPEAT_1(3, _HOSTACTIONS_CASE)
          }
          break;
      #endif

      case Control:

        #define CONTROL_BACK 0
        #define CONTROL_TEMP (CONTROL_BACK + 1)
        #define CONTROL_MOTION (CONTROL_TEMP + 1)
        #define CONTROL_FWRETRACT (CONTROL_MOTION + ENABLED(FWRETRACT))
        #define CONTROL_PARKMENU (CONTROL_FWRETRACT + ENABLED(NOZZLE_PARK_FEATURE, EXTJYERSUI))
        #define CONTROL_LEDS (CONTROL_PARKMENU + ANY(CASE_LIGHT_MENU, LED_CONTROL_MENU))
        #define CONTROL_ENCODER_DIR (CONTROL_LEDS + 1)
        #define CONTROL_VISUAL (CONTROL_ENCODER_DIR + 1)
        #define CONTROL_HOSTSETTINGS (CONTROL_VISUAL + ENABLED(HOST_ACTION_COMMANDS, HAS_HOSTACTION_MENUS))
        #define CONTROL_ADVANCED (CONTROL_HOSTSETTINGS + 1)
        #define CONTROL_SAVE (CONTROL_ADVANCED + ENABLED(EEPROM_SETTINGS))
        #define CONTROL_RESTORE (CONTROL_SAVE + ENABLED(EEPROM_SETTINGS))
        #define CONTROL_RESET (CONTROL_RESTORE + ENABLED(EEPROM_SETTINGS))
        #define CONTROL_REBOOT (CONTROL_RESET + 1)
        #define CONTROL_INFO (CONTROL_REBOOT + 1)
        #define CONTROL_TOTAL CONTROL_INFO

        switch (item) {
          case CONTROL_BACK:
            if (draw)
              Draw_Menu_Item(row, ICON_Back, GET_TEXT_F(MSG_BACK));
            else 
              Draw_Main_Menu(2);
            break;
          case CONTROL_TEMP:
            if (draw)
              Draw_Menu_Item(row, ICON_Temperature, GET_TEXT_F(MSG_TEMPERATURE), nullptr, true);
            else
              Draw_Menu(TempMenu);
            break;
          case CONTROL_MOTION:
            if (draw)
              Draw_Menu_Item(row, ICON_Motion, GET_TEXT_F(MSG_MOTION), nullptr, true);
            else
              Draw_Menu(Motion);
            break;
          #if ENABLED(FWRETRACT)
            case CONTROL_FWRETRACT:
            if (draw)
              Draw_Menu_Item(row, ICON_StepE, GET_TEXT_F(MSG_AUTORETRACT), nullptr, true);
            else
              Draw_Menu(FwRetraction);
            break;
          #endif
          #if ENABLED(NOZZLE_PARK_FEATURE) && EXTJYERSUI
            case CONTROL_PARKMENU:
            if (draw)
              Draw_Menu_Item(row, ICON_ParkPos, GET_TEXT_F(MSG_FILAMENT_PARK_ENABLED), nullptr, true);
            else
              Draw_Menu(Parkmenu);
            break;
          #endif
          #if ANY(CASE_LIGHT_MENU, LED_CONTROL_MENU)
            case CONTROL_LEDS:
              if (draw)
                Draw_Menu_Item(row, ICON_LedControl, GET_TEXT_F(MSG_LEDS), nullptr, true);
              else
                Draw_Menu(Ledsmenu);
              break;
          #endif
          case CONTROL_ENCODER_DIR:
            if (draw) {
              Draw_Menu_Item(row, ICON_Motion, GET_TEXT_F(MSG_REV_ENCODER_DIR));
              Draw_Checkbox(row, HMI_datas.rev_encoder_dir);
            }
            else {
              HMI_datas.rev_encoder_dir = !HMI_datas.rev_encoder_dir;
              Draw_Checkbox(row, HMI_datas.rev_encoder_dir);
            }
            break;
          case CONTROL_VISUAL:
            if (draw)
              Draw_Menu_Item(row, ICON_PrintSize, GET_TEXT_F(MSG_VISUAL_SETTINGS), nullptr, true);
            else
              Draw_Menu(Visual);
            break;
          #if ENABLED(HOST_ACTION_COMMANDS, HAS_HOSTACTION_MENUS)
            case CONTROL_HOSTSETTINGS:
              if (draw)
                Draw_Menu_Item(row, ICON_Contact, GET_TEXT_F(MSG_HOST_SETTINGS), nullptr, true);
              else 
                Draw_Menu(HostSettings);
              break;
          #endif
          case CONTROL_ADVANCED:
            if (draw)
              Draw_Menu_Item(row, ICON_Version, GET_TEXT_F(MSG_ADVANCED_SETTINGS), nullptr, true);
            else
              Draw_Menu(Advanced);
            break;
          #if ENABLED(EEPROM_SETTINGS)
            case CONTROL_SAVE:
              if (draw)
                Draw_Menu_Item(row, ICON_WriteEEPROM, GET_TEXT_F(MSG_STORE_EEPROM));
              else
                AudioFeedback(settings.save());
              break;
            case CONTROL_RESTORE:
              if (draw)
                Draw_Menu_Item(row, ICON_ReadEEPROM, GET_TEXT_F(MSG_LOAD_EEPROM));
              else
                AudioFeedback(settings.load());
              break;
            case CONTROL_RESET:
              if (draw)
                Draw_Menu_Item(row, ICON_ResumeEEPROM, GET_TEXT_F(MSG_RESTORE_DEFAULTS));
              else {
                settings.reset();
                AudioFeedback();
              }
              break;
          #endif
          case CONTROL_REBOOT:
            if (draw)
              Draw_Menu_Item(row, ICON_Reboot, GET_TEXT_F(MSG_RESET_PRINTER));
            else {
              RebootPrinter();
            }
            break;
          case CONTROL_INFO:
            if (draw)
              Draw_Menu_Item(row, ICON_Info, GET_TEXT_F(MSG_INFO_SCREEN));
            else
              Draw_Menu(Info);
            break;
        }
        break;

      case TempMenu:

        #define TEMP_BACK 0
        #define TEMP_HOME (TEMP_BACK + DISABLED(HAS_RC_CPU))
        #define TEMP_HOTEND (TEMP_HOME + ENABLED(HAS_HOTEND))
        #define TEMP_BED (TEMP_HOTEND + ENABLED(HAS_HEATED_BED))
        #define TEMP_FAN (TEMP_BED + ENABLED(HAS_FAN))
        #define TEMP_MPC (TEMP_FAN + ENABLED(HAS_HOTEND, MPCTEMP))
        #define TEMP_PID (TEMP_MPC + (ANY(HAS_HOTEND, HAS_HEATED_BED) && ANY(PIDTEMP, PIDTEMPBED)))
        #define TEMP_AUTOTEMP_MENU (TEMP_PID + ENABLED(HAS_AUTOTEMP_MENU))
        #define TEMP_AUTO_FAN_EXTRUDER (TEMP_AUTOTEMP_MENU + ENABLED(HAS_HOTEND, EXTJYERSUI, HAS_AUTO_FAN_0))
        #define TEMP_PREHEAT1 (TEMP_AUTO_FAN_EXTRUDER + (PREHEAT_COUNT >= 1))
        #define TEMP_PREHEAT2 (TEMP_PREHEAT1 + (PREHEAT_COUNT >= 2))
        #define TEMP_PREHEAT3 (TEMP_PREHEAT2 + (PREHEAT_COUNT >= 3))
        #define TEMP_PREHEAT4 (TEMP_PREHEAT3 + (PREHEAT_COUNT >= 4))
        #define TEMP_PREHEAT5 (TEMP_PREHEAT4 + (PREHEAT_COUNT >= 5))
        #define TEMP_PREHEAT6 (TEMP_PREHEAT5 + (PREHEAT_COUNT >= 6))
        #define TEMP_PREHEAT7 (TEMP_PREHEAT6 + (PREHEAT_COUNT >= 7))
        #define TEMP_PREHEAT8 (TEMP_PREHEAT7 + (PREHEAT_COUNT >= 8))
        #define TEMP_PREHEAT9 (TEMP_PREHEAT8 + (PREHEAT_COUNT >= 9))
        #define TEMP_PREHEAT10 (TEMP_PREHEAT9 + (PREHEAT_COUNT >= 10))
        #define TEMP_TOTAL TEMP_PREHEAT10

        switch (item) {
          case TEMP_BACK:
            if (draw)
              Draw_Menu_Item(row, ICON_Back, GET_TEXT_F(MSG_BACK));
            else
              Draw_Menu(Control, CONTROL_TEMP);
            break;
          #if !HAS_RC_CPU
            case TEMP_HOME:
              if (draw) Draw_Menu_Item(row, ICON_Back, GET_TEXT_F(MSG_MAIN));
              else Draw_Quick_Home();
              break;
          #endif
          #if HAS_HOTEND           
            case TEMP_HOTEND:
              if (draw) {
                Draw_Menu_Item(row, ICON_SetEndTemp, GET_TEXT_F(MSG_HOTEND_TEMPERATURE));
                Draw_Float(thermalManager.temp_hotend[0].target, row, false, 1);
              }
              else
                Modify_Value(thermalManager.temp_hotend[0].target, MIN_E_TEMP, MAX_E_TEMP, 1);
              break;
          #endif
          #if HAS_HEATED_BED
            case TEMP_BED:
              if (draw) {
                Draw_Menu_Item(row, ICON_SetBedTemp, GET_TEXT_F(MSG_BED_TEMPERATURE));
                Draw_Float(thermalManager.temp_bed.target, row, false, 1);
              }
              else
                Modify_Value(thermalManager.temp_bed.target, MIN_BED_TEMP, MAX_BED_TEMP, 1);
              break;
          #endif
          #if HAS_FAN
            case TEMP_FAN:
              Draw_item_fan_speed(draw, row);
              break;
          #endif
          #if HAS_HOTEND && BOTH(MPC_AUTOTUNE, MPC_AUTOTUNE_MENU)
            case TEMP_MPC:
              if (draw)
                Draw_Menu_Item(row, ICON_Step, GET_TEXT_F(MSG_MPC), nullptr, true);
              else {
                  #if !HAS_RC_CPU
                    Get_fact_multiplicator(MULT_MPC);
                  #endif
                Draw_Menu(MPC);
              }
              break;
          #endif
          #if (HAS_HOTEND || HAS_HEATED_BED) && ANY(PIDTEMP, PIDTEMPBED)
            case TEMP_PID:
              if (draw) {
                Draw_Menu_Item(row, ICON_Step, GET_TEXT_F(MSG_PID), nullptr, true);
               }
              else
                Draw_Menu(PID);
              break;
          #endif
          #if HAS_AUTOTEMP_MENU
            case TEMP_AUTOTEMP_MENU:
              if (draw) {
                Draw_Menu_Item(row, ICON_SetEndTemp, F("Autotemp Menu"), nullptr, true);
               }
              else {
                Get_fact_multiplicator(MULT_AUTOTEMP);
                Draw_Menu(AUTOTEMP_menu);
                }
              break;
          #endif
          #if HAS_HOTEND && EXTJYERSUI && HAS_AUTO_FAN_0
            case TEMP_AUTO_FAN_EXTRUDER:
              if (draw) {
                Draw_Menu_Item(row, ICON_SetBedTemp, F("Temp Auto Fan Off"));
                Draw_Float(HMI_datas.extruder_auto_fan_temperature, row, false, 1);
              }
              else
                Modify_Value(HMI_datas.extruder_auto_fan_temperature, MIN_BED_TEMP, 100, 1);
              break;
          #endif
          
        #define _TEMP_PREHEAT_CASE(N) \
          case TEMP_PREHEAT##N: { \
            if (draw) Draw_Menu_Item(row, ICON_Step, F(PREHEAT_## N ##_LABEL), nullptr, true); \
            else Draw_Menu(Preheat##N); \
          } break;

        REPEAT_1(PREHEAT_COUNT, _TEMP_PREHEAT_CASE)
        }
        break;

      #if (HAS_HOTEND || HAS_HEATED_BED) && ANY(PIDTEMP, PIDTEMPBED)
        case PID:

          #define PID_BACK 0
          #define PID_HOME (PID_BACK + DISABLED(HAS_RC_CPU))
          #define PID_HOTEND (PID_HOME + (HAS_HOTEND && ENABLED(PIDTEMP)))
          #define PID_BED (PID_HOTEND + (HAS_HEATED_BED && ENABLED(PIDTEMPBED)))
          #define PID_CYCLES (PID_BED + 1)
          #define PID_SAVE (PID_CYCLES +1)
          #define PID_TOTAL PID_SAVE

          switch (item) {
            case PID_BACK:
              if (draw)
                Draw_Menu_Item(row, ICON_Back, GET_TEXT_F(MSG_BACK));
              else
                Draw_Menu(TempMenu, TEMP_PID);
              break;
            #if !HAS_RC_CPU
              case PID_HOME:
                if (draw) Draw_Menu_Item(row, ICON_Back, GET_TEXT_F(MSG_MAIN));
                else Draw_Quick_Home();
                break;
            #endif          
            #if HAS_HOTEND && ENABLED(PIDTEMP)
              case PID_HOTEND:
                if (draw)
                  Draw_Menu_Item(row, ICON_HotendTemp, GET_TEXT_F(MSG_HOTEND_PID_AUTOTUNE), nullptr, true);
                else {
                  #if !HAS_RC_CPU
                    Get_fact_multiplicator(MULT_HOTEND);
                  #endif
                  Draw_Menu(HotendPID);
                }
                break;
            #endif
            #if HAS_HEATED_BED && ENABLED(PIDTEMPBED)
              case PID_BED:
                if (draw)
                  Draw_Menu_Item(row, ICON_BedTemp, GET_TEXT_F(MSG_BED_PID_AUTOTUNE), nullptr, true);
                else {
                  #if !HAS_RC_CPU
                    Get_fact_multiplicator(MULT_BED);
                  #endif
                  Draw_Menu(BedPID);
                }
                break;
            #endif
            case PID_CYCLES:
              if (draw) {
                Draw_Menu_Item(row, ICON_FanSpeed, GET_TEXT_F(MSG_PID_CYCLE));
                Draw_Float(temp_val.PID_cycles, row, false, 1);
              }
              else
                Modify_Value(temp_val.PID_cycles, 3, 50, 1);
              break;
            case PID_SAVE:
              if(draw) {
                Draw_Menu_Item(row, ICON_WriteEEPROM, GET_TEXT_F(MSG_PID_AUTOTUNE_SAVE));
              }
              else {
                AudioFeedback(settings.save());
              }
              break;
          }
          break;
      #endif // HAS_HOTEND || HAS_HEATED_BED

      #if HAS_HOTEND && ENABLED(PIDTEMP)
        case HotendPID:

          #define HOTENDPID_BACK 0
          #define HOTENDPID_HOME (HOTENDPID_BACK + DISABLED(HAS_RC_CPU))
          #define HOTENDPID_TUNE (HOTENDPID_HOME + 1)
          #define HOTENDPID_TEMP (HOTENDPID_TUNE + 1)
          #define HOTENDPID_FAN (HOTENDPID_TEMP + 1)
          #define HOTENDPID_KP (HOTENDPID_FAN + 1)
          #define HOTENDPID_KI (HOTENDPID_KP + 1)
          #define HOTENDPID_KD (HOTENDPID_KI + 1)
          #define HOTENDPID_MULT (HOTENDPID_KD + DISABLED(HAS_RC_CPU))
          #define HOTENDPID_TOTAL HOTENDPID_MULT

          switch (item) {
            case HOTENDPID_BACK:
              if (draw)
                Draw_Menu_Item(row, ICON_Back, GET_TEXT_F(MSG_BACK));
              else
                Draw_Menu(PID, PID_HOTEND);
              break;
            #if !HAS_RC_CPU
              case HOTENDPID_HOME:
                if (draw) Draw_Menu_Item(row, ICON_Back, GET_TEXT_F(MSG_MAIN));
                else Draw_Quick_Home();
                break;
            #endif           
            case HOTENDPID_TUNE:
              if (draw)
                Draw_Menu_Item(row, ICON_HotendTemp, GET_TEXT_F(MSG_PID_AUTOTUNE));
              else
                Do_HotendPID();
              break;
            case HOTENDPID_TEMP:
              if (draw) {
                Draw_Menu_Item(row, ICON_Temperature, GET_TEXT_F(MSG_TEMPERATURE));
                Draw_Float(temp_val.PID_e_temp, row, false, 1);
              }
              else
                Modify_Value(temp_val.PID_e_temp, MIN_E_TEMP, MAX_E_TEMP, 1);
              break;
            #if HAS_FAN
              case HOTENDPID_FAN:
                Draw_item_fan_speed(draw, row);
                break;
            #endif
            case HOTENDPID_KP:
              if (draw) {
                sprintf_P(cmd, PSTR("%s %s: "), GET_TEXT(MSG_PID_P), GET_TEXT(MSG_PID_VALUE));
                Draw_Menu_Item(row, ICON_Version, F(cmd));
                Draw_Float(thermalManager.temp_hotend[0].pid.p(), row, false, 100);
              }
              else {
                #if !HAS_RC_CPU
                  temp_val.flag_multiplicator = true;
                #endif
                Modify_Value(thermalManager.temp_hotend[0].pid.Kp, 0, 9990, 100, thermalManager.updatePID);
                }
              break;
            case HOTENDPID_KI:
              if (draw) {
                sprintf_P(cmd, PSTR("%s %s: "), GET_TEXT(MSG_PID_I), GET_TEXT(MSG_PID_VALUE));
                Draw_Menu_Item(row, ICON_Version, F(cmd));
                Draw_Float(thermalManager.temp_hotend[0].pid.i(), row, false, 100);
              }
              else {
                #if !HAS_RC_CPU
                  temp_val.flag_multiplicator = true;
                #endif
                Modify_Value(thermalManager.temp_hotend[0].pid.Ki, 0, 9990, 100, thermalManager.updatePID);
                }
              break;
            case HOTENDPID_KD:
              if (draw) {
                sprintf_P(cmd, PSTR("%s %s: "), GET_TEXT(MSG_PID_D), GET_TEXT(MSG_PID_VALUE));
                Draw_Menu_Item(row, ICON_Version, F(cmd));
                Draw_Float(thermalManager.temp_hotend[0].pid.d(), row, false, 100);
              }
              else{
                #if !HAS_RC_CPU
                  temp_val.flag_multiplicator = true;
                #endif
                Modify_Value(thermalManager.temp_hotend[0].pid.Kd, 0, 9990, 100, thermalManager.updatePID);
              }
              break;
            #if !HAS_RC_CPU
              case HOTENDPID_MULT:
                if (draw) {
                  Draw_Menu_Item(row, ICON_Homing, F("Enc. mult. factor"));
                  Draw_Option(temp_val.hotendmult, num_mult, row);
                }
                else {
                  temp_val.nummult = MULT_HOTEND;
                  Modify_Option(temp_val.hotendmult, num_mult, 2);
                }
              break;
            #endif
          }
          break;
      #endif // HAS_HOTEND

      #if HAS_HOTEND && BOTH(MPC_AUTOTUNE, MPC_AUTOTUNE_MENU)
        case MPC: 

            #define MPC_BACK 0
            #define MPC_HOME (MPC_BACK + DISABLED(HAS_RC_CPU))
            #define MPC_TUNE (MPC_HOME + 1)
            #define MPC_TARGET (MPC_TUNE + 1)
            #define MPC_POWR (MPC_TARGET + ENABLED(MPC_EDIT_MENU))
            #define MPC_HCAP (MPC_POWR + ENABLED(MPC_EDIT_MENU))
            #define MPC_FILH (MPC_HCAP + ENABLED(MPC_EDIT_MENU))
            #define MPC_RESP (MPC_FILH + ENABLED(MPC_EDIT_MENU)) // mmm enable when working
            #define MPC_XFER (MPC_RESP + ENABLED(MPC_EDIT_MENU))
            #define MPC_XFAN (MPC_XFER + ENABLED(MPC_EDIT_MENU))
            #define MPC_MULT (MPC_XFAN + (DISABLED(HAS_RC_CPU) && ENABLED(MPC_EDIT_MENU)))
            #define MPC_SAVE (MPC_MULT + 1)
            #define MPC_TOTAL MPC_SAVE

            switch (item) {
              case MPC_BACK:
                if (draw)
                  Draw_Menu_Item(row, ICON_Back, GET_TEXT_F(MSG_BACK));
                else
                  Draw_Menu(TempMenu, TEMP_MPC);
                break;
              #if !HAS_RC_CPU
                case MPC_HOME:
                  if (draw) Draw_Menu_Item(row, ICON_Back, GET_TEXT_F(MSG_MAIN));
                  else Draw_Quick_Home();
                  break;
              #endif
              case MPC_TUNE:
                if (draw)
                  Draw_Menu_Item(row, ICON_HotendTemp, GET_TEXT_F(MSG_MPC_AUTOTUNE));
                else {
                  Popup_Handler(MPCWait);
                  thermalManager.MPC_autotune(active_extruder);
                  planner.synchronize();
                }
                break;
              case MPC_TARGET:
                if (draw) {
                  Draw_Menu_Item(row, ICON_HotendTemp, F("Temp target:"));
                  Draw_Option(temp_val.MPC_targetmode, MPC_target, row);
                }
                else 
                  Modify_Option(temp_val.MPC_targetmode, MPC_target, 1);
                break;
              #if ENABLED(MPC_EDIT_MENU)
                case MPC_POWR:
                  if (draw) {
                    Draw_Menu_Item(row, ICON_Temperature, GET_TEXT_F(MSG_MPC_POWER));
                    Draw_Float(mpc.heater_power, row, false, 10);
                  }
                  else
                    Modify_Value(mpc.heater_power, 0, 200, 10);
                  break;
                case MPC_HCAP:
                  if (draw) {
                    Draw_Menu_Item(row, ICON_SetBedTemp, GET_TEXT_F(MSG_MPC_BLOCK_HEAT_CAPACITY));
                    Draw_Float(mpc.block_heat_capacity, row, false, 10);
                  }
                  else 
                    Modify_Value(mpc.block_heat_capacity, 0, 40, 10);
                  break;
                case MPC_RESP:
                  if (draw) {
                    Draw_Menu_Item(row, ICON_Contact, GET_TEXT_F(MSG_SENSOR_RESPONSIVENESS));
                    Draw_Float(mpc.sensor_responsiveness, row, false, 10000);
                  }
                  else {
                    #if !HAS_RC_CPU
                      temp_val.flag_multiplicator = true;
                    #endif
                    Modify_Value(mpc.sensor_responsiveness, 0, 1, 10000);
                  }
                  break;
                case MPC_XFER:
                  if (draw) {
                    Draw_Menu_Item(row, ICON_FanSpeed, GET_TEXT_F(MSG_MPC_AMBIENT_XFER_COEFF));
                    Draw_Float(mpc.ambient_xfer_coeff_fan0, row, false, 10000);
                  }
                  else {
                    #if !HAS_RC_CPU
                      temp_val.flag_multiplicator = true;
                    #endif
                    Modify_Value(mpc.ambient_xfer_coeff_fan0, 0, 1, 10000);
                  }
                  break;
                case MPC_FILH:
                  if (draw)
                    Draw_Menu_Item(row, ICON_Version, F("Filament C"), nullptr, true);
                  else {
                    #if !HAS_RC_CPU
                      Get_fact_multiplicator(MULT_FILTYPE);
                    #endif
                    Draw_Menu(MPCFilc);
                  }
                  break;
                #if ENABLED(MPC_INCLUDE_FAN)
                  case MPC_XFAN:
                    if (draw) {
                      temp_val.editable_value = thermalManager.temp_hotend[0].fanCoefficient();
                      Draw_Menu_Item(row, ICON_FanSpeed, GET_TEXT_F(MSG_MPC_AMBIENT_XFER_COEFF_FAN));
                      Draw_Float(temp_val.editable_value, row, false, 10000);
                    }
                    else {
                      #if !HAS_RC_CPU
                        temp_val.flag_multiplicator = true;
                      #endif
                      Modify_Value(temp_val.editable_value, 0, 1, 10000);
                    }
                    break;
                #endif
                #if !HAS_RC_CPU
                  case MPC_MULT:
                    if (draw) {
                      Draw_Menu_Item(row, ICON_Homing, F("Enc. mult. factor"));
                      Draw_Option(temp_val.mpcmult, num_mult, row);
                    }
                    else {
                      temp_val.nummult = MULT_MPC;
                      Modify_Option(temp_val.mpcmult, num_mult, 2);
                    }
                  break;
                #endif
              #endif
              case MPC_SAVE:
                if (draw)
                  Draw_Menu_Item(row, ICON_WriteEEPROM, GET_TEXT_F(MSG_STORE_EEPROM));
                else
                  AudioFeedback(settings.save());
                break;
            }

        break;

        #if ENABLED(MPC_EDIT_MENU)
          case MPCFilc:

              #define MPCFILC_BACK 0
              #define MPCFILC_FILH_M (MPCFILC_BACK + 1)
              #define MPCFILC_FILH_C (MPCFILC_FILH_M + 1)
              #define MPCFILC_MULT (MPCFILC_FILH_C + DISABLED(HAS_RC_CPU))
              #define MPCFILC_TOTAL MPCFILC_MULT

              switch (item) {
                case MPCFILC_BACK:
                  if (draw)
                    Draw_Menu_Item(row, ICON_Back, GET_TEXT_F(MSG_BACK));
                  else {
                    #if !HAS_RC_CPU
                      Get_fact_multiplicator(MULT_MPC);
                    #endif
                    Draw_Menu(MPC, MPC_FILH);
                  }
                  break;
                case MPCFILC_FILH_M:
                  if (draw) {
                    Draw_Menu_Item(row, ICON_Version, F("Fil. C predef"));
                    Draw_Option(temp_val.filheatcap, Filament_heat_capacity, row);
                  }
                  else 
                    Modify_Option(temp_val.filheatcap, Filament_heat_capacity, 3);
                  break;
                case MPCFILC_FILH_C:
                  if (draw) {
                    Draw_Menu_Item(row, ICON_Version, F("Fil. C value"));
                    Draw_Float(mpc.filament_heat_capacity_permm, row, false, 10000);
                  }
                  else {
                    if (temp_val.filheatcap == 3) {
                      #if !HAS_RC_CPU
                        temp_val.flag_multiplicator = true;
                      #endif
                      Modify_Value(mpc.filament_heat_capacity_permm, 0, 1, 10000);
                    }
                  }
                  break;
                #if !HAS_RC_CPU
                  case MPCFILC_MULT:
                    if (draw) {
                      Draw_Menu_Item(row, ICON_Homing, F("Enc. mult. factor"));
                      Draw_Option(temp_val.fil_typemult, num_mult, row);
                    }
                    else {
                      temp_val.nummult = MULT_FILTYPE;
                      Modify_Option(temp_val.fil_typemult, num_mult, 2);
                    }
                    break;
                #endif
              }
          break;
        #endif

    #endif

      #if HAS_HEATED_BED && ENABLED(PIDTEMPBED)
        case BedPID:

          #define BEDPID_BACK 0
          #define BEDPID_HOME (BEDPID_BACK + DISABLED(HAS_RC_CPU))
          #define BEDPID_TUNE (BEDPID_HOME + 1)
          #define BEDPID_TEMP (BEDPID_TUNE + 1)
          #define BEDPID_KP (BEDPID_TEMP + 1)
          #define BEDPID_KI (BEDPID_KP + 1)
          #define BEDPID_KD (BEDPID_KI + 1)
          #define BEDPID_MULT (BEDPID_KD + DISABLED(HAS_RC_CPU))
          #define BEDPID_TOTAL BEDPID_MULT

          switch (item) {
            case BEDPID_BACK:
              if (draw)
                Draw_Menu_Item(row, ICON_Back, GET_TEXT_F(MSG_BACK));
              else
                Draw_Menu(PID, PID_BED);
              break;
            #if !HAS_RC_CPU
              case BEDPID_HOME:
                if (draw) Draw_Menu_Item(row, ICON_Back, GET_TEXT_F(MSG_MAIN));
                else Draw_Quick_Home();
                break;
            #endif            
            case BEDPID_TUNE:
              if (draw)
                Draw_Menu_Item(row, ICON_BedTemp, GET_TEXT_F(MSG_PID_AUTOTUNE));
              else 
                Do_BedPID();
              break;
            case BEDPID_TEMP:
              if (draw) {
                Draw_Menu_Item(row, ICON_Temperature, GET_TEXT_F(MSG_TEMPERATURE));
                Draw_Float(temp_val.PID_bed_temp, row, false, 1);
              }
              else
                Modify_Value(temp_val.PID_bed_temp, MIN_BED_TEMP, MAX_BED_TEMP, 1);
              break;
            case BEDPID_KP:
              if (draw) {
                sprintf_P(cmd, PSTR("%s %s: "), GET_TEXT(MSG_PID_P), GET_TEXT(MSG_PID_VALUE));
                Draw_Menu_Item(row, ICON_Version, F(cmd));
                Draw_Float(thermalManager.temp_bed.pid.p(), row, false, 100);
              }
              else {
                #if !HAS_RC_CPU
                  temp_val.flag_multiplicator = true;
                #endif
                Modify_Value(thermalManager.temp_bed.pid.Kp, 0, 9990, 100);
              }
              break;
            case BEDPID_KI:
              if (draw) {
                sprintf_P(cmd, PSTR("%s %s: "), GET_TEXT(MSG_PID_I), GET_TEXT(MSG_PID_VALUE));
                Draw_Menu_Item(row, ICON_Version, F(cmd));
                Draw_Float(thermalManager.temp_bed.pid.i(), row, false, 100);
              }
              else {
                #if !HAS_RC_CPU
                  temp_val.flag_multiplicator = true;
                #endif
                Modify_Value(thermalManager.temp_bed.pid.Ki, 0, 9990, 100);
              }
              break;
            case BEDPID_KD:
              if (draw) {
                sprintf_P(cmd, PSTR("%s %s: "), GET_TEXT(MSG_PID_D), GET_TEXT(MSG_PID_VALUE));
                Draw_Menu_Item(row, ICON_Version, F(cmd));
                Draw_Float(thermalManager.temp_bed.pid.d(), row, false, 100);
              }
              else {
                #if !HAS_RC_CPU
                  temp_val.flag_multiplicator = true;
                #endif
                Modify_Value(thermalManager.temp_bed.pid.Kd, 0, 9990, 100);
              }
              break;
            #if !HAS_RC_CPU
              case BEDPID_MULT:
                if (draw) {
                  Draw_Menu_Item(row, ICON_Homing, F("Enc. mult. factor"));
                  Draw_Option(temp_val.bedmult, num_mult, row);
                }
                else {
                  temp_val.nummult = MULT_BED;
                  Modify_Option(temp_val.bedmult, num_mult, 2);
                }
              break;
            #endif
          }
          break;
      #endif // HAS_HEATED_BED

      #if HAS_AUTOTEMP_MENU
        case AUTOTEMP_menu:

          #define AUTOTEMP_BACK 0
          #define AUTOTEMP_HOME (AUTOTEMP_BACK + 1)
          #define AUTOTEMP_ENABLE (AUTOTEMP_HOME + 1)
          #define AUTOTEMP_TMIN (AUTOTEMP_ENABLE + 1)
          #define AUTOTEMP_TMAX (AUTOTEMP_TMIN + 1)
          #define AUTOTEMP_VFACTOR (AUTOTEMP_TMAX + 1)
          #define AUTOTEMP_MULT (AUTOTEMP_VFACTOR + 1)
          #define AUTOTEMP_TOTAL AUTOTEMP_MULT

          switch (item) {
            case AUTOTEMP_BACK:
              if (draw)
                Draw_Menu_Item(row, ICON_Back, GET_TEXT_F(MSG_BACK));
              else {
                if (temp_val.flag_tune_submenu) {
                  Return_to_tune();
                  //Redraw_Menu(false, true, true);
                }
                else
                  Draw_Menu(TempMenu, TEMP_AUTOTEMP_MENU);
              }
              break;
            case AUTOTEMP_HOME:
              if (draw) Draw_Menu_Item(row, ICON_Back, GET_TEXT_F(MSG_MAIN));
              else Draw_Quick_Home();
              break;
            case AUTOTEMP_ENABLE:
              if (draw) {
                  Draw_Menu_Item(row, ICON_SetEndTemp, F("Autotemp active"));
                  Draw_Checkbox(row, temp_val.autotemp_enable);
                }
                else {
                  temp_val.autotemp_enable = !temp_val.autotemp_enable;
                  Draw_Checkbox(row, temp_val.autotemp_enable);
                }
                break;
            case AUTOTEMP_TMIN:
              if (draw) {
                Draw_Menu_Item(row, ICON_SetEndTemp, F("Min Temp"));
                Draw_Float(planner.autotemp.min, row, false, 1);
              }
              else
                if (!temp_val.printing) Modify_Value(planner.autotemp.min, 0, thermalManager.hotend_max_target(0), 1);
              break;
            case AUTOTEMP_TMAX:
              if (draw) {
                Draw_Menu_Item(row, ICON_SetEndTemp, F("Max Temp"));
                Draw_Float(planner.autotemp.max, row, false, 1);
              }
              else
                Modify_Value(planner.autotemp.max, planner.autotemp.min, thermalManager.hotend_max_target(0), 1);
              break;
            case AUTOTEMP_VFACTOR:
              if (draw) {
                Draw_Menu_Item(row, ICON_Step, F("Factor constant"));
                Draw_Float(planner.autotemp.factor, row, false, 100);
              }
              else {
                temp_val.flag_multiplicator = true;
                Modify_Value(planner.autotemp.factor, 0, 10, 100);
              }
              break;
            case AUTOTEMP_MULT:
              if (draw) {
                Draw_Menu_Item(row, ICON_Homing, F("Enc. mult. factor"));
                Draw_Option(temp_val.autotempmult, num_mult, row);
              }
              else {
                temp_val.nummult = MULT_AUTOTEMP;
                Modify_Option(temp_val.autotempmult, num_mult, 2);
              }
              break;
          }
          break;
      #endif

      #if HAS_PREHEAT
        #define _PREHEAT_SUBMENU_CASE(N) case Preheat##N: preheat_submenu((N) - 1, item, TEMP_PREHEAT##N); break;
        REPEAT_1(PREHEAT_COUNT, _PREHEAT_SUBMENU_CASE)
      #endif

      case Motion:

        #define MOTION_BACK 0
        #define MOTION_HOME (MOTION_BACK + DISABLED(HAS_RC_CPU))
        #define MOTION_HOMEOFFSETS (MOTION_HOME + 1)
        #define MOTION_SPEED (MOTION_HOMEOFFSETS + 1)
        #define MOTION_ACCEL (MOTION_SPEED + 1)
        #define MOTION_JERK (MOTION_ACCEL + ENABLED(HAS_CLASSIC_JERK))
        #define MOTION_JD (MOTION_JERK + ENABLED(HAS_JUNCTION_DEVIATION))
        #define MOTION_STEPS (MOTION_JD + 1)
        #define MOTION_INVERT_DIR_EXTR (MOTION_STEPS + ENABLED(HAS_HOTEND, EXTJYERSUI))
        #define MOTION_FLOW (MOTION_INVERT_DIR_EXTR + ENABLED(HAS_HOTEND))
        #define MOTION_ASS (MOTION_FLOW + ENABLED(ADAPTIVE_STEP_SMOOTHING))
        #define MOTION_TOTAL MOTION_ASS

        switch (item) {
          case MOTION_BACK:
            if (draw)
              Draw_Menu_Item(row, ICON_Back, GET_TEXT_F(MSG_BACK));
            else
              Draw_Menu(Control, CONTROL_MOTION);
            break;
          #if !HAS_RC_CPU
            case MOTION_HOME:
              if (draw) Draw_Menu_Item(row, ICON_Back, GET_TEXT_F(MSG_MAIN));
              else Draw_Quick_Home();
              break;
          #endif
          case MOTION_HOMEOFFSETS:
            if (draw)
              Draw_Menu_Item(row, ICON_SetHome, GET_TEXT_F(MSG_SET_HOME_OFFSETS), nullptr, true);
            else
              Draw_Menu(HomeOffsets);
            break;
          case MOTION_SPEED:
            if (draw)
              Draw_Menu_Item(row, ICON_MaxSpeed, GET_TEXT_F(MSG_MAXSPEED), nullptr, true);
            else
              Draw_Menu(MaxSpeed);
            break;
          case MOTION_ACCEL:
            if (draw) 
              Draw_Menu_Item(row, ICON_MaxAccelerated, GET_TEXT_F(MSG_AMAX_EN), nullptr, true);
            
            else {
              #if !HAS_RC_CPU
                Get_fact_multiplicator(MULT_MAXACC);
              #endif
              Draw_Menu(MaxAcceleration);
            }
            break;
          #if HAS_CLASSIC_JERK
            case MOTION_JERK:
              if (draw)
                Draw_Menu_Item(row, ICON_MaxJerk, GET_TEXT_F(MSG_VEN_JERK), nullptr, true);
              else {
                #if !HAS_RC_CPU
                    Get_fact_multiplicator(MULT_JERK);
                  #endif
                Draw_Menu(MaxJerk);
              }
              break;
          #endif
          #if HAS_JUNCTION_DEVIATION
            case MOTION_JD:
              if (draw)
                Draw_Menu_Item(row, ICON_MaxJerk, GET_TEXT_F(MSG_JUNCTION_DEVIATION_MENU), nullptr, true);
              else
                Draw_Menu(JDmenu);
              break;
          #endif
          case MOTION_STEPS:
            if (draw)
              Draw_Menu_Item(row, ICON_Step, GET_TEXT_F(MSG_STEPS_PER_MM), nullptr, true);
            else {
              #if !HAS_RC_CPU
                Get_fact_multiplicator(MULT_STEPS);
              #endif
              Draw_Menu(Steps);
            }
            break;
          #if HAS_HOTEND
            #if EXTJYERSUI
              case MOTION_INVERT_DIR_EXTR:
                if (draw) {
                  Draw_Menu_Item(row, ICON_Motion, GET_TEXT_F(MSG_EXTRUDER_INVERT));
                  Draw_Checkbox(row, HMI_datas.invert_dir_extruder);
                }
                else {
                  HMI_datas.invert_dir_extruder = !HMI_datas.invert_dir_extruder;
                  DWIN_Invert_Extruder();
                  Draw_Checkbox(row, HMI_datas.invert_dir_extruder);
                }
                break;
            #endif
            case MOTION_FLOW:
              if (draw) {
                Draw_Menu_Item(row, ICON_Speed, GET_TEXT_F(MSG_FLOW));
                Draw_Float(planner.flow_percentage[0], row, false, 1);
              }
              else
                Modify_Value(planner.flow_percentage[0], MIN_FLOW_RATE, MAX_FLOW_RATE, 1);
              break;
          #endif
          #if ENABLED(ADAPTIVE_STEP_SMOOTHING)
            case MOTION_ASS:
              if (draw) {
                Draw_Menu_Item(row, ICON_Motion, F("Adapt. Step Smooth."));
                Draw_Checkbox(row, HMI_datas.ass);
              }
              else {
                HMI_datas.ass = !HMI_datas.ass;
                Draw_Checkbox(row, HMI_datas.ass);
              }
              break;
          #endif
        }
        break;
      #if ENABLED(FWRETRACT)
        case FwRetraction:

          #define FWR_BACK 0
          #define FWR_HOME (FWR_BACK + DISABLED(HAS_RC_CPU))
          #define FWR_RET_AUTO (FWR_HOME + ENABLED(FWRETRACT_AUTORETRACT))
          #define FWR_RET_LENGTH (FWR_RET_AUTO + 1)
          #define FWR_RET_SPEED (FWR_RET_LENGTH + 1)
          #define FWR_ZLIFT (FWR_RET_SPEED + 1)
          #define FWR_REC_EXT_LENGTH (FWR_ZLIFT + 1)
          #define FWR_REC_SPEED (FWR_REC_EXT_LENGTH + 1)
          #define FWR_RESET (FWR_REC_SPEED + 1)
          #define FWR_TOTAL FWR_RESET

          switch (item) {
          
            case FWR_BACK:
            if (draw)
              Draw_Menu_Item(row, ICON_Back, GET_TEXT_F(MSG_BACK));
            else {
              if (temp_val.flag_tune_submenu) {
                Return_to_tune();
                //Redraw_Menu(false, true, true);
              }
              else
                Draw_Menu(Control, CONTROL_FWRETRACT);
            }
            break;
            #if !HAS_RC_CPU
              case FWR_HOME:
                if (draw) Draw_Menu_Item(row, ICON_Back, GET_TEXT_F(MSG_MAIN));
                else Draw_Quick_Home();
                break;
            #endif
            #if ENABLED(FWRETRACT_AUTORETRACT)
              case FWR_RET_AUTO:
                if (draw) {
                  temp_val.auto_fw_retract = fwretract.autoretract_enabled;
                  Draw_Menu_Item(row, ICON_StepE, GET_TEXT_F(MSG_AUTORETRACT_ONOFF));
                  Draw_Checkbox(row, temp_val.auto_fw_retract);
                }
                else {
                  if (MIN_AUTORETRACT <= MAX_AUTORETRACT) {
                    temp_val.auto_fw_retract = !temp_val.auto_fw_retract;
                    fwretract.enable_autoretract(temp_val.auto_fw_retract);
                    Draw_Checkbox(row, temp_val.auto_fw_retract);
                  }
                }
                break;
            #endif
            case FWR_RET_LENGTH:
              if (draw) {
                Draw_Menu_Item(row, ICON_FWRetLength, GET_TEXT_F(MSG_CONTROL_RETRACT));
                Draw_Float(fwretract.settings.retract_length, row, false, 10);
              }
              else
                Modify_Value(fwretract.settings.retract_length, 0, 10, 10);
              break;
            case FWR_RET_SPEED:
              if (draw) {
                Draw_Menu_Item(row, ICON_FWRetSpeed, GET_TEXT_F(MSG_SINGLENOZZLE_RETRACT_SPEED));
                Draw_Float(fwretract.settings.retract_feedrate_mm_s, row, false, 10);
              }
              else
                Modify_Value(fwretract.settings.retract_feedrate_mm_s, 1, 90, 10);
              break;
            case FWR_ZLIFT:
              if (draw) {
                Draw_Menu_Item(row, ICON_FWRetZRaise, GET_TEXT_F(MSG_CONTROL_RETRACT_ZHOP));
                Draw_Float(fwretract.settings.retract_zraise, row, false, 100);
              }
              else
                Modify_Value(fwretract.settings.retract_zraise, 0, 10, 100);
              break;
            case FWR_REC_EXT_LENGTH:
              if (draw) {
                Draw_Menu_Item(row, ICON_FWRecExtLength, GET_TEXT_F(MSG_CONTROL_RETRACT_RECOVER));
                Draw_Float(fwretract.settings.retract_recover_extra, row, false, 10);
              }
              else
                Modify_Value(fwretract.settings.retract_recover_extra, -10, 10, 10);
              break;
            case FWR_REC_SPEED:
              if (draw) {
                Draw_Menu_Item(row, ICON_FWRecSpeed, GET_TEXT_F(MSG_SINGLENOZZLE_UNRETRACT_SPEED));
                Draw_Float(fwretract.settings.retract_recover_feedrate_mm_s, row, false, 10);
              }
              else
                Modify_Value(fwretract.settings.retract_recover_feedrate_mm_s, 1, 90, 10);
              break;
            case FWR_RESET:
              if (draw)
                Draw_Menu_Item(row, ICON_StepE, GET_TEXT_F(MSG_BUTTON_RESET));
              else {
                fwretract.reset();
                Draw_Menu(FwRetraction);
              }
              break;
          }
          break;
      #endif

      #if ENABLED(NOZZLE_PARK_FEATURE) && EXTJYERSUI
        case Parkmenu:

          #define PARKMENU_BACK 0
          #define PARKMENU_HOME (PARKMENU_BACK + DISABLED(HAS_RC_CPU))
          #define PARKMENU_POSX (PARKMENU_HOME + 1)
          #define PARKMENU_POSY (PARKMENU_POSX + 1)
          #define PARKMENU_POSZ (PARKMENU_POSY + 1)
          #define PARKMENU_TOTAL PARKMENU_POSZ

          switch (item) {
            case PARKMENU_BACK:
              if (draw)
                Draw_Menu_Item(row, ICON_Back, GET_TEXT_F(MSG_BACK));
              else
                Draw_Menu(Control, CONTROL_PARKMENU);
              break;
            #if !HAS_RC_CPU
              case PARKMENU_HOME:
                if (draw) Draw_Menu_Item(row, ICON_Back, GET_TEXT_F(MSG_MAIN));
                else Draw_Quick_Home();
                break;
            #endif
            case PARKMENU_POSX:
              if (draw) {
                Draw_Menu_Item(row, ICON_ParkPosX, GET_TEXT_F(MSG_FILAMENT_PARK_X));
                Draw_Float(HMI_datas.Park_point.x, row, false, 1);
              }
              else
                Modify_Value(HMI_datas.Park_point.x, X_MIN_POS, X_MAX_POS, 1);
              break;
            case PARKMENU_POSY:
              if (draw) {
                Draw_Menu_Item(row, ICON_ParkPosY, GET_TEXT_F(MSG_FILAMENT_PARK_Y));
                Draw_Float(HMI_datas.Park_point.y, row, false, 1);
              }
              else
                Modify_Value(HMI_datas.Park_point.y, Y_MIN_POS, Y_MAX_POS, 1);
              break;
            case PARKMENU_POSZ:
              if (draw) {
                Draw_Menu_Item(row, ICON_ParkPosZ, GET_TEXT_F(MSG_FILAMENT_PARK_Z));
                Draw_Float(HMI_datas.Park_point.z, row, false, 1);
              }
              else
                Modify_Value(HMI_datas.Park_point.z, MIN_PARK_POINT_Z, Z_MAX_POS, 1);
              break;
          }
        break;
      #endif

      case HomeOffsets:

        #define HOMEOFFSETS_BACK 0
        #define HOMEOFFSETS_HOME (HOMEOFFSETS_BACK + DISABLED(HAS_RC_CPU))
        #define HOMEOFFSETS_XOFFSET (HOMEOFFSETS_HOME + 1)
        #define HOMEOFFSETS_YOFFSET (HOMEOFFSETS_XOFFSET + 1)
        #define HOMEOFFSETS_TOTAL HOMEOFFSETS_YOFFSET

        switch (item) {
          case HOMEOFFSETS_BACK:
            if (draw)
              Draw_Menu_Item(row, ICON_Back, GET_TEXT_F(MSG_BACK));
            else
              Draw_Menu(Motion, MOTION_HOMEOFFSETS);
            break;
          #if !HAS_RC_CPU
            case HOMEOFFSETS_HOME:
              if (draw) Draw_Menu_Item(row, ICON_Back, GET_TEXT_F(MSG_MAIN));
              else Draw_Quick_Home();
              break;
          #endif
          case HOMEOFFSETS_XOFFSET:
            if (draw) {
              Draw_Menu_Item(row, ICON_StepX, GET_TEXT_F(MSG_HOME_OFFSET_X));
              Draw_Float(home_offset.x, row, false, 100);
            }
            else
              Modify_Value(home_offset.x, -MAX_XY_OFFSET, MAX_XY_OFFSET, 100);
            break;
          case HOMEOFFSETS_YOFFSET:
            if (draw) {
              Draw_Menu_Item(row, ICON_StepY, GET_TEXT_F(MSG_HOME_OFFSET_Y));
              Draw_Float(home_offset.y, row, false, 100);
            }
            else
              Modify_Value(home_offset.y, -MAX_XY_OFFSET, MAX_XY_OFFSET, 100);
            break;
        }
        break;
      case MaxSpeed:

        #define SPEED_BACK 0
        #define SPEED_HOME (SPEED_BACK + DISABLED(HAS_RC_CPU))
        #define SPEED_X (SPEED_HOME + 1)
        #define SPEED_Y (SPEED_X + 1)
        #define SPEED_Z (SPEED_Y + 1)
        #define SPEED_E (SPEED_Z + ENABLED(HAS_HOTEND))
        #define SPEED_TOTAL SPEED_E

        switch (item) {
          case SPEED_BACK:
            if (draw)
              Draw_Menu_Item(row, ICON_Back, GET_TEXT_F(MSG_BACK));
            else
              Draw_Menu(Motion, MOTION_SPEED);
            break;
          #if !HAS_RC_CPU
            case SPEED_HOME:
              if (draw) Draw_Menu_Item(row, ICON_Back, GET_TEXT_F(MSG_MAIN));
              else Draw_Quick_Home();
              break;
          #endif
          case SPEED_X:
            if (draw) {
              Draw_Menu_Item(row, ICON_MaxSpeedX, GET_TEXT_F(MSG_MAXSPEED_X));
              Draw_Float(planner.settings.max_feedrate_mm_s[X_AXIS], row, false, FEEDRATE_UNIT);
            }
            else
              Modify_Value(planner.settings.max_feedrate_mm_s[X_AXIS], min_feedrate_edit_values[X_AXIS], max_feedrate_edit_values[X_AXIS], FEEDRATE_UNIT);
            break;

          #if HAS_Y_AXIS
            case SPEED_Y:
              if (draw) {
                Draw_Menu_Item(row, ICON_MaxSpeedY, GET_TEXT_F(MSG_MAXSPEED_Y));
                Draw_Float(planner.settings.max_feedrate_mm_s[Y_AXIS], row, false, FEEDRATE_UNIT);
              }
              else
                Modify_Value(planner.settings.max_feedrate_mm_s[Y_AXIS], min_feedrate_edit_values[Y_AXIS], max_feedrate_edit_values[Y_AXIS], FEEDRATE_UNIT);
              break;
          #endif

          #if HAS_Z_AXIS
            case SPEED_Z:
              if (draw) {
                Draw_Menu_Item(row, ICON_MaxSpeedZ, GET_TEXT_F(MSG_MAXSPEED_Z));
                Draw_Float(planner.settings.max_feedrate_mm_s[Z_AXIS], row, false, FEEDRATE_UNIT);
              }
              else
                Modify_Value(planner.settings.max_feedrate_mm_s[Z_AXIS], min_feedrate_edit_values[Z_AXIS], max_feedrate_edit_values[Z_AXIS], FEEDRATE_UNIT);
              break;
          #endif

          #if HAS_HOTEND
            case SPEED_E:
              if (draw) {
                Draw_Menu_Item(row, ICON_MaxSpeedE, GET_TEXT_F(MSG_MAXSPEED_E));
                Draw_Float(planner.settings.max_feedrate_mm_s[E_AXIS], row, false, FEEDRATE_UNIT);
              }
              else
                Modify_Value(planner.settings.max_feedrate_mm_s[E_AXIS], min_feedrate_edit_values[E_AXIS], max_feedrate_edit_values[E_AXIS], FEEDRATE_UNIT);
              break;
          #endif
        }
        break;

      case MaxAcceleration:

        #define ACCEL_BACK 0
        #define ACCEL_HOME (ACCEL_BACK + DISABLED(HAS_RC_CPU))
        #define ACCEL_X (ACCEL_HOME + 1)
        #define ACCEL_Y (ACCEL_X + 1)
        #define ACCEL_Z (ACCEL_Y + 1)
        #define ACCEL_E (ACCEL_Z + ENABLED(HAS_HOTEND))
        #define ACCEL_MULT (ACCEL_E + DISABLED(HAS_RC_CPU))
        #define ACCEL_TOTAL ACCEL_MULT

        switch (item) {
          case ACCEL_BACK:
            if (draw)
              Draw_Menu_Item(row, ICON_Back, GET_TEXT_F(MSG_BACK));
            else
              Draw_Menu(Motion, MOTION_ACCEL);
            break;
          #if !HAS_RC_CPU
            case ACCEL_HOME:
              if (draw) Draw_Menu_Item(row, ICON_Back, GET_TEXT_F(MSG_MAIN));
              else Draw_Quick_Home();
              break;
          #endif
          case ACCEL_X:
            if (draw) {
              Draw_Menu_Item(row, ICON_MaxAccX, GET_TEXT_F(MSG_AMAX_A));
              Draw_Float(planner.settings.max_acceleration_mm_per_s2[X_AXIS], row, false, ACCELERATION_UNIT);
            }
            else {
              #if !HAS_RC_CPU
                temp_val.flag_multiplicator = true;
              #endif
              Modify_Value(planner.settings.max_acceleration_mm_per_s2[X_AXIS], min_acceleration_edit_values[X_AXIS], max_acceleration_edit_values[X_AXIS], ACCELERATION_UNIT);
              }
            break;
          case ACCEL_Y:
            if (draw) {
              Draw_Menu_Item(row, ICON_MaxAccY, GET_TEXT_F(MSG_AMAX_B));
              Draw_Float(planner.settings.max_acceleration_mm_per_s2[Y_AXIS], row, false, ACCELERATION_UNIT);
            }
            else {
              #if !HAS_RC_CPU
                temp_val.flag_multiplicator = true;
              #endif
              Modify_Value(planner.settings.max_acceleration_mm_per_s2[Y_AXIS], min_acceleration_edit_values[Y_AXIS], max_acceleration_edit_values[Y_AXIS], ACCELERATION_UNIT);
            }
            break;
          case ACCEL_Z:
            if (draw) {
              Draw_Menu_Item(row, ICON_MaxAccZ, GET_TEXT_F(MSG_AMAX_C));
              Draw_Float(planner.settings.max_acceleration_mm_per_s2[Z_AXIS], row, false, ACCELERATION_UNIT);
            }
            else {
              #if !HAS_RC_CPU
                temp_val.flag_multiplicator = true;
              #endif
              Modify_Value(planner.settings.max_acceleration_mm_per_s2[Z_AXIS], min_acceleration_edit_values[Z_AXIS], max_acceleration_edit_values[Z_AXIS], ACCELERATION_UNIT);
              }
            break;
          #if HAS_HOTEND
            case ACCEL_E:
              if (draw) {
                Draw_Menu_Item(row, ICON_MaxAccE, GET_TEXT_F(MSG_AMAX_E));
                Draw_Float(planner.settings.max_acceleration_mm_per_s2[E_AXIS], row, false, ACCELERATION_UNIT);
              }
              else {
              #if !HAS_RC_CPU
                temp_val.flag_multiplicator = true;
              #endif
                Modify_Value(planner.settings.max_acceleration_mm_per_s2[E_AXIS], min_acceleration_edit_values[E_AXIS], max_acceleration_edit_values[E_AXIS], ACCELERATION_UNIT);
              }
              break;
          #endif
          #if !HAS_RC_CPU
            case ACCEL_MULT:
              if (draw) {
                Draw_Menu_Item(row, ICON_Homing, F("Enc. mult. factor"));
                Draw_Option(temp_val.maxaccmult, num_mult, row);
              }
              else {
                temp_val.nummult = MULT_MAXACC;
                Modify_Option(temp_val.maxaccmult, num_mult, 2);
              }
            break;
          #endif
        }
        break;
      #if HAS_CLASSIC_JERK
        case MaxJerk:

          #define JERK_BACK 0
          #define JERK_HOME (JERK_BACK + DISABLED(HAS_RC_CPU))
          #define JERK_X (JERK_HOME + 1)
          #define JERK_Y (JERK_X + 1)
          #define JERK_Z (JERK_Y + 1)
          #define JERK_E (JERK_Z + ENABLED(HAS_HOTEND))
          #define JERK_MULT (JERK_E + DISABLED(HAS_RC_CPU))
          #define JERK_TOTAL JERK_MULT

          switch (item) {
            case JERK_BACK:
              if (draw)
                Draw_Menu_Item(row, ICON_Back, GET_TEXT_F(MSG_BACK));
              else
                Draw_Menu(Motion, MOTION_JERK);
              break;
            #if !HAS_RC_CPU
              case JERK_HOME:
                if (draw) Draw_Menu_Item(row, ICON_Back, GET_TEXT_F(MSG_MAIN));
                else Draw_Quick_Home();
                break;
            #endif 
            case JERK_X:
              if (draw) {
                Draw_Menu_Item(row, ICON_MaxSpeedJerkX, GET_TEXT_F(MSG_VA_JERK));
                Draw_Float(planner.max_jerk.x, row, false, JERK_UNIT);
              }
              else {
                #if !HAS_RC_CPU
                  temp_val.flag_multiplicator = true;
                #endif
                Modify_Value(planner.max_jerk.x, min_jerk_edit_values[X_AXIS], max_jerk_edit_values[X_AXIS], JERK_UNIT);
              }
              break;
            case JERK_Y:
              if (draw) {
                Draw_Menu_Item(row, ICON_MaxSpeedJerkY, GET_TEXT_F(MSG_VB_JERK));
                Draw_Float(planner.max_jerk.y, row, false, JERK_UNIT);
              }
              else { 
                #if !HAS_RC_CPU
                  temp_val.flag_multiplicator = true;
                #endif
                Modify_Value(planner.max_jerk.y, min_jerk_edit_values[Y_AXIS], max_jerk_edit_values[Y_AXIS], JERK_UNIT);
              }
              break;
            case JERK_Z:
              if (draw) {
                Draw_Menu_Item(row, ICON_MaxSpeedJerkZ, GET_TEXT_F(MSG_VC_JERK));
                Draw_Float(planner.max_jerk.z, row, false, JERK_UNIT);
              }
              else {
                #if !HAS_RC_CPU
                  temp_val.flag_multiplicator = true;
                #endif
                Modify_Value(planner.max_jerk.z, min_jerk_edit_values[Z_AXIS], max_jerk_edit_values[Z_AXIS], JERK_UNIT);;
              }
              break;
            #if HAS_HOTEND
              case JERK_E:
                if (draw) {
                  Draw_Menu_Item(row, ICON_MaxSpeedJerkE, GET_TEXT_F(MSG_VE_JERK));
                  Draw_Float(planner.max_jerk.e, row, false, JERK_UNIT);
                }
                else {
                  #if !HAS_RC_CPU
                    temp_val.flag_multiplicator = true;
                  #endif
                  Modify_Value(planner.max_jerk.e, min_jerk_edit_values[E_AXIS], max_jerk_edit_values[E_AXIS], JERK_UNIT);
                }
                break;
            #endif
            #if !HAS_RC_CPU
              case JERK_MULT:
                if (draw) {
                  Draw_Menu_Item(row, ICON_Homing, F("Enc. mult. factor"));
                  Draw_Option(temp_val.jerkmult, num_mult, row);
                }
                else {
                  temp_val.nummult = MULT_JERK;
                  Modify_Option(temp_val.jerkmult, num_mult, 2);
                }
              break;
            #endif
          }
          break;
      #endif
      #if HAS_JUNCTION_DEVIATION
        case JDmenu:

          #define JD_BACK 0
          #define JD_HOME (JD_BACK + DISABLED(HAS_RC_CPU))
          #define JD_SETTING_JD_MM (JD_HOME + ENABLED(HAS_HOTEND))
          #define JD_TOTAL JD_SETTING_JD_MM

          switch (item) {
            case JD_BACK:
              if (draw)
                Draw_Menu_Item(row, ICON_Back, GET_TEXT_F(MSG_BACK));
              else
                Draw_Menu(Motion, MOTION_JD);
              break;
            #if !HAS_RC_CPU
              case JD_HOME:
                if (draw) Draw_Menu_Item(row, ICON_Back, GET_TEXT_F(MSG_MAIN));
                else Draw_Quick_Home();
                break;
              #endif
            #if HAS_HOTEND
              case JD_SETTING_JD_MM:
                if (draw) {
                  Draw_Menu_Item(row, ICON_MaxJerk, GET_TEXT_F(MSG_JUNCTION_DEVIATION));
                  Draw_Float(planner.junction_deviation_mm, row, false, 100);
                }
                else
                  Modify_Value(planner.junction_deviation_mm, MIN_JD_MM, MAX_JD_MM, 100);
                break;
              #endif
          }
          break;
      #endif
      case Steps:

        #define STEPS_BACK 0
        #define STEPS_HOME (STEPS_BACK + DISABLED(HAS_RC_CPU))
        #define STEPS_X (STEPS_HOME + 1)
        #define STEPS_Y (STEPS_X + 1)
        #define STEPS_Z (STEPS_Y + 1)
        #define STEPS_E_CALIBRATION (STEPS_Z + ENABLED(HAS_HOTEND, HAS_E_CALIBRATION))
        #define STEPS_E (STEPS_E_CALIBRATION + ENABLED(HAS_HOTEND))
        #define STEPS_MULT (STEPS_E + DISABLED(HAS_RC_CPU))
        #define STEPS_TOTAL STEPS_MULT

        switch (item) {
          case STEPS_BACK:
            if (draw)
              Draw_Menu_Item(row, ICON_Back, GET_TEXT_F(MSG_BACK));
            else
              Draw_Menu(Motion, MOTION_STEPS);
            break;
          #if !HAS_RC_CPU
            case STEPS_HOME:
              if (draw) Draw_Menu_Item(row, ICON_Back, GET_TEXT_F(MSG_MAIN));
              else Draw_Quick_Home();
              break;
          #endif
          case STEPS_X:
            if (draw) {
              Draw_Menu_Item(row, ICON_StepX, GET_TEXT_F(MSG_A_STEPS));
              Draw_Float(planner.settings.axis_steps_per_mm[X_AXIS], row, false, STEPS_UNIT);
            }
            else {
              #if !HAS_RC_CPU
                temp_val.flag_multiplicator = true;
              #endif
              Modify_Value(planner.settings.axis_steps_per_mm[X_AXIS], min_steps_edit_values[X_AXIS], max_steps_edit_values[X_AXIS], STEPS_UNIT);
            }
            break;
          case STEPS_Y:
            if (draw) {
              Draw_Menu_Item(row, ICON_StepY, GET_TEXT_F(MSG_B_STEPS));
              Draw_Float(planner.settings.axis_steps_per_mm[Y_AXIS], row, false, STEPS_UNIT);
            }
            else {
              #if !HAS_RC_CPU
                temp_val.flag_multiplicator = true;
              #endif
              Modify_Value(planner.settings.axis_steps_per_mm[Y_AXIS], min_steps_edit_values[Y_AXIS], max_steps_edit_values[Y_AXIS], STEPS_UNIT);
            }
            break;
          case STEPS_Z:
            if (draw) {
              Draw_Menu_Item(row, ICON_StepZ, GET_TEXT_F(MSG_C_STEPS));
              Draw_Float(planner.settings.axis_steps_per_mm[Z_AXIS], row, false, STEPS_UNIT);
            }
            else {
              #if !HAS_RC_CPU
                temp_val.flag_multiplicator = true;
              #endif
              Modify_Value(planner.settings.axis_steps_per_mm[Z_AXIS], min_steps_edit_values[Z_AXIS], max_steps_edit_values[Z_AXIS], STEPS_UNIT);
            }
            break;
          #if HAS_HOTEND
            #if HAS_E_CALIBRATION
              case STEPS_E_CALIBRATION:
                if (draw)
                  Draw_Menu_Item(row, ICON_StepE, GET_TEXT_F(MSG_E_CALIBRATION_TOOL));
                else
                  DWIN_E_Calibration();
                break;
            #endif
            case STEPS_E:
              if (draw) {
                Draw_Menu_Item(row, ICON_StepE, GET_TEXT_F(MSG_E_STEPS));
                Draw_Float(planner.settings.axis_steps_per_mm[E_AXIS], row, false, STEPS_UNIT);
              }
              else {
                #if !HAS_RC_CPU
                  temp_val.flag_multiplicator = true;
                #endif
                Modify_Value(planner.settings.axis_steps_per_mm[E_AXIS], min_steps_edit_values[E_AXIS], max_steps_edit_values[E_AXIS], STEPS_UNIT);
              }
              break;
          #endif
          #if !HAS_RC_CPU
            case STEPS_MULT:
              if (draw) {
                Draw_Menu_Item(row, ICON_Homing, F("Enc. mult. factor"));
                Draw_Option(temp_val.stepsmult, num_mult, row);
              }
              else {
                temp_val.nummult = MULT_STEPS;
                Modify_Option(temp_val.stepsmult, num_mult, 2);
              }
            break;
          #endif
        }
        break;

      #if ANY(CASE_LIGHT_MENU, LED_CONTROL_MENU)
        case Ledsmenu:

          #define LEDS_BACK 0
          #define LEDS_CASELIGHT (LEDS_BACK + ENABLED(CASE_LIGHT_MENU))
          #define LEDS_LED_CONTROL_MENU (LEDS_CASELIGHT + ENABLED(LED_CONTROL_MENU))
          #define LEDS_TOTAL LEDS_LED_CONTROL_MENU

          switch (item) {

          case LEDS_BACK:
            if (draw)
              Draw_Menu_Item(row, ICON_Back, GET_TEXT_F(MSG_BACK));
            else
              Draw_Menu(Control, CONTROL_LEDS);
            break;
          #if ENABLED(CASE_LIGHT_MENU)
            case LEDS_CASELIGHT:
              if (draw) {
                #if ENABLED(CASELIGHT_USES_BRIGHTNESS)
                  Draw_Menu_Item(row, ICON_CaseLight, GET_TEXT_F(MSG_CASE_LIGHT), nullptr, true); 
                #else
                  Draw_Menu_Item(row, ICON_CaseLight, GET_TEXT_F(MSG_CASE_LIGHT));
                  Draw_Checkbox(row, caselight.on);
                #endif
              }
              else {
                #if ENABLED(CASELIGHT_USES_BRIGHTNESS)
                  Draw_Menu(CaseLightmenu);
                #else
                  caselight.on = !caselight.on;
                  caselight.update_enabled();
                  Draw_Checkbox(row, caselight.on);
                  DWIN_UpdateLCD();
                #endif
              }
              break;
          #endif
          #if ENABLED(LED_CONTROL_MENU)
            case LEDS_LED_CONTROL_MENU:
              if (draw)
                Draw_Menu_Item(row, ICON_LedControl, GET_TEXT_F(MSG_LED_CONTROL), nullptr, true); 
              else 
                Draw_Menu(LedControlmenu);
              break;
          #endif
          }
        break;
      #endif

      #if ENABLED(CASE_LIGHT_MENU, CASELIGHT_USES_BRIGHTNESS)
        case CaseLightmenu:

          #define CASE_LIGHT_BACK 0
          #define CASE_LIGHT_ON (CASE_LIGHT_BACK + 1)
          #define CASE_LIGHT_USES_BRIGHT (CASE_LIGHT_ON + 1)
          #define CASE_LIGHT_TOTAL CASE_LIGHT_USES_BRIGHT

          switch (item) {

            case CASE_LIGHT_BACK:
              if (draw)
                Draw_Menu_Item(row, ICON_Back, GET_TEXT_F(MSG_BACK));
              else
                Draw_Menu(Ledsmenu, LEDS_CASELIGHT);
              break;
            case CASE_LIGHT_ON:
              if (draw) {
                  Draw_Menu_Item(row, ICON_CaseLight, GET_TEXT_F(MSG_CASE_LIGHT));
                  Draw_Checkbox(row, caselight.on);
              }
              else {
                  caselight.on = !caselight.on;
                  caselight.update_enabled();
                  Draw_Checkbox(row, caselight.on);
                  DWIN_UpdateLCD();
              }
              break;
            case CASE_LIGHT_USES_BRIGHT:
              if (draw) {
                Draw_Menu_Item(row, ICON_Brightness, GET_TEXT_F(MSG_CASE_LIGHT_BRIGHTNESS));
                Draw_Float(caselight.brightness, row, false, 1);
              }
              else
                Modify_Value(caselight.brightness, 0, 255, 1);
              break;
          }
        break;
      #endif

      #if ENABLED(LED_CONTROL_MENU)
        case LedControlmenu:

          #define LEDCONTROL_BACK 0
          #define LEDCONTROL_LIGHTON (LEDCONTROL_BACK + !BOTH(CASE_LIGHT_MENU, CASE_LIGHT_USE_NEOPIXEL))
          #define LEDCONTROL_PRESETS_MENU (LEDCONTROL_LIGHTON + BOTH(HAS_COLOR_LEDS, LED_COLOR_PRESETS))
          #define LEDCONTROL_CUSTOM_MENU (LEDCONTROL_PRESETS_MENU + ENABLED(HAS_COLOR_LEDS))
          #define LEDCONTROL_TOTAL LEDCONTROL_CUSTOM_MENU

          switch (item) {
            case LEDCONTROL_BACK:
              if (draw)
                Draw_Menu_Item(row, ICON_Back, GET_TEXT_F(MSG_BACK));
              else
                Draw_Menu(Ledsmenu, LEDS_LED_CONTROL_MENU);
              break;
            #if !BOTH(CASE_LIGHT_MENU, CASE_LIGHT_USE_NEOPIXEL)
              case LEDCONTROL_LIGHTON:
                if (draw) {
                  Draw_Menu_Item(row, ICON_LedControl, GET_TEXT_F(MSG_LEDS));
                  Draw_Checkbox(row, leds.lights_on);
                }
                else {
                  leds.toggle();
                  Draw_Checkbox(row, leds.lights_on);
                  DWIN_UpdateLCD();
                }
               break; 
            #endif
            #if HAS_COLOR_LEDS
              #if ENABLED(LED_COLOR_PRESETS)
                case LEDCONTROL_PRESETS_MENU:
                    if (draw)
                      Draw_Menu_Item(row, ICON_LedControl, GET_TEXT_F(MSG_LED_PRESETS));
                    else 
                      Draw_Menu(LedControlpresets);
                  break;
                #endif
              case LEDCONTROL_CUSTOM_MENU:
                if (draw)
                  Draw_Menu_Item(row, ICON_LedControl, GET_TEXT_F(MSG_CUSTOM_LEDS));
                else 
                  Draw_Menu(LedControlcustom);
                break;
            #endif
          }
          break;

          #if HAS_COLOR_LEDS
            #if ENABLED(LED_COLOR_PRESETS)
              case LedControlpresets:

                #define LEDCONTROL_PRESETS_BACK 0
                #define LEDCONTROL_PRESETS_WHITE (LEDCONTROL_PRESETS_BACK + 1)
                #define LEDCONTROL_PRESETS_RED (LEDCONTROL_PRESETS_WHITE + 1)
                #define LEDCONTROL_PRESETS_ORANGE (LEDCONTROL_PRESETS_RED + 1)
                #define LEDCONTROL_PRESETS_YELLOW (LEDCONTROL_PRESETS_ORANGE + 1)
                #define LEDCONTROL_PRESETS_GREEN (LEDCONTROL_PRESETS_YELLOW + 1)
                #define LEDCONTROL_PRESETS_BLUE (LEDCONTROL_PRESETS_GREEN + 1)
                #define LEDCONTROL_PRESETS_INDIGO (LEDCONTROL_PRESETS_BLUE + 1)
                #define LEDCONTROL_PRESETS_VIOLET (LEDCONTROL_PRESETS_INDIGO + 1)
                #define LEDCONTROL_PRESETS_TOTAL LEDCONTROL_PRESETS_VIOLET

                switch (item) {
                  case LEDCONTROL_PRESETS_BACK:
                    if (draw)
                      Draw_Menu_Item(row, ICON_Back, GET_TEXT_F(MSG_BACK));
                    else
                      Draw_Menu(LedControlmenu, LEDCONTROL_PRESETS_MENU);
                    break;
                  case LEDCONTROL_PRESETS_WHITE:
                    if (draw)
                      Draw_Menu_Item(row, ICON_LedControl, GET_TEXT_F(MSG_SET_LEDS_WHITE));
                    else   
                      leds.set_white();
                    break;
                  case LEDCONTROL_PRESETS_RED:
                    if (draw)
                      Draw_Menu_Item(row, ICON_LedControl, GET_TEXT_F(MSG_SET_LEDS_RED));
                    else   
                      leds.set_red();
                    break;
                  case LEDCONTROL_PRESETS_ORANGE:
                    if (draw)
                      Draw_Menu_Item(row, ICON_LedControl, GET_TEXT_F(MSG_SET_LEDS_ORANGE));
                    else   
                      leds.set_orange();
                    break;
                  case LEDCONTROL_PRESETS_YELLOW:
                    if (draw)
                      Draw_Menu_Item(row, ICON_LedControl, GET_TEXT_F(MSG_SET_LEDS_YELLOW));
                    else   
                      leds.set_yellow();
                    break;
                  case LEDCONTROL_PRESETS_GREEN:
                    if (draw)
                      Draw_Menu_Item(row, ICON_LedControl, GET_TEXT_F(MSG_SET_LEDS_GREEN));
                    else   
                      leds.set_green();
                    break;
                  case LEDCONTROL_PRESETS_BLUE:
                    if (draw)
                      Draw_Menu_Item(row, ICON_LedControl, GET_TEXT_F(MSG_SET_LEDS_BLUE));
                    else   
                      leds.set_blue();
                    break;
                  case LEDCONTROL_PRESETS_INDIGO:
                    if (draw)
                      Draw_Menu_Item(row, ICON_LedControl, GET_TEXT_F(MSG_SET_LEDS_INDIGO));
                    else   
                      leds.set_indigo();
                    break;
                  case LEDCONTROL_PRESETS_VIOLET:
                    if (draw)
                      Draw_Menu_Item(row, ICON_LedControl, GET_TEXT_F(MSG_SET_LEDS_VIOLET));
                    else   
                      leds.set_violet();
                    break;
                }
                break;
            #endif

            case LedControlcustom:

              #define LEDCONTROL_CUSTOM_BACK 0
              #define LEDCONTROL_CUSTOM_RED (LEDCONTROL_CUSTOM_BACK + 1)
              #define LEDCONTROL_CUSTOM_GREEN (LEDCONTROL_CUSTOM_RED + 1)
              #define LEDCONTROL_CUSTOM_BLUE (LEDCONTROL_CUSTOM_GREEN + 1)
              #define LEDCONTROL_CUSTOM_WHITE (LEDCONTROL_CUSTOM_BLUE + ENABLED(HAS_WHITE_LED))
              #define LEDCONTROL_CUSTOM_TOTAL LEDCONTROL_CUSTOM_WHITE

              switch (item) {
                case LEDCONTROL_PRESETS_BACK:
                  if (draw)
                    Draw_Menu_Item(row, ICON_Back, GET_TEXT_F(MSG_BACK));
                  else
                    Draw_Menu(LedControlmenu, LEDCONTROL_CUSTOM_MENU);
                  break;
                case LEDCONTROL_CUSTOM_RED:
                  if (draw) {
                    Draw_Menu_Item(row, ICON_LedControl, GET_TEXT_F(MSG_INTENSITY_R));
                    Draw_Float(leds.color.r, row, false, 1);
                  }
                  else
                    Modify_Value(leds.color.r, 0, 255, 1);
                  break;
                case LEDCONTROL_CUSTOM_GREEN:
                  if (draw) {
                    Draw_Menu_Item(row, ICON_LedControl, GET_TEXT_F(MSG_INTENSITY_G));
                    Draw_Float(leds.color.g, row, false, 1);
                  }
                  else
                    Modify_Value(leds.color.g, 0, 255, 1);
                  break;
                case LEDCONTROL_CUSTOM_BLUE:
                  if (draw) {
                    Draw_Menu_Item(row, ICON_LedControl, GET_TEXT_F(MSG_INTENSITY_B));
                    Draw_Float(leds.color.b, row, false, 1);
                  }
                  else
                    Modify_Value(leds.color.b, 0, 255, 1);
                  break;
                #if HAS_WHITE_LED
                  case LEDCONTROL_CUSTOM_WHITE:
                    if (draw) {
                      Draw_Menu_Item(row, ICON_LedControl, GET_TEXT_F(MSG_INTENSITY_W));
                      Draw_Float(leds.color.w, row, false, 1);
                    }
                    else
                      Modify_Value(leds.color.w, 0, 255, 1);
                    break;
                #endif
              }
              break;
          #endif
      #endif

      case Visual:

        #define VISUAL_BACK 0
        #define VISUAL_HOME (VISUAL_BACK + DISABLED(HAS_RC_CPU))
        #define VISUAL_BACKLIGHT (VISUAL_HOME + 1)
        #define VISUAL_BRIGHTNESS (VISUAL_BACKLIGHT + 1)
        #define VISUAL_AUTOOFF (VISUAL_BRIGHTNESS + HAS_BACKLIGHT_TIMEOUT)
        #define VISUAL_REFRESHDISPLAY (VISUAL_AUTOOFF + ENABLED(CPU_STM32G0B1))
        #define VISUAL_FAN_PERCENT (VISUAL_REFRESHDISPLAY + ENABLED(HAS_FAN))
        #define VISUAL_LEV_BLINKING (VISUAL_FAN_PERCENT + ENABLED(HAS_LEV_BLINKING))
        #define VISUAL_SHOW_REAL_POS (VISUAL_LEV_BLINKING + ENABLED(HAS_SHOW_REAL_POS_MENU))
        #define VISUAL_TIME_FORMAT (VISUAL_SHOW_REAL_POS + 1)
        #define VISUAL_MAIN_ICONS (VISUAL_TIME_FORMAT + ENABLED(DWIN_CREALITY_LCD_CUSTOM_ICONS))
        #define VISUAL_THEME_RED_BLUE (VISUAL_MAIN_ICONS + ENABLED(DWIN_CREALITY_LCD_CUSTOM_ICONS))
        #define VISUAL_COLOR_THEMES (VISUAL_THEME_RED_BLUE + 1)
        #define VISUAL_SHORTCUTS_MODES (VISUAL_COLOR_THEMES + ENABLED(HAS_SHORTCUTS))
        #define VISUAL_FILE_TUMBNAILS (VISUAL_SHORTCUTS_MODES + (ENABLED(HAS_GCODE_PREVIEW) && ENABLED(HAS_NORMAL_GCODE_PREVIEW)))
        #define VISUAL_TOTAL VISUAL_FILE_TUMBNAILS

        switch (item) {
          case VISUAL_BACK:
            if (draw)
              Draw_Menu_Item(row, ICON_Back, GET_TEXT_F(MSG_BACK));
            else
              Draw_Menu(Control, CONTROL_VISUAL);
            break;
          #if !HAS_RC_CPU
            case VISUAL_HOME:
              if (draw) Draw_Menu_Item(row, ICON_Back, GET_TEXT_F(MSG_MAIN));
              else Draw_Quick_Home();
              break;
          #endif
          case VISUAL_BACKLIGHT:
            if (draw)
              Draw_Menu_Item(row, ICON_Brightness, GET_TEXT_F(MSG_BRIGHTNESS_OFF));
            else
              ui.set_brightness(0);
            break;
          case VISUAL_BRIGHTNESS:
            if (draw) {
              Draw_Menu_Item(row, ICON_Brightness, GET_TEXT_F(MSG_BRIGHTNESS));
              Draw_Float(ui.brightness, row, false, 1);
            }
            else
              Modify_Value(ui.brightness, LCD_BRIGHTNESS_MIN, LCD_BRIGHTNESS_MAX, 1, ui.refresh_brightness);
            break;
          #if HAS_BACKLIGHT_TIMEOUT
            case VISUAL_AUTOOFF:
              if (draw) {
                Draw_Menu_Item(row, ICON_Brightness, GET_TEXT_F(MSG_LCD_TIMEOUT_SEC));
                Draw_Float(ui.lcd_backlight_timeout, row, false, 1);
              }
              else
                Modify_Value(ui.lcd_backlight_timeout, LCD_BKL_TIMEOUT_MIN, LCD_BKL_TIMEOUT_MAX, 1, ui.refresh_backlight_timeout);
              break;
          #endif
          #if ENABLED(CPU_STM32G0B1)
            case VISUAL_REFRESHDISPLAY:
              if (draw) {
                Draw_Menu_Item(row, ICON_Brightness, F("Refresh LCD time"));
                Draw_Float(temp_val.refreshtimevalue, row, false, 1);
              }
              else
                Modify_Value(temp_val.refreshtimevalue, 15, 3600, 1);
              break;
          #endif
          #if HAS_FAN
            case VISUAL_FAN_PERCENT:
              if (draw) {
                Draw_Menu_Item(row, ICON_FanSpeed, GET_TEXT_F(MSG_FAN_SPEED_PERCENT));
                Draw_Checkbox(row, HMI_datas.fan_percent);
              }
              else {
                HMI_datas.fan_percent = !HMI_datas.fan_percent;
                Draw_Checkbox(row, HMI_datas.fan_percent);
                #if HAS_FAN
                  temp_val.force_restore_fan = true;
                #endif
                Redraw_Screen();
              }
              break;
          #endif
          #if ENABLED(HAS_LEV_BLINKING)
            case VISUAL_LEV_BLINKING:
              if (draw) {
                Draw_Menu_Item(row, ICON_Zoffset, F("Blink LEV Ind."));
                Draw_Checkbox(row, HMI_datas.lev_blinking);
              }
              else {
                HMI_datas.lev_blinking = !HMI_datas.lev_blinking;
                Draw_Checkbox(row, HMI_datas.lev_blinking);
                Redraw_Screen();
              }
              break;
          #endif
          #if ENABLED(HAS_SHOW_REAL_POS_MENU)
            case VISUAL_SHOW_REAL_POS:
              if (draw) {
                Draw_Menu_Item(row, ICON_Step, F("Show real pos."));
                Draw_Checkbox(row, HMI_datas.show_real_pos);
              }
              else {
                HMI_datas.show_real_pos = !HMI_datas.show_real_pos;
                Draw_Checkbox(row, HMI_datas.show_real_pos);
                Redraw_Screen();
              }
              break;
          #endif
          case VISUAL_TIME_FORMAT:
            if (draw) {
              Draw_Menu_Item(row, ICON_PrintTime, GET_TEXT_F(MSG_PROGRESS_IN_HHMM));
              Draw_Checkbox(row, HMI_datas.time_format_textual);
            }
            else {
              HMI_datas.time_format_textual = !HMI_datas.time_format_textual;
              Draw_Checkbox(row, HMI_datas.time_format_textual);
            }
            break;
          #if ENABLED(DWIN_CREALITY_LCD_CUSTOM_ICONS)
            case VISUAL_MAIN_ICONS:
              if (draw) {
                Draw_Menu_Item(row, ICON_MaxSpeed, GET_TEXT(MSG_MAIN_ICONS));
                Draw_Option(temp_val.iconmain, icon_main, row);
              }
              else {
                Modify_Option(temp_val.iconmain, icon_main, 3);
              }
              break;
            case VISUAL_THEME_RED_BLUE:
              if (draw) {
                Draw_Menu_Item(row, ICON_MaxSpeed, GET_TEXT_F(MSG_MAIN_THEMES));
                Draw_Checkbox(row, HMI_datas.pack_red);
              }
              else {
                HMI_datas.pack_red = !HMI_datas.pack_red;
                Draw_Checkbox(row, HMI_datas.pack_red);
              }
              break;  
          #endif
          case VISUAL_COLOR_THEMES:
            if (draw)
              Draw_Menu_Item(row, ICON_MaxSpeed, GET_TEXT_F(MSG_COLORS_SELECT), nullptr, true);
            else
              Draw_Menu(ColorSettings);
          break;
          #if HAS_SHORTCUTS
            case VISUAL_SHORTCUTS_MODES:
              if (draw)
                Draw_Menu_Item(row, ICON_Shortcut, GET_TEXT_F(MSG_SHORTCUT_MENU), nullptr, true);
               else
                Draw_Menu(ShortcutSettings);
               break;
          #endif
          #if HAS_GCODE_PREVIEW && HAS_NORMAL_GCODE_PREVIEW
            case VISUAL_FILE_TUMBNAILS:
              if (draw) {
                temp_val.sd_item_flag = false;
                Draw_Menu_Item(row, ICON_File, GET_TEXT_F(MSG_GCODE_THUMBNAILS));
                Draw_Checkbox(row, HMI_datas.show_gcode_thumbnails);
              }
              else {
                HMI_datas.show_gcode_thumbnails = !HMI_datas.show_gcode_thumbnails;
                Draw_Checkbox(row, HMI_datas.show_gcode_thumbnails);
              }
              break;
          #endif
        }
        break;
      
      #if HAS_SHORTCUTS
        case ShortcutSettings:
          #define SHORTCUTSETTINGS_BACK  0
          #define SHORTCUTSETTINGS_HOME  (SHORTCUTSETTINGS_BACK + DISABLED(HAS_RC_CPU))
          #define SHORTCUTSETTINGS_ENABLE  (SHORTCUTSETTINGS_HOME + 1)
          #define SHORTCUTSETTINGS_1     (SHORTCUTSETTINGS_ENABLE + 1)
          #define SHORTCUTSETTINGS_2     (SHORTCUTSETTINGS_1 + 1)
          #define SHORTCUTSETTINGS_3     (SHORTCUTSETTINGS_2 + 1)
          #define SHORTCUTSETTINGS_4     (SHORTCUTSETTINGS_3 + 1)
          #define SHORTCUTSETTINGS_5     (SHORTCUTSETTINGS_4 + 1)
          #define SHORTCUTSETTINGS_6     (SHORTCUTSETTINGS_5 + 1)
          #define SHORTCUTSETTINGS_7     (SHORTCUTSETTINGS_6 + 1)
          #define SHORTCUTSETTINGS_8     (SHORTCUTSETTINGS_7 + 1)
          #define SHORTCUTSETTINGS_9     (SHORTCUTSETTINGS_8 + 1)
          #define SHORTCUTSETTINGS_10    (SHORTCUTSETTINGS_9 + 1)
          #define SHORTCUTSETTINGS_TOTAL SHORTCUTSETTINGS_10

          switch (item) {
            case SHORTCUTSETTINGS_BACK:
              if (draw)
                Draw_Menu_Item(row, ICON_Back, GET_TEXT_F(MSG_BACK));
              else
                Draw_Menu(Visual, VISUAL_SHORTCUTS_MODES);
              break;
            #if !HAS_RC_CPU
              case SHORTCUTSETTINGS_HOME:
                if (draw) Draw_Menu_Item(row, ICON_Back, GET_TEXT_F(MSG_MAIN));
                else Draw_Quick_Home();
                break;
            #endif

            case SHORTCUTSETTINGS_ENABLE:
              if (draw) {
                Draw_Menu_Item(row, ICON_Shortcut, GET_TEXT(MSG_MODE_SHORTCUT));
                Draw_Checkbox(row, temp_val.shortcutsmode);
              }
              else {
                temp_val.shortcutsmode = !temp_val.shortcutsmode;
                Init_Main(); 
                Draw_Checkbox(row, temp_val.shortcutsmode);
                Redraw_Menu(false);
              }
              break;

            #define _SHORTCUT_CASE(N) \
              case SHORTCUTSETTINGS_##N: { \
                if (draw) { \
                  sprintf_P(cmd, PSTR("%s #%s"), GET_TEXT(MSG_SHORTCUT), STRINGIFY(N)); \
                  Draw_Menu_Item(row, ICON_Shortcut, F(cmd)); \
                  Draw_Option(temp_val.shortcut[N - 1], shortcut_list, row); \
                } else { \
                  Modify_Option(temp_val.shortcut[N - 1], shortcut_list, NB_Shortcuts); \
                } \
            } break;

          REPEAT_1(NB_Shortcuts, _SHORTCUT_CASE)
          }
        break;      
      #endif

      case ColorSettings:

        #define COLORSETTINGS_BACK 0
        #define COLORSETTINGS_HOME (COLORSETTINGS_BACK + DISABLED(HAS_RC_CPU))
        #define COLORSETTINGS_CURSOR (COLORSETTINGS_HOME + 1)
        #define COLORSETTINGS_SPLIT_LINE (COLORSETTINGS_CURSOR + 1)
        #define COLORSETTINGS_ITEMS_MENU_TEXT (COLORSETTINGS_SPLIT_LINE + 1)
        #define COLORSETTINGS_ICONS_MENU_TEXT (COLORSETTINGS_ITEMS_MENU_TEXT + 1)
        #define COLORSETTINGS_BACKGROUND (COLORSETTINGS_ICONS_MENU_TEXT + DISABLED(HAS_TJC_DISPLAY))
        #define COLORSETTINGS_MENU_TOP_TXT (COLORSETTINGS_BACKGROUND + 1)
        #define COLORSETTINGS_MENU_TOP_BG (COLORSETTINGS_MENU_TOP_TXT + 1)
        #define COLORSETTINGS_SELECT_TXT (COLORSETTINGS_MENU_TOP_BG + 1)
        #define COLORSETTINGS_SELECT_BG (COLORSETTINGS_SELECT_TXT + 1)
        #define COLORSETTINGS_HIGHLIGHT_BORDER (COLORSETTINGS_SELECT_BG + 1)
        #define COLORSETTINGS_POPUP_HIGHLIGHT (COLORSETTINGS_HIGHLIGHT_BORDER + 1)
        #define COLORSETTINGS_POPUP_TXT (COLORSETTINGS_POPUP_HIGHLIGHT + 1)
        #define COLORSETTINGS_POPUP_BG (COLORSETTINGS_POPUP_TXT + 1)
        #define COLORSETTINGS_ICON_CONFIRM_TXT (COLORSETTINGS_POPUP_BG + 1)
        #define COLORSETTINGS_ICON_CONFIRM_BG (COLORSETTINGS_ICON_CONFIRM_TXT + 1)
        #define COLORSETTINGS_ICON_CANCEL_TXT (COLORSETTINGS_ICON_CONFIRM_BG + 1)
        #define COLORSETTINGS_ICON_CANCEL_BG (COLORSETTINGS_ICON_CANCEL_TXT + 1)
        #define COLORSETTINGS_ICON_CONTINUE_TXT (COLORSETTINGS_ICON_CANCEL_BG + 1)
        #define COLORSETTINGS_ICON_CONTINUE_BG (COLORSETTINGS_ICON_CONTINUE_TXT + 1)
        #define COLORSETTINGS_PRINT_SCREEN_TXT (COLORSETTINGS_ICON_CONTINUE_BG + 1)
        #define COLORSETTINGS_PRINT_FILENAME (COLORSETTINGS_PRINT_SCREEN_TXT + 1)
        #define COLORSETTINGS_PROGRESS_BAR (COLORSETTINGS_PRINT_FILENAME + 1)
        #define COLORSETTINGS_PROGRESS_PERCENT (COLORSETTINGS_PROGRESS_BAR + 1)
        #define COLORSETTINGS_REMAIN_TIME (COLORSETTINGS_PROGRESS_PERCENT + 1)
        #define COLORSETTINGS_ELAPSED_TIME (COLORSETTINGS_REMAIN_TIME + 1)
        #define COLORSETTINGS_PROGRESS_STATUS_BAR (COLORSETTINGS_ELAPSED_TIME + 1)
        #define COLORSETTINGS_PROGRESS_STATUS_AREA (COLORSETTINGS_PROGRESS_STATUS_BAR + 1)
        #define COLORSETTINGS_PROGRESS_STATUS_PERCENT (COLORSETTINGS_PROGRESS_STATUS_AREA + 1)
        #define COLORSETTINGS_PROGRESS_COORDINATES (COLORSETTINGS_PROGRESS_STATUS_PERCENT + 1)
        #define COLORSETTINGS_PROGRESS_COORDINATES_LINE (COLORSETTINGS_PROGRESS_COORDINATES + 1)
        #define COLORSETTINGS_TOTAL COLORSETTINGS_PROGRESS_COORDINATES_LINE

        switch (item) {
          case COLORSETTINGS_BACK:
            if (draw)
              Draw_Menu_Item(row, ICON_Back, GET_TEXT_F(MSG_BACK));
            else
              Draw_Menu(Visual, VISUAL_COLOR_THEMES);
            break;
          #if !HAS_RC_CPU
            case COLORSETTINGS_HOME:
              if (draw) Draw_Menu_Item(row, ICON_Back, GET_TEXT_F(MSG_MAIN));
              else Draw_Quick_Home();
              break;
          #endif
          case COLORSETTINGS_CURSOR:
            if (draw) {
              Draw_Menu_Item(row, ICON_MaxSpeed, F("Cursor"));
              Draw_Option(HMI_datas.cursor_color, color_names, row, false, true);
            }
            else
              Modify_Option(HMI_datas.cursor_color, color_names, Custom_Colors);
            break;
          case COLORSETTINGS_SPLIT_LINE:
            if (draw) {
              Draw_Menu_Item(row, ICON_MaxSpeed, F("Menu Split Line"));
              Draw_Option(HMI_datas.menu_split_line, color_names, row, false, true);
            }
            else
              Modify_Option(HMI_datas.menu_split_line, color_names, Custom_Colors);
            break;
          case COLORSETTINGS_ITEMS_MENU_TEXT:
              if (draw) {
                Draw_Menu_Item(row, ICON_MaxSpeed, F("Item Menu Text"));
                Draw_Option(HMI_datas.items_menu_text, color_names, row, false, true);
              }
              else {
                Modify_Option(HMI_datas.items_menu_text, color_names, Custom_Colors);
              }
              break;
            case COLORSETTINGS_ICONS_MENU_TEXT:
              if (draw) {
                Draw_Menu_Item(row, ICON_MaxSpeed, F("Icon Menu Text"));
                Draw_Option(HMI_datas.icons_menu_text, color_names, row, false, true);
              }
              else {
                Modify_Option(HMI_datas.icons_menu_text, color_names, Custom_Colors);
              }
              break;
            #if !HAS_TJC_DISPLAY
              case COLORSETTINGS_BACKGROUND:
                if (draw) {
                  Draw_Menu_Item(row, ICON_MaxSpeed, F("Background"));
                  Draw_Option(HMI_datas.background, color_names, row, false, true);
                }
                else {
                  Modify_Option(HMI_datas.background, color_names, Custom_Colors);
                }
                break;
            #endif
            case COLORSETTINGS_MENU_TOP_TXT:
              if (draw) {
                Draw_Menu_Item(row, ICON_MaxSpeed, F("Menu Header Text"));
                Draw_Option(HMI_datas.menu_top_txt, color_names, row, false, true);
              }
              else
                Modify_Option(HMI_datas.menu_top_txt, color_names, Custom_Colors);
              break;
            case COLORSETTINGS_MENU_TOP_BG:
              if (draw) {
                Draw_Menu_Item(row, ICON_MaxSpeed, F("Menu Header Bg"));
                Draw_Option(HMI_datas.menu_top_bg, color_names, row, false, true);
              }
              else
                Modify_Option(HMI_datas.menu_top_bg, color_names, Custom_Colors);
              break;
            case COLORSETTINGS_SELECT_TXT:
              if (draw) {
                Draw_Menu_Item(row, ICON_MaxSpeed, F("Select Text"));
                Draw_Option(HMI_datas.select_txt, color_names, row, false, true);
              }
              else
                Modify_Option(HMI_datas.select_txt, color_names, Custom_Colors);
              break;
            case COLORSETTINGS_SELECT_BG:
              if (draw) {
                Draw_Menu_Item(row, ICON_MaxSpeed, F("Select Bg"));
                Draw_Option(HMI_datas.select_bg, color_names, row, false, true);
              }
              else
                Modify_Option(HMI_datas.select_bg, color_names, Custom_Colors);
              break;
            case COLORSETTINGS_HIGHLIGHT_BORDER:
              if (draw) {
                Draw_Menu_Item(row, ICON_MaxSpeed, F("Highlight Box"));
                Draw_Option(HMI_datas.highlight_box, color_names, row, false, true);
              }
              else
                Modify_Option(HMI_datas.highlight_box, color_names, Custom_Colors);
              break;
            case COLORSETTINGS_POPUP_HIGHLIGHT:
              if (draw) {
                Draw_Menu_Item(row, ICON_MaxSpeed, F("Popup Highlight"));
                Draw_Option(HMI_datas.popup_highlight, color_names, row, false, true);
              }
              else {
                Modify_Option(HMI_datas.popup_highlight, color_names, Custom_Colors);
              }
              break;
            case COLORSETTINGS_POPUP_TXT:
              if (draw) {
                Draw_Menu_Item(row, ICON_MaxSpeed, F("Popup Text"));
                Draw_Option(HMI_datas.popup_txt, color_names, row, false, true);
              }
              else {
                Modify_Option(HMI_datas.popup_txt, color_names, Custom_Colors);
              }
              break;
            case COLORSETTINGS_POPUP_BG:
              if (draw) {
                Draw_Menu_Item(row, ICON_MaxSpeed, F("Popup Bg"));
                Draw_Option(HMI_datas.popup_bg, color_names, row, false, true);
              }
              else {
                Modify_Option(HMI_datas.popup_bg, color_names, Custom_Colors);
              }
              break;
            case COLORSETTINGS_ICON_CONFIRM_TXT:
              if (draw) {
                Draw_Menu_Item(row, ICON_MaxSpeed, F("Confirm Icon Txt"));
                Draw_Option(HMI_datas.ico_confirm_txt, color_names, row, false, true);
              }
              else {
              Modify_Option(HMI_datas.ico_confirm_txt, color_names, Custom_Colors);
              }
              break;
            case COLORSETTINGS_ICON_CONFIRM_BG:
              if (draw) {
                Draw_Menu_Item(row, ICON_MaxSpeed, F("Confirm Icon Bg"));
                Draw_Option(HMI_datas.ico_confirm_bg, color_names, row, false, true);
              }
              else {
              Modify_Option(HMI_datas.ico_confirm_bg, color_names, Custom_Colors);
              }
              break;
            case COLORSETTINGS_ICON_CANCEL_TXT:
              if (draw) {
                Draw_Menu_Item(row, ICON_MaxSpeed, F("Cancel Icon Text"));
                Draw_Option(HMI_datas.ico_cancel_txt, color_names, row, false, true);
              }
              else {
              Modify_Option(HMI_datas.ico_cancel_txt, color_names, Custom_Colors);
              }
              break;
            case COLORSETTINGS_ICON_CANCEL_BG:
              if (draw) {
                Draw_Menu_Item(row, ICON_MaxSpeed, F("Cancel Icon Bg"));
                Draw_Option(HMI_datas.ico_cancel_bg, color_names, row, false, true);
              }
              else {
              Modify_Option(HMI_datas.ico_cancel_bg, color_names, Custom_Colors);
              }
              break;
            case COLORSETTINGS_ICON_CONTINUE_TXT:
              if (draw) {
                Draw_Menu_Item(row, ICON_MaxSpeed, F("Continue Ico Txt"));
                Draw_Option(HMI_datas.ico_continue_txt, color_names, row, false, true);
              }
              else {
              Modify_Option(HMI_datas.ico_continue_txt, color_names, Custom_Colors);
              }
              break;
            case COLORSETTINGS_ICON_CONTINUE_BG:
              if (draw) {
                Draw_Menu_Item(row, ICON_MaxSpeed, F("Continue Ico Bg"));
                Draw_Option(HMI_datas.ico_continue_bg, color_names, row, false, true);
              }
              else {
              Modify_Option(HMI_datas.ico_continue_bg, color_names, Custom_Colors);
              }
              break;
            case COLORSETTINGS_PRINT_SCREEN_TXT:
              if (draw) {
                Draw_Menu_Item(row, ICON_MaxSpeed, F("Print Screen Txt"));
                Draw_Option(HMI_datas.print_screen_txt, color_names, row, false, true);
              }
              else {
                Modify_Option(HMI_datas.print_screen_txt, color_names, Custom_Colors_no_Black);
              }
              break;
            case COLORSETTINGS_PRINT_FILENAME:
              if (draw) {
                Draw_Menu_Item(row, ICON_MaxSpeed, F("Print Filename"));
                Draw_Option(HMI_datas.print_filename, color_names, row, false, true);
              }
              else {
                Modify_Option(HMI_datas.print_filename, color_names, Custom_Colors_no_Black);
              }
              break;
              case COLORSETTINGS_PROGRESS_BAR:
              if (draw) {
                Draw_Menu_Item(row, ICON_MaxSpeed, F("Progress Bar"));
                Draw_Option(HMI_datas.progress_bar, color_names, row, false, true);
              }
              else {
                Modify_Option(HMI_datas.progress_bar, color_names, Custom_Colors);
              }
              break;
            case COLORSETTINGS_PROGRESS_PERCENT:
              if (draw) {
                Draw_Menu_Item(row, ICON_MaxSpeed, F("Progress %"));
                Draw_Option(HMI_datas.progress_percent, color_names, row, false, true);
              }
              else
                Modify_Option(HMI_datas.progress_percent, color_names, Custom_Colors);
              break;
            case COLORSETTINGS_REMAIN_TIME:
              if (draw) {
                Draw_Menu_Item(row, ICON_MaxSpeed, F("Remaining Time"));
                Draw_Option(HMI_datas.remain_time, color_names, row, false, true);
              }
              else
                Modify_Option(HMI_datas.remain_time, color_names, Custom_Colors);
              break;
              case COLORSETTINGS_ELAPSED_TIME:
              if (draw) {
                Draw_Menu_Item(row, ICON_MaxSpeed, F("Elapsed Time"));
                Draw_Option(HMI_datas.elapsed_time, color_names, row, false, true);
              }
              else
                Modify_Option(HMI_datas.elapsed_time, color_names, Custom_Colors);
              break;
            case COLORSETTINGS_PROGRESS_STATUS_BAR:
              if (draw) {
                Draw_Menu_Item(row, ICON_MaxSpeed, F("Status Bar Text"));
                Draw_Option(HMI_datas.status_bar_text, color_names, row, false, true);
              }
              else
                Modify_Option(HMI_datas.status_bar_text, color_names, Custom_Colors);
              break;
            case COLORSETTINGS_PROGRESS_STATUS_AREA:
              if (draw) {
                Draw_Menu_Item(row, ICON_MaxSpeed, F("Status Area Text"));
                Draw_Option(HMI_datas.status_area_text, color_names, row, false, true);
              }
              else
                Modify_Option(HMI_datas.status_area_text, color_names, Custom_Colors);
              break;
            case COLORSETTINGS_PROGRESS_STATUS_PERCENT:
              if (draw) {
                Draw_Menu_Item(row, ICON_MaxSpeed, F("Status Area %"));
                Draw_Option(HMI_datas.status_area_percent, color_names, row, false, true);
              }
              else {
                Modify_Option(HMI_datas.status_area_percent, color_names, Custom_Colors_no_Black);
              }
              break;  
            case COLORSETTINGS_PROGRESS_COORDINATES:
              if (draw) {
                Draw_Menu_Item(row, ICON_MaxSpeed, F("Coordinates Text"));
                Draw_Option(HMI_datas.coordinates_text, color_names, row, false, true);
              }
              else
                Modify_Option(HMI_datas.coordinates_text, color_names, Custom_Colors);
              break;
            case COLORSETTINGS_PROGRESS_COORDINATES_LINE:
              if (draw) {
                Draw_Menu_Item(row, ICON_MaxSpeed, F("Coordinates Line"));
                Draw_Option(HMI_datas.coordinates_split_line, color_names, row, false, true);
              }
              else
                Modify_Option(HMI_datas.coordinates_split_line, color_names, Custom_Colors);
              break;
          } // switch (item)
        break;

      #if ENABLED(HOST_ACTION_COMMANDS, HAS_HOSTACTION_MENUS)
        case HostSettings:

          #define HOSTSETTINGS_BACK 0
          #define HOSTSETTINGS_HOME  (HOSTSETTINGS_BACK + DISABLED(HAS_RC_CPU))
          #define HOSTSETTINGS_ACTIONCOMMANDS (HOSTSETTINGS_HOME + 1)
          #define HOSTSETTINGS_TOTAL HOSTSETTINGS_ACTIONCOMMANDS

          switch (item) {
            case HOSTSETTINGS_BACK:
              if (draw)
                Draw_Menu_Item(row, ICON_Back, GET_TEXT_F(MSG_BACK));
              else
                Draw_Menu(Control, CONTROL_HOSTSETTINGS);
              break;
            #if !HAS_RC_CPU
              case HOSTSETTINGS_HOME:
                if (draw) Draw_Menu_Item(row, ICON_Back, GET_TEXT_F(MSG_MAIN));
                else Draw_Quick_Home();
                break;
            #endif
            case HOSTSETTINGS_ACTIONCOMMANDS:
              if (draw) {
                temp_val.sd_item_flag = false;
                Draw_Menu_Item(row, ICON_File, GET_TEXT_F(MSG_HOST_ACTIONS));
              }
              else {
                Draw_Menu(ActionCommands);
              }
              break;
          }
          break;

        case ActionCommands:

          #define ACTIONCOMMANDS_BACK 0
          #define ACTIONCOMMANDS_HOME (ACTIONCOMMANDS_BACK + DISABLED(HAS_RC_CPU))
          #define ACTIONCOMMANDS_1 (ACTIONCOMMANDS_HOME + 1)
          #define ACTIONCOMMANDS_2 (ACTIONCOMMANDS_1 + 1)
          #define ACTIONCOMMANDS_3 (ACTIONCOMMANDS_2 + 1)
          #define ACTIONCOMMANDS_TOTAL ACTIONCOMMANDS_3

          temp_val.sd_item_flag = false;

          switch (item) {
            case ACTIONCOMMANDS_BACK:
              if (draw) {
                Draw_Menu_Item(row, ICON_Back, GET_TEXT_F(MSG_BACK));
              }
              else {
                Draw_Menu(HostSettings, HOSTSETTINGS_ACTIONCOMMANDS);
              }
              break;
            #if !HAS_RC_CPU
              case ACTIONCOMMANDS_HOME:
                if (draw) Draw_Menu_Item(row, ICON_Back, GET_TEXT_F(MSG_MAIN));
                else Draw_Quick_Home();
                break;
            #endif

            #define _ACTIONCOMMANDS_CASE(N) \
              case ACTIONCOMMANDS_##N: { \
                if (draw) { \
                sprintf_P(cmd, PSTR("Action #%i"), (N)); \
                Draw_Menu_Item(row, ICON_File, F(cmd)); \
                Draw_String(action##N, row); } \
                else Modify_String(action##N, 8, true); \
              } break ;
            REPEAT_1(3, _ACTIONCOMMANDS_CASE)
          }
          break;
      #endif

      case Advanced:

        #define ADVANCED_BACK 0
        #define ADVANCED_HOME (ADVANCED_BACK + DISABLED(HAS_RC_CPU))
        #define ADVANCED_BEEPER (ADVANCED_HOME + ENABLED(SOUND_MENU_ITEM))
        #define ADVANCED_PHYSICAL (ADVANCED_BEEPER + ENABLED(EXTJYERSUI, HAS_PHYSICAL_MENUS))
        #define ADVANCED_PROBE (ADVANCED_PHYSICAL + ENABLED(HAS_BED_PROBE))
        #define ADVANCED_TMC (ADVANCED_PROBE + (ENABLED(HAS_TRINAMIC_CONFIG) && DISABLED(HAS_RC_CPU)))
        #define ADVANCED_CORNER (ADVANCED_TMC + 1)
        #define ADVANCED_LA (ADVANCED_CORNER + ENABLED(LIN_ADVANCE))
        #define ADVANCED_INPUT_SHAPING (ADVANCED_LA + ENABLED(HAS_SHAPING))
        #define ADVANCED_FILMENU (ADVANCED_INPUT_SHAPING + 1)
        #define ADVANCED_SORT_SD (ADVANCED_FILMENU + ALL(SDSUPPORT, SDCARD_SORT_ALPHA, SDSORT_GCODE))
        #define ADVANCED_REPRINT (ADVANCED_SORT_SD + 1)
        #define ADVANCED_POWER_LOSS (ADVANCED_REPRINT + ENABLED(POWER_LOSS_RECOVERY))
        #define ADVANCED_ENDSDIAG (ADVANCED_POWER_LOSS + ENABLED(HAS_ES_DIAG))
        #define ADVANCED_BAUDRATE_MODE (ADVANCED_ENDSDIAG + ENABLED(BAUD_RATE_GCODE))
        #define ADVANCED_SCREENLOCK (ADVANCED_BAUDRATE_MODE + ENABLED(HAS_SCREENLOCK))
        #define ADVANCED_TOTAL ADVANCED_SCREENLOCK

        switch (item) {
          case ADVANCED_BACK:
            if (draw)
              Draw_Menu_Item(row, ICON_Back, GET_TEXT_F(MSG_BACK));
            else
              Draw_Menu(Control, CONTROL_ADVANCED);
            break;
          #if !HAS_RC_CPU
            case ADVANCED_HOME:
              if (draw) Draw_Menu_Item(row, ICON_Back, GET_TEXT_F(MSG_MAIN));
              else Draw_Quick_Home();
              break;
          #endif
          #if ENABLED(SOUND_MENU_ITEM)
            case ADVANCED_BEEPER:
              if (draw) {
                Draw_Menu_Item(row, ICON_Version, GET_TEXT_F(MSG_SOUND_ENABLE));
                Draw_Checkbox(row, ui.sound_on);
              }
              else {
                ui.sound_on = !ui.sound_on;
                Draw_Checkbox(row, ui.sound_on);
              }
              break;
          #endif

          #if EXTJYERSUI && HAS_PHYSICAL_MENUS
            case ADVANCED_PHYSICAL:
              if (draw)
                Draw_Menu_Item(row, ICON_PhySet, GET_TEXT_F(MSG_PHY_SET), nullptr, true);
              else {
                Update_Status("Warning! To increase Bed size, please increase X/Y max position first.");
                Draw_Menu(PhySetMenu);
              }
              break;
          #endif

          #if HAS_BED_PROBE
            case ADVANCED_PROBE:
              if (draw)
                Draw_Menu_Item(row, ICON_Probe, GET_TEXT_F(MSG_ZPROBE_SETTINGS), nullptr, true);
              else
                Draw_Menu(ProbeMenu);
              break;
          #endif

          #if HAS_TRINAMIC_CONFIG && !HAS_RC_CPU
            case ADVANCED_TMC:
              if (draw)
                Draw_Menu_Item(row, ICON_Motion, F("TMC Drivers"), nullptr, true);
              else
                Draw_Menu(TMCMenu);
              break;
          #endif

          case ADVANCED_CORNER:
            if (draw) {
              Draw_Menu_Item(row, ICON_MaxAccelerated, GET_TEXT_F(MSG_BED_SCREW_INSET));
              Draw_Float(temp_val.corner_pos, row, false, 10);
            }
            else
              Modify_Value(temp_val.corner_pos, 1, 100, 10);
            break;

          #if ENABLED(LIN_ADVANCE)
            case ADVANCED_LA:
              if (draw) {
                Draw_Menu_Item(row, ICON_MaxAccelerated, GET_TEXT_F(MSG_ADVANCE_K_E));
                Draw_Float(planner.extruder_advance_K[0], row, false, 100);
              }
              else
                Modify_Value(planner.extruder_advance_K[0], 0, 10, 100);
              break;
          #endif

          #if HAS_SHAPING
            case ADVANCED_INPUT_SHAPING:
              if (draw)
                Draw_Menu_Item(row, ICON_MaxAccelerated, GET_TEXT_F(MSG_INPUT_SHAPING), nullptr, true);
              else
                Draw_Menu(ISmenu);
              break;
          #endif

          case ADVANCED_FILMENU:
              if (draw)
                Draw_Menu_Item(row, ICON_FilSet, GET_TEXT_F(MSG_FILAMENT_SET), nullptr, true);
              else
                Draw_Menu(Filmenu);
              break;
          #if ALL(SDSUPPORT, SDCARD_SORT_ALPHA, SDSORT_GCODE)
            case ADVANCED_SORT_SD:
              if (draw) {
                Draw_Menu_Item(row, ICON_File, GET_TEXT_F(MSG_SORT_SD));
                Draw_Checkbox(row, HMI_datas.sdsort_alpha);
              }
              else {
                temp_val.old_sdsort = HMI_datas.sdsort_alpha;
                HMI_datas.sdsort_alpha = !HMI_datas.sdsort_alpha;
                Draw_Checkbox(row, HMI_datas.sdsort_alpha);
              }
              break;
          #endif
          case ADVANCED_REPRINT:
              if (draw) {
                Draw_Menu_Item(row, ICON_File, F("Re-Print on/off"));
                Draw_Checkbox(row, HMI_datas.reprint_on);
              }
              else {
                HMI_datas.reprint_on = !HMI_datas.reprint_on;
                Draw_Checkbox(row, HMI_datas.reprint_on);
              }
              break;  
          #if ENABLED(POWER_LOSS_RECOVERY)
            case ADVANCED_POWER_LOSS:
              if (draw) {
                Draw_Menu_Item(row, ICON_Motion, GET_TEXT_F(MSG_OUTAGE_RECOVERY));
                Draw_Checkbox(row, recovery.enabled);
              }
              else {
                recovery.enable(!recovery.enabled);
                Draw_Checkbox(row, recovery.enabled);
              }
              break;
          #endif
          #if HAS_ES_DIAG
            case ADVANCED_ENDSDIAG:
              if (draw) {
                sprintf_P(cmd, PSTR("%s %s"), GET_TEXT(MSG_LCD_ENDSTOPS), GET_TEXT(MSG_DEBUG_MENU));
                Draw_Menu_Item(row, ICON_ESDiag, F(cmd));
              }
              else 
                EndSDiag.Draw_ends_diag();
              break;
          #endif
          #if ENABLED(BAUD_RATE_GCODE)
            case ADVANCED_BAUDRATE_MODE:
              if (draw) {
                sprintf_P(cmd, PSTR("115K %s"), GET_TEXT(MSG_INFO_BAUDRATE));
                Draw_Menu_Item(row, ICON_Setspeed, F(cmd));
                Draw_Checkbox(row, HMI_datas.baudratemode);
                }
                else {
                  HMI_datas.baudratemode = !HMI_datas.baudratemode;
                  SetBaudRate();
                  Draw_Checkbox(row, HMI_datas.baudratemode);
                  sprintf_P(cmd, GET_TEXT(MSG_INFO_BAUDRATE_CHANGED), HMI_datas.baudratemode ? 115200 : 250000);
                  Update_Status(cmd);
                  }
              break;
          #endif
          #if HAS_SCREENLOCK
            case ADVANCED_SCREENLOCK:
              if (draw) 
                  Draw_Menu_Item(row, ICON_Lock, GET_TEXT_F(MSG_LOCKSCREEN));
              else 
                  DWIN_ScreenLock();
              break;
          #endif
        }
        break;
      
      #if EXTJYERSUI && HAS_PHYSICAL_MENUS
        case PhySetMenu:

          #define PHYSET_BACK 0
          #define PHYSET_XBEDSIZE (PHYSET_BACK + 1)
          #define PHYSET_YBEDSIZE (PHYSET_XBEDSIZE + 1)
          #define PHYSET_XMINPOS (PHYSET_YBEDSIZE + 1)
          #define PHYSET_YMINPOS (PHYSET_XMINPOS + 1)
          #define PHYSET_XMAXPOS (PHYSET_YMINPOS + 1)
          #define PHYSET_YMAXPOS (PHYSET_XMAXPOS + 1)
          #define PHYSET_ZMAXPOS (PHYSET_YMAXPOS + 1)
          #define PHYSET_TOTAL PHYSET_ZMAXPOS

          switch (item) {
            case PHYSET_BACK:
              if (draw)
                Draw_Menu_Item(row, ICON_Back, GET_TEXT_F(MSG_BACK));
              else
                Draw_Menu(Advanced, ADVANCED_PHYSICAL);
              break;
            case PHYSET_XBEDSIZE:
              if (draw) {
                Draw_Menu_Item(row, ICON_BedSizeX, GET_TEXT_F(MSG_PHY_XBEDSIZE));
                Draw_Float(HMI_datas.x_bed_size, row, false, 1);
              }
              else {
                temp_val.axis = NO_AXIS_ENUM;
                Modify_Value(HMI_datas.x_bed_size, X_BED_MIN, X_MAX_POS, 1);
              }
              break;
            case PHYSET_YBEDSIZE:
              if (draw) {
                Draw_Menu_Item(row, ICON_BedSizeY, GET_TEXT_F(MSG_PHY_YBEDSIZE));
                Draw_Float(HMI_datas.y_bed_size, row, false, 1);
              }
              else {
                temp_val.axis = NO_AXIS_ENUM;
                Modify_Value(HMI_datas.y_bed_size, Y_BED_MIN, Y_MAX_POS, 1);
              }
              break;
            case PHYSET_XMINPOS:
              if (draw) {
                Draw_Menu_Item(row, ICON_MaxPosX, GET_TEXT_F(MSG_PHY_XMINPOS));
                Draw_Float(HMI_datas.x_min_pos, row, false, 1);
              }
              else {
                TERN_(NOZZLE_PARK_FEATURE, temp_val.last_pos = HMI_datas.x_min_pos);
                temp_val.axis = X_AXIS;
                Modify_Value(HMI_datas.x_min_pos, -100, 500, 1);
              }
              break;
            case PHYSET_YMINPOS:
              if (draw) {
                Draw_Menu_Item(row, ICON_MaxPosY, GET_TEXT_F(MSG_PHY_YMINPOS));
                Draw_Float(HMI_datas.y_min_pos, row, false, 1);
              }
              else {
                TERN_(NOZZLE_PARK_FEATURE, temp_val.last_pos = HMI_datas.y_min_pos);
                temp_val.axis = Y_AXIS;
                Modify_Value(HMI_datas.y_min_pos, -100, 500, 1);
              }
              break;
            case PHYSET_XMAXPOS:
              if (draw) {
                Draw_Menu_Item(row, ICON_MaxPosX, GET_TEXT_F(MSG_PHY_XMAXPOS));
                Draw_Float(HMI_datas.x_max_pos, row, false, 1);
              }
              else {
                TERN_(NOZZLE_PARK_FEATURE, temp_val.last_pos = HMI_datas.x_max_pos);
                temp_val.axis = X_AXIS;
                Modify_Value(HMI_datas.x_max_pos, X_BED_MIN, 999, 1);
              }
              break;
            case PHYSET_YMAXPOS:
              if (draw) {
                Draw_Menu_Item(row, ICON_MaxPosY, GET_TEXT_F(MSG_PHY_YMAXPOS));
                Draw_Float(HMI_datas.y_max_pos, row, false, 1);
              }
              else {
                TERN_(NOZZLE_PARK_FEATURE, temp_val.last_pos = HMI_datas.y_max_pos);
                temp_val.axis = Y_AXIS;
                Modify_Value(HMI_datas.y_max_pos, Y_BED_MIN, 999, 1);
              }
              break;
            case PHYSET_ZMAXPOS:
              if (draw) {
                Draw_Menu_Item(row, ICON_MaxPosZ, GET_TEXT_F(MSG_PHY_ZMAXPOS));
                Draw_Float(HMI_datas.z_max_pos, row, false, 1);
              }
              else {
                temp_val.axis = Z_AXIS;
                Modify_Value(HMI_datas.z_max_pos, 100, 999, 1);
              }
              break;
          }
        break;
      #endif

      #if HAS_BED_PROBE
        case ProbeMenu:

          #define PROBE_BACK 0
          #define PROBE_HOME (PROBE_BACK + DISABLED(HAS_RC_CPU))
          #define PROBE_XOFFSET (PROBE_HOME + 1)
          #define PROBE_YOFFSET (PROBE_XOFFSET + 1)
          #define PROBE_ZOFFSET (PROBE_YOFFSET + 1)
          #define PROBE_MAG_MOUNTED (PROBE_ZOFFSET + BOTH(EXTJYERSUI, HAS_MAG_MOUNTED_PROBE))
          #define PROBE_PMARGIN (PROBE_MAG_MOUNTED + ENABLED(EXTJYERSUI))
          #define PROBE_Z_FEEDR_FAST (PROBE_PMARGIN + ENABLED(EXTJYERSUI))
          #define PROBE_Z_FEEDR_SLOW (PROBE_Z_FEEDR_FAST + ENABLED(EXTJYERSUI))
          #define PROBE_HSMODE (PROBE_Z_FEEDR_SLOW + ENABLED(BLTOUCH))
          #define PROBE_VALIDATE (PROBE_HSMODE + ENABLED(BLTOUCH_VALIDATE_ON_HOMING))
          #define PROBE_ALARMR (PROBE_VALIDATE + ENABLED(BLTOUCH))
          #define PROBE_SELFTEST (PROBE_ALARMR + ENABLED(BLTOUCH))
          #define PROBE_MOVEP (PROBE_SELFTEST + ENABLED(BLTOUCH))
          #define PROBE_TEST (PROBE_MOVEP + 1)
          #define PROBE_TEST_COUNT (PROBE_TEST + 1)
          #define PROBE_TOTAL PROBE_TEST_COUNT

          static uint8_t testcount = 4;

          switch (item) {
              case PROBE_BACK:
                if (draw)
                  Draw_Menu_Item(row, ICON_Back, GET_TEXT_F(MSG_BACK));
                else
                  Draw_Menu(Advanced, ADVANCED_PROBE);
                break;
              #if !HAS_RC_CPU
                case PROBE_HOME:
                  if (draw) Draw_Menu_Item(row, ICON_Back, GET_TEXT_F(MSG_MAIN));
                  else Draw_Quick_Home();
                  break;
              #endif
              case PROBE_XOFFSET:
                if (draw) {
                  Draw_Menu_Item(row, ICON_StepX, GET_TEXT_F(MSG_ZPROBE_XOFFSET));
                  Draw_Float(probe.offset.x, row, false, 10);
                }
                else
                  Modify_Value(probe.offset.x, -MAX_XY_OFFSET, MAX_XY_OFFSET, 10);
                break;
              case PROBE_YOFFSET:
                if (draw) {
                  Draw_Menu_Item(row, ICON_StepY, GET_TEXT_F(MSG_ZPROBE_YOFFSET));
                  Draw_Float(probe.offset.y, row, false, 10);
                }
                else
                  Modify_Value(probe.offset.y, -MAX_XY_OFFSET, MAX_XY_OFFSET, 10);
                break;
              case PROBE_ZOFFSET:
                if (draw) {
                  Draw_Menu_Item(row, ICON_StepY, GET_TEXT_F(MSG_ZPROBE_ZOFFSET));
                  Draw_Float(probe.offset.z, row, false, 100);
                }
                else
                  Modify_Value(probe.offset.z, MIN_Z_OFFSET, MAX_Z_OFFSET, 100);
                break;
              #if EXTJYERSUI
                #if HAS_MAG_MOUNTED_PROBE
                  case PROBE_MAG_MOUNTED:
                    if (draw)
                      Draw_Menu_Item(row, ICON_Probe, F("Mag Probe Settings"), nullptr, true);
                    else
                      Draw_Menu(MagMountedProbeMenu);
                    break;
                #endif
                case PROBE_PMARGIN:
                  if (draw) {
                    Draw_Menu_Item(row, ICON_ProbeMargin, GET_TEXT_F(MSG_ZPROBE_MARGIN));
                    Draw_Float(HMI_datas.probing_margin, row, false, 1);
                    }
                  else
                    Modify_Value(HMI_datas.probing_margin, MIN_PROBE_MARGIN, MAX_PROBE_MARGIN, 1);
                  break;
                case PROBE_Z_FEEDR_FAST:
                  if (draw) {
                    Draw_Menu_Item(row, ICON_ProbeZSpeed, GET_TEXT_F(MSG_ZPROBEF_FAST));
                    Draw_Float(HMI_datas.zprobefeedfast, row, false, 1);
                  }
                  else
                    Modify_Value(HMI_datas.zprobefeedfast, MIN_Z_PROBE_FEEDRATE * 2, MAX_Z_PROBE_FEEDRATE, 1);
                  break;
                case PROBE_Z_FEEDR_SLOW:
                  if (draw) {
                    Draw_Menu_Item(row, ICON_ProbeZSpeed, GET_TEXT_F(MSG_ZPROBEF_SLOW));
                    Draw_Float(HMI_datas.zprobefeedslow, row, false, 1);
                  }
                  else
                    Modify_Value(HMI_datas.zprobefeedslow, MIN_Z_PROBE_FEEDRATE, MAX_Z_PROBE_FEEDRATE, 1);
                  break;
              #endif
              #if ENABLED(BLTOUCH)
                case PROBE_HSMODE:
                  if (draw) {
                    sprintf_P(cmd, PSTR("%s %s"), GET_TEXT(MSG_BLTOUCH), GET_TEXT(MSG_BLTOUCH_SPEED_MODE));
                    Draw_Menu_Item(row, ICON_Probe, F(cmd));
                    Draw_Checkbox(row, bltouch.high_speed_mode);
                  }
                  else {
                    bltouch.high_speed_mode = !bltouch.high_speed_mode;
                    Draw_Checkbox(row, bltouch.high_speed_mode);
                  }
                  break;
                #if ENABLED(BLTOUCH_VALIDATE_ON_HOMING)
                  case PROBE_VALIDATE:
                    if (draw) {
                      Draw_Menu_Item(row, ICON_Probe, F("Probe validation"));
                      Draw_Checkbox(row, HMI_datas.bltouch_validate_ui);
                    }
                    else {
                      HMI_datas.bltouch_validate_ui = !HMI_datas.bltouch_validate_ui;
                      Draw_Checkbox(row, HMI_datas.bltouch_validate_ui);
                    }
                    break;
                #endif
                case PROBE_ALARMR:
                  if (draw) {
                  Draw_Menu_Item(row, ICON_Probe, GET_TEXT_F(MSG_BLTOUCH_RESET));
                  }
                  else {
                    gcode.process_subcommands_now(F("M280 P0 S160"));
                    AudioFeedback();
                  }
                  break;
                case PROBE_SELFTEST:
                  if (draw) {
                    Draw_Menu_Item(row, ICON_Probe, GET_TEXT_F(MSG_BLTOUCH_SELFTEST));
                  }
                  else {
                    gcode.process_subcommands_now(F("M280 P0 S120\nG4 P1000\nM280 P0 S160"));
                    planner.synchronize();
                    AudioFeedback();
                  }
                  break;
                case PROBE_MOVEP:
                  if (draw) {
                    Draw_Menu_Item(row, ICON_Probe, GET_TEXT_F(MSG_BLTOUCH_DEPLOY));
                    Draw_Checkbox(row, temp_val.probe_deployed);
                  }
                  else {
                    temp_val.probe_deployed = !temp_val.probe_deployed;
                    if (temp_val.probe_deployed == true)  gcode.process_subcommands_now(F("M280 P0 S10"));
                    else  gcode.process_subcommands_now(F("M280 P0 S90"));
                    Draw_Checkbox(row, temp_val.probe_deployed);
                  }
                  break;
              #endif
              case PROBE_TEST:
                if (draw)
                  Draw_Menu_Item(row, ICON_Probe, GET_TEXT_F(MSG_M48_TEST));
                else {
                  sprintf_P(cmd, PSTR("G28O\nM48 X%s Y%s P%i"), dtostrf((X_BED_SIZE + X_MIN_POS) / 2.0f, 1, 3, str_1), dtostrf((Y_BED_SIZE + Y_MIN_POS) / 2.0f, 1, 3, str_2), testcount);
                  gcode.process_subcommands_now(cmd);
                }
                break;
              case PROBE_TEST_COUNT:
                if (draw) {
                  Draw_Menu_Item(row, ICON_Probe, GET_TEXT_F(MSG_M48_COUNT));
                  Draw_Float(testcount, row, false, 1);
                }
                else
                  Modify_Value(testcount, 4, 50, 1);
                break;
          }
          break;
      #endif  // HAS_PROBE_MENU

      #if HAS_BED_PROBE && HAS_MAG_MOUNTED_PROBE && EXTJYERSUI
        case MagMountedProbeMenu:

          #define MAGPROBE_BACK 0
          #define MAGPROBE_POSX (PROBE_BACK + 1)
          #define MAGPROBE_POSY (MAGPROBE_POSX + 1)
          #define MAGPROBE_POSZ (MAGPROBE_POSY + 1)
          #define MAGPROBE_TOTAL MAGPROBE_POSZ

          switch (item) {
            case MAGPROBE_BACK:
              if (draw)
                Draw_Menu_Item(row, ICON_Back, GET_TEXT_F(MSG_BACK));
              else
                Draw_Menu(ProbeMenu, PROBE_MAG_MOUNTED);
              break;
            case MAGPROBE_POSX:
              if (draw) {
                Draw_Menu_Item(row, ICON_MoveX, F("Pos X to Dock"));
                Draw_Float(HMI_datas.mag_mounted_posx, row, false, 10);
              }
              else
                Modify_Value(HMI_datas.mag_mounted_posx, X_MIN_POS, X_MAX_POS, 10);
              break;
            case MAGPROBE_POSY:
              if (draw) {
                Draw_Menu_Item(row, ICON_MoveY, F("Pos Y to Dock"));
                Draw_Float(HMI_datas.mag_mounted_posy, row, false, 10);
              }
              else
                Modify_Value(HMI_datas.mag_mounted_posy, Y_MIN_POS, Y_MAX_POS, 10);
              break;
            case MAGPROBE_POSZ:
              if (draw) {
                Draw_Menu_Item(row, ICON_MoveZ, F("Lift Z Probe up"));
                Draw_Float(HMI_datas.mag_mounted_posz, row, false, 10);
              }
              else
                Modify_Value(HMI_datas.mag_mounted_posz, 0, 40, 10);
              break;
          }
        break;
      #endif

      #if HAS_TRINAMIC_CONFIG && !HAS_RC_CPU
        case TMCMenu:

          #define TMC_BACK 0
          #define TMC_STEPPER_CURRENT_X (TMC_BACK + AXIS_IS_TMC(X))
          #define TMC_STEPPER_CURRENT_Y (TMC_STEPPER_CURRENT_X + AXIS_IS_TMC(Y))
          #define TMC_STEPPER_CURRENT_Z (TMC_STEPPER_CURRENT_Y + AXIS_IS_TMC(Z))
          #define TMC_STEPPER_CURRENT_E (TMC_STEPPER_CURRENT_Z + AXIS_IS_TMC(E0))
          #define TMC_TOTAL TMC_STEPPER_CURRENT_E

          switch (item) {

            case TMC_BACK:
              if (draw)
                Draw_Menu_Item(row, ICON_Back, F("Back"));
              else
                Draw_Menu(Advanced, ADVANCED_TMC);
              break;

            #if AXIS_IS_TMC(X)
              case TMC_STEPPER_CURRENT_X:
                if (draw) {
                  Draw_Menu_Item(row, ICON_StepX, F("Step. X current"));
                  temp_val.stepper_current_x = stepperX.getMilliamps();
                  Draw_Float(temp_val.stepper_current_x, row, false, 1);
                }
                else {
                  Modify_Value(temp_val.stepper_current_x, TMC_EDIT_MIN_CURRENT, TMC_EDIT_MAX_CURRENT, 1, [](){stepperX.rms_current(temp_val.stepper_current_x);});
                }
                break;
            #endif

            #if AXIS_IS_TMC(Y)
              case TMC_STEPPER_CURRENT_Y:
                if (draw) {
                  Draw_Menu_Item(row, ICON_StepY, F("Step. Y current"));
                  temp_val.stepper_current_y = stepperY.getMilliamps();
                  Draw_Float(temp_val.stepper_current_y, row, false, 1);
                }
                else {
                  Modify_Value(temp_val.stepper_current_y, TMC_EDIT_MIN_CURRENT, TMC_EDIT_MAX_CURRENT, 1, [](){stepperY.rms_current(temp_val.stepper_current_y);});
                }
                break;
            #endif

            #if AXIS_IS_TMC(Z)
              case TMC_STEPPER_CURRENT_Z:
                if (draw) {
                  Draw_Menu_Item(row, ICON_StepZ, F("Step. Z current"));
                  temp_val.stepper_current_z = stepperZ.getMilliamps();
                  Draw_Float(temp_val.stepper_current_z, row, false, 1);
                }
                else {
                  Modify_Value(temp_val.stepper_current_z, TMC_EDIT_MIN_CURRENT, TMC_EDIT_MAX_CURRENT, 1, [](){stepperZ.rms_current(temp_val.stepper_current_z);});
                }
                break;
            #endif

            #if AXIS_IS_TMC(E0)
              case TMC_STEPPER_CURRENT_E:
                if (draw) {
                  Draw_Menu_Item(row, ICON_StepE, F("Step. E current"));
                  temp_val.stepper_current_e = stepperE0.getMilliamps();
                  Draw_Float(temp_val.stepper_current_e, row, false, 1);
                }
                else {
                  Modify_Value(temp_val.stepper_current_e, TMC_EDIT_MIN_CURRENT, TMC_EDIT_MAX_CURRENT, 1, [](){stepperE0.rms_current(temp_val.stepper_current_e);});
                }
                break;
            #endif
          };
          break;
      #endif // HAS_TRINAMIC_CONFIG && !HAS_RC_CPU

      #if HAS_SHAPING
        case ISmenu:

          #define SHAPING_BACK 0
          #define SHAPING_HOME (SHAPING_BACK + DISABLED(HAS_RC_CPU))
          #define SHAPING_X_FREQ_ENA_DIS (SHAPING_HOME + ENABLED(INPUT_SHAPING_X))
          #define SHAPING_X_FREQ (SHAPING_X_FREQ_ENA_DIS + ENABLED(INPUT_SHAPING_X))
          #define SHAPING_X_ZETA (SHAPING_X_FREQ + ENABLED(INPUT_SHAPING_X))
          #define SHAPING_Y_FREQ_ENA_DIS (SHAPING_X_ZETA + ENABLED(INPUT_SHAPING_Y))
          #define SHAPING_Y_FREQ (SHAPING_Y_FREQ_ENA_DIS + ENABLED(INPUT_SHAPING_Y))
          #define SHAPING_Y_ZETA (SHAPING_Y_FREQ + ENABLED(INPUT_SHAPING_Y))
          #define SHAPING_TOTAL SHAPING_Y_ZETA

          switch (item) {
            case SHAPING_BACK:
              if (draw)
                Draw_Menu_Item(row, ICON_Back, GET_TEXT_F(MSG_BACK));
              else
                Draw_Menu(Advanced, ADVANCED_INPUT_SHAPING);
              break;
            #if !HAS_RC_CPU
              case SHAPING_HOME:
                if (draw) Draw_Menu_Item(row, ICON_Back, GET_TEXT_F(MSG_MAIN));
                else Draw_Quick_Home();
                break;
            #endif
            #if ENABLED(INPUT_SHAPING_X)
              case SHAPING_X_FREQ_ENA_DIS:
                if (draw) {
                //temp_val.flag_IS_X = (stepper.get_shaping_frequency(X_AXIS) != 0);
                Draw_Menu_Item(row, ICON_MoveX, GET_TEXT_F(MSG_SHAPING_X_ENA));
                Draw_Checkbox(row, temp_val.flag_IS_X);
                }
                else {
                  temp_val.flag_IS_X = !temp_val.flag_IS_X;
                  stepper.set_shaping_frequency(X_AXIS, temp_val.flag_IS_X ? SHAPING_FREQ_X : 0.0f);
                  Redraw_Menu(false);
                }
              break;
              case SHAPING_X_FREQ:
                if (draw) {
                  Draw_Menu_Item(row, ICON_MoveX, GET_TEXT_F(MSG_SHAPING_X_FREQ));
                  Draw_Float(stepper.get_shaping_frequency(X_AXIS), row, false, 100);
                }
                else {
                  if (temp_val.flag_IS_X) {
                    temp_val.flag_freq = true;
                    temp_val.axis = X_AXIS;
                    temp_val.editable_decimal = stepper.get_shaping_frequency(X_AXIS);
                    Modify_Value(temp_val.editable_decimal, min_frequency, max_freq_edit, 100);
                  }
                }
              break;
              case SHAPING_X_ZETA:
                if (draw) {
                  Draw_Menu_Item(row, ICON_MoveX, GET_TEXT_F(MSG_SHAPING_X_ZETA));
                  Draw_Float(stepper.get_shaping_damping_ratio(X_AXIS), row, false, 100);
                }
                else {
                  if (temp_val.flag_IS_X) {
                    temp_val.flag_freq = false;
                    temp_val.axis = X_AXIS;
                    temp_val.editable_decimal = stepper.get_shaping_damping_ratio(X_AXIS);
                    Modify_Value(temp_val.editable_decimal, 0.0f, 1.0f, 100);
                  }
                }
              break;
            #endif
            #if ENABLED(INPUT_SHAPING_Y)
              case SHAPING_Y_FREQ_ENA_DIS:
                if (draw) {
                //temp_val.flag_IS_Y = (stepper.get_shaping_frequency(Y_AXIS) != 0);
                Draw_Menu_Item(row, ICON_MoveY, GET_TEXT_F(MSG_SHAPING_Y_ENA));
                Draw_Checkbox(row, temp_val.flag_IS_Y);
                }
                else {
                  temp_val.flag_IS_Y = !temp_val.flag_IS_Y;
                  stepper.set_shaping_frequency(Y_AXIS, temp_val.flag_IS_Y ? SHAPING_FREQ_Y : 0.0f);
                  Redraw_Menu(false);
                }
              break;
              case SHAPING_Y_FREQ:
                if (draw) {
                  Draw_Menu_Item(row, ICON_MoveY, GET_TEXT_F(MSG_SHAPING_Y_FREQ));
                  Draw_Float(stepper.get_shaping_frequency(Y_AXIS), row, false, 100);
                }
                else {
                  if (temp_val.flag_IS_Y) {
                    temp_val.flag_freq = true;
                    temp_val.axis = Y_AXIS;
                    temp_val.editable_decimal = stepper.get_shaping_frequency(Y_AXIS);
                    Modify_Value(temp_val.editable_decimal, min_frequency, max_freq_edit, 100);
                  }
                }
              break;
              case SHAPING_Y_ZETA:
                if (draw) {
                  Draw_Menu_Item(row, ICON_MoveY, GET_TEXT_F(MSG_SHAPING_Y_ZETA));
                  Draw_Float(stepper.get_shaping_damping_ratio(Y_AXIS), row, false, 100);
                }
                else {
                  if (temp_val.flag_IS_Y) {
                    temp_val.flag_freq = false;
                    temp_val.axis = Y_AXIS;
                    temp_val.editable_decimal = stepper.get_shaping_damping_ratio(Y_AXIS);
                    Modify_Value(temp_val.editable_decimal, 0.0f, 1.0f, 100);
                  }
                }
              break;
            #endif 
          }
        break;
      #endif

      case Filmenu:

        #define FIL_BACK 0
        #define FIL_HOME (FIL_BACK + DISABLED(HAS_RC_CPU))
        #define FIL_SENSORENABLED (FIL_HOME + ENABLED(HAS_FILAMENT_SENSOR))
        #define FIL_RUNOUTACTIVE (FIL_SENSORENABLED + (ENABLED(HAS_FILAMENT_SENSOR, EXTJYERSUI)))
        #define FIL_SENSORDISTANCE (FIL_RUNOUTACTIVE + 1)
        #define FIL_LOAD (FIL_SENSORDISTANCE + ENABLED(ADVANCED_PAUSE_FEATURE))
        #define FIL_UNLOAD (FIL_LOAD + ENABLED(ADVANCED_PAUSE_FEATURE))
        #define FIL_UNLOAD_FEEDRATE (FIL_UNLOAD + (ENABLED(ADVANCED_PAUSE_FEATURE, EXTJYERSUI)))
        #define FIL_FAST_LOAD_FEEDRATE (FIL_UNLOAD_FEEDRATE + (ENABLED(ADVANCED_PAUSE_FEATURE, EXTJYERSUI)))
        #define FIL_PARK_NOZZLE_TIMEOUT (FIL_FAST_LOAD_FEEDRATE + (ENABLED(ADVANCED_PAUSE_FEATURE, EXTJYERSUI) && DISABLED(HAS_RC_CPU)))
        #define FIL_COLD_EXTRUDE  (FIL_PARK_NOZZLE_TIMEOUT + ENABLED(PREVENT_COLD_EXTRUSION))
        #define FIL_TOTAL FIL_COLD_EXTRUDE

        switch (item) {
          case FIL_BACK:
            if (draw)
              Draw_Menu_Item(row, ICON_Back, GET_TEXT_F(MSG_BACK));
            else
              Draw_Menu(Advanced, ADVANCED_FILMENU);
            break;
          #if !HAS_RC_CPU
            case FIL_HOME:
              if (draw) Draw_Menu_Item(row, ICON_Back, GET_TEXT_F(MSG_MAIN));
              else Draw_Quick_Home();
              break;
          #endif
          #if HAS_FILAMENT_SENSOR
            case FIL_SENSORENABLED:
              if (draw) {
                Draw_Menu_Item(row, ICON_Extruder, GET_TEXT_F(MSG_RUNOUT_SENSOR));
                if (runout.mode[0] == RM_NONE) runout.enabled[0] = false;
                Draw_Checkbox(row, runout.enabled[0]);
              }
              else {
                if (runout.mode[0] == RM_NONE) runout.enabled[0] = false;
                else runout.enabled[0] = !runout.enabled[0];
                Draw_Checkbox(row, runout.enabled[0]);
              }
              break;
            #if EXTJYERSUI 
              case FIL_RUNOUTACTIVE:
                if (draw) {
                  Draw_Menu_Item(row, ICON_FilSet, GET_TEXT_F(MSG_RUNOUT_ENABLE));
                  Draw_Option(temp_val.rsensormode, runoutsensor_modes, row);
                }
                else {
                  runout.reset();
                  temp_val.State_runoutenable = runout.enabled[0];
                  runout.enabled[0] = false;
                  Modify_Option(temp_val.rsensormode, runoutsensor_modes, 3);
                }
                break;
            #endif
            case FIL_SENSORDISTANCE:
              if (draw) {
                temp_val.editable_distance = runout.runout_distance();
                Draw_Menu_Item(row, ICON_MaxAccE, GET_TEXT_F(MSG_RUNOUT_DISTANCE_MM));
                Draw_Float(temp_val.editable_distance, row, false, 10);
              }
              else
                Modify_Value(temp_val.editable_distance, 0, 999, 10);
              break;
          #endif

          #if ENABLED(ADVANCED_PAUSE_FEATURE)
            case FIL_LOAD:
              if (draw) {
                Draw_Menu_Item(row, ICON_WriteEEPROM, GET_TEXT_F(MSG_FILAMENT_LOAD));
                Draw_Float(fc_settings[0].load_length, row, false, 1);
              }
              else
                Modify_Value(fc_settings[0].load_length, 0, EXTRUDE_MAXLENGTH, 1);
              break;
            case FIL_UNLOAD:
              if (draw) {
                Draw_Menu_Item(row, ICON_ReadEEPROM, GET_TEXT_F(MSG_FILAMENT_UNLOAD));
                Draw_Float(fc_settings[0].unload_length, row, false, 1);
              }
              else
                Modify_Value(fc_settings[0].unload_length, 0, EXTRUDE_MAXLENGTH, 1);
              break;
            #if EXTJYERSUI
              case FIL_UNLOAD_FEEDRATE:
                if (draw) {
                  Draw_Menu_Item(row, ICON_FilUnload, GET_TEXT_F(MSG_FILAMENT_UNLOAD_RATE));
                  Draw_Float(HMI_datas.fil_unload_feedrate, row, false, 1);
                }
                else
                  Modify_Value(HMI_datas.fil_unload_feedrate, MIN_FIL_CHANGE_FEEDRATE, MAX_FIL_CHANGE_FEEDRATE, 1);
                break;
              case FIL_FAST_LOAD_FEEDRATE:
                if (draw) {
                  Draw_Menu_Item(row, ICON_FilLoad, GET_TEXT_F(MSG_FILAMENT_LOAD_RATE));
                  Draw_Float(HMI_datas.fil_fast_load_feedrate, row, false, 1);
                }
                else
                  Modify_Value(HMI_datas.fil_fast_load_feedrate, MIN_FIL_CHANGE_FEEDRATE, MAX_FIL_CHANGE_FEEDRATE, 1);
                break;
                #if !HAS_RC_CPU
                  case FIL_PARK_NOZZLE_TIMEOUT:
                    if (draw) {
                      Draw_Menu_Item(row, ICON_Step, F("Nozzle Timeout"));
                      Draw_Float(HMI_datas.park_nozzle_timeout, row, false, 1);
                    }
                    else
                      Modify_Value(HMI_datas.park_nozzle_timeout, MIN_PARK_NOZZLE_TIMEOUT, MAX_PARK_NOZZLE_TIMEOUT, 1);
                    break;
                #endif
            #endif
          #endif // ADVANCED_PAUSE_FEATURE

          #if ENABLED(PREVENT_COLD_EXTRUSION)
            case FIL_COLD_EXTRUDE:
              if (draw) {
                Draw_Menu_Item(row, ICON_Cool, GET_TEXT_F(MSG_INFO_MIN_TEMP));
                Draw_Float(thermalManager.extrude_min_temp, row, false, 1);
              }
              else {
                Modify_Value(thermalManager.extrude_min_temp, 0, MAX_E_TEMP, 1);
                thermalManager.allow_cold_extrude = (thermalManager.extrude_min_temp == 0);
              }
              break;
          #endif
        }
      break;

      case InfoMain:
      case Info:

        #define INFO_BACK 0
        #define INFO_PRINTCOUNT (INFO_BACK + ENABLED(PRINTCOUNTER))
        #define INFO_PRINTTIME (INFO_PRINTCOUNT + ENABLED(PRINTCOUNTER))
        #define INFO_SIZE (INFO_PRINTTIME + 1)
        #define INFO_VERSION (INFO_SIZE + 1)
        #define INFO_CONTACT (INFO_VERSION + 1)

        #if ENABLED(PRINTCOUNTER)
          #define INFO_RESET_PRINTCOUNTER (INFO_CONTACT + 1)
          #define INFO_TOTAL INFO_RESET_PRINTCOUNTER
        #else
          #define INFO_TOTAL INFO_BACK
        #endif

        switch (item) {
          case INFO_BACK:
            if (draw) {
              #if EXTJYERSUI && HAS_MESH
                sprintf_P(cmd, PSTR("%s %ix%i - EEProm Size: %i bytes (/%i max)"), CUSTOM_MACHINE_NAME, HMI_datas.grid_max_points, HMI_datas.grid_max_points, sizeof(HMI_datas), eeprom_data_size);
                Update_Status(cmd);
              #else
                Update_Status(CUSTOM_MACHINE_NAME);
              #endif
              Draw_Menu_Item(row, ICON_Back, GET_TEXT_F(MSG_BACK));

              #if ENABLED(PRINTCOUNTER)
                char row1[50], row2[50], buf[32];
                printStatistics ps = print_job_timer.getStats();

                sprintf_P(row1, PSTR("%s: %i, %i %s"), GET_TEXT(MSG_INFO_PRINT_COUNT), ps.totalPrints, ps.finishedPrints, GET_TEXT(MSG_INFO_COMPLETED_PRINTS));
                sprintf_P(row2, PSTR("%sm %s"), dtostrf(ps.filamentUsed / 1000, 1, 2, str_1), GET_TEXT(MSG_INFO_PRINT_FILAMENT));
                Draw_Menu_Item(INFO_PRINTCOUNT, ICON_HotendTemp, row1, row2, false, true);

                duration_t(print_job_timer.getStats().printTime).toString(buf);
                sprintf_P(row1, PSTR("%s: %s"), GET_TEXT(MSG_INFO_PRINT_TIME), buf);
                duration_t(print_job_timer.getStats().longestPrint).toString(buf);
                sprintf_P(row2, PSTR("%s: %s"), GET_TEXT(MSG_INFO_PRINT_LONGEST), buf);
                Draw_Menu_Item(INFO_PRINTTIME, ICON_PrintTime, row1, row2, false, true);
              #endif
              #if HAS_PHYSICAL_MENUS && EXTJYERSUI
                sprintf_P(row1, PSTR("%ix%ix%i"), HMI_datas.x_bed_size, HMI_datas.y_bed_size, HMI_datas.z_max_pos);
                Draw_Menu_Item(INFO_SIZE, ICON_PrintSize, F(row1), nullptr, false, true);
              #else
                Draw_Menu_Item(INFO_SIZE, ICON_PrintSize, F(MACHINE_SIZE), nullptr, false, true);
              #endif
              sprintf_P(cmd, PSTR("%s (%s)"), BUILD_NUMBER, STM_CPU);
              Draw_Menu_Item(INFO_VERSION, ICON_Version, F(SHORT_BUILD_VERSION), F(cmd), false, true);
              Draw_Menu_Item(INFO_CONTACT, ICON_Contact, F(CORP_WEBSITE1), F(CORP_WEBSITE2), false, true);
            }
            else {
              Update_Status("");
              if (menu == Info)
                Draw_Menu(Control, CONTROL_INFO);
              else
                Draw_Main_Menu(3);
            }
            break;
          #if ENABLED(PRINTCOUNTER)
            case INFO_RESET_PRINTCOUNTER:
              if (draw) {
                Draw_Menu_Item(row, ICON_HotendTemp, GET_TEXT_F(MSG_INFO_PRINT_COUNT_RESET));
              }
              else {
                print_job_timer.initStats();
                ui.reset_status();
                Draw_Menu(Info);
                AudioFeedback();
              }
              break;
          #endif
        }
        break;

      #if HAS_MESH
        case Leveling:

          #define LEVELING_BACK 0
          #define LEVELING_ACTIVE (LEVELING_BACK + 1)
          #define LEVELING_GET_TILT (LEVELING_ACTIVE + ENABLED(HAS_BED_PROBE, AUTO_BED_LEVELING_UBL))
          #define LEVELING_GET_MESH (LEVELING_GET_TILT + 1)
          #define LEVELING_MANUAL (LEVELING_GET_MESH + 1)
          #define LEVELING_VIEW (LEVELING_MANUAL + 1)
          #define LEVELING_SETTINGS (LEVELING_VIEW + 1)
          #define LEVELING_SLOT (LEVELING_SETTINGS + ENABLED(AUTO_BED_LEVELING_UBL))
          #define LEVELING_LOAD (LEVELING_SLOT + ENABLED(AUTO_BED_LEVELING_UBL))
          #define LEVELING_SAVE (LEVELING_LOAD + ENABLED(AUTO_BED_LEVELING_UBL))
          #define LEVELING_TOTAL LEVELING_SAVE

          switch (item) {
            case LEVELING_BACK:
              if (draw)
                Draw_Menu_Item(row, ICON_Back, GET_TEXT_F(MSG_BACK));
              else
                Draw_Main_Menu(3);
              break;
            case LEVELING_ACTIVE:
              if (draw) {
                Draw_Menu_Item(row, ICON_Probe, GET_TEXT_F(MSG_MESH_LEVELING));
                Draw_Checkbox(row, planner.leveling_active);
              }
              else {
                if (!planner.leveling_active) {
                  set_bed_leveling_enabled(!planner.leveling_active);
                  if (!planner.leveling_active) {
                    Confirm_Handler(LevelError);
                    break;
                  }
                }
                else  set_bed_leveling_enabled(!planner.leveling_active);
                  
                Draw_Checkbox(row, planner.leveling_active);
                HMI_datas.leveling_active = planner.leveling_active;
              }
              break;
            #if ENABLED(HAS_BED_PROBE, AUTO_BED_LEVELING_UBL)
              case LEVELING_GET_TILT:
                if (draw)
                  Draw_Menu_Item(row, ICON_Tilt, GET_TEXT_F(MSG_UBL_AUTOTILT));
                else {
                  if (bedlevel.storage_slot < 0) {
                    Popup_Handler(MeshSlot);
                    break;
                  }
                  Autotilt_Common();
                  Redraw_Menu();
                }
                break;
            #endif
            case LEVELING_GET_MESH:
              if (draw)
                Draw_Menu_Item(row, ICON_Mesh, GET_TEXT_F(MSG_UBL_BUILD_MESH_MENU));
              else {
                #if ENABLED(AUTO_BED_LEVELING_UBL)
                    if (bedlevel.storage_slot <0) {
                      Popup_Handler(MeshSlot, true);
                      break;
                    }
                #endif             
                #if HAS_LEVELING_HEAT
                  HeatBeforeLeveling();
                #endif
                Update_Status("");
                Popup_Handler(Home);
                gcode.home_all_axes(true);
                #if ENABLED(AUTO_BED_LEVELING_UBL) 
                  #if HAS_BED_PROBE
                    TERN_(EXTJYERSUI, temp_val.cancel_lev = 0);
                    #if HAS_LIVEMESH
                      Draw_LiveMesh();
                    #else
                      TERN(EXTJYERSUI, Confirm_Handler(Level2), Popup_Handler(Level));
                    #endif
                    gcode.process_subcommands_now(F("G29 P1"));
                    gcode.process_subcommands_now(F("G29 P3\nG29 P3\nG29 P3\nG29 P3\nG29 P3\nG29 P3\nG29 P3\nG29 P3\nG29 P3\nG29 P3\nG29 P3\nG29 P3\nG29 P3\nG29 P3\nG29 P3\nM420 S1"));
                    planner.synchronize();  
                    #if EXTJYERSUI
                      Update_Status(temp_val.cancel_lev ? GET_TEXT(MSG_PROBING_CANCELLED) : GET_TEXT(MSG_MESH_DONE));
                      if (!temp_val.cancel_lev) Viewmesh();
                      else temp_val.cancel_lev = 0;
                    #else
                      Update_Status(GET_TEXT(MSG_MESH_DONE));
                      Viewmesh();
                    #endif
                  #else
                    level_state = planner.leveling_active;
                    set_bed_leveling_enabled(false);
                    BedLevelTools.goto_mesh_value = true;
                    BedLevelTools.mesh_x = BedLevelTools.mesh_y = 0;
                    Popup_Handler(MoveWait);
                    BedLevelTools.manual_mesh_move();
                    gcode.process_subcommands_now(F("M211 S0"));
                    Draw_Menu(UBLMesh);
                  #endif
                #elif HAS_BED_PROBE
                  TERN_(EXTJYERSUI,temp_val.cancel_lev = 0);
                  #if HAS_LIVEMESH
                    Draw_LiveMesh();
                  #else
                    TERN(EXTJYERSUI, Confirm_Handler(Level2), Popup_Handler(Level));
                  #endif
                  gcode.process_subcommands_now(F("G29"));
                  planner.synchronize();
                  #if EXTJYERSUI
                    Update_Status(temp_val.cancel_lev ? GET_TEXT(MSG_PROBING_CANCELLED) : GET_TEXT(MSG_MESH_DONE));
                    if (!temp_val.cancel_lev) Viewmesh();
                    else temp_val.cancel_lev = 0;
                  #else
                    Update_Status(GET_TEXT(MSG_MESH_DONE));
                    Viewmesh();
                  #endif
                #else
                  level_state = planner.leveling_active;
                  set_bed_leveling_enabled(false);
                  temp_val.gridpoint = 1;
                  Popup_Handler(MoveWait);
                  gcode.process_subcommands_now(F("M211 S0\nG29"));
                  planner.synchronize();
                  Draw_Menu(ManualMesh);
                #endif
              }
              break;
            case LEVELING_MANUAL:
              if (draw)
                Draw_Menu_Item(row, ICON_Mesh, GET_TEXT_F(MSG_UBL_MESH_EDIT), nullptr, true);
              else {
                #if ENABLED(AUTO_BED_LEVELING_BILINEAR)
                  if (!leveling_is_valid()) {
                    Confirm_Handler(InvalidMesh);
                    break;
                  }
                #endif
                #if ENABLED(AUTO_BED_LEVELING_UBL)
                  if (bedlevel.storage_slot < 0) {
                    Popup_Handler(MeshSlot);
                    break;
                  }
                #endif
                #if HAS_LEVELING_HEAT
                  HeatBeforeLeveling();
                #endif
                Update_Status("");
                if (axes_should_home()) {
                  Popup_Handler(Home);
                  gcode.home_all_axes(true);
                }
                // add here graphical edit temp_val.popup
                level_state = planner.leveling_active;
                set_bed_leveling_enabled(false);
                BedLevelTools.goto_mesh_value = false;
                #if HAS_GRAPHICAL_LEVELMANUAL && EXTJYERSUI
                  Popup_Handler(EditMesh);
                #else
                  Popup_Handler(MoveWait);
                  BedLevelTools.manual_mesh_move();
                  gcode.process_subcommands_now(F("M211 S0"));
                  Draw_Menu(LevelManual);
                #endif
              }
              break;
            case LEVELING_VIEW:
              if (draw)
                Draw_Menu_Item(row, ICON_Mesh, GET_TEXT_F(MSG_MESH_VIEW), nullptr, true);
              else {
                #if ENABLED(AUTO_BED_LEVELING_UBL)
                  if (bedlevel.storage_slot < 0) {
                    Popup_Handler(MeshSlot);
                    break;
                  }
                #endif
                Draw_Menu(LevelView);
              }
              break;
            case LEVELING_SETTINGS:
              if (draw)
                Draw_Menu_Item(row, ICON_Step, GET_TEXT_F(MSG_ADVANCED_SETTINGS), nullptr, true);
              else
                Draw_Menu(LevelSettings);
              break;
            #if ENABLED(AUTO_BED_LEVELING_UBL)
            case LEVELING_SLOT:
              if (draw) {
                Draw_Menu_Item(row, ICON_PrintSize, GET_TEXT_F(MSG_UBL_STORAGE_SLOT));
                Draw_Float(bedlevel.storage_slot, row, false, 1);
              }
              else
                Modify_Value(bedlevel.storage_slot, 0, settings.calc_num_meshes() - 1, 1);
              break;
            case LEVELING_LOAD:
              if (draw)
                Draw_Menu_Item(row, ICON_ReadEEPROM, GET_TEXT_F(MSG_UBL_LOAD_MESH));
              else {
                if (bedlevel.storage_slot < 0) {
                  Popup_Handler(MeshSlot);
                  break;
                }
                gcode.process_subcommands_now(F("G29 L"));
                planner.synchronize();
                AudioFeedback(true);
              }
              break;
            case LEVELING_SAVE:
              if (draw)
                Draw_Menu_Item(row, ICON_WriteEEPROM, GET_TEXT_F(MSG_UBL_SAVE_MESH));
              else {
                if (bedlevel.storage_slot < 0) {
                  Popup_Handler(MeshSlot, true);
                  break;
                }
                gcode.process_subcommands_now(F("G29 S"));
                planner.synchronize();
                AudioFeedback(true);
              }
              break;
            #endif
          }
          break;

        case LevelView:

          #define LEVELING_VIEW_BACK 0
          #define LEVELING_VIEW_HOME (LEVELING_VIEW_BACK + DISABLED(HAS_RC_CPU))
          #define LEVELING_VIEW_MESH (LEVELING_VIEW_HOME + 1)
          #define LEVELING_VIEW_TEXT (LEVELING_VIEW_MESH + 1)
          #define LEVELING_VIEW_ASYMMETRIC (LEVELING_VIEW_TEXT + 1)
          #define LEVELING_VIEW_TOTAL LEVELING_VIEW_ASYMMETRIC

          switch (item) {
            case LEVELING_VIEW_BACK:
              if (draw)
                Draw_Menu_Item(row, ICON_Back, GET_TEXT_F(MSG_BACK));
              else
                Draw_Menu(Leveling, LEVELING_VIEW);
              break;
            #if !HAS_RC_CPU
              case LEVELING_VIEW_HOME:
                if (draw) Draw_Menu_Item(row, ICON_Back, GET_TEXT_F(MSG_MAIN));
                else Draw_Quick_Home();
                break;
            #endif
            case LEVELING_VIEW_MESH:
              if (draw)
                Draw_Menu_Item(row, ICON_PrintSize, GET_TEXT_F(MSG_MESH_VIEW), nullptr, true);
              else
                Draw_Menu(MeshViewer);
              break;
            case LEVELING_VIEW_TEXT:
              if (draw) {
                Draw_Menu_Item(row, ICON_Contact, GET_TEXT_F(MSG_MESH_VIEW_TEXT));
                Draw_Checkbox(row, BedLevelTools.viewer_print_value);
              }
              else {
                BedLevelTools.viewer_print_value = !BedLevelTools.viewer_print_value;
                Draw_Checkbox(row, BedLevelTools.viewer_print_value);
              }
              break;
            case LEVELING_VIEW_ASYMMETRIC:
              if (draw) {
                Draw_Menu_Item(row, ICON_Axis, GET_TEXT_F(MSG_MESH_VIEW_ASYMMETRIC));
                Draw_Checkbox(row, BedLevelTools.viewer_asymmetric_range);
              }
              else {
                BedLevelTools.viewer_asymmetric_range = !BedLevelTools.viewer_asymmetric_range;
                Draw_Checkbox(row, BedLevelTools.viewer_asymmetric_range);
              }
              break;
          }
          break;

        case LevelSettings:

          #define LEVELING_SETTINGS_BACK 0
          #define LEVELING_SETTINGS_HOME (LEVELING_SETTINGS_BACK + DISABLED(HAS_RC_CPU))
          #define LEVELING_SETTINGS_LEVELTEMP_MODE (LEVELING_SETTINGS_HOME + ENABLED(HAS_LEVELING_HEAT))
          #define LEVELING_SETTINGS_HOTENDTEMP (LEVELING_SETTINGS_LEVELTEMP_MODE + ENABLED(HAS_LEVELING_HEAT))
          #define LEVELING_SETTINGS_BEDTEMP (LEVELING_SETTINGS_HOTENDTEMP + ENABLED(HAS_LEVELING_HEAT))
          #define LEVELING_SETTINGS_MESHGRID (LEVELING_SETTINGS_BEDTEMP + ENABLED(EXTJYERSUI, HAS_MESH))
          #define LEVELING_SETTINGS_MESHINSET (LEVELING_SETTINGS_MESHGRID + ENABLED(EXTJYERSUI, HAS_MESH))
          #define LEVELING_SETTINGS_FADE (LEVELING_SETTINGS_MESHINSET + 1)
          #define LEVELING_SETTINGS_TILT (LEVELING_SETTINGS_FADE + ENABLED(AUTO_BED_LEVELING_UBL, HAS_BED_PROBE))
          #define LEVELING_SETTINGS_TILT_AFTER_N_PRINTS (LEVELING_SETTINGS_TILT + (ENABLED(AUTO_BED_LEVELING_UBL, HAS_BED_PROBE) && DISABLED(HAS_NO_AUTOTILT_AFTERNPRINT)))
          #define LEVELING_SETTINGS_PLANE (LEVELING_SETTINGS_TILT_AFTER_N_PRINTS + ENABLED(AUTO_BED_LEVELING_UBL))
          #define LEVELING_SETTINGS_ZERO (LEVELING_SETTINGS_PLANE + ENABLED(AUTO_BED_LEVELING_UBL))
          #define LEVELING_SETTINGS_UNDEF (LEVELING_SETTINGS_ZERO + ENABLED(AUTO_BED_LEVELING_UBL))
          #define LEVELING_SETTINGS_TOTAL LEVELING_SETTINGS_UNDEF

          switch (item) {
            case LEVELING_SETTINGS_BACK:
              if (draw)
                Draw_Menu_Item(row, ICON_Back, GET_TEXT_F(MSG_BACK));
              else
                Draw_Menu(Leveling, LEVELING_SETTINGS);
              break;
            #if !HAS_RC_CPU
              case LEVELING_SETTINGS_HOME:
                if (draw) Draw_Menu_Item(row, ICON_Back, GET_TEXT_F(MSG_MAIN));
                else Draw_Quick_Home();
                break;
            #endif 
            #if HAS_LEVELING_HEAT
              case LEVELING_SETTINGS_LEVELTEMP_MODE:
              if (draw) {
                Draw_Menu_Item(row, ICON_Homing, GET_TEXT_F(MSG_LEVELTEMP_MODE));
                Draw_Option(temp_val.LevelingTempmode, LevelingTemp_modes, row);
              }
              else
                Modify_Option(temp_val.LevelingTempmode, LevelingTemp_modes, 3);
              break;
              case LEVELING_SETTINGS_HOTENDTEMP:
                if (draw) {
                  Draw_Menu_Item(row, ICON_SetEndTemp, GET_TEXT_F(MSG_HOTEND_TEMPERATURE));
                  Draw_Float(HMI_datas.LevelingTemp_hotend, row, false, 1);
                }
                else
                  Modify_Value(HMI_datas.LevelingTemp_hotend, MIN_E_TEMP, MAX_E_TEMP, 1);
                break;
              case LEVELING_SETTINGS_BEDTEMP:
                if (draw) {
                Draw_Menu_Item(row, ICON_SetBedTemp, GET_TEXT_F(MSG_BED_TEMPERATURE));
                Draw_Float(HMI_datas.LevelingTemp_bed, row, false, 1);
              }
              else
                Modify_Value(HMI_datas.LevelingTemp_bed, MIN_BED_TEMP, MAX_BED_TEMP, 1);
              break;
            #endif
            #if EXTJYERSUI && HAS_MESH
              case LEVELING_SETTINGS_MESHGRID:
              if (draw) {
                Draw_Menu_Item(row, ICON_MeshPoints, GET_TEXT_F(MSG_SET_MESH_GRID));
                Draw_Option(_GridxGrid, GridxGrid, row);
              }
              else
                Modify_Option(_GridxGrid, GridxGrid, NB_Grids - 1);
              break;
              case LEVELING_SETTINGS_MESHINSET:
                if (draw)
                  Draw_Menu_Item(row, ICON_ProbeMargin, GET_TEXT_F(MSG_MESH_INSET), nullptr, true);
                else
                  Draw_Menu(MeshInsetMenu);
                break;
            #endif
            case LEVELING_SETTINGS_FADE:
                if (draw) {
                  Draw_Menu_Item(row, ICON_Fade, GET_TEXT_F(MSG_Z_FADE_HEIGHT));
                  Draw_Float(planner.z_fade_height, row, false, 1);
                }
                else {
                  Modify_Value(planner.z_fade_height, 0, Z_MAX_POS, 1);
                  planner.z_fade_height = -1;
                  set_z_fade_height(planner.z_fade_height);
                }
                break;

            #if ENABLED(AUTO_BED_LEVELING_UBL)
              #if HAS_BED_PROBE
                case LEVELING_SETTINGS_TILT:
                  if (draw) {
                    Draw_Menu_Item(row, ICON_Tilt, GET_TEXT_F(MSG_LCD_TILTING_GRID_SIZE));
                    Draw_Float(BedLevelTools.tilt_grid, row, false, 1);
                  }
                  else
                    Modify_Value(BedLevelTools.tilt_grid, 1, 8, 1);
                  break;
                #if !HAS_NO_AUTOTILT_AFTERNPRINT
                  case LEVELING_SETTINGS_TILT_AFTER_N_PRINTS:
                    if (draw) {
                      Draw_Menu_Item(row, ICON_Tilt, GET_TEXT_F(MSG_UBL_AUTOTILT_AFTER_N_PRINTS));
                      Draw_Float(temp_val.NPrinted, row, false, 1);
                    }
                    else 
                      Modify_Value(temp_val.NPrinted, 0, 200, 1);
                    break;
                #endif
              #endif
              case LEVELING_SETTINGS_PLANE:
                if (draw)
                  Draw_Menu_Item(row, ICON_ResumeEEPROM, GET_TEXT_F(MSG_MESH_TO_PLANE));
                else {
                  if (BedLevelTools.create_plane_from_mesh()) {
                    Confirm_Handler(NocreatePlane);
                break;
                }
                  gcode.process_subcommands_now(F("M420 S1"));
                  planner.synchronize();
                  AudioFeedback(true);
                }
                break;
              case LEVELING_SETTINGS_ZERO:
                if (draw)
                  Draw_Menu_Item(row, ICON_Mesh, GET_TEXT_F(MSG_MESH_ZERO));
                else
                  ZERO(bedlevel.z_values);
                break;
              case LEVELING_SETTINGS_UNDEF:
                if (draw)
                  Draw_Menu_Item(row, ICON_Mesh, GET_TEXT_F(MSG_MESH_CLEAR));
                else
                  bedlevel.invalidate();
                break;
            #endif // AUTO_BED_LEVELING_UBL
          }
          break;

        case MeshViewer:
          #define MESHVIEW_BACK 0
          #define MESHVIEW_TOTAL MESHVIEW_BACK

          if (item == MESHVIEW_BACK) {
            if (draw) {
              Draw_Menu_Item(0, ICON_Back, GET_TEXT_F(MSG_BACK));
              temp_val.flag_resetstatus = false;
              BedLevelTools.Draw_Bed_Mesh();
              BedLevelTools.Set_Mesh_Viewer_Status();
            }
            else if (!temp_val.drawing_mesh) {           
              Draw_Menu(LevelView, LEVELING_VIEW_MESH);
              temp_val.flag_resetstatus = true;
              Update_Status("");  
            }
          }
          break;

        case LevelManual:

          #define LEVELING_M_BACK 0
          #define LEVELING_M_HOME (LEVELING_M_BACK + DISABLED(HAS_RC_CPU))
          #define LEVELING_M_MODELIVE (LEVELING_M_HOME + 1)
          #define LEVELING_M_X (LEVELING_M_MODELIVE + 1)
          #define LEVELING_M_Y (LEVELING_M_X + 1)
          #define LEVELING_M_NEXT (LEVELING_M_Y + 1)
          #define LEVELING_M_PREV (LEVELING_M_NEXT + 1)
          #define LEVELING_M_OFFSET (LEVELING_M_PREV + 1)
          #define LEVELING_M_UP (LEVELING_M_OFFSET + 1)
          #define LEVELING_M_DOWN (LEVELING_M_UP + 1)
          #define LEVELING_M_GOTO_VALUE (LEVELING_M_DOWN + 1)
          #define LEVELING_M_UNDEF (LEVELING_M_GOTO_VALUE + ENABLED(AUTO_BED_LEVELING_UBL))
          #define LEVELING_M_TOTAL LEVELING_M_UNDEF

          temp_val.evenvalue = (GRID_MAX_POINTS_X % 2 == 0);

          switch (item) {
            case LEVELING_M_BACK:
              if (draw)
                Draw_Menu_Item(row, ICON_Back, GET_TEXT_F(MSG_BACK));
              else {
                temp_val.liveadjust = TERN(USER_MOVE_LIVE, true, false);
                BedLevelTools.goto_mesh_value = false;
                temp_val.flag_leveling_m = true;
                gcode.process_subcommands_now(F("M211 S1"));
                set_bed_leveling_enabled(level_state);
                TERN_(AUTO_BED_LEVELING_BILINEAR, bedlevel.refresh_bed_level());
                Popup_Handler(SaveLevel, true);
              }
              break;
            #if !HAS_RC_CPU
              case LEVELING_M_HOME:
                if (draw) Draw_Menu_Item(row, ICON_Back, GET_TEXT_F(MSG_MAIN));
                else {
                  temp_val.flag_QuickHome = true;
                  temp_val.liveadjust = TERN(USER_MOVE_LIVE, true, false);
                  BedLevelTools.goto_mesh_value = false;
                  temp_val.flag_leveling_m = true;
                  gcode.process_subcommands_now(F("M211 S1"));
                  set_bed_leveling_enabled(level_state);
                  TERN_(AUTO_BED_LEVELING_BILINEAR, bedlevel.refresh_bed_level());
                  Popup_Handler(SaveLevel, true);
                  }
                break;
            #endif
            case LEVELING_M_MODELIVE:
              if (draw) {
                Draw_Menu_Item(row, ICON_Axis, GET_TEXT_F(MSG_LIVE_ADJUSTMENT));
                Draw_Checkbox(row, temp_val.liveadjust);
              }
              else {
                temp_val.liveadjust = !temp_val.liveadjust;
                Draw_Checkbox(row, temp_val.liveadjust);
              }
              break;
            case LEVELING_M_X:
              if (draw) {
                Draw_Menu_Item(row, ICON_MoveX, GET_TEXT_F(MSG_MESH_X));
                Draw_Float(BedLevelTools.mesh_x, row, 0, 1);
              }
              else
                Modify_Value(BedLevelTools.mesh_x, 0, GRID_MAX_POINTS_X - 1, 1);
              break;
            case LEVELING_M_Y:
              if (draw) {
                Draw_Menu_Item(row, ICON_MoveY, GET_TEXT_F(MSG_MESH_Y));
                Draw_Float(BedLevelTools.mesh_y, row, 0, 1);
              }
              else
                Modify_Value(BedLevelTools.mesh_y, 0, GRID_MAX_POINTS_Y - 1, 1);
              break;
            case LEVELING_M_NEXT:
              if (draw) {
                if ((BedLevelTools.mesh_x != (GRID_MAX_POINTS_X - 1) && (!temp_val.evenvalue)) || BedLevelTools.mesh_y != (GRID_MAX_POINTS_Y - 1))
                  Draw_Menu_Item(row, ICON_More, GET_TEXT_F(MSG_LEVEL_BED_NEXT_POINT));
                else if (temp_val.evenvalue && BedLevelTools.mesh_x != 0)
                  Draw_Menu_Item(row, ICON_More, GET_TEXT_F(MSG_LEVEL_BED_NEXT_POINT));
                else
                  Draw_Menu_Item(row, ICON_More, F("Finished"));
              }
              else {
                BedLevelTools.next_point();
                }
                break;
            case LEVELING_M_PREV:
                if (draw) {
                  Draw_Menu_Item(row, ICON_More, GET_TEXT_F(MSG_LEVEL_BED_PREV_POINT));
                }
                else {
                  BedLevelTools.prev_point();
                }
                break;
            case LEVELING_M_OFFSET:
              if (draw) {
                Draw_Menu_Item(row, ICON_SetZOffset, GET_TEXT_F(MSG_OFFSET_Z));
                Draw_Float(bedlevel.z_values[BedLevelTools.mesh_x][BedLevelTools.mesh_y], row, false, 100);
              }
              else {
                if (isnan(bedlevel.z_values[BedLevelTools.mesh_x][BedLevelTools.mesh_y]))
                  bedlevel.z_values[BedLevelTools.mesh_x][BedLevelTools.mesh_y] = 0;
                Modify_Value(bedlevel.z_values[BedLevelTools.mesh_x][BedLevelTools.mesh_y], MIN_Z_OFFSET, MAX_Z_OFFSET, 100);
              }
              break;
            case LEVELING_M_UP:
              if (draw) {
                sprintf_P(cmd, PSTR("%s %s"), GET_TEXT(MSG_BABYSTEP_Z), GET_TEXT(MSG_UP));
                Draw_Menu_Item(row, ICON_Axis, F(cmd));
                }
              else if (bedlevel.z_values[BedLevelTools.mesh_x][BedLevelTools.mesh_y] < MAX_Z_OFFSET) {
                bedlevel.z_values[BedLevelTools.mesh_x][BedLevelTools.mesh_y] += 0.01;
                gcode.process_subcommands_now(F("M290 Z0.01"));
                planner.synchronize();
                current_position.z += 0.01f;
                sync_plan_position();
                Draw_Float(bedlevel.z_values[BedLevelTools.mesh_x][BedLevelTools.mesh_y], row - 1, false, 100);
              }
              break;
            case LEVELING_M_DOWN:
              if (draw) {
                sprintf_P(cmd, PSTR("%s %s"), GET_TEXT(MSG_BABYSTEP_Z), GET_TEXT(MSG_DOWN));
                Draw_Menu_Item(row, ICON_AxisD, F(cmd));
                }
              else if (bedlevel.z_values[BedLevelTools.mesh_x][BedLevelTools.mesh_y] > MIN_Z_OFFSET) {
                bedlevel.z_values[BedLevelTools.mesh_x][BedLevelTools.mesh_y] -= 0.01;
                gcode.process_subcommands_now(F("M290 Z-0.01"));
                planner.synchronize();
                current_position.z -= 0.01f;
                sync_plan_position();
                Draw_Float(bedlevel.z_values[BedLevelTools.mesh_x][BedLevelTools.mesh_y], row - 2, false, 100);
              }
              break;
            case LEVELING_M_GOTO_VALUE:
              if (draw) {
                Draw_Menu_Item(row, ICON_Probe, GET_TEXT_F(MSG_MESH_SNAP_Z));
                Draw_Checkbox(row, BedLevelTools.goto_mesh_value);
              }
              else {
                BedLevelTools.goto_mesh_value = !BedLevelTools.goto_mesh_value;
                current_position.z = 0;
                BedLevelTools.manual_mesh_move(true);
                Draw_Checkbox(row, BedLevelTools.goto_mesh_value);
              }
              break;
            #if ENABLED(AUTO_BED_LEVELING_UBL)
            case LEVELING_M_UNDEF:
              if (draw)
                Draw_Menu_Item(row, ICON_ResumeEEPROM, GET_TEXT_F(MSG_LEVEL_BED_CLEAR_POINT));
              else {
                BedLevelTools.manual_value_update(true);
                Redraw_Menu(false);
              }
              break;
            #endif
          }
          break;
        #if EXTJYERSUI
          case MeshInsetMenu:

            #define MESHINSET_BACK 0
            #define MESHINSET_PMARGIN (MESHINSET_BACK + 1)
            #define MESHINSET_MINX (MESHINSET_PMARGIN + 1)
            #define MESHINSET_MAXX (MESHINSET_MINX + 1)
            #define MESHINSET_MINY (MESHINSET_MAXX + 1)
            #define MESHINSET_MAXY (MESHINSET_MINY + 1)
            #define MESHINSET_MAX_AREA (MESHINSET_MAXY + 1)
            #define MESHINSET_CENTER_AREA (MESHINSET_MAX_AREA + 1)
            #define MESHINSET_TOTAL MESHINSET_CENTER_AREA


            switch (item) {
              case MESHINSET_BACK:
                if (draw)
                  Draw_Menu_Item(row, ICON_Back, GET_TEXT_F(MSG_BACK));
                else
                  Draw_Menu(LevelSettings, LEVELING_SETTINGS_MESHINSET);
              break;
              case MESHINSET_PMARGIN:
                  if (draw) {
                    Draw_Menu_Item(row, ICON_ProbeMargin, GET_TEXT_F(MSG_ZPROBE_MARGIN));
                    Draw_Float(HMI_datas.probing_margin, row, false, 1);
                    }
                  else
                    Modify_Value(HMI_datas.probing_margin, MIN_PROBE_MARGIN, MAX_PROBE_MARGIN, 1);
                  break;
              case MESHINSET_MINX:
                if (draw) {
                  Draw_Menu_Item(row, ICON_ProbeMargin, GET_TEXT_F(MSG_MESH_MIN_X));
                  Draw_Float(HMI_datas.mesh_min_x, row, false, 1);
                }
                else {
                  temp_val.last_meshinset_value = HMI_datas.mesh_min_x;
                  Modify_Value(HMI_datas.mesh_min_x, MIN_MESH_INSET, MAX_MESH_INSET, 1);
                }
                break;
              case MESHINSET_MAXX:
                if (draw) {
                  Draw_Menu_Item(row, ICON_ProbeMargin, GET_TEXT_F(MSG_MESH_MAX_X));
                  Draw_Float(HMI_datas.mesh_max_x, row, false, 1);
                }
                else {
                  temp_val.last_meshinset_value = HMI_datas.mesh_max_x;
                  Modify_Value(HMI_datas.mesh_max_x, MIN_MESH_INSET , MAX_MESH_INSET, 1);
                }
                break;
              case MESHINSET_MINY:
                if (draw) {
                  Draw_Menu_Item(row, ICON_ProbeMargin, GET_TEXT_F(MSG_MESH_MIN_Y));
                  Draw_Float(HMI_datas.mesh_min_y, row, false, 1);
                }
                else {
                  temp_val.last_meshinset_value = HMI_datas.mesh_min_y;
                  Modify_Value(HMI_datas.mesh_min_y, MIN_MESH_INSET, MAX_MESH_INSET, 1);
                }
                break;
              case MESHINSET_MAXY:
                if (draw) {
                  Draw_Menu_Item(row, ICON_ProbeMargin, GET_TEXT_F(MSG_MESH_MAX_Y));
                  Draw_Float(HMI_datas.mesh_max_y, row, false, 1);
                }
                else {
                  temp_val.last_meshinset_value = HMI_datas.mesh_max_y;
                  Modify_Value(HMI_datas.mesh_max_y, MIN_MESH_INSET , MAX_MESH_INSET, 1);
                }
                break;
              case MESHINSET_MAX_AREA:
                if (draw) 
                  Draw_Menu_Item(row, ICON_ProbeMargin, GET_TEXT_F(MSG_MESH_AMAX));
                else MaxMeshArea();
                break;
              case MESHINSET_CENTER_AREA:
                if (draw) 
                  Draw_Menu_Item(row, ICON_ProbeMargin, GET_TEXT_F(MSG_MESH_CENTER));
                else  CenterMeshArea();
                break;
            }
          break;
        #endif

      #endif // HAS_MESH

      #if ENABLED(AUTO_BED_LEVELING_UBL) && !HAS_BED_PROBE
        case UBLMesh:

          #define UBL_M_BACK 0
          #define UBL_M_MODELIVE (UBL_M_BACK + 1)
          #define UBL_M_NEXT (UBL_M_MODELIVE + 1)
          #define UBL_M_PREV (UBL_M_NEXT + 1)
          #define UBL_M_OFFSET (UBL_M_PREV + 1)
          #define UBL_M_UP (UBL_M_OFFSET + 1)
          #define UBL_M_DOWN (UBL_M_UP + 1)
          #define UBL_M_TOTAL UBL_M_DOWN

          temp_val.evenvalue = (GRID_MAX_POINTS_X % 2 == 0);

          switch (item) {
            case UBL_M_BACK:
              if (draw)
                Draw_Menu_Item(row, ICON_Back, GET_TEXT_F(MSG_BACK));
              else {
                temp_val.liveadjust = TERN(USER_MOVE_LIVE, true, false);
                BedLevelTools.goto_mesh_value = false;
                gcode.process_subcommands_now(F("M211 S1"));
                set_bed_leveling_enabled(level_state);
                Dis_steppers_and_cooldown();
                Draw_Menu(Leveling, LEVELING_GET_MESH);
              }
              break;
            case UBL_M_MODELIVE:
              if (draw) {
                Draw_Menu_Item(row, ICON_Axis, GET_TEXT_F(MSG_LIVE_ADJUSTMENT));
                Draw_Checkbox(row, temp_val.liveadjust);
              }
              else {
                temp_val.liveadjust = !temp_val.liveadjust;
                Draw_Checkbox(row, temp_val.liveadjust);
              }
              break;
            case UBL_M_NEXT:
              if (draw) {
                if (BedLevelTools.mesh_x != (GRID_MAX_POINTS_X - 1) || BedLevelTools.mesh_y != (GRID_MAX_POINTS_Y - 1))
                  Draw_Menu_Item(row, ICON_More, GET_TEXT_F(MSG_LEVEL_BED_NEXT_POINT));
                else if (temp_val.evenvalue && BedLevelTools.mesh_x != 0)
                  Draw_Menu_Item(row, ICON_More, GET_TEXT_F(MSG_LEVEL_BED_NEXT_POINT));
                else
                  Draw_Menu_Item(row, ICON_More, GET_TEXT_F(MSG_UBL_SAVE_MESH));
              }
              else {
                BedLevelTools.next_point();
              }
              break;
            case UBL_M_PREV:
              if (draw)
                Draw_Menu_Item(row, ICON_More, GET_TEXT_F(MSG_LEVEL_BED_PREV_POINT));
              else {
                BedLevelTools.prev_point();
              }
              break;
            case UBL_M_OFFSET:
              if (draw) {
                Draw_Menu_Item(row, ICON_SetZOffset, GET_TEXT_F(MSG_OFFSET_Z));
                Draw_Float(bedlevel.z_values[BedLevelTools.mesh_x][BedLevelTools.mesh_y], row, false, 100);
              }
              else {
                if (isnan(bedlevel.z_values[BedLevelTools.mesh_x][BedLevelTools.mesh_y]))
                  bedlevel.z_values[BedLevelTools.mesh_x][BedLevelTools.mesh_y] = 0;
                Modify_Value(bedlevel.z_values[BedLevelTools.mesh_x][BedLevelTools.mesh_y], MIN_Z_OFFSET, MAX_Z_OFFSET, 100);
              }
              break;
            case UBL_M_UP:
              if (draw) {
                sprintf_P(cmd, PSTR("%s %s"), GET_TEXT(MSG_BABYSTEP_Z), GET_TEXT(MSG_UP));
                Draw_Menu_Item(row, ICON_Axis, F(cmd));
              }
              else if (bedlevel.z_values[BedLevelTools.mesh_x][BedLevelTools.mesh_y] < MAX_Z_OFFSET) {
                bedlevel.z_values[BedLevelTools.mesh_x][BedLevelTools.mesh_y] += 0.01;
                gcode.process_subcommands_now(F("M290 Z0.01"));
                planner.synchronize();
                current_position.z += 0.01f;
                sync_plan_position();
                Draw_Float(bedlevel.z_values[BedLevelTools.mesh_x][BedLevelTools.mesh_y], row - 1, false, 100);
                }
              break;
            case UBL_M_DOWN:
              if (draw) {
                sprintf_P(cmd, PSTR("%s %s"), GET_TEXT(MSG_BABYSTEP_Z), GET_TEXT(MSG_DOWN));
                Draw_Menu_Item(row, ICON_Axis, F(cmd));
              }
              else if (bedlevel.z_values[BedLevelTools.mesh_x][BedLevelTools.mesh_y] > MIN_Z_OFFSET) {
                bedlevel.z_values[BedLevelTools.mesh_x][BedLevelTools.mesh_y] -= 0.01;
                gcode.process_subcommands_now(F("M290 Z-0.01"));
                planner.synchronize();
                current_position.z -= 0.01f;
                sync_plan_position();
                Draw_Float(bedlevel.z_values[BedLevelTools.mesh_x][BedLevelTools.mesh_y], row - 2, false, 100);
              }
              break;
          }
          break;
      #endif // AUTO_BED_LEVELING_UBL && !HAS_BED_PROBE

      #if ENABLED(PROBE_MANUALLY)
        case ManualMesh:

          #define MMESH_BACK 0
          #define MMESH_MODELIVE (MMESH_BACK + 1)
          #define MMESH_NEXT (MMESH_MODELIVE + 1)
          #define MMESH_OFFSET (MMESH_NEXT + 1)
          #define MMESH_UP (MMESH_OFFSET + 1)
          #define MMESH_DOWN (MMESH_UP + 1)
          #define MMESH_OLD (MMESH_DOWN + 1)
          #define MMESH_TOTAL MMESH_OLD

          switch (item) {
            case MMESH_BACK:
              if (draw)
                Draw_Menu_Item(row, ICON_Back, GET_TEXT_F(MSG_BUTTON_CANCEL));
              else {
                temp_val.liveadjust = TERN(USER_MOVE_LIVE, true, false);
                gcode.process_subcommands_now(F("M211 S1\nG29 A"));
                planner.synchronize();
                set_bed_leveling_enabled(level_state);
                Dis_steppers_and_cooldown();
                Draw_Menu(Leveling, LEVELING_GET_MESH);
              }
              break;
            case MMESH_MODELIVE:
              if (draw) {
                Draw_Menu_Item(row, ICON_Axis, GET_TEXT_F(MSG_LIVE_ADJUSTMENT));
                Draw_Checkbox(row, temp_val.liveadjust);
              }
              else {
                temp_val.liveadjust = !temp_val.liveadjust;
                Draw_Checkbox(row, temp_val.liveadjust);
              }
              break;
            case MMESH_NEXT:
              if (draw) {
                if (temp_val.gridpoint < GRID_MAX_POINTS)
                  Draw_Menu_Item(row, ICON_More, GET_TEXT_F(MSG_LEVEL_BED_NEXT_POINT));
                else
                  Draw_Menu_Item(row, ICON_More, GET_TEXT_F(MSG_UBL_SAVE_MESH));
              }
              else if (temp_val.gridpoint < GRID_MAX_POINTS) {
                Popup_Handler(MoveWait);
                gcode.process_subcommands_now(F("G29"));
                planner.synchronize();
                temp_val.gridpoint++;
                Redraw_Menu();
              }
              else {
                gcode.process_subcommands_now(F("G29"));
                planner.synchronize();
                temp_val.flag_noprobe = true;
                Viewmesh();
              }
              break;
            case MMESH_OFFSET:
              if (draw) {
                Draw_Menu_Item(row, ICON_SetZOffset, GET_TEXT_F(MSG_OFFSET_Z));
                #ifdef MANUAL_PROBE_START_Z
                  current_position.z = MANUAL_PROBE_START_Z;
                #endif
                Draw_Float(current_position.z, row, false, 100);
              }
              else
                Modify_Value(current_position.z, MIN_Z_OFFSET, MAX_Z_OFFSET, 100);
              break;
            case MMESH_UP:
              if (draw) {
                sprintf_P(cmd, PSTR("%s %s"), GET_TEXT(MSG_BABYSTEP_Z), GET_TEXT(MSG_UP));
                Draw_Menu_Item(row, ICON_Axis, F(cmd));
              }
              else if (current_position.z < MAX_Z_OFFSET) {
                gcode.process_subcommands_now(F("M290 Z0.01"));
                planner.synchronize();
                current_position.z += 0.01f;
                sync_plan_position();
                Draw_Float(current_position.z, row - 1, false, 100);
                }
              break;
            case MMESH_DOWN:
              if (draw) {
                sprintf_P(cmd, PSTR("%s %s"), GET_TEXT(MSG_BABYSTEP_Z), GET_TEXT(MSG_DOWN));
                Draw_Menu_Item(row, ICON_AxisD, F(cmd));
              }
              else if (current_position.z > MIN_Z_OFFSET) {
                gcode.process_subcommands_now(F("M290 Z-0.01"));
                planner.synchronize();
                current_position.z -= 0.01f;
                sync_plan_position();
                Draw_Float(current_position.z, row - 2, false, 100);
              }
              break;
            case MMESH_OLD:
              uint8_t mesh_x, mesh_y;
              // 0,0 -> 1,0 -> 2,0 -> 2,1 -> 1,1 -> 0,1 -> 0,2 -> 1,2 -> 2,2
              mesh_y = (temp_val.gridpoint - 1) / (GRID_MAX_POINTS_Y);
              mesh_x = (temp_val.gridpoint - 1) % (GRID_MAX_POINTS_X);

              if (mesh_y % 2 == 1)
                mesh_x = (GRID_MAX_POINTS_X) - mesh_x - 1;

              const float currval = bedlevel.z_values[mesh_x][mesh_y];

              if (draw) {
                Draw_Menu_Item(row, ICON_Zoffset, GET_TEXT_F(MSG_MESH_EDIT_Z));
                Draw_Float(currval, row, false, 100);
              }
              else if (!isnan(currval)) {
                current_position.z = currval;
                planner.synchronize();
                planner.buffer_line(current_position, homing_feedrate(Z_AXIS), active_extruder);
                planner.synchronize();
                Draw_Float(current_position.z, row - 3, false, 100);
              }
              break;
          }
          break;
      #endif // PROBE_MANUALLY

      case Tune:

        #define TUNE_BACK 0
        #define TUNE_BACKLIGHT_OFF (TUNE_BACK + 1)
        #define TUNE_BACKLIGHT (TUNE_BACKLIGHT_OFF + 1)
        #define TUNE_SPEED (TUNE_BACKLIGHT + 1)
        #define TUNE_FLOW (TUNE_SPEED + ENABLED(HAS_HOTEND))
        #define TUNE_LIN_ADVANCE (TUNE_FLOW + ENABLED(HAS_HOTEND, LIN_ADVANCE))
        #define TUNE_HOTEND (TUNE_LIN_ADVANCE + ENABLED(HAS_HOTEND))
        #define TUNE_BED (TUNE_HOTEND + ENABLED(HAS_HEATED_BED))
        #define TUNE_FAN (TUNE_BED + ENABLED(HAS_FAN))
        #define TUNE_AUTOTEMP (TUNE_FAN + ENABLED(HAS_AUTOTEMP_MENU))
        #define TUNE_ZOFFSET (TUNE_AUTOTEMP + ENABLED(HAS_ZOFFSET_ITEM))
        #define TUNE_ZUP (TUNE_ZOFFSET + ENABLED(HAS_ZOFFSET_ITEM))
        #define TUNE_ZDOWN (TUNE_ZUP + ENABLED(HAS_ZOFFSET_ITEM))
        #define TUNE_FWRETRACT (TUNE_ZDOWN + ENABLED(FWRETRACT))
        #define TUNE_CANCEL_OBJECTS (TUNE_FWRETRACT + ENABLED(HAS_CANCEL_OBJECTS))
        #define TUNE_HOTEND_GRAPH (TUNE_CANCEL_OBJECTS + (ENABLED(EXTJYERSUI, PID_GRAPH, HAS_TEMP_SENSOR) && DISABLED(HAS_RC_CPU)))
        #define TUNE_BED_GRAPH (TUNE_HOTEND_GRAPH + (ENABLED(EXTJYERSUI, PID_GRAPH, HAS_HEATED_BED) && DISABLED(HAS_RC_CPU)))
        #define TUNE_HOSTACTIONS (TUNE_BED_GRAPH + ENABLED(HOST_ACTION_COMMANDS, HAS_HOSTACTION_MENUS))
        #define TUNE_CHANGEFIL (TUNE_HOSTACTIONS + ENABLED(FILAMENT_LOAD_UNLOAD_GCODES))
        #define TUNE_PARK_NOZZLE_TIMEOUT (TUNE_CHANGEFIL + (ENABLED(ADVANCED_PAUSE_FEATURE, EXTJYERSUI) && DISABLED(HAS_RC_CPU)))
        #define TUNE_FILSENSORENABLED (TUNE_PARK_NOZZLE_TIMEOUT + ENABLED(HAS_FILAMENT_SENSOR))
        #define TUNE_FILSENSORDISTANCE (TUNE_FILSENSORENABLED + ENABLED(HAS_FILAMENT_SENSOR))
        #define TUNE_CASELIGHT (TUNE_FILSENSORDISTANCE + ENABLED(CASE_LIGHT_MENU))
        #define TUNE_LEDCONTROL (TUNE_CASELIGHT + (ENABLED(LED_CONTROL_MENU) && DISABLED(CASE_LIGHT_USE_NEOPIXEL)))
        #define TUNE_SCREENLOCK (TUNE_LEDCONTROL + ENABLED(HAS_SCREENLOCK))
        #define TUNE_RESTORE_DISPLAY (TUNE_SCREENLOCK + 1)     
        #define TUNE_TOTAL TUNE_RESTORE_DISPLAY

        switch (item) {
          case TUNE_BACK:
            if (draw)
              Draw_Menu_Item(row, ICON_Back, GET_TEXT_F(MSG_BACK));
            else {
              temp_val.flag_selection = false;
              temp_val.flag_tune_submenu = false;
              temp_val.flag_tune_menu = false;
              Draw_Print_Screen();
            }
            break;
          case TUNE_SPEED:
            if (draw) {
              Draw_Menu_Item(row, ICON_Speed, GET_TEXT_F(MSG_SPEED));
              Draw_Float(feedrate_percentage, row, false, 1);
            }
            else
              Modify_Value(feedrate_percentage, MIN_PRINT_SPEED, MAX_PRINT_SPEED, 1);
            break;
          
          case TUNE_BACKLIGHT_OFF:
            if (draw)
              Draw_Menu_Item(row, ICON_Brightness, GET_TEXT_F(MSG_BRIGHTNESS_OFF));
            else
              ui.set_brightness(0);
            break;

          case TUNE_BACKLIGHT:
            if (draw) {
              Draw_Menu_Item(row, ICON_Brightness, GET_TEXT_F(MSG_BRIGHTNESS));
              Draw_Float(ui.brightness, row, false, 1);
            }
            else
              Modify_Value(ui.brightness, LCD_BRIGHTNESS_MIN, LCD_BRIGHTNESS_MAX, 1, ui.refresh_brightness);
            break;

          #if HAS_HOTEND
            case TUNE_FLOW:
              if (draw) {
                Draw_Menu_Item(row, ICON_StepE, GET_TEXT_F(MSG_FLOW));
                Draw_Float(planner.flow_percentage[0], row, false, 1);
              }
              else
                Modify_Value(planner.flow_percentage[0], MIN_FLOW_RATE, MAX_FLOW_RATE, 1);
              break;
              #if ENABLED(LIN_ADVANCE)
                case TUNE_LIN_ADVANCE:
                  if (draw) {
                    Draw_Menu_Item(row, ICON_MaxAccelerated, GET_TEXT_F(MSG_ADVANCE_K_E));
                    Draw_Float(planner.extruder_advance_K[0], row, false, 100);
                  }
                  else
                    Modify_Value(planner.extruder_advance_K[0], 0, 10, 100);
                  break;
              #endif
            case TUNE_HOTEND:
              if (draw) {
                Draw_Menu_Item(row, ICON_SetEndTemp, GET_TEXT_F(MSG_HOTEND_TEMPERATURE));
                Draw_Float(thermalManager.temp_hotend[0].target, row, false, 1);
              }
              else
                Modify_Value(thermalManager.temp_hotend[0].target, MIN_E_TEMP, MAX_E_TEMP, 1);
              break;
          #endif

          #if HAS_HEATED_BED
            case TUNE_BED:
              if (draw) {
                Draw_Menu_Item(row, ICON_SetBedTemp, GET_TEXT_F(MSG_BED_TEMPERATURE));
                Draw_Float(thermalManager.temp_bed.target, row, false, 1);
              }
              else
                Modify_Value(thermalManager.temp_bed.target, MIN_BED_TEMP, MAX_BED_TEMP, 1);
              break;
          #endif

          #if HAS_FAN
            case TUNE_FAN:
              Draw_item_fan_speed(draw, row);
              break;
          #endif

          #if HAS_AUTOTEMP_MENU
            case TUNE_AUTOTEMP:
              if (draw) {
                Draw_Menu_Item(row, ICON_SetEndTemp, F("Autotemp Menu"), nullptr, true);
               }
              else {
                temp_val.tune_item = TUNE_AUTOTEMP;
                temp_val.flag_tune_submenu = true;
                Get_fact_multiplicator(MULT_AUTOTEMP);
                Draw_Menu(AUTOTEMP_menu);
                }
              break;
          #endif

          #if HAS_ZOFFSET_ITEM
            case TUNE_ZOFFSET:
              if (draw) {
                Draw_Menu_Item(row, ICON_SetZOffset, GET_TEXT_F(MSG_OFFSET_Z));
                Draw_Float(temp_val.zoffsetvalue, row, false, 100);
              }
              else {
                Modify_Value(temp_val.zoffsetvalue, MIN_Z_OFFSET, MAX_Z_OFFSET, 100);
              }
              break;
            case TUNE_ZUP:
              if (draw) {
                sprintf_P(cmd, PSTR("%s %s"), GET_TEXT(MSG_OFFSET_Z), GET_TEXT(MSG_UP));
                Draw_Menu_Item(row, ICON_Axis, F(cmd));
              }
              else if (temp_val.zoffsetvalue < MAX_Z_OFFSET) {
                gcode.process_subcommands_now(F("M290 Z0.01"));
                temp_val.zoffsetvalue += 0.01;
                Draw_Float(temp_val.zoffsetvalue, row - 1, false, 100);
              }
              break;
            case TUNE_ZDOWN:
              if (draw) {
                sprintf_P(cmd, PSTR("%s %s"), GET_TEXT(MSG_OFFSET_Z), GET_TEXT(MSG_DOWN));
                Draw_Menu_Item(row, ICON_AxisD, F(cmd));
              }
              else if (temp_val.zoffsetvalue > MIN_Z_OFFSET) {
                gcode.process_subcommands_now(F("M290 Z-0.01"));
                temp_val.zoffsetvalue -= 0.01;
                Draw_Float(temp_val.zoffsetvalue, row - 2, false, 100);
              }
              break;
          #endif

          #if ENABLED(FWRETRACT)
            case TUNE_FWRETRACT:
              if (draw)
                Draw_Menu_Item(row, ICON_StepE, GET_TEXT_F(MSG_AUTORETRACT), nullptr, true);
              else {
                temp_val.tune_item = TUNE_FWRETRACT;
                temp_val.flag_tune_submenu = true;
                Draw_Menu(FwRetraction);
                }
              break;
          #endif

          #if HAS_CANCEL_OBJECTS
            case TUNE_CANCEL_OBJECTS:
              if (draw)
                Draw_Menu_Item(row, ICON_PrintSize, F("Cancel Objects Menu"), nullptr, true);
              else {
                if (cancelable.object_count != 0) {
                  temp_val.flag_tune_submenu = true;
                  Draw_Menu(CancelObjects_menu);
                }
                else ui.set_status("M486 don't exist in the file Gcodes");
              }
              break;
          #endif

          #if EXTJYERSUI && PID_GRAPH && !HAS_RC_CPU
            #if HAS_TEMP_SENSOR
              case TUNE_HOTEND_GRAPH:
                if (draw)
                  Draw_Menu_Item(row, ICON_HotendTemp, F("Hotend Temp Graph"));
                else
                  DWIN_Draw_Plot_Nozzle_Bed(true);
                break;
            #endif
            #if HAS_HEATED_BED
              case TUNE_BED_GRAPH:
                if (draw)
                  Draw_Menu_Item(row, ICON_BedTemp, F("Bed Temp Graph"));
                else
                  DWIN_Draw_Plot_Nozzle_Bed(false);
                break;
            #endif
          #endif

          #if ENABLED(HOST_ACTION_COMMANDS, HAS_HOSTACTION_MENUS)
            case TUNE_HOSTACTIONS:
              if (draw)
                Draw_Menu_Item(row, ICON_SetHome, GET_TEXT_F(MSG_HOST_ACTIONS), nullptr, true);
              else {
                temp_val.tune_item = TUNE_HOSTACTIONS;
                temp_val.flag_tune_submenu = true;
                Draw_Menu(HostActions);
                }
              break;
          #endif

          #if ENABLED(FILAMENT_LOAD_UNLOAD_GCODES)
            case TUNE_CHANGEFIL:
              if (draw)
                Draw_Menu_Item(row, ICON_ResumeEEPROM, GET_TEXT_F(MSG_FILAMENTCHANGE));
              else {
                Keep_selection();
                if (!print_job_timer.isPaused()) Popup_Handler(ConfFilChange);
                else
                  Update_Status("Filament change not allowed while paused print, only while printing");     
              }
              break;
          #endif

          #if ENABLED(ADVANCED_PAUSE_FEATURE, EXTJYERSUI) && !HAS_RC_CPU
            case TUNE_PARK_NOZZLE_TIMEOUT:
              if (draw) {
                Draw_Menu_Item(row, ICON_Step, F("Nozzle Timeout"));
                Draw_Float(HMI_datas.park_nozzle_timeout, row, false, 1);
              }
              else
                Modify_Value(HMI_datas.park_nozzle_timeout, MIN_PARK_NOZZLE_TIMEOUT, MAX_PARK_NOZZLE_TIMEOUT, 1);
              break;
          #endif

          #if HAS_FILAMENT_SENSOR
            case TUNE_FILSENSORENABLED:
              if (draw) {
                Draw_Menu_Item(row, ICON_Extruder, GET_TEXT_F(MSG_RUNOUT_SENSOR));
                Draw_Checkbox(row, runout.enabled[0]);
              }
              else {
                runout.enabled[0] = !runout.enabled[0];
                Draw_Checkbox(row, runout.enabled[0]);
              }
              break;
            case TUNE_FILSENSORDISTANCE:
              if (draw) {
                temp_val.editable_distance = runout.runout_distance();
                Draw_Menu_Item(row, ICON_MaxAccE, GET_TEXT_F(MSG_RUNOUT_DISTANCE_MM));
                Draw_Float(temp_val.editable_distance, row, false, 10);
              }
              else
                Modify_Value(temp_val.editable_distance, 0, 999, 10);
              break;
          #endif
          #if ENABLED(CASE_LIGHT_MENU)
            case TUNE_CASELIGHT:
              if (draw) {
                Draw_Menu_Item(row, ICON_CaseLight, GET_TEXT_F(MSG_CASE_LIGHT));
                Draw_Checkbox(row, caselight.on);
              }
              else {
                caselight.on = !caselight.on;
                caselight.update_enabled();
                Draw_Checkbox(row, caselight.on);
              }
              break;  
          #endif
          #if ENABLED(LED_CONTROL_MENU) && DISABLED(CASE_LIGHT_USE_NEOPIXEL)
            case TUNE_LEDCONTROL:
              if (draw) {
                Draw_Menu_Item(row, ICON_LedControl, GET_TEXT_F(MSG_LEDS));
                Draw_Checkbox(row, leds.lights_on);
              }
              else {
                leds.toggle();
                Draw_Checkbox(row, leds.lights_on);
              }
              break;
          #endif
          #if HAS_SCREENLOCK
            case TUNE_SCREENLOCK:
              if (draw) 
                Draw_Menu_Item(row, ICON_Lock, GET_TEXT_F(MSG_LOCKSCREEN));
              else 
                DWIN_ScreenLock();
              break;
          #endif
          case TUNE_RESTORE_DISPLAY:
            if (draw) 
              Draw_Menu_Item(row, ICON_ResumeEEPROM, F("Restore Display"));
            else {
              #if HAS_FAN
                temp_val.force_restore_fan = true;
              #endif
              Redraw_Screen();
            }
            break;

        }
        break;
      
      #if HAS_CANCEL_OBJECTS
        case CancelObjects_menu:
              
          #define TUNE_CO_BACK 0
          #define TUNE_CO_INFO (TUNE_CO_BACK + 1)
          #define TUNE_CO_CANCEL_CURRENT (TUNE_CO_INFO + 1)
          #define TUNE_CO_CANCEL_INDEX_NB (TUNE_CO_CANCEL_CURRENT + 1)
          #define TUNE_CO_TOTAL TUNE_CO_CANCEL_INDEX_NB

          temp_val.menucancelobjects = true;

          switch (item) {
            case TUNE_CO_BACK:
              if (draw) {
                Draw_Menu_Item(row, ICON_Back, GET_TEXT_F(MSG_BACK));
                char row1[25], row2[50];
                sprintf_P(row1, PSTR("%s: %i"), "Total Objects: ", cancelable.object_count);
                sprintf_P(row2, PSTR("%s: %i / %i"), "Canceled: ", Get_nb_canceled_objects(), cancelable.object_count);
                Draw_Menu_Item(TUNE_CO_INFO, ICON_HotendTemp, row1, row2, false, true);
              }
              else {
                temp_val.menucancelobjects = false;
                temp_val.flag_tune_submenu = false;
                Draw_Menu(Tune, TUNE_CANCEL_OBJECTS);
              }
              break;
            case TUNE_CO_CANCEL_CURRENT:
              if (draw) {
                Draw_Menu_Item(row, ICON_HotendTemp, F("Cancel Object"));
                Draw_Float(cancelable.active_object, row, false, 1);
              }
              else
                DWIN_Cancel_object(cancelable.active_object);
              break;
            case TUNE_CO_CANCEL_INDEX_NB:
              if (draw) {
                Draw_Menu_Item(row, ICON_HotendTemp, F("Cancel Object #"));
                Draw_Float(cancelable.active_object, row, false, 1);
              }
              else {
                temp_val.index_nb = cancelable.active_object;
                Modify_Value(temp_val.index_nb, -1, cancelable.object_count - 1, 1);
              }
              break;
          }
        break;
      #endif
        
      #if HAS_PREHEAT && HAS_HOTEND

        case PreheatHotend:

          #define PREHEATHOTEND_BACK 0
          #define PREHEATHOTEND_CONTINUE (PREHEATHOTEND_BACK + 1)
          #define PREHEATHOTEND_1 (PREHEATHOTEND_CONTINUE + (PREHEAT_COUNT >= 1))
          #define PREHEATHOTEND_2 (PREHEATHOTEND_1 + (PREHEAT_COUNT >= 2))
          #define PREHEATHOTEND_3 (PREHEATHOTEND_2 + (PREHEAT_COUNT >= 3))
          #define PREHEATHOTEND_4 (PREHEATHOTEND_3 + (PREHEAT_COUNT >= 4))
          #define PREHEATHOTEND_5 (PREHEATHOTEND_4 + (PREHEAT_COUNT >= 5))
          #define PREHEATHOTEND_6 (PREHEATHOTEND_5 + (PREHEAT_COUNT >= 6))
          #define PREHEATHOTEND_7 (PREHEATHOTEND_6 + (PREHEAT_COUNT >= 7))
          #define PREHEATHOTEND_8 (PREHEATHOTEND_7 + (PREHEAT_COUNT >= 8))
          #define PREHEATHOTEND_9 (PREHEATHOTEND_8 + (PREHEAT_COUNT >= 9))
          #define PREHEATHOTEND_10 (PREHEATHOTEND_9 + (PREHEAT_COUNT >= 10))
          #define PREHEATHOTEND_CUSTOM (PREHEATHOTEND_10 + 1)
          #define PREHEATHOTEND_TOTAL PREHEATHOTEND_CUSTOM

          switch (item) {
            case PREHEATHOTEND_BACK:
              if (draw)
                Draw_Menu_Item(row, ICON_Back, GET_TEXT_F(MSG_BUTTON_CANCEL));
              else {
                thermalManager.setTargetHotend(0, 0);
                TERN_(HAS_FAN, thermalManager.set_fan_speed(0, 0));
                #if HAS_E_CALIBRATION
                  if (temp_val.popup_EC == EC_Stage2) {
                  temp_val.selection = temp_val.old_last_selection;
                  temp_val.process = temp_val.old_process;
                  e_calibrate.Draw_Stage1();
                  }
                  else 
                #endif
                    Redraw_Menu(false, true, true, true);
              }
              break;
            case PREHEATHOTEND_CONTINUE:
              if (draw)
                Draw_Menu_Item(row, ICON_SetEndTemp, GET_TEXT_F(MSG_BUTTON_CONTINUE));
              else {
                Popup_Handler(Heating);
                Update_Status(GET_TEXT(MSG_HEATING));
                thermalManager.wait_for_hotend(0);
                #if HAS_E_CALIBRATION
                  if (temp_val.popup_EC == EC_Stage2) { e_calibrate.Draw_Stage2(); break;}
                #endif

                switch (temp_val.last_menu) {
                  case Prepare:
                    temp_val.flag_chg_fil = true;
                    Popup_Handler(FilChange);
                    sprintf_P(cmd, PSTR("M600 B1 R%i"), thermalManager.temp_hotend[0].target);
                    gcode.process_subcommands_now(cmd);
                    temp_val.flag_chg_fil = false;
                    Draw_Menu(Prepare, PREPARE_CHANGEFIL);
                    break;
                  #if ENABLED(FILAMENT_LOAD_UNLOAD_GCODES)
                    case ChangeFilament:
                      switch (temp_val.last_selection) {
                        case CHANGEFIL_LOAD:
                          temp_val.flag_chg_fil = true;
                          Popup_Handler(FilLoad);
                          Update_Status(GET_TEXT(MSG_FILAMENTLOAD)); 
                          gcode.process_subcommands_now(F("M701"));
                          planner.synchronize();
                          temp_val.flag_chg_fil = false;
                          Draw_Menu(ChangeFilament, CHANGEFIL_LOAD);
                          break;
                        case CHANGEFIL_UNLOAD:
                          Popup_Handler(FilLoad, true);
                          Update_Status(GET_TEXT(MSG_FILAMENTUNLOAD));
                          gcode.process_subcommands_now(F("M702"));
                          planner.synchronize();
                          Draw_Menu(ChangeFilament, CHANGEFIL_UNLOAD);
                          break;
                        case CHANGEFIL_CHANGE:
                          temp_val.flag_chg_fil = true;
                          Popup_Handler(FilChange);
                          Update_Status(GET_TEXT(MSG_FILAMENTCHANGE));
                          sprintf_P(cmd, PSTR("M600 B1 R%i"), thermalManager.temp_hotend[0].target);
                          gcode.process_subcommands_now(cmd);
                          temp_val.flag_chg_fil = false;
                          Draw_Menu(ChangeFilament, CHANGEFIL_CHANGE);
                          break;
                      }
                      break;
                  #endif
                  default:
                    Redraw_Menu(true, true, true);
                    break;
                }
              }
              break;


          #define _PREHEAT_HOTEND_CASE(N) \
            case PREHEATHOTEND_##N: \
              if (draw) Draw_Menu_Item(row, ICON_Temperature, F(PREHEAT_## N ##_LABEL)); \
              else ui.preheat_hotend_and_fan((N) - 1); \
              break;

            REPEAT_1(PREHEAT_COUNT, _PREHEAT_HOTEND_CASE)

            case PREHEATHOTEND_CUSTOM:
              if (draw) {
                Draw_Menu_Item(row, ICON_Temperature, GET_TEXT_F(MSG_PREHEAT_CUSTOM));
                Draw_Float(thermalManager.temp_hotend[0].target, row, false, 1);
              }
              else
                Modify_Value(thermalManager.temp_hotend[0].target, EXTRUDE_MINTEMP, MAX_E_TEMP, 1);
              break;
          }
          break;

      #endif // HAS_PREHEAT && HAS_HOTEN  
    }
  }

  FSTR_P CrealityDWINClass::Get_Menu_Title(uint8_t menu) {
    switch (menu) {
      case MainMenu:          return GET_TEXT_F(MSG_MAIN);
      case Prepare:           return GET_TEXT_F(MSG_PREPARE);
      case HomeMenu:          return GET_TEXT_F(MSG_HOMING);
      case Move:              return GET_TEXT_F(MSG_MOVE_AXIS);
      #if HAS_HOTEND && HAS_E_CALIBRATION
        case MoveE100:        return F("E-Calibration");
      #endif
      case ManualLevel:       return GET_TEXT_F(MSG_BED_TRAMMING_MANUAL);
      #if HAS_ZOFFSET_ITEM
        case ZOffset:         return GET_TEXT_F(MSG_OFFSET_Z);
      #endif
      #if HAS_PREHEAT
        case Preheat:         return GET_TEXT_F(MSG_PREHEAT);
      #endif
      #if ENABLED(FILAMENT_LOAD_UNLOAD_GCODES)
        case ChangeFilament:  return GET_TEXT_F(MSG_FILAMENTCHANGE);
      #endif
      #if HAS_CUSTOM_MENU
        case MenuCustom:
          #ifdef CUSTOM_MENU_CONFIG_TITLE
            return F(CUSTOM_MENU_CONFIG_TITLE);
          #else
            return F("Custom Commands");
          #endif
      #endif
      #if ENABLED(HOST_ACTION_COMMANDS, HAS_HOSTACTION_MENUS)
        case HostActions:       return GET_TEXT_F(MSG_HOST_ACTIONS);
      #endif
      case Control:           return GET_TEXT_F(MSG_CONTROL);
      case TempMenu:          return GET_TEXT_F(MSG_TEMPERATURE);
      #if HAS_AUTOTEMP_MENU
        case AUTOTEMP_menu:     return F("AUTOTEMP Settings");
      #endif
      #if HAS_HOTEND || HAS_HEATED_BED
        case PID:             
              sprintf_P(cmd, PSTR("%s %s"), GET_TEXT(MSG_PID), GET_TEXT(MSG_CONFIGURATION));
              return F(cmd);
      #endif
      #if HAS_HOTEND && BOTH(MPC_AUTOTUNE, MPC_AUTOTUNE_MENU)
        case MPC:
              sprintf_P(cmd, PSTR("%s %s"), GET_TEXT(MSG_MPC), GET_TEXT(MSG_CONFIGURATION));
              return F(cmd);
        #if ENABLED(MPC_EDIT_MENU)
          case MPCFilc:          return F("Filament C Settings");
        #endif
      #endif
      #if HAS_HOTEND && ENABLED(PIDTEMP)
        case HotendPID:       
              sprintf_P(cmd, PSTR("%s %s"), GET_TEXT(MSG_HOTEND_TEMPERATURE), GET_TEXT(MSG_PID));
              return F(cmd);
      #endif
      #if HAS_HEATED_BED && ENABLED(PIDTEMPBED)
        case BedPID:          
              sprintf_P(cmd, PSTR("%s %s"), GET_TEXT(MSG_BED_TEMPERATURE), GET_TEXT(MSG_PID));
              return F(cmd);
      #endif
      #if HAS_PREHEAT
        #define _PREHEAT_TITLE_CASE(N) case Preheat##N: sprintf_P(cmd, PSTR("%s %s"), PREHEAT_## N ##_LABEL, GET_TEXT(MSG_CONFIGURATION)); return F(cmd);
        REPEAT_1(PREHEAT_COUNT, _PREHEAT_TITLE_CASE)
      #endif
      case Motion:
              sprintf_P(cmd, PSTR("%s %s"), GET_TEXT(MSG_MOTION), GET_TEXT(MSG_CONFIGURATION));
              return F(cmd);
      #if ENABLED(FWRETRACT)
        case FwRetraction:
              sprintf_P(cmd, PSTR("%s %s"), GET_TEXT(MSG_FWRETRACT), GET_TEXT(MSG_CONFIGURATION));
              return F(cmd);
      #endif
      #if ENABLED(NOZZLE_PARK_FEATURE)
        case Parkmenu:
              sprintf_P(cmd, PSTR("%s %s"), GET_TEXT(MSG_FILAMENT_PARK_ENABLED), GET_TEXT(MSG_CONFIGURATION));
              return F(cmd);
      #endif
      #if ANY(CASE_LIGHT_MENU, LED_CONTROL_MENU)
        case Ledsmenu: 
              sprintf_P(cmd, PSTR("%s %s"), GET_TEXT(MSG_LEDS), GET_TEXT(MSG_CONFIGURATION));
              return F(cmd);
        #if ENABLED(CASE_LIGHT_MENU, CASELIGHT_USES_BRIGHTNESS)
          case CaseLightmenu: return GET_TEXT_F(MSG_CASE_LIGHT);
        #endif
        #if ENABLED(LED_CONTROL_MENU)
          case LedControlmenu: return GET_TEXT_F(MSG_LED_CONTROL);
          #if HAS_COLOR_LEDS
            #if ENABLED(LED_COLOR_PRESETS)
              case LedControlpresets: return GET_TEXT_F(MSG_LED_PRESETS);
            #endif
            case LedControlcustom: return GET_TEXT_F(MSG_CUSTOM_LEDS);
          #endif
        #endif
      #endif
      case HomeOffsets:       return GET_TEXT_F(MSG_SET_HOME_OFFSETS);
      case MaxSpeed:          return GET_TEXT_F(MSG_SPEED);
      case MaxAcceleration:   return GET_TEXT_F(MSG_ACCELERATION);
      #if HAS_CLASSIC_JERK
        case MaxJerk:         return GET_TEXT_F(MSG_JERK);
      #endif
      #if HAS_JUNCTION_DEVIATION
        case JDmenu:          return GET_TEXT_F(MSG_JUNCTION_DEVIATION_MENU);
      #endif
      case Steps:             return GET_TEXT_F(MSG_STEPS_PER_MM);
      case Visual:            return GET_TEXT_F(MSG_VISUAL_SETTINGS);
      case HostSettings:      return GET_TEXT_F(MSG_HOST_SETTINGS);
      #if ENABLED(HOST_ACTION_COMMANDS, HAS_HOSTACTION_MENUS)
        case ActionCommands:    return GET_TEXT_F(MSG_HOST_ACTIONS);
      #endif
      case Advanced:          return GET_TEXT_F(MSG_ADVANCED_SETTINGS);
      #if EXTJYERSUI && HAS_PHYSICAL_MENUS
        case PhySetMenu:      return GET_TEXT_F(MSG_PHY_SET);
      #endif
      #if HAS_BED_PROBE
        case ProbeMenu:       return GET_TEXT_F(MSG_ZPROBE_SETTINGS);
        #if HAS_MAG_MOUNTED_PROBE && EXTJYERSUI
          case MagMountedProbeMenu:       return F("Mag Probe Settings");
        #endif
      #endif
      #if HAS_TRINAMIC_CONFIG && !HAS_RC_CPU
        case TMCMenu:         return F("Trinamic Menu");
      #endif
      #if HAS_SHAPING
        case ISmenu:     return GET_TEXT_F(MSG_INPUT_SHAPING);
      #endif
      case Filmenu:           return GET_TEXT_F(MSG_FILAMENT_SET);
      case ColorSettings:     return GET_TEXT_F(MSG_COLORS_SELECT);
      #if HAS_SHORTCUTS
        case ShortcutSettings:  return GET_TEXT_F(MSG_SHORTCUT_MENU);
      #endif
      case Info:              return GET_TEXT_F(MSG_INFO_SCREEN);
      case InfoMain:          return GET_TEXT_F(MSG_INFO_SCREEN);
      #if HAS_MESH
        case Leveling:        return GET_TEXT_F(MSG_BED_LEVELING);
        case LevelView:       return GET_TEXT_F(MSG_MESH_VIEW);
        case LevelSettings:
          sprintf_P(cmd, PSTR("%s %s"), GET_TEXT(MSG_BED_LEVELING), GET_TEXT(MSG_CONFIGURATION));
          return F(cmd);
        #if EXTJYERSUI
          case MeshInsetMenu: 
            sprintf_P(cmd, PSTR("%s %s"), GET_TEXT(MSG_MESH_INSET), GET_TEXT(MSG_CONFIGURATION));
            return F(cmd);
        #endif
        case MeshViewer:      return GET_TEXT_F(MSG_MESH_VIEW);
        case LevelManual:     return GET_TEXT_F(MSG_UBL_FINE_TUNE_MESH);
      #endif
      #if ENABLED(AUTO_BED_LEVELING_UBL) && !HAS_BED_PROBE
        case UBLMesh:         return GET_TEXT_F(MSG_UBL_LEVEL_BED);
      #endif
      #if ENABLED(PROBE_MANUALLY)
        case ManualMesh:      return GET_TEXT_F(MSG_MESH_LEVELING);
      #endif
      case Tune:              return GET_TEXT_F(MSG_TUNE);
      #if HAS_CANCEL_OBJECTS
        case CancelObjects_menu:  return F("Cancel Objects");
      #endif
      case PreheatHotend:
              sprintf_P(cmd, PSTR("%s %s"), GET_TEXT(MSG_PREHEAT), GET_TEXT(MSG_HOTEND));
              return F(cmd);
    }
    return F("");
  }

  uint8_t CrealityDWINClass::Get_Menu_Size(uint8_t menu) {
    switch (menu) {
      case Prepare:           return PREPARE_TOTAL;
      case HomeMenu:          return HOME_TOTAL;
      case Move:              return MOVE_TOTAL;
      #if HAS_HOTEND && HAS_E_CALIBRATION
        case MoveE100:        return MOVE_E100_TOTAL;
      #endif
      case ManualLevel:       return MLEVEL_TOTAL;
      #if HAS_ZOFFSET_ITEM
        case ZOffset:         return ZOFFSET_TOTAL;
      #endif
      #if HAS_PREHEAT
        case Preheat:         return PREHEAT_TOTAL;
      #endif
      #if ENABLED(FILAMENT_LOAD_UNLOAD_GCODES)
        case ChangeFilament:  return CHANGEFIL_TOTAL;
      #endif
      #if HAS_CUSTOM_MENU
        case MenuCustom:      return CUSTOM_MENU_TOTAL;
      #endif
      #if ENABLED(HOST_ACTION_COMMANDS, HAS_HOSTACTION_MENUS)
        case HostActions:       return HOSTACTIONS_TOTAL;
      #endif
      case Control:           return CONTROL_TOTAL;
      case TempMenu:          return TEMP_TOTAL;
      #if HAS_AUTOTEMP_MENU
        case AUTOTEMP_menu:     return AUTOTEMP_TOTAL;
      #endif
      #if HAS_HOTEND || HAS_HEATED_BED
        case PID:             return PID_TOTAL;
      #endif
      #if HAS_HOTEND && BOTH(MPC_AUTOTUNE, MPC_AUTOTUNE_MENU)
        case MPC:             return MPC_TOTAL;
        #if ENABLED(MPC_EDIT_MENU)
          case MPCFilc:         return MPCFILC_TOTAL;
        #endif
      #endif
      #if HAS_HOTEND && ENABLED(PIDTEMP)
        case HotendPID:       return HOTENDPID_TOTAL;
      #endif
      #if HAS_HEATED_BED && ENABLED(PIDTEMPBED)
        case BedPID:          return BEDPID_TOTAL;
      #endif
      #if HAS_PREHEAT
      case Preheat1 ... CAT(Preheat, PREHEAT_COUNT):
                            return PREHEAT_SUBMENU_TOTAL;
      #endif
      case Motion:            return MOTION_TOTAL;
      #if ENABLED(FWRETRACT)
        case FwRetraction:    return FWR_TOTAL;
      #endif
      #if ENABLED(NOZZLE_PARK_FEATURE) && EXTJYERSUI
        case Parkmenu:        return PARKMENU_TOTAL;
      #endif
      #if ANY(CASE_LIGHT_MENU, LED_CONTROL_MENU)
        case Ledsmenu:         return LEDS_TOTAL;
        #if ENABLED(CASE_LIGHT_MENU, CASELIGHT_USES_BRIGHTNESS)
          case CaseLightmenu: return CASE_LIGHT_TOTAL;
        #endif
        #if ENABLED(LED_CONTROL_MENU)
          case LedControlmenu: return LEDCONTROL_TOTAL;
          #if HAS_COLOR_LEDS
            #if ENABLED(LED_COLOR_PRESETS)
              case LedControlpresets: return LEDCONTROL_PRESETS_TOTAL;
            #endif
            case LedControlcustom: return LEDCONTROL_CUSTOM_TOTAL;
          #endif
        #endif
      #endif
      case HomeOffsets:       return HOMEOFFSETS_TOTAL;
      case MaxSpeed:          return SPEED_TOTAL;
      case MaxAcceleration:   return ACCEL_TOTAL;
      #if HAS_CLASSIC_JERK
        case MaxJerk:         return JERK_TOTAL;
      #endif
      #if HAS_JUNCTION_DEVIATION
        case JDmenu:          return JD_TOTAL;
      #endif
      case Steps:             return STEPS_TOTAL;
      case Visual:            return VISUAL_TOTAL;
      #if ENABLED(HOST_ACTION_COMMANDS, HAS_HOSTACTION_MENUS)
        case HostSettings:      return HOSTSETTINGS_TOTAL;
        case ActionCommands:    return ACTIONCOMMANDS_TOTAL;
      #endif
      case Advanced:          return ADVANCED_TOTAL;
      #if EXTJYERSUI && HAS_PHYSICAL_MENUS
        case PhySetMenu:      return PHYSET_TOTAL;
      #endif
      #if HAS_BED_PROBE
        case ProbeMenu:       return PROBE_TOTAL;
        #if HAS_MAG_MOUNTED_PROBE && EXTJYERSUI
          case MagMountedProbeMenu:       return MAGPROBE_TOTAL;
        #endif
      #endif
      #if HAS_TRINAMIC_CONFIG && !HAS_RC_CPU
        case TMCMenu:         return TMC_TOTAL;
      #endif
      #if HAS_SHAPING
        case ISmenu:     return SHAPING_TOTAL;
      #endif
      case Filmenu:           return FIL_TOTAL;
      case Info:              return INFO_TOTAL;
      case InfoMain:          return INFO_TOTAL;
      #if ENABLED(AUTO_BED_LEVELING_UBL) && !HAS_BED_PROBE
        case UBLMesh:         return UBL_M_TOTAL;
      #endif
      #if ENABLED(PROBE_MANUALLY)
        case ManualMesh:      return MMESH_TOTAL;
      #endif
      #if HAS_MESH
        case Leveling:        return LEVELING_TOTAL;
        case LevelView:       return LEVELING_VIEW_TOTAL;
        case LevelSettings:   return LEVELING_SETTINGS_TOTAL;
        case MeshViewer:      return MESHVIEW_TOTAL;
        case LevelManual:     return LEVELING_M_TOTAL;
        #if EXTJYERSUI
          case MeshInsetMenu: return MESHINSET_TOTAL;
        #endif
      #endif
      case Tune:              return TUNE_TOTAL;
      #if HAS_CANCEL_OBJECTS
        case CancelObjects_menu:  return TUNE_CO_TOTAL;
      #endif
      #if HAS_PREHEAT && HAS_HOTEND
        case PreheatHotend:   return PREHEATHOTEND_TOTAL;
      #endif
      case ColorSettings:     return COLORSETTINGS_TOTAL;
        case ShortcutSettings:  return SHORTCUTSETTINGS_TOTAL;
      #if HAS_SHORTCUTS
      #endif
    }
    return 0;
  }

  /* Popup Config */

  void CrealityDWINClass::Popup_Handler(PopupID popupid, bool option/*=false*/) {
    temp_val.popup = temp_val.last_popup = popupid;
    switch (popupid) {
      case Pause:         Draw_Popup(GET_TEXT_F(MSG_PAUSE_PRINT), F(""), F(""), Popup); break;
      case Stop:          Draw_Popup(GET_TEXT_F(MSG_STOP_PRINT), F(""), F(""), Popup); break;
      case Resume:        Draw_Popup(GET_TEXT_F(MSG_RESUME_PRINT), GET_TEXT_F(MSG_RESUME_PRINT2), GET_TEXT_F(MSG_RESUME_PRINT3), Popup); break;
      case ConfFilChange: 
            sprintf_P(cmd,PSTR("%s %s"),GET_TEXT(MSG_BUTTON_CONFIRM),GET_TEXT(MSG_FILAMENT_CHANGE));
            Draw_Popup(F(cmd), F(""), F(""), Popup); 
            break;
      case PurgeMore:     Draw_Popup(GET_TEXT_F(MSG_FILAMENT_CHANGE_OPTION_PURGE), GET_TEXT_F(MSG_FILAMENT_CHANGE_FINISH), F(""), Popup); break;
      case SaveLevel:     Draw_Popup(option ? GET_TEXT_F(MSG_UBL_FINE_TUNE_MESH_COMPLETE) : GET_TEXT_F(MSG_LEVEL_BED_DONE), GET_TEXT_F(MSG_UBL_SAVE_TO_EEPROM), F(""), Popup); break;
      case MeshSlot:      Draw_Popup(GET_TEXT_F(MSG_NO_VALID_MESH_SLOT), option ? GET_TEXT_F(MSG_NO_VALID_MESH2) : GET_TEXT_F(MSG_NO_VALID_MESH3), GET_TEXT_F(MSG_NO_VALID_MESH4), Popup, ICON_AutoLeveling); break;
      case ETemp:         Draw_Popup(GET_TEXT_F(MSG_HOTEND_TOO_COLD), GET_TEXT_F(MSG_HOTEND_TOO_COLD2), F(""), Popup); break;
      case ManualProbing: Draw_Popup(GET_TEXT_F(MSG_MANUAL_PROBING), GET_TEXT_F(MSG_MANUAL_PROBING_CONFIRM), GET_TEXT_F(MSG_MANUAL_PROBING_CANCEL), Popup, ICON_AutoLeveling); break;
      case Level:         Draw_Popup(option ? GET_TEXT_F(MSG_PROBING_IN_PROGRESS) : GET_TEXT_F(MSG_AUTO_BED_LEVELING), GET_TEXT_F(MSG_PLEASE_WAIT), F(""), Wait, ICON_AutoLeveling); break;
      case Home:          Draw_Popup(option ? GET_TEXT_F(MSG_PAUSE_PRINT_PARKING) : GET_TEXT_F(MSG_HOMING), GET_TEXT_F(MSG_PLEASE_WAIT), F(""), Wait, ICON_BLTouch); break;
      case MoveWait:      Draw_Popup(GET_TEXT_F(MSG_MOVING), GET_TEXT_F(MSG_PLEASE_WAIT), F(""), Wait, ICON_BLTouch); break;
      case Heating:       Draw_Popup(GET_TEXT_F(MSG_HEATING), GET_TEXT_F(MSG_PLEASE_WAIT), F(""), Wait, ICON_BLTouch); break;
      case FilLoad:       Draw_Popup(option ? GET_TEXT_F(MSG_FILAMENT_UNLOADING) : GET_TEXT_F(MSG_FILAMENT_LOADING), GET_TEXT_F(MSG_PLEASE_WAIT), F(""), Wait, ICON_BLTouch); break;
      case FilChange:     Draw_Popup(option ? GET_TEXT_F(MSG_END_PROCESS) : GET_TEXT_F(MSG_FILAMENT_CHANGE), GET_TEXT_F(MSG_PLEASE_WAIT), F(""), Wait, ICON_BLTouch); break;
      case TempWarn:      Draw_Popup(option ? GET_TEXT_F(MSG_HOTEND_TOO_COLD) : GET_TEXT_F(MSG_HOTEND_TOO_HOT), F(""), F(""), Wait, option ? ICON_TempTooLow : ICON_TempTooHigh); break;
      case Runout:        Draw_Popup(GET_TEXT_F(MSG_FILAMENT_RUNOUT), F(""), F(""), Wait, ICON_BLTouch); break;
      #if EITHER(PIDTEMP, PIDTEMPBED) && !PID_GRAPH
        case PIDWait:       Draw_Popup(option ? GET_TEXT_F(MSG_BED_PID_AUTOTUNE) : GET_TEXT_F(MSG_HOTEND_PID_AUTOTUNE), GET_TEXT_F(MSG_IN_PROGRESS), GET_TEXT_F(MSG_PLEASE_WAIT), Wait, option ? 0 : ICON_BLTouch); break;
      #endif
      #if ENABLED(MPC_AUTOTUNE)
        case MPCWait:     Draw_Popup(GET_TEXT_F(MSG_MPC_AUTOTUNE), GET_TEXT_F(MSG_IN_PROGRESS), GET_TEXT_F(MSG_PLEASE_WAIT), Wait, ICON_BLTouch); break; 
      #endif
      case Resuming:      Draw_Popup(GET_TEXT_F(MSG_RESUMING_PRINT), GET_TEXT_F(MSG_PLEASE_WAIT), F(""), Wait, ICON_BLTouch); break;
      case ConfirmStartPrint: Draw_Popup(option ? GET_TEXT_F(MSG_LOADING_PREVIEW) : GET_TEXT_F(MSG_PRINT_FILE), F(""), F(""), Popup); break;
      case Reprint:       Draw_Popup(F(""), GET_TEXT_F(MSG_REPRINT_FILE), F(""), Popup); break;
      #if HAS_CUSTOM_MENU
        case Custom:        Draw_Popup(F("Running Custom GCode"), F("Please wait until done."), F(""), Wait, ICON_BLTouch); break;
      #endif
      case Bed_T_Wizard: Draw_Popup(F("Bed Tramming with Probe"), F("Mode Wizard -> Confirm"), F("Mode Classic -> Cancel"), Popup, ICON_AutoLeveling); break;
      #if HAS_LIVEMESH
        case Level2:        Draw_Popup(GET_TEXT_F(MSG_AUTO_BED_LEVELING), GET_TEXT_F(MSG_PLEASE_WAIT), GET_TEXT_F(MSG_CANCEL_TO_STOP), Wait, ICON_AutoLeveling); break;
      #endif
      #if HAS_GRAPHICAL_LEVELMANUAL && EXTJYERSUI
        case EditMesh: Draw_Popup(F("Edit Mesh"), F("Mode Graphic -> Confirm"), F("Mode Classic -> Cancel"), Popup, ICON_AutoLeveling); break;
        case Finish_Editvalue: Draw_Popup(F("Edit Mesh"), F("Next point -> Confirm"), F("Finish -> Cancel"), Popup, ICON_AutoLeveling); break;
      #endif
      default: break;
    }
  }

  void CrealityDWINClass::Confirm_Handler(PopupID popupid, bool option/*=false*/) {
    temp_val.popup = popupid;
    switch (popupid) {
      case FilInsert:         Draw_Popup(GET_TEXT_F(MSG_INSERT_FILAMENT), GET_TEXT_F(MSG_ADVANCED_PAUSE_WAITING), F(""), Confirm); break;
      case HeaterTime:        Draw_Popup(GET_TEXT_F(MSG_HEATER_TIMEOUT), GET_TEXT_F(MSG_FILAMENT_CHANGE_HEAT), F(""), Confirm); break;
      case UserInput:         Draw_Popup(option ? GET_TEXT_F(MSG_STOPPED) :  GET_TEXT_F(MSG_WAITING_FOR_INPUT), GET_TEXT_F(MSG_ADVANCED_PAUSE_WAITING), F(""), Confirm); break;
      case LevelError:        Draw_Popup(GET_TEXT_F(MSG_COULDNT_ENABLE_LEVELING), GET_TEXT_F(MSG_VALID_MESH_MUST_EXIST), F(""), Confirm); break;
      case InvalidMesh:       Draw_Popup(GET_TEXT_F(MSG_VALID_MESH_MUST_EXIST), GET_TEXT_F(MSG_VALID_MESH_MUST_EXIST2), GET_TEXT_F(MSG_VALID_MESH_MUST_EXIST3), Confirm); break;
      case NocreatePlane:     Draw_Popup(GET_TEXT_F(MSG_COULDNT_CREATE_PLANE), GET_TEXT_F(MSG_VALID_MESH_MUST_EXIST), F(""), Confirm); break;
      #if ENABLED(PIDTEMP)
        case BadextruderNumber: Draw_Popup(GET_TEXT_F(MSG_PID_FAILED), GET_TEXT_F(MSG_PID_BAD_HEATER_ID), F(""), Confirm); break;
      #endif
      #if HAS_PID_HEATING
        case TemptooHigh:       Draw_Popup(GET_TEXT_F(MSG_PID_FAILED), GET_TEXT_F(MSG_PID_TEMP_TOO_HIGH), F(""), Confirm); break;
        case PIDTimeout:        Draw_Popup(GET_TEXT_F(MSG_PID_FAILED), GET_TEXT_F(MSG_PID_TIMEOUT), F(""), Confirm); break;
        case PIDDone:           Draw_Popup(F(""), GET_TEXT_F(MSG_PID_AUTOTUNE_DONE), F(""), Confirm, ICON_TempTooLow); break;
      #endif
      #if !HAS_LIVEMESH
        case Level2:            Draw_Popup(GET_TEXT_F(MSG_AUTO_BED_LEVELING), GET_TEXT_F(MSG_PLEASE_WAIT), GET_TEXT_F(MSG_CANCEL_TO_STOP), Confirm, ICON_AutoLeveling); break;
      #endif
      #if ENABLED(MPC_AUTOTUNE)
        case MPC_Temperror:   Draw_Popup(GET_TEXT_F(MSG_PID_FAILED), F(STR_MPC_AUTOTUNE STR_MPC_TEMPERATURE_ERROR), F(""), Confirm, ICON_TempTooHigh); break; 
        case MPC_Interrupted: Draw_Popup(GET_TEXT_F(MSG_ERROR), F(STR_MPC_AUTOTUNE_INTERRUPTED), F(""), Confirm, ICON_TempTooHigh); break;
        case MPC_Done: Draw_Popup(GET_TEXT_F(MSG_MPC_AUTOTUNE), GET_TEXT_F(MSG_DONE), F(""), Confirm, ICON_TempTooLow); break;
      #endif
      #if HAS_GRAPHICAL_LEVELMANUAL && EXTJYERSUI
        case Update_Editvalue: Draw_Popup(F("Turn knob to edit"), F("Continue to validate"), F(""), Editvalue_Mesh, ICON_AutoLeveling); break;
      #endif
      default: break;
    }
  }

  void CrealityDWINClass::Main_Menu_Control() {
    uint8_t Real_Count;
    EncoderState encoder_diffState = Encoder_ReceiveAnalyze();
    
    #if HAS_SHORTCUTS
      if (temp_val.shortcutsmode) Real_Count =  PAGE_COUNT + (temp_val.Available_Shortcut_Count - 3);
      else Real_Count =  PAGE_COUNT - 3;
    #endif
    if (encoder_diffState == ENCODER_DIFF_NO) return;
    #if HAS_SHORTCUTS
      if (encoder_diffState == ENCODER_DIFF_CW && temp_val.selection < Real_Count) {
    #else
      if (encoder_diffState == ENCODER_DIFF_CW && temp_val.selection < PAGE_COUNT - 1) {
    #endif
      #if HAS_SHORTCUTS
        if (temp_val.shortcutsmode) {
          if ((temp_val.selection == 4)  && (temp_val.old_sel_shortcut != 4)) temp_val.selection = temp_val.old_sel_shortcut - 1 ;
          if ((temp_val.selection == 5)  && (temp_val.old_sel_shortcut != 5)) temp_val.selection = temp_val.old_sel_shortcut ;
        }
      #endif
      temp_val.selection++; // Select Down
      #if HAS_SHORTCUTS
        if ((temp_val.shortcutsmode) && (temp_val.selection >= 4) && (temp_val.Available_Shortcut_Count > 1)) {
            if (temp_val.flagdir) {
              temp_val.activ_shortcut = 5;
            }
            else {
              temp_val.activ_shortcut = (temp_val.selection > temp_val.old_sel_shortcut)? 5 : 4;
              temp_val.flagdir = true;
            }
            temp_val.current_shortcut_0 =  Get_Available_Shortcut(temp_val.old_sel_shortcut - 4);
            temp_val.current_shortcut_1 =  Get_Available_Shortcut(temp_val.old_sel_shortcut - 3);
            temp_val.old_sel_shortcut = temp_val.selection;
        }
        else {
          if (temp_val.Available_Shortcut_Count == 1) temp_val.activ_shortcut = 4;
          else temp_val.activ_shortcut = 0;
        }
      #endif
      Main_Menu_Icons();
    }
    else if (encoder_diffState == ENCODER_DIFF_CCW && temp_val.selection > 0) {
      #if HAS_SHORTCUTS
        if (temp_val.shortcutsmode) {
          if (((temp_val.selection == 4)  && (temp_val.old_sel_shortcut != 4)) || ((temp_val.selection == 5)  && (temp_val.old_sel_shortcut != 5))) 
            temp_val.selection = temp_val.old_sel_shortcut;
        }
      #endif
      temp_val.selection--; // Select Up
      #if HAS_SHORTCUTS
        if ((temp_val.shortcutsmode) && (temp_val.selection >= 4) && (temp_val.Available_Shortcut_Count > 1)) {
          temp_val.flagdir = false;
          temp_val.activ_shortcut = 4;
          temp_val.current_shortcut_0 =  Get_Available_Shortcut(temp_val.selection - 4);
          temp_val.current_shortcut_1 =  Get_Available_Shortcut(temp_val.selection - 3);
          temp_val.old_sel_shortcut = temp_val.selection;
        }
        else {
          if (temp_val.Available_Shortcut_Count == 1) temp_val.activ_shortcut = 4;
          else temp_val.activ_shortcut = 0;
        }
      #endif
      Main_Menu_Icons();
    }
    else if (encoder_diffState == ENCODER_DIFF_ENTER) {

    temp_val.sd_item_flag = false;

    #if HAS_SHORTCUTS
      if (temp_val.shortcutsmode) {
        if (temp_val.selection <= 4) temp_val._selection = temp_val.selection;
        else temp_val._selection = temp_val.activ_shortcut;
      }
      else temp_val._selection = temp_val.selection;

      switch (temp_val._selection) {
        case PAGE_PRINT: 
          card.mount();
          #if ALL(SDSUPPORT, SDCARD_SORT_ALPHA, SDSORT_GCODE)
            temp_val.SDremoved = true;
            DWIN_Sort_SD(card.isMounted());
          #endif
          temp_val.sd_item_flag = true; 
          Draw_SD_List(); 
          break;
        case PAGE_PREPARE:  Draw_Menu(Prepare); break;
        case PAGE_CONTROL:  Draw_Menu(Control); break;
        case PAGE_INFO_LEVELING:  Draw_Menu(TERN(HAS_MESH, Leveling, InfoMain)); break;
        case PAGE_SHORTCUT0 :  Apply_shortcut(temp_val.current_shortcut_0); break;
        case PAGE_SHORTCUT1 :  Apply_shortcut(temp_val.current_shortcut_1); break;
      }
    #else
      switch (temp_val.selection) {
        case PAGE_PRINT: 
          card.mount();
          #if ALL(SDSUPPORT, SDCARD_SORT_ALPHA, SDSORT_GCODE)
            temp_val.SDremoved = true;
            DWIN_Sort_SD(card.isMounted());
          #endif
          temp_val.sd_item_flag = true; 
          Draw_SD_List(); 
          break;
        case PAGE_PREPARE:  Draw_Menu(Prepare); break;
        case PAGE_CONTROL:  Draw_Menu(Control); break;
        case PAGE_INFO_LEVELING:  Draw_Menu(TERN(HAS_MESH, Leveling, InfoMain)); break;
      }
    #endif
    }
    DWIN_UpdateLCD();
  }

  void CrealityDWINClass::Menu_Control() {
    #if HAS_HOTEND && HAS_E_CALIBRATION
      while(temp_val.flag_wait != false) { return; }
    #endif
    uint16_t cColor = GetColor(HMI_datas.cursor_color, Rectangle_Color);
    const uint16_t color_background = TERN(HAS_TJC_DISPLAY, Color_Bg_Black, GetColor(HMI_datas.background, Color_Bg_Black));
    EncoderState encoder_diffState = Encoder_ReceiveAnalyze();
    if (encoder_diffState == ENCODER_DIFF_NO) return;
    if (encoder_diffState == ENCODER_DIFF_CW && temp_val.selection < Get_Menu_Size(temp_val.active_menu)) {
      DWIN_Draw_Rectangle(1, color_background, 0, MBASE(temp_val.selection - temp_val.scrollpos) - 18, 14, MBASE(temp_val.selection - temp_val.scrollpos) + 33);
      temp_val.selection++; // Select Down
      if (temp_val.selection > temp_val.scrollpos+MROWS) {
        temp_val.scrollpos++;
        DWIN_Frame_AreaMove(1, 2, MLINE, color_background, 0, 31, DWIN_WIDTH, 349);
        Menu_Item_Handler(temp_val.active_menu, temp_val.selection);
      }
      if ((cColor == color_background) || ((cColor == Color_Black) && (HMI_datas.background == 0)))
        DWIN_Draw_Rectangle(0, GetColor(HMI_datas.items_menu_text, Color_White), 0, MBASE(temp_val.selection - temp_val.scrollpos) - 18, 14, MBASE(temp_val.selection - temp_val.scrollpos) + 31);
      else
        DWIN_Draw_Rectangle(1, cColor, 0, MBASE(temp_val.selection - temp_val.scrollpos) - 18, 14, MBASE(temp_val.selection - temp_val.scrollpos) + 31);
    }
    else if (encoder_diffState == ENCODER_DIFF_CCW && temp_val.selection > 0) {
      DWIN_Draw_Rectangle(1, color_background, 0, MBASE(temp_val.selection - temp_val.scrollpos) - 18, 14, MBASE(temp_val.selection - temp_val.scrollpos) + 33);
      temp_val.selection--; // Select Up
      if (temp_val.selection < temp_val.scrollpos) {
        temp_val.scrollpos--;
        DWIN_Frame_AreaMove(1, 3, MLINE, color_background, 0, 31, DWIN_WIDTH, 349);
        Menu_Item_Handler(temp_val.active_menu, temp_val.selection);
      }
      if ((cColor == color_background) || ((cColor == Color_Black) && (HMI_datas.background == 0)))
        DWIN_Draw_Rectangle(0, GetColor(HMI_datas.items_menu_text, Color_White), 0, MBASE(temp_val.selection-temp_val.scrollpos) - 18, 14, MBASE(temp_val.selection-temp_val.scrollpos) + 31);
      else
        DWIN_Draw_Rectangle(1, cColor, 0, MBASE(temp_val.selection - temp_val.scrollpos) - 18, 14, MBASE(temp_val.selection - temp_val.scrollpos) + 31);
    }
    else if (encoder_diffState == ENCODER_DIFF_ENTER)
      Menu_Item_Handler(temp_val.active_menu, temp_val.selection, false);
    DWIN_UpdateLCD();
  }

  void CrealityDWINClass::Value_Control() {
    EncoderState encoder_diffState = Encoder_ReceiveAnalyze();
    float valuegap = 0; 
    if (encoder_diffState == ENCODER_DIFF_NO) return;
    if (encoder_diffState == ENCODER_DIFF_CW) {
      valuegap += EncoderRate.encoderMoveValue;
      if (temp_val.active_menu == Move)
        temp_val.tempvalue += (EncoderRate.encoderMoveValue * temp_val.unitmultip);
      #if !HAS_RC_CPU
        else if (temp_val.flag_multiplicator)
          temp_val.tempvalue += (EncoderRate.encoderMoveValue * temp_val.factmultip);
      #endif
      else 
        temp_val.tempvalue += EncoderRate.encoderMoveValue;
      }
    else if (encoder_diffState == ENCODER_DIFF_CCW) {
      valuegap -= EncoderRate.encoderMoveValue;
      if (temp_val.active_menu == Move)
        temp_val.tempvalue -= (EncoderRate.encoderMoveValue * temp_val.unitmultip);
      #if !HAS_RC_CPU
        else if (temp_val.flag_multiplicator)
          temp_val.tempvalue -= (EncoderRate.encoderMoveValue * temp_val.factmultip);
      #endif
      else 
        temp_val.tempvalue -= EncoderRate.encoderMoveValue;
    }
    else if (encoder_diffState == ENCODER_DIFF_ENTER) {
      temp_val.process = Menu;
      #if !HAS_RC_CPU
        temp_val.flag_multiplicator = false;
      #endif
      EncoderRate.enabled = false;
      Draw_Float(temp_val.tempvalue / temp_val.valueunit, temp_val.selection - temp_val.scrollpos, false, temp_val.valueunit);
      DWIN_UpdateLCD();
      if (temp_val.active_menu == ZOffset && temp_val.zoffsetmode != 0) {
        planner.synchronize();
        if (temp_val.zoffsetmode == 1) {
          current_position.z += (temp_val.tempvalue / temp_val.valueunit - temp_val.zoffsetvalue);
          planner.buffer_line(current_position, homing_feedrate(Z_AXIS), active_extruder);
        }
        current_position.z = 0;
        sync_plan_position();
      }

      if (TERN0(PIDTEMP, valuepointer == &thermalManager.temp_hotend[0].pid.Ki) || TERN0(PIDTEMPBED, valuepointer == &thermalManager.temp_bed.pid.Ki))
        temp_val.tempvalue = scalePID_i(temp_val.tempvalue);
      if (TERN0(PIDTEMP, valuepointer == &thermalManager.temp_hotend[0].pid.Kd) || TERN0(PIDTEMPBED, valuepointer == &thermalManager.temp_bed.pid.Kd))
        temp_val.tempvalue = scalePID_d(temp_val.tempvalue);
      switch (temp_val.valuetype) {
        case 0: *(float*)valuepointer = temp_val.tempvalue / temp_val.valueunit; break;
        case 1: *(uint8_t*)valuepointer = temp_val.tempvalue / temp_val.valueunit; break;
        case 2: *(uint16_t*)valuepointer = temp_val.tempvalue / temp_val.valueunit; break;
        case 3: *(int16_t*)valuepointer = temp_val.tempvalue / temp_val.valueunit; break;
        case 4: *(uint32_t*)valuepointer = temp_val.tempvalue / temp_val.valueunit; break;
        case 5: *(int8_t*)valuepointer = temp_val.tempvalue / temp_val.valueunit; break;
      }
      switch (temp_val.active_menu) {
        case Move:
          planner.synchronize();
          planner.buffer_line(current_position, manual_feedrate_mm_s[temp_val.selection - 1], active_extruder);
          break;
        #if HAS_HOTEND && HAS_E_CALIBRATION
          case MoveE100:
            temp_val.real_value = (planner.settings.axis_steps_per_mm[E_AXIS] * 100) / (120 - temp_val.control_value);
            planner.settings.axis_steps_per_mm[E_AXIS] = temp_val.real_value;
            planner.synchronize();
            Redraw_Menu();
          break;
        #endif
        #if HAS_MESH
          #if ENABLED(PROBE_MANUALLY)
            case ManualMesh:
              planner.synchronize();
              planner.buffer_line(current_position, homing_feedrate(Z_AXIS), active_extruder);
              planner.synchronize();
              break;
          #endif
          #if ENABLED(AUTO_BED_LEVELING_UBL) && !HAS_BED_PROBE
            case UBLMesh:     BedLevelTools.manual_mesh_move(true); break;
          #endif
          case LevelManual: BedLevelTools.manual_mesh_move(temp_val.selection == LEVELING_M_OFFSET); break;
          #if EXTJYERSUI
            case MeshInsetMenu:
              if ((temp_val.last_meshinset_value != (temp_val.tempvalue / temp_val.valueunit)) && (valuepointer != &HMI_datas.probing_margin)) { temp_val.last_meshinset_value = 0; ApplyMeshLimits(); }
              break;
          #endif
        #endif
      }
      if (valuepointer == &planner.flow_percentage[0])
        planner.refresh_e_factor(0);
      #if HAS_FILAMENT_SENSOR
        if (valuepointer == &temp_val.editable_distance)
          runout.set_runout_distance(temp_val.editable_distance, 0);
      #endif
      #if HAS_FAN
        if (valuepointer == &temp_val.fan_speed) {
          if (HMI_datas.fan_percent) {
            switch (temp_val.fan_speed) {
              case 0: thermalManager.fan_speed[0] = 0; break;
              case 1 ... 99: thermalManager.fan_speed[0] = int((temp_val.fan_speed * 255) / 100) + 1; break;
              case 100: thermalManager.fan_speed[0] = 255; break;
            }
          }
          else thermalManager.fan_speed[0] = temp_val.fan_speed;
        }
      #endif
      #if ENABLED(CASE_LIGHT_MENU, CASELIGHT_USES_BRIGHTNESS)
        if (valuepointer == &caselight.brightness)
          caselight.update_brightness();
      #endif
      #if HAS_COLOR_LEDS
        if ((valuepointer == &leds.color.r) || (valuepointer == &leds.color.g) || (valuepointer == &leds.color.b))
          ApplyLEDColor();
          #if HAS_WHITE_LED
            if (valuepointer == &leds.color.w) ApplyLEDColor();
          #endif
      #endif
      #if EXTJYERSUI && HAS_PHYSICAL_MENUS
        if (temp_val.active_menu == PhySetMenu) {
          ExtJyersui.ApplyPhySet();

          #if HAS_MESH
            MaxMeshArea();
          #endif

          #if ENABLED(NOZZLE_PARK_FEATURE)
            if ((valuepointer == &HMI_datas.x_max_pos) && (HMI_datas.Park_point.x > temp_val.last_pos / 2))  HMI_datas.Park_point.x = HMI_datas.Park_point.x + (HMI_datas.x_max_pos - temp_val.last_pos);
            if ((valuepointer == &HMI_datas.y_max_pos) && (HMI_datas.Park_point.y > temp_val.last_pos / 2))  HMI_datas.Park_point.y = HMI_datas.Park_point.y + (HMI_datas.y_max_pos - temp_val.last_pos);
            if ((valuepointer == &HMI_datas.x_min_pos) && (HMI_datas.Park_point.x < temp_val.last_pos / 2))  HMI_datas.Park_point.x = HMI_datas.Park_point.x + (HMI_datas.x_min_pos - temp_val.last_pos);
            if ((valuepointer == &HMI_datas.y_min_pos) && (HMI_datas.Park_point.y < temp_val.last_pos / 2))  HMI_datas.Park_point.y = HMI_datas.Park_point.y + (HMI_datas.y_min_pos - temp_val.last_pos);
          #endif
        
          if (temp_val.axis == NO_AXIS_ENUM) {
          sprintf_P(cmd, PSTR("Warning! Size Bed is now: %ix%i, don't forget to set X/Y max position!"), HMI_datas.x_bed_size, HMI_datas.y_bed_size);
          Update_Status(cmd);
          }
        }
      #endif
      #if ENABLED(MPCTEMP, MPC_INCLUDE_FAN)
        if (valuepointer == &temp_val.editable_value) 
          thermalManager.temp_hotend[0].applyFanAdjustment(temp_val.editable_value);
      #endif
      #if HAS_AUTOTEMP_MENU
        if (valuepointer == &planner.autotemp.min) {
          NOLESS(planner.autotemp.max, planner.autotemp.min);
          Draw_Menu(AUTOTEMP_menu, AUTOTEMP_TMIN);
        }
        if (valuepointer == &planner.autotemp.factor) {
            if (temp_val.tempvalue == 0) {
              temp_val.autotemp_enable = false;
              Draw_Menu(AUTOTEMP_menu, AUTOTEMP_VFACTOR);
            }
          }
      #endif
      #if HAS_SHAPING
        if (valuepointer == &temp_val.editable_decimal) {
          if (temp_val.flag_freq) stepper.set_shaping_frequency(temp_val.axis, temp_val.editable_decimal);
          else stepper.set_shaping_damping_ratio(temp_val.axis, temp_val.editable_decimal);
          Redraw_Menu(false);
        }
      #endif
      #if HAS_CANCEL_OBJECTS
        if (valuepointer == &temp_val.index_nb) DWIN_Cancel_object(temp_val.index_nb);
      #endif
      if (funcpointer) funcpointer();
      return;
    }
    //NOLESS(temp_val.tempvalue, (temp_val.valuemin * temp_val.valueunit));
    //NOMORE(temp_val.tempvalue, (temp_val.valuemax * temp_val.valueunit));
    LIMIT(temp_val.tempvalue, (temp_val.valuemin * temp_val.valueunit), (temp_val.valuemax * temp_val.valueunit));
    Draw_Float(temp_val.tempvalue / temp_val.valueunit, temp_val.selection - temp_val.scrollpos, true, temp_val.valueunit);
    DWIN_UpdateLCD();

    if (valuepointer == &ui.brightness) {
      *(uint8_t*)valuepointer = temp_val.tempvalue / temp_val.valueunit;
      ui.refresh_brightness();
    }

      switch (temp_val.active_menu) {
          case Move:
            if (temp_val.livemove) {
              *(float*)valuepointer = temp_val.tempvalue / temp_val.valueunit;
              planner.synchronize();
              planner.buffer_line(current_position, manual_feedrate_mm_s[temp_val.selection - 1], active_extruder);
            }
            break;
          #if HAS_MESH
            #if ENABLED(PROBE_MANUALLY)
              case ManualMesh:
                if (temp_val.liveadjust) {
                  planner.synchronize();
                  *(float*)valuepointer = temp_val.tempvalue / temp_val.valueunit;
                  planner.buffer_line(current_position, homing_feedrate(Z_AXIS), active_extruder);
                  planner.synchronize();
                }
                break;
            #endif
            #if ENABLED(AUTO_BED_LEVELING_UBL) && !HAS_BED_PROBE
              case UBLMesh:
                if (temp_val.liveadjust) {
                  sprintf_P(cmd, PSTR("M290 Z%s"), dtostrf((valuegap / temp_val.valueunit), 1, 3, str_1));
                  gcode.process_subcommands_now(cmd);
                  planner.synchronize();
                  *(float*)valuepointer = temp_val.tempvalue / temp_val.valueunit;
                  current_position.z += (float)(valuegap / temp_val.valueunit);
                  sync_plan_position();
                }
                break;
            #endif
            case LevelManual: 
              if (temp_val.liveadjust && BedLevelTools.goto_mesh_value && (temp_val.selection == LEVELING_M_OFFSET)) {
                sprintf_P(cmd, PSTR("M290 Z%s"), dtostrf((valuegap / temp_val.valueunit), 1, 3, str_1));
                gcode.process_subcommands_now(cmd);
                planner.synchronize();
                *(float*)valuepointer = temp_val.tempvalue / temp_val.valueunit;
                current_position.z += (float)(valuegap / temp_val.valueunit);
                sync_plan_position();
              }
              break;
          #endif
          case ZOffset:
            if (temp_val.zoffsetmode == 2) {
              planner.synchronize();
              sprintf_P(cmd, PSTR("M290 Z%s"), dtostrf((valuegap / temp_val.valueunit), 1, 3, str_1));
              gcode.process_subcommands_now(cmd);
              planner.synchronize();
            }
            break;
          case Tune:
            if (temp_val.selection == TUNE_ZOFFSET) {
              sprintf_P(cmd, PSTR("M290 Z%s"), dtostrf((valuegap / temp_val.valueunit), 1, 3, str_1));
              gcode.process_subcommands_now(cmd);
            }
            break;
          #if ENABLED(CASE_LIGHT_MENU, CASELIGHT_USES_BRIGHTNESS)
            case CaseLightmenu:
              *(uint8_t*)valuepointer = temp_val.tempvalue / temp_val.valueunit;
              caselight.update_brightness();
              break;
          #endif
          #if ENABLED(LED_CONTROL_MENU, HAS_COLORS_LEDS)
            case LedControlmenu:
              *(uint8_t*)valuepointer = temp_val.tempvalue / temp_val.valueunit;
              leds.update();
              break;
          #endif
          default: break;   
        }
  }

  void CrealityDWINClass::Option_Control() {
    EncoderState encoder_diffState = Encoder_ReceiveAnalyze();
    if (encoder_diffState == ENCODER_DIFF_NO) return;
    if (encoder_diffState == ENCODER_DIFF_CW)
      temp_val.tempvalue += EncoderRate.encoderMoveValue;
    else if (encoder_diffState == ENCODER_DIFF_CCW)
      temp_val.tempvalue -= EncoderRate.encoderMoveValue;
    else if (encoder_diffState == ENCODER_DIFF_ENTER) {
      temp_val.process = Menu;
      EncoderRate.enabled = false;
      if (valuepointer == &color_names) {
        switch (temp_val.selection) {
          case COLORSETTINGS_CURSOR: HMI_datas.cursor_color = temp_val.tempvalue; Redraw_Screen(); break;
          case COLORSETTINGS_SPLIT_LINE: HMI_datas.menu_split_line = temp_val.tempvalue; Redraw_Screen(); break;
          case COLORSETTINGS_ITEMS_MENU_TEXT: HMI_datas.items_menu_text =  temp_val.tempvalue; Redraw_Screen(); break;
          case COLORSETTINGS_ICONS_MENU_TEXT: HMI_datas.icons_menu_text =  temp_val.tempvalue; Redraw_Screen(); break;
          #if !HAS_TJC_DISPLAY
            case COLORSETTINGS_BACKGROUND: HMI_datas.background =  temp_val.tempvalue; Redraw_Screen(); break;
          #endif
          case COLORSETTINGS_MENU_TOP_TXT: HMI_datas.menu_top_txt = temp_val.tempvalue; Redraw_Screen(); break;
          case COLORSETTINGS_MENU_TOP_BG: HMI_datas.menu_top_bg = temp_val.tempvalue; Redraw_Screen(); break;
          case COLORSETTINGS_SELECT_TXT: HMI_datas.select_txt = temp_val.tempvalue; Redraw_Screen();break;
          case COLORSETTINGS_SELECT_BG: HMI_datas.select_bg = temp_val.tempvalue; Redraw_Screen(); break;
          case COLORSETTINGS_HIGHLIGHT_BORDER: HMI_datas.highlight_box = temp_val.tempvalue; Redraw_Screen(); break;
          case COLORSETTINGS_POPUP_HIGHLIGHT: HMI_datas.popup_highlight = temp_val.tempvalue; Redraw_Screen(); break;
          case COLORSETTINGS_POPUP_TXT: HMI_datas.popup_txt = temp_val.tempvalue; Redraw_Screen(); break;
          case COLORSETTINGS_POPUP_BG: HMI_datas.popup_bg = temp_val.tempvalue; Redraw_Screen(); break;
          case COLORSETTINGS_ICON_CONFIRM_TXT: HMI_datas.ico_confirm_txt = temp_val.tempvalue; Redraw_Screen(); break;
          case COLORSETTINGS_ICON_CONFIRM_BG: HMI_datas.ico_confirm_bg = temp_val.tempvalue; Redraw_Screen(); break;
          case COLORSETTINGS_ICON_CANCEL_TXT: HMI_datas.ico_cancel_txt = temp_val.tempvalue; Redraw_Screen(); break;
          case COLORSETTINGS_ICON_CANCEL_BG: HMI_datas.ico_cancel_bg = temp_val.tempvalue; Redraw_Screen(); break;
          case COLORSETTINGS_ICON_CONTINUE_TXT: HMI_datas.ico_continue_txt = temp_val.tempvalue; Redraw_Screen(); break;
          case COLORSETTINGS_ICON_CONTINUE_BG: HMI_datas.ico_continue_bg = temp_val.tempvalue; Redraw_Screen(); break;
          case COLORSETTINGS_PRINT_SCREEN_TXT: HMI_datas.print_screen_txt = temp_val.tempvalue; Redraw_Screen(); break;
          case COLORSETTINGS_PRINT_FILENAME: HMI_datas.print_filename = temp_val.tempvalue; Redraw_Screen(); break;
          case COLORSETTINGS_PROGRESS_BAR: HMI_datas.progress_bar = temp_val.tempvalue; Redraw_Screen(); break;
          case COLORSETTINGS_PROGRESS_PERCENT: HMI_datas.progress_percent = temp_val.tempvalue; Redraw_Screen(); break;
          case COLORSETTINGS_REMAIN_TIME: HMI_datas.remain_time = temp_val.tempvalue; Redraw_Screen(); break;
          case COLORSETTINGS_ELAPSED_TIME: HMI_datas.elapsed_time = temp_val.tempvalue; Redraw_Screen(); break;
          case COLORSETTINGS_PROGRESS_STATUS_BAR: HMI_datas.status_bar_text = temp_val.tempvalue; Redraw_Screen(); break;
          case COLORSETTINGS_PROGRESS_STATUS_AREA: HMI_datas.status_area_text = temp_val.tempvalue; Redraw_Screen(); break;
          case COLORSETTINGS_PROGRESS_STATUS_PERCENT: HMI_datas.status_area_percent = temp_val.tempvalue; Redraw_Screen(); break;
          case COLORSETTINGS_PROGRESS_COORDINATES: HMI_datas.coordinates_text = temp_val.tempvalue; Redraw_Screen(); break;
          case COLORSETTINGS_PROGRESS_COORDINATES_LINE: HMI_datas.coordinates_split_line = temp_val.tempvalue; Redraw_Screen(); break;
        }
        //Redraw_Screen();
      }
      #if HAS_SHORTCUTS
        else if (valuepointer == &shortcut_list) {
          switch (temp_val.selection) {
            #define _SHORTCUT_SETTINGS(N) \
              case SHORTCUTSETTINGS_##N: { \
                temp_val.shortcut[N - 1] = temp_val.tempvalue; \
                Set_Current_Shortcuts(); \
                break; }

            REPEAT_1(NB_Shortcuts, _SHORTCUT_SETTINGS)
          }
        }
      #endif
      #if ENABLED(DWIN_CREALITY_LCD_CUSTOM_ICONS)
        else if (valuepointer == &icon_main) {
          temp_val.iconmain = temp_val.tempvalue;
          HMI_datas.main_icon = temp_val.iconmain;
        }
      #endif
      #if HAS_HOTEND && BOTH(MPC_AUTOTUNE, MPC_AUTOTUNE_MENU)
        #if ENABLED(MPC_EDIT_MENU)
          else if (valuepointer == &Filament_heat_capacity) {
            temp_val.filheatcap = temp_val.tempvalue;
            ApplyFilHeatCapacity();
            Redraw_Menu(false);
          }
        #endif
        else if (valuepointer == &MPC_target) {
          temp_val.MPC_targetmode = temp_val.tempvalue;
          ApplyMPCTarget();
        }
      #endif
      #if HAS_LEVELING && HAS_LEVELING_HEAT
        else if (valuepointer == &LevelingTemp_modes) {
          temp_val.LevelingTempmode = temp_val.tempvalue;
          HMI_datas.ena_LevelingTemp_hotend = false;
          HMI_datas.ena_LevelingTemp_bed = false;
          switch (temp_val.LevelingTempmode) {
            case 0: HMI_datas.ena_LevelingTemp_hotend = true; HMI_datas.ena_LevelingTemp_bed = true; break;
            case 1: HMI_datas.ena_LevelingTemp_hotend = true ; break;
            case 2: HMI_datas.ena_LevelingTemp_bed = true; break;
            default: break;
          }
          //if (temp_val.LevelingTempmode == 0 || temp_val.LevelingTempmode == 1) HMI_datas.ena_LevelingTemp_hotend = true;
          //if (temp_val.LevelingTempmode == 0 || temp_val.LevelingTempmode == 2) HMI_datas.ena_LevelingTemp_bed = true;
        }
      #endif
      else if (valuepointer == &preheat_modes)
        temp_val.preheatmode = temp_val.tempvalue;
      else if (valuepointer == &zoffset_modes) {
        temp_val.zoffsetmode = temp_val.tempvalue;
        if (temp_val.zoffsetmode == 1 || temp_val.zoffsetmode == 2) {
          if (axes_should_home()) {
            Popup_Handler(Home);
            gcode.home_all_axes(true);
          }
          Popup_Handler(MoveWait);
          #if ENABLED(Z_SAFE_HOMING)
            planner.synchronize();
            sprintf_P(cmd, PSTR("G0 F4000 X%s Y%s"), dtostrf(Z_SAFE_HOMING_X_POINT, 1, 3, str_1), dtostrf(Z_SAFE_HOMING_Y_POINT, 1, 3, str_2));
            gcode.process_subcommands_now(cmd);
          #else
            sprintf_P(cmd, PSTR("G0 F4000 X%s Y%s"), dtostrf((X_BED_SIZE + X_MIN_POS) / 2.0f, 1, 3, str_1), dtostrf((Y_BED_SIZE + Y_MIN_POS) / 2.0f, 1, 3, str_2));
            gcode.process_subcommands_now(cmd);
          #endif
          gcode.process_subcommands_now(F("G0 F300 Z0"));
          planner.synchronize();
          Redraw_Menu();
        }
      }
      else if (valuepointer == &num_modes) {
        temp_val.nummode = temp_val.tempvalue;
        switch (temp_val.nummode) {
          case 0: temp_val.unitmultip = 1; break;
          case 1: temp_val.unitmultip = 10; break;
          case 2: temp_val.unitmultip = 100; break;
          default: break;
        }
      }
      #if !HAS_RC_CPU
        else if (valuepointer == &num_mult) {
          temp_val.factmultip = POW(10, temp_val.tempvalue);
          switch (temp_val.nummult) {
            case MULT_HOTEND: temp_val.hotendmult = temp_val.tempvalue; break;
            case MULT_MPC: temp_val.mpcmult = temp_val.tempvalue; break;
            case MULT_FILTYPE: temp_val.fil_typemult = temp_val.tempvalue; break;
            case MULT_BED: temp_val.bedmult = temp_val.tempvalue; break;
            case MULT_MAXACC: temp_val.maxaccmult = temp_val.tempvalue; break;
            case MULT_JERK: temp_val.jerkmult = temp_val.tempvalue; break;
            case MULT_STEPS: temp_val.stepsmult = temp_val.tempvalue; break;
            case MULT_AUTOTEMP: temp_val.autotempmult = temp_val.tempvalue; break;
          }
        }
      #endif
      #if HAS_FILAMENT_SENSOR
        else if (valuepointer == &runoutsensor_modes) {
          temp_val.rsensormode = temp_val.tempvalue;
          runout.reset();
          switch (temp_val.rsensormode) {
           case 0: runout.mode[0] = RM_NONE; break; // None 
           case 1: runout.mode[0] = RM_OUT_ON_HIGH; break; // mode HIGH
           case 2: runout.mode[0] = RM_OUT_ON_LOW; break; // mode LOW
           case 3: runout.mode[0] = RM_MOTION_SENSOR; break; // mode MOTION
          }
          runout.setup();
          runout.reset();
          runout.enabled[0] = temp_val.State_runoutenable;
          Redraw_Menu(false);
        }
      #endif
      #if EXTJYERSUI && HAS_MESH
        else if (valuepointer == &GridxGrid) {
          _GridxGrid = temp_val.tempvalue;
          HMI_datas.grid_max_points = Get_Grid_Max_XY(_GridxGrid);
          ApplyMeshLimits();
       }
      #endif

      Draw_Option(temp_val.tempvalue, static_cast<const char * const *>(valuepointer), temp_val.selection - temp_val.scrollpos, false, (valuepointer == &color_names));
      DWIN_UpdateLCD();
      return;
    }
    //NOLESS(temp_val.tempvalue, temp_val.valuemin);
    //NOMORE(temp_val.tempvalue, temp_val.valuemax);
    LIMIT(temp_val.tempvalue, temp_val.valuemin, temp_val.valuemax);
    Draw_Option(temp_val.tempvalue, static_cast<const char * const *>(valuepointer), temp_val.selection - temp_val.scrollpos, true);
    DWIN_UpdateLCD();
  }


  #if HAS_GCODE_PREVIEW
    #if HAS_LIGHT_GCODE_PREVIEW

      void CrealityDWINClass::Get_Value(char *buf, const char * const key, float &value) {
        char num[10] = "";
        char * posptr = 0;
        uint8_t i = 0;
        if (!!value) return;
        posptr = strstr(buf, key);
        if (posptr != nullptr) {
          while (i < sizeof(num)) {
            char c = posptr[0];
            if (!ISEOL(c) && (c != 0)) {
              if ((c > 47 && c < 58) || (c == '.')) num[i++] = c;
              posptr++;
            }
            else {
              num[i] = '\0';
              value = atof(num);
              return;
            }
          }
        }
      }

      bool CrealityDWINClass::Has_Preview() {
        const char * tbstart = "; jpeg thumbnail begin 180x180";
        char * posptr = 0;
        uint8_t nbyte = 1;
        uint32_t indx = 0;
        char buf[256];

        fileprop.clear();
        fileprop.setname(card.filename);

        card.openFileRead(fileprop.name);

        while ((nbyte > 0) && (indx < 16 * sizeof(buf)) && !fileprop.thumbstart) {
          nbyte = card.read(buf, sizeof(buf) - 1);
          if (nbyte > 0) {
            buf[nbyte] = '\0';
            Get_Value(buf, ";TIME:", fileprop.time);
            Get_Value(buf, ";Filament used:", fileprop.filament);
            Get_Value(buf, ";Layer height:", fileprop.layer);
            posptr = strstr(buf, tbstart);
            if (posptr != NULL) {
              fileprop.thumbstart = indx + (posptr - &buf[0]);
            }
            else {
              indx += _MAX(10, nbyte - (signed)strlen(tbstart));
              card.setIndex(indx);
            }
          }
        }

        if (!fileprop.thumbstart) {
          card.closefile();
          Update_Status("Thumbnail not found");
          return 0;
        }

        // Get the size of the thumbnail
        card.setIndex(fileprop.thumbstart + strlen(tbstart));
        for (uint8_t i = 0; i < 16; i++) {
          char c = card.get();
          if (!ISEOL(c)) {
            buf[i] = c;
          }
          else {
            buf[i] = 0;
            break;
          }
        }
        fileprop.thumbsize = atoi(buf);

        // Exit if there isn't a thumbnail
        if (!fileprop.thumbsize) {
          card.closefile();
          Update_Status("Invalid Thumbnail Size");
          return 0;
        }

        uint16_t readed = 0;
        uint8_t buf64[fileprop.thumbsize];

        fileprop.thumbdata = new uint8_t[3 + 3 * (fileprop.thumbsize / 4)];  // Reserve space for the JPEG thumbnail

        while (readed < fileprop.thumbsize) {
          uint8_t c = card.get();
          if (!ISEOL(c) && (c != ';') && (c != ' ')) {
            buf64[readed] = c;
            readed++;
          }
        }
        card.closefile();
        buf64[readed] = 0;

        fileprop.thumbsize = decode_base64(buf64, fileprop.thumbdata);  card.closefile();
        DWIN_Save_JPEG_in_SRAM(0x5a, (uint8_t *)fileprop.thumbdata, fileprop.thumbsize, 0x0000);
        delete[] fileprop.thumbdata;
        return true;
    }

    void CrealityDWINClass::Preview_DrawFromSD() {
      bool has_preview = Has_Preview();
      temp_val.file_preview = has_preview;
      Popup_Handler(ConfirmStartPrint, has_preview);
      Draw_Title(GET_TEXT(MSG_PRINT_FILE));
      if (has_preview) {
        if (fileprop.time) {
          sprintf_P(header1, PSTR("Time: %ih %02im"), (uint16_t)fileprop.time / 3600, ((uint16_t)fileprop.time % 3600) / 60);
        }
        if (fileprop.filament) {
          sprintf_P(header2, PSTR("Filament used: %sm"), dtostrf(fileprop.filament, 1, 2, str_1));
        }
        if (fileprop.layer) {
          sprintf_P(header3, PSTR("Layer height: %smm"), dtostrf(fileprop.layer, 1, 2, str_1));
        }
        sprintf_P(cmd, PSTR("%s - %s - %s..."), (fileprop.time) ? header1 : GET_TEXT(MSG_HEADER_NO_TIME), (fileprop.filament) ? header2 : GET_TEXT(MSG_HEADER_NO_FILAMENT_USED), (fileprop.layer) ? header3 : GET_TEXT(MSG_HEADER_NO_LAYER_HEIGHT));
        Update_Status(cmd);
        DWIN_SRAM_Memory_Icon_Display(48,78,0x0000);
      }
      else gcode.process_subcommands_now(F("M117 Preview not found"));
    }

    #else

      bool CrealityDWINClass::find_and_decode_gcode_preview(char *name, uint8_t preview_type, uint16_t *address, bool onlyCachedFileIcon/*=false*/) {
        // Won't work if we don't copy the name
        // for (char *c = &name[0]; *c; c++) *c = tolower(*c);

        char file_name[strlen(name) + 1]; // Room for filename and null
        sprintf_P(file_name, PSTR("%s"), name);
        char file_path[strlen(name) + 1 + MAXPATHNAMELENGTH]; // Room for path, filename and null
        sprintf_P(file_path, PSTR("%s/%s"), card.getWorkDirName(), file_name);

        // Check if cached
        bool use_cache = preview_type == Thumnail_Icon;
        if (use_cache) {
          auto it = image_cache.find(file_path+to_string(Thumnail_Icon));
          if (it != image_cache.end()) { // already cached
            if (it->second == 0) return false; // no image available
            *address = it->second;
            return true;
          } else if (onlyCachedFileIcon) return false;
        }

        const uint16_t buff_size = 256;
        char public_buf[buff_size+1];
        uint8_t output_buffer[14328];
        uint32_t position_in_file = 0;
        char *encoded_image = NULL;

        card.openFileRead(file_name);
        uint8_t n_reads = 0;
        int16_t data_read = card.read(public_buf, buff_size);
        card.setIndex(card.getIndex()+data_read);
        char key[31] = "";
        switch (preview_type) {
          case Thumnail_Icon: strcpy_P(key, PSTR("; jpeg thumbnail begin 50x50")); break;
          case Thumnail_Preview: strcpy_P(key, PSTR("; jpeg thumbnail begin 180x180")); break;
        }
        while(n_reads < 16 && data_read) { // Max 16 passes so we don't loop forever
        if (Encoder_ReceiveAnalyze() != ENCODER_DIFF_NO) return false;
          encoded_image = strstr(public_buf, key);
          if (encoded_image) {
            uint32_t index_bw = &public_buf[buff_size] - encoded_image;
            position_in_file = card.getIndex() - index_bw;
            break;
          }

          card.setIndex(card.getIndex()-32);
          data_read = card.read(public_buf, buff_size);
          card.setIndex(card.getIndex()+data_read);

          n_reads++;
        }

        // If we found the image, decode it
        if (encoded_image) {
        memset(public_buf, 0, sizeof(public_buf));
        card.setIndex(position_in_file+23); // ; jpeg thumbnail begin <move here>180x180 99999
        while (card.get() != ' '); // ; jpeg thumbnail begin 180x180 <move here>180x180

        char size_buf[10];
        for (size_t i = 0; i < sizeof(size_buf); i++)
        {
          uint8_t c = card.get();
          if (ISEOL(c)) {
            size_buf[i] = 0;
            break;
          }
          else
            size_buf[i] = c;
        }
        uint16_t image_size = atoi(size_buf);
        uint16_t stored_in_buffer = 0;
        uint8_t encoded_image_data[image_size+1];
        while (stored_in_buffer < image_size) {
          char c = card.get();
          if (ISEOL(c) || c == ';' || c == ' ') {
            continue;
          }
          else {
            encoded_image_data[stored_in_buffer] = c;
            stored_in_buffer++;
          }
        }

        encoded_image_data[stored_in_buffer] = 0;
        unsigned int output_size = decode_base64(encoded_image_data, output_buffer);
        if (next_available_address + output_size >= 0x7530) { // cache is full, invalidate it
          next_available_address = 0;
          image_cache.clear();
          SERIAL_ECHOLNPGM("Preview cache full, cleaning up...");
        }
        DWIN_Save_JPEG_in_SRAM(0x5a, (uint8_t *)output_buffer, output_size, next_available_address);
        *address = next_available_address;
        if(use_cache) {
          image_cache[file_path+to_string(preview_type)] = next_available_address;
          next_available_address += output_size + 1;
        }
        } else if (use_cache)  // If we didn't find the image, but we are using the cache, mark it as image not available
        {
          image_cache[file_path+to_string(preview_type)] = 0;
        }

        card.closefile(); 
        gcode.process_subcommands_now(F("M117")); // Clear the message sent by the card API
        return encoded_image;
      }

      bool CrealityDWINClass::find_and_decode_gcode_header(char *name, uint8_t header_type) {
        char file_name[strlen(name) + 1]; // Room for filename and null
        sprintf_P(file_name, PSTR("%s"), name);
        char file_path[strlen(name) + 1 + MAXPATHNAMELENGTH]; // Room for path, filename and null
        sprintf_P(file_path, PSTR("%s/%s"), card.getWorkDirName(), file_name);

        const uint16_t buff_size = 256;
        char public_buf[buff_size+1];
        uint32_t position_in_file = 0;
        char *encoded_header = NULL;

        card.openFileRead(file_name);
        uint8_t n_reads = 0;
        int16_t data_read = card.read(public_buf, buff_size);
        card.setIndex(card.getIndex()+data_read);
        char key[16] = "";
        switch (header_type) {
          case Header_Time: strcpy_P(key, PSTR(";TIME")); break;
          case Header_Filament: strcpy_P(key, PSTR(";Filament used")); break;
          case Header_Layer: strcpy_P(key, PSTR(";Layer height")); break;
        }
        while(n_reads < 16 && data_read) { // Max 16 passes so we don't loop forever
        if (Encoder_ReceiveAnalyze() != ENCODER_DIFF_NO) return false;
          encoded_header = strstr(public_buf, key);
          if (encoded_header) {
            uint32_t index_bw = &public_buf[buff_size] - encoded_header;
            position_in_file = card.getIndex() - index_bw;
            break;
          }

          card.setIndex(card.getIndex()-17);
          data_read = card.read(public_buf, buff_size);
          card.setIndex(card.getIndex()+data_read);

          n_reads++;
        }

        if (encoded_header) {
          switch (header_type) {
            case Header_Time: card.setIndex(position_in_file+5); break;
            case Header_Filament: card.setIndex(position_in_file+14); break;
            case Header_Layer: card.setIndex(position_in_file+13); break;
          }
          while (card.get() != ':'); // ; jpeg thumbnail begin 180x180 <move here>180x180
            
          char out_buf[12];
          uint8_t stored_in_buffer = 0;
          for (int i = 0; i < 12; i++)
          {
            char c = card.get();
            if (ISEOL(c) || c == ';') {
              break;
            }
            else {
              out_buf[stored_in_buffer] = c;
              stored_in_buffer++;
            }
          }
          sprintf_P(str_1, PSTR("%s"), out_buf);
        }
        card.closefile();
      return encoded_header;
      }
    #endif
  #endif // G-code preview

  void CrealityDWINClass::File_Control() {
    const uint16_t color_background = TERN(HAS_TJC_DISPLAY, Color_Bg_Black, GetColor(HMI_datas.background, Color_Bg_Black));
    //add
    typedef TextScroller<MENU_CHAR_LIMIT> Scroller;
    static Scroller scroller;

    uint16_t cColor = GetColor(HMI_datas.cursor_color, Rectangle_Color);
    EncoderState encoder_diffState = Encoder_ReceiveAnalyze();

    #if HAS_GCODE_PREVIEW && HAS_NORMAL_GCODE_PREVIEW
      if (ELAPSED(millis(), name_scroll_time), encoder_diffState == ENCODER_DIFF_NO) {
    #else
      if (encoder_diffState == ENCODER_DIFF_NO) {
    #endif
      if (temp_val.selection > 0) {
        card.getfilename_sorted(SD_ORDER(temp_val.selection - 1, card.get_num_Files()));
        char * const filename = card.longest_filename();
        size_t len = strlen(filename);
        size_t pos = len;
        if (!card.flag.filenameIsDir)
          while (pos && filename[pos] != '.') pos--;
        if (pos > MENU_CHAR_LIMIT) {
          static millis_t time = 0;
          if (PENDING(millis(), time)) return;
          time = millis() + 200;

          Scroller::Buffer buf;
          const char* const name = scroller.scroll(pos, buf, filename);
          DWIN_Draw_Rectangle(1, color_background, LBLX, MBASE(temp_val.selection - temp_val.scrollpos) - 14, 271, MBASE(temp_val.selection - temp_val.scrollpos) + 28);
          Draw_Menu_Item(temp_val.selection-temp_val.scrollpos, card.flag.filenameIsDir ? ICON_More : ICON_File, name, NULL, NULL, false, true);
          DWIN_UpdateLCD();
        }
      }
      return;
    }
    if (encoder_diffState == ENCODER_DIFF_CW && temp_val.selection < card.get_num_Files()) {
      DWIN_Draw_Rectangle(1, color_background, 0, MBASE(temp_val.selection - temp_val.scrollpos) - 18, 14, MBASE(temp_val.selection - temp_val.scrollpos) + 33);
      if (temp_val.selection > 0) {
        DWIN_Draw_Rectangle(1, color_background, LBLX, MBASE(temp_val.selection - temp_val.scrollpos) - 14, 271, MBASE(temp_val.selection - temp_val.scrollpos) + 28);
        Draw_SD_Item(temp_val.selection, temp_val.selection - temp_val.scrollpos, true);
      }
      scroller.reset();
      temp_val.selection++; // Select Down
      if (temp_val.selection > temp_val.scrollpos + MROWS) {
        temp_val.scrollpos++;
        DWIN_Frame_AreaMove(1, 2, MLINE, color_background, 0, 31, DWIN_WIDTH, 349);
        Draw_SD_Item(temp_val.selection, temp_val.selection - temp_val.scrollpos, true);
      }

      #if HAS_GCODE_PREVIEW && HAS_NORMAL_GCODE_PREVIEW
        thumbtime = millis() + SCROLL_WAIT;
        name_scroll_time = millis() + SCROLL_WAIT;
      #endif
      if ((cColor == color_background) || ((cColor == Color_Black) && (HMI_datas.background == 0))) {
        DWIN_Draw_Rectangle(0, GetColor(HMI_datas.items_menu_text, Color_White), 0, MBASE(temp_val.selection - temp_val.scrollpos) - 18, 8, MBASE(temp_val.selection - temp_val.scrollpos) + 31);
      }
      else
      DWIN_Draw_Rectangle(1, cColor, 0, MBASE(temp_val.selection - temp_val.scrollpos) - 18, 8, MBASE(temp_val.selection - temp_val.scrollpos) + 31);
    }
    else if (encoder_diffState == ENCODER_DIFF_CCW && temp_val.selection > 0) {
      DWIN_Draw_Rectangle(1, color_background, 0, MBASE(temp_val.selection - temp_val.scrollpos) - 18, 14, MBASE(temp_val.selection - temp_val.scrollpos) + 33);
      DWIN_Draw_Rectangle(1, color_background, LBLX, MBASE(temp_val.selection - temp_val.scrollpos) - 14, 271, MBASE(temp_val.selection - temp_val.scrollpos) + 28);
      Draw_SD_Item(temp_val.selection, temp_val.selection - temp_val.scrollpos, true);
      scroller.reset();
      temp_val.selection--; // Select Up
      if (temp_val.selection < temp_val.scrollpos) {
        temp_val.scrollpos--;
        DWIN_Frame_AreaMove(1, 3, MLINE, color_background, 0, 31, DWIN_WIDTH, 349);
        Draw_SD_Item(temp_val.selection, temp_val.selection - temp_val.scrollpos, true);
      }

      #if HAS_GCODE_PREVIEW && HAS_NORMAL_GCODE_PREVIEW
        thumbtime = millis() + SCROLL_WAIT;
        name_scroll_time = millis() + SCROLL_WAIT;
      #endif
      
      if ((cColor == color_background) || ((cColor == Color_Black) && (HMI_datas.background == 0))) 
        DWIN_Draw_Rectangle(0, GetColor(HMI_datas.items_menu_text, Color_White), 0, MBASE(temp_val.selection - temp_val.scrollpos) - 18, 8, MBASE(temp_val.selection - temp_val.scrollpos) + 31);
      else
        DWIN_Draw_Rectangle(1, cColor, 0, MBASE(temp_val.selection - temp_val.scrollpos) - 18, 8, MBASE(temp_val.selection - temp_val.scrollpos) + 31);
    }
    else if (encoder_diffState == ENCODER_DIFF_ENTER) {
      if (temp_val.selection == 0) {
        if (card.flag.workDirIsRoot) {
          temp_val.process = Main;
          Draw_Main_Menu();
        }
        else {
          card.cdup();
          #if ALL(SDSUPPORT, SDCARD_SORT_ALPHA, SDSORT_GCODE)
            DWIN_Sort_SD(card.isMounted());
          #endif
          Draw_SD_List();
        }
      }
      else {
        #if ALL(SDSUPPORT, SDCARD_SORT_ALPHA, SDSORT_GCODE)
          DWIN_Sort_SD(card.isMounted());
        #endif
        card.getfilename_sorted(SD_ORDER(temp_val.selection - 1, card.get_num_Files())); 
        if (card.flag.filenameIsDir) {
          card.cd(card.filename);
          #if ALL(SDSUPPORT, SDCARD_SORT_ALPHA, SDSORT_GCODE)
            DWIN_Sort_SD(card.isMounted());
          #endif
          Draw_SD_List();
        }
        else {
          #if HAS_GCODE_PREVIEW
            #if HAS_LIGHT_GCODE_PREVIEW
              Preview_DrawFromSD();
            #else
              uint16_t image_address;
              bool has_preview = find_and_decode_gcode_preview(card.filename, Thumnail_Preview, &image_address);
              temp_val.file_preview = has_preview;
              bool has_header_time = find_and_decode_gcode_header(card.filename, Header_Time);
              if (has_header_time) {
                header_time_s = atof(str_1); 
                sprintf_P(header1, GET_TEXT(MSG_HEADER_TIME), (uint8_t)(header_time_s / 3600), (uint8_t)((header_time_s / 60)%60), (uint8_t)(header_time_s%60));
              }
              bool has_header_filament = find_and_decode_gcode_header(card.filename, Header_Filament);
              if (has_header_filament) {
                size_t nb = 0;
                for (size_t i = 0; i<strlen(str_1); i++)
                {
                  nb = i;
                  str_2[i] = str_1[i];
                if (str_1[i] == '.') break;
                }
                for (size_t i = 1; i<3; i++)
                {
                str_2[nb+i] = str_1[nb+i];
                }
                str_2[nb+3] = '\0';
                sprintf_P(header2, GET_TEXT(MSG_HEADER_FILAMENT_USED), str_2);
              }
              bool has_header_layer = find_and_decode_gcode_header(card.filename, Header_Layer);
              if (has_header_layer) {
                sprintf_P(header3, GET_TEXT(MSG_HEADER_LAYER_HEIGHT), dtostrf(atof(str_1), 1, 2, str_3));
              }
              Popup_Handler(ConfirmStartPrint, has_preview);
              Draw_Title(GET_TEXT(MSG_PRINT_FILE));
              if (has_preview) {
                file_preview_image_address = image_address;
                DWIN_SRAM_Memory_Icon_Display(48,78,image_address);
              }
              else gcode.process_subcommands_now(F("M117 Preview not found")); 

              if (has_header_time || has_header_filament || has_header_layer) {
                sprintf_P(cmd, PSTR("%s - %s - %s..."), (has_header_time) ? header1 : GET_TEXT(MSG_HEADER_NO_TIME), (has_header_filament) ? header2 : GET_TEXT(MSG_HEADER_NO_FILAMENT_USED), (has_header_layer) ? header3 : GET_TEXT(MSG_HEADER_NO_LAYER_HEIGHT));
                Update_Status(cmd);
              }
              else gcode.process_subcommands_now(F("M117 Header not found"));
            #endif
          #else
            gcode.process_subcommands_now(F("M220 S100\nM221 S100"));  // Initialize Flow and Feerate to 100%
            strcpy(reprint_filename, card.filename);
            #if ENABLED(HAS_BED_PROBE, AUTO_BED_LEVELING_UBL) && !HAS_NO_AUTOTILT_AFTERNPRINT
                CrealityDWIN.Autotilt_AfterNPrint(temp_val.NPrinted);
            #endif
            card.openAndPrintFile(card.filename);
          #endif
        }
      }
    }
    DWIN_UpdateLCD();
  }

  void CrealityDWINClass::Print_Screen_Control() {
    EncoderState encoder_diffState = Encoder_ReceiveAnalyze();
    if (encoder_diffState == ENCODER_DIFF_NO) return;
    if (encoder_diffState == ENCODER_DIFF_CW && temp_val.selection < PRINT_COUNT - 1) {
      temp_val.selection++; // Select Down
      Print_Screen_Icons();
    }
    else if (encoder_diffState == ENCODER_DIFF_CCW && temp_val.selection > 0) {
      temp_val.selection--; // Select Up
      Print_Screen_Icons();
    }
    else if (encoder_diffState == ENCODER_DIFF_ENTER) {
      switch (temp_val.selection) {
        case PRINT_SETUP:
          temp_val.flag_selection = false;
          temp_val.flag_tune_submenu = false;
          temp_val.flag_tune_menu = true;
          Draw_Menu(Tune);
          Update_Status_Bar(true);
          break;
        case PRINT_PAUSE_RESUME:
          if (temp_val.paused) {
            if (temp_val.sdprint) {
              wait_for_user = false;
              #if ENABLED(PARK_HEAD_ON_PAUSE)
                card.startOrResumeFilePrinting();
                TERN_(POWER_LOSS_RECOVERY, recovery.prepare());
              #else
                #if HAS_HEATED_BED
                  cmd[sprintf_P(cmd, PSTR("M140 S%i"), temp_val.pausebed)] = '\0';
                  gcode.process_subcommands_now(cmd);
                #endif
                #if HAS_EXTRUDERS
                  cmd[sprintf_P(cmd, PSTR("M109 S%i"), temp_val.pausetemp)] = '\0';
                  gcode.process_subcommands_now(cmd);
                #endif
                TERN_(HAS_FAN, thermalManager.fan_speed[0] = temp_val.pausefan);
                planner.synchronize();
                TERN_(SDSUPPORT, queue.inject(F("M24")));
              #endif
            }
            else {
              TERN_(HOST_ACTION_COMMANDS, hostui.resume());
            }
            Draw_Print_Screen();
          }
          else
            Popup_Handler(Pause);
          break;
        case PRINT_STOP: Popup_Handler(Stop); break;
      }
    }
    DWIN_UpdateLCD();
  }

  void CrealityDWINClass::Popup_Control() {
    EncoderState encoder_diffState = Encoder_ReceiveAnalyze();
    if (encoder_diffState == ENCODER_DIFF_NO) return;
    if (encoder_diffState == ENCODER_DIFF_CW && temp_val.selection < 1) {
      temp_val.selection++;
      if (!temp_val.flag_busy) Popup_Select(temp_val.popup == Wizard_Bed_T);
    }
    else if (encoder_diffState == ENCODER_DIFF_CCW && temp_val.selection > 0) {
      temp_val.selection--;
      if (!temp_val.flag_busy) Popup_Select(temp_val.popup == Wizard_Bed_T);
    }
    else if (encoder_diffState == ENCODER_DIFF_ENTER) {
      switch (temp_val.popup) {
        case Pause:
          if (temp_val.selection == 0) {
            if (temp_val.sdprint) {
              #if ENABLED(POWER_LOSS_RECOVERY)
                if (recovery.enabled) recovery.save(true);
              #endif
              #if ENABLED(PARK_HEAD_ON_PAUSE)
                Popup_Handler(Home, true);
                #if ENABLED(SDSUPPORT)
                  if (IS_SD_PRINTING()) card.pauseSDPrint();
                #endif
                planner.synchronize();
                queue.inject(F("M125"));
                planner.synchronize();
              #else
                queue.inject(F("M25"));
                TERN_(HAS_HOTEND, temp_val.pausetemp = thermalManager.temp_hotend[0].target);
                TERN_(HAS_HEATED_BED, temp_val.pausebed = thermalManager.temp_bed.target);
                TERN_(HAS_FAN, temp_val.pausefan = thermalManager.fan_speed[0]);
                thermalManager.cooldown();
              #endif
            }
            else {
              TERN_(HOST_ACTION_COMMANDS, hostui.pause());
            }
          }
          Draw_Print_Screen();
          break;
        case Stop:
          if (temp_val.selection == 0) {
            if (temp_val.sdprint) {
              ui.abort_print();
              thermalManager.cooldown();
            }
            else {
              TERN_(HOST_ACTION_COMMANDS, hostui.cancel());
            }
            TERN_(DEBUG_DWIN, SERIAL_ECHOLNPGM("DWIN_Print_Aborted"));
          }
          else
            Draw_Print_Screen();
          break;
        case Resume:
          if (temp_val.selection == 0)
            queue.inject(F("M1000"));
          else {
            queue.inject(F("M1000 C"));
            Draw_Main_Menu();
          }
          break;

        #if HAS_HOTEND
          case ETemp:
            if (temp_val.selection == 0) {
              thermalManager.setTargetHotend(EXTRUDE_MINTEMP, 0);
              TERN_(HAS_FAN, thermalManager.set_fan_speed(0, MAX_FAN_SPEED));
              Draw_Menu(PreheatHotend);
            }
            else {
              #if HAS_E_CALIBRATION
                if (temp_val.popup_EC == EC_Stage2) {
                  temp_val.selection = temp_val.old_last_selection;
                  temp_val.process = temp_val.old_process;
                  e_calibrate.Draw_Stage1();
                  }
                else 
              #endif
                  Redraw_Menu(true, true, false);
            }
            break;
        #endif

        #if HAS_E_CALIBRATION
          case EC_Stage1:
            if (temp_val.selection == 0) 
              e_calibrate.Draw_Stage2();
            else {          
              Redraw_Menu(true, true, (temp_val.active_menu != Steps));
            }
            break;
        #endif

        #if HAS_BED_PROBE
          case ManualProbing:
            if (temp_val.selection == 0) {
              Popup_Handler(Level, true);
              char buf[80];
              temp_val.zval = probe.probe_at_point(current_position.x, current_position.y, PROBE_PT_STOW, 0, false);
              if (isnan(temp_val.zval)) {
                Update_Status(GET_TEXT(MSG_ZPROBE_UNREACHABLE));
                Redraw_Menu(true, true, false);
              }
              else {
                const float dif = temp_val.zval - temp_val.corner_avg;
                if (dif == 0) Update_Status(GET_TEXT(MSG_CORNER_ZEROED));
                else sprintf_P(buf, dif > 0 ? GET_TEXT(MSG_CORNER_HIGH) : GET_TEXT(MSG_CORNER_LOW), dtostrf(abs(dif), 1, 3, str_1));
                if (abs(dif) <= 0.05) AudioFeedback(true);
                Update_Status(buf);
                Popup_Handler(ManualProbing);
              }
            }
            else {
              Update_Status("");
              Redraw_Menu(true, true, false);
            }
            break;
        #endif

        #if ENABLED(ADVANCED_PAUSE_FEATURE)
          case ConfFilChange:
            if (temp_val.selection == 0) {
              if (thermalManager.temp_hotend[0].target < thermalManager.extrude_min_temp)
                Popup_Handler(ETemp);
              else {
                temp_val.flag_chg_fil = true;
                if (thermalManager.temp_hotend[0].is_below_target(-2)) {
                  Popup_Handler(Heating);
                  Update_Status(GET_TEXT(MSG_HEATING));
                  thermalManager.wait_for_hotend(0);
                }
                Popup_Handler(FilChange);
                Update_Status(GET_TEXT(MSG_FILAMENTCHANGE));
                sprintf_P(cmd, PSTR("M600 B1 R%i"), thermalManager.temp_hotend[0].target);
                gcode.process_subcommands_now(cmd);
              }
            }
            else
              Redraw_Menu(true, true, false);
            break;
          case PurgeMore:
            if (temp_val.selection == 0) {
              pause_menu_response = PAUSE_RESPONSE_EXTRUDE_MORE;
              Popup_Handler(FilChange);
            }
            else {
              pause_menu_response = PAUSE_RESPONSE_RESUME_PRINT;
              if (temp_val.printing) Popup_Handler(Resuming);
              else {
                if (temp_val.flag_chg_fil) Popup_Handler(FilChange, true);
                else Redraw_Menu(true, true, (temp_val.active_menu==PreheatHotend));
              }
            }
            break;
        #endif // ADVANCED_PAUSE_FEATURE
        case ConfirmStartPrint:
          if (temp_val.selection==0) {
            #if ENABLED(HAS_BED_PROBE, AUTO_BED_LEVELING_UBL) && !HAS_NO_AUTOTILT_AFTERNPRINT
              CrealityDWIN.Autotilt_AfterNPrint(temp_val.NPrinted);
            #endif
            gcode.process_subcommands_now(F("M220 S100\nM221 S100"));  // Initialize Flow and Feerate to 100%
            strcpy(reprint_filename, card.filename);
            card.openAndPrintFile(card.filename);}
          else{
            Redraw_Menu(true, true, true);
            gcode.process_subcommands_now(F("M117"));}
          break;

        case Reprint:
          if (temp_val.selection == 0) {
            #if ENABLED(HAS_BED_PROBE, AUTO_BED_LEVELING_UBL) && !HAS_NO_AUTOTILT_AFTERNPRINT
              CrealityDWIN.Autotilt_AfterNPrint(temp_val.NPrinted);
            #endif
            #if HAS_GCODE_PREVIEW
              temp_val.file_preview = true;
            #endif
            gcode.process_subcommands_now(F("M220 S100\nM221 S100"));  // Initialize Flow and Feerate to 100%
            card.openAndPrintFile(reprint_filename);
            }
          else { 
          TERN_(DEBUG_DWIN, SERIAL_ECHOLNPGM("DWIN_Print_Finished"));
          Draw_Main_Menu();
          }
          break;

        #if HAS_MESH
          #if HAS_GRAPHICAL_LEVELMANUAL && EXTJYERSUI
            case EditMesh:
              if (temp_val.selection == 0) Draw_EditMesh(true);
              else {
                BedLevelTools.LevelManual_flag = true;
                Popup_Handler(MoveWait);
                BedLevelTools.manual_mesh_move();
                gcode.process_subcommands_now(F("M211 S0"));
                Draw_Menu(LevelManual); 
              }
              break;

            case Finish_Editvalue:
              if (temp_val.selection == 0) Draw_EditMesh();
              else {
                temp_val.flag_leveling_m = true;
                BedLevelTools.LevelManual_flag = false;
                Popup_Handler(SaveLevel, true);
              }
              break;
          #endif

          case SaveLevel:
            #if HAS_GRAPHICAL_LEVELMANUAL && EXTJYERSUI
              if (!BedLevelTools.LevelManual_flag) {
                BedLevelTools.LevelManual_flag = true;
                gcode.process_subcommands_now(F("M211 S1"));
                set_bed_leveling_enabled(level_state);
                TERN_(AUTO_BED_LEVELING_BILINEAR, bedlevel.refresh_bed_level());
              }
            #endif
            if (temp_val.selection == 0) {
              #if !HAS_BED_PROBE || ENABLED(PROBE_MANUALLY)
                if (temp_val.flag_noprobe) {
                  #if ENABLED(AUTO_BED_LEVELING_UBL)
                    BedLevelTools.goto_mesh_value = false;
                  #endif
                  temp_val.flag_noprobe = false;
                  temp_val.liveadjust = TERN(USER_MOVE_LIVE, true, false);
                  gcode.process_subcommands_now(F("M211 S1"));
                  set_bed_leveling_enabled(level_state);
                }
              #endif
              #if ENABLED(AUTO_BED_LEVELING_UBL)
                #if ENABLED(EEPROM_SETTINGS)
                  gcode.process_subcommands_now(F("G29 S"));
                  planner.synchronize();
                  AudioFeedback(true);
                #endif  
              #else
                #if ENABLED(EEPROM_SETTINGS)
                  AudioFeedback(settings.save());
                #endif
              #endif
            }
            Dis_steppers_and_cooldown();
            if (temp_val.flag_leveling_m) {
              temp_val.flag_leveling_m = false;
              #if !HAS_RC_CPU
                if (temp_val.flag_QuickHome) Draw_Quick_Home();
                else Draw_Menu(Leveling, LEVELING_MANUAL);
              #else
                Draw_Menu(Leveling, LEVELING_MANUAL);
              #endif
            }
            else Draw_Menu(Leveling, LEVELING_GET_MESH);
            break;
        #endif

        #if ENABLED(AUTO_BED_LEVELING_UBL)
          case MeshSlot:
            if (temp_val.selection==0) Draw_Menu(Leveling, LEVELING_SLOT);
            else Redraw_Menu(true, true, false);
            break;
        #endif
        
        #if HAS_BED_PROBE
          case Bed_T_Wizard:
            if (temp_val.selection == 0) {
              temp_val.process = Popup; temp_val.popup = Wizard_Bed_T;
              temp_val.selection = 0;
              Bed_Tramming_wizard(true);
              }
            else Bed_Tramming_wizard(false);
            break;

          case Wizard_Bed_T:
            if (temp_val.selection == 0) { temp_val.selection = 0; Bed_Tramming_wizard(true); }
            else { temp_val.flag_busy = false; Redraw_Menu(true, true); }
            break;
        #endif
        default: break;
      
      }
    }
    DWIN_UpdateLCD();
  }

  void CrealityDWINClass::Confirm_Control() {
    EncoderState encoder_diffState = Encoder_ReceiveAnalyze();
    
    if (encoder_diffState == ENCODER_DIFF_NO) return;
    if (encoder_diffState == ENCODER_DIFF_ENTER) {
      switch (temp_val.popup) {
        #if HAS_LEVELING
          #if HAS_LIVEMESH
            case Level3:
          #else
            case Level2:
          #endif
          #if EXTJYERSUI
            temp_val.cancel_lev = 1;
          #endif
          wait_for_user = false;
          //Redraw_Menu(true, true, false);
          Draw_Menu(Leveling, LEVELING_GET_MESH);
          queue.inject(F("M84"));
          break;
        #endif
        #if HAS_ES_DIAG
          case endsdiag:
            wait_for_user = false;
			      Redraw_Menu(true, true, false);
            break;  
        #endif
        #if HAS_HOTEND && HAS_E_CALIBRATION
          case EC_Stage4:
            wait_for_user = false;
            AudioFeedback(settings.save());
            Draw_Menu(Steps, STEPS_E_CALIBRATION);
            break;
        #endif
        case Complete:
          #if HAS_GCODE_PREVIEW
            temp_val.file_preview = false;
          #endif
          queue.inject(F("M84"));
          temp_val.flag_resetstatus = true;
          temp_val.flag_stop_print = false;
          if (HMI_datas.reprint_on)  Popup_Handler(Reprint);
          else { TERN_(DEBUG_DWIN, SERIAL_ECHOLNPGM("DWIN_Print_Finished")); Draw_Main_Menu(); }
          break;
        case FilInsert:
          Popup_Handler(FilChange);
          wait_for_user = false;
          break;
        case HeaterTime:
          Popup_Handler(Heating);
          Update_Status(GET_TEXT(MSG_HEATING));
          wait_for_user = false;
          break;
		    #if HAS_MESH
          case viewmesh:
            while (temp_val.drawing_mesh != false);
            temp_val.flag_viewmesh = false;
            DWIN_Draw_Rectangle(1, TERN(HAS_TJC_DISPLAY, Color_Bg_Black, GetColor(HMI_datas.background, Color_Bg_Black)), 0, KEY_Y_START, DWIN_WIDTH-2, DWIN_HEIGHT-2);
            Draw_Status_Area(true);
            Update_Status_Bar(true);
            wait_for_user = false;
            Popup_Handler(SaveLevel);
            break;
		    #endif
        #if EXTJYERSUI && PID_GRAPH && (HAS_TEMP_SENSOR || HAS_HEATED_BED) && !HAS_RC_CPU
          case Plottemp:
            Clear_Screen(2);
            Update_Status_Bar(true);
            wait_for_user = false;
            Redraw_Menu(true, true, false);
            break;
        #endif
        default:
          wait_for_user = false;
          Redraw_Menu(true, true, false);
          break;
      }
    }
    DWIN_UpdateLCD();
  }

  #if HAS_HOSTACTION_MENUS
    void CrealityDWINClass::Keyboard_Control() {
      const uint8_t keyboard_size = 34;
      static uint8_t key_selection = 0, cursor = 0;
      static char string[31];
      static bool uppercase = false, locked = false;
      if (temp_val.reset_keyboard) {
        if (strcmp(stringpointer, "-") == 0) stringpointer[0] = '\0';
        key_selection = 0, cursor = strlen(stringpointer);
        uppercase = false, locked = false;
        temp_val.reset_keyboard = false;
        strcpy(string, stringpointer);
      }
      EncoderState encoder_diffState = Encoder_ReceiveAnalyze();
      if (encoder_diffState == ENCODER_DIFF_NO) return;
      if (encoder_diffState == ENCODER_DIFF_CW && key_selection < keyboard_size) {
        Draw_Keys(key_selection, false, uppercase, locked);
        key_selection++;
        Draw_Keys(key_selection, true, uppercase, locked);
      }
      else if (encoder_diffState == ENCODER_DIFF_CCW && key_selection > 0) {
        Draw_Keys(key_selection, false, uppercase, locked);
        key_selection--;
        Draw_Keys(key_selection, true, uppercase, locked);
      }
      else if (encoder_diffState == ENCODER_DIFF_ENTER) {
        if (key_selection < 28) {
          if (key_selection == 19) {
            if (!temp_val.numeric_keyboard) {
              if (locked) {
                uppercase = false, locked = false;
                Draw_Keyboard(temp_val.keyboard_restrict, false, key_selection, uppercase, locked);
              } else if (uppercase) {
                locked = true;
                Draw_Keyboard(temp_val.keyboard_restrict, false, key_selection, uppercase, locked);
              }
              else {
                uppercase = true;
                Draw_Keyboard(temp_val.keyboard_restrict, false, key_selection, uppercase, locked);
              }
            }
          }
          else if (key_selection == 27) {
            cursor--;
            string[cursor] = '\0';
          }
          else {
            uint8_t index = key_selection;
            if (index > 19) index--;
            if (index > 27) index--;
            const char *keys;
            if (temp_val.numeric_keyboard) keys = "1234567890&<>() {}[]*\"\':;!?";
            else keys = (uppercase) ? "QWERTYUIOPASDFGHJKLZXCVBNM" : "qwertyuiopasdfghjklzxcvbnm";
            if (!(temp_val.keyboard_restrict && temp_val.numeric_keyboard && index > 9)) {
              string[cursor] = keys[index];
              cursor++;
              string[cursor] = '\0';
            }
            if (!locked && uppercase) {
              uppercase = false;
              Draw_Keyboard(temp_val.keyboard_restrict, false, key_selection, uppercase, locked);
            }
          }
        }
        else {
          switch (key_selection) {
            case 28:
              if (!temp_val.numeric_keyboard) uppercase = false, locked = false;
              Draw_Keyboard(temp_val.keyboard_restrict, !temp_val.numeric_keyboard, key_selection, uppercase, locked);
              break;
            case 29:
              string[cursor] = '-';
              cursor++;
              string[cursor] = '\0';
              break;
            case 30:
              string[cursor] = '_';
              cursor++;
              string[cursor] = '\0';
              break;
            case 31:
              if (!temp_val.keyboard_restrict) {
                string[cursor] = ' ';
                cursor++;
                string[cursor] = '\0';
              }
              break;
            case 32:
              if (!temp_val.keyboard_restrict) {
                string[cursor] = '.';
                cursor++;
                string[cursor] = '\0';
              }
              break;
            case 33:
              if (!temp_val.keyboard_restrict) {
                string[cursor] = '/';
                cursor++;
                string[cursor] = '\0';
              }
              break;
            case 34:
              if (string[0] == '\0') strcpy(string, "-");
              strcpy(stringpointer, string);
              temp_val.process = Menu;
              DWIN_Draw_Rectangle(1, TERN(HAS_TJC_DISPLAY, Color_Bg_Black, GetColor(HMI_datas.background, Color_Bg_Black)), 0, KEY_Y_START, DWIN_WIDTH-2, DWIN_HEIGHT-2);
              Draw_Status_Area(true);
              Update_Status_Bar(true);
              break;
          }
        }
        if (strlen(string) > temp_val.maxstringlen) string[temp_val.maxstringlen] = '\0', cursor = temp_val.maxstringlen;
        Draw_String(string, temp_val.selection, (temp_val.process==Keyboard), (temp_val.maxstringlen > 10));
      }
      DWIN_UpdateLCD();
    }
  #endif


  /* In-Menu Value Modification */

  void CrealityDWINClass::Setup_Value(float value, float min, float max, float unit, uint8_t type) {
    if (TERN0(PIDTEMP, valuepointer == &thermalManager.temp_hotend[0].pid.Ki) || TERN0(PIDTEMPBED, valuepointer == &thermalManager.temp_bed.pid.Ki))
      temp_val.tempvalue = unscalePID_i(value) * unit;
    else if (TERN0(PIDTEMP, valuepointer == &thermalManager.temp_hotend[0].pid.Kd) || TERN0(PIDTEMPBED, valuepointer == &thermalManager.temp_bed.pid.Kd))
      temp_val.tempvalue = unscalePID_d(value) * unit;
    else
      temp_val.tempvalue = value * unit;
    temp_val.valuemin = min;
    temp_val.valuemax = max;
    temp_val.valueunit = unit;
    temp_val.valuetype = type;
    temp_val.process = Value;
    EncoderRate.enabled = true;
    Draw_Float(temp_val.tempvalue / unit, temp_val.selection - temp_val.scrollpos, true, temp_val.valueunit);
  }

  void CrealityDWINClass::Modify_Value(float &value, float min, float max, float unit, void (*f)()/*=nullptr*/) {
    valuepointer = &value;
    funcpointer = f;
    Setup_Value((float)value, min, max, unit, 0);
  }
  void CrealityDWINClass::Modify_Value(uint8_t &value, float min, float max, float unit, void (*f)()/*=nullptr*/) {
    valuepointer = &value;
    funcpointer = f;
    Setup_Value((float)value, min, max, unit, 1);
  }
  void CrealityDWINClass::Modify_Value(uint16_t &value, float min, float max, float unit, void (*f)()/*=nullptr*/) {
    valuepointer = &value;
    funcpointer = f;
    Setup_Value((float)value, min, max, unit, 2);
  }
  void CrealityDWINClass::Modify_Value(int16_t &value, float min, float max, float unit, void (*f)()/*=nullptr*/) {
    valuepointer = &value;
    funcpointer = f;
    Setup_Value((float)value, min, max, unit, 3);
  }
  void CrealityDWINClass::Modify_Value(uint32_t &value, float min, float max, float unit, void (*f)()/*=nullptr*/) {
    valuepointer = &value;
    funcpointer = f;
    Setup_Value((float)value, min, max, unit, 4);
  }
  void CrealityDWINClass::Modify_Value(int8_t &value, float min, float max, float unit, void (*f)()/*=nullptr*/) {
    valuepointer = &value;
    funcpointer = f;
    Setup_Value((float)value, min, max, unit, 5);
  }

  void CrealityDWINClass::Modify_Option(uint8_t value, const char * const * options, uint8_t max) {
    temp_val.tempvalue = value;
    valuepointer = const_cast<const char * *>(options);
    temp_val.valuemin = 0;
    temp_val.valuemax = max;
    temp_val.process = Option;
    EncoderRate.enabled = true;
    Draw_Option(value, options, temp_val.selection - temp_val.scrollpos, true);
  }

  #if HAS_HOSTACTION_MENUS
    void CrealityDWINClass::Modify_String(char * string, uint8_t maxlength, bool restrict) {
      stringpointer = string;
      temp_val.maxstringlen = maxlength;
      temp_val.reset_keyboard = true;
      Draw_Keyboard(restrict, false);
      Draw_String(string, temp_val.selection, true, (temp_val.maxstringlen > 10));
    }
  #endif

  /* Main Functions */

  #if HAS_SHORTCUTS
    void CrealityDWINClass::Init_Main() {
      temp_val.Available_Shortcut_Count = Get_Available_Shortcut_Count();
      if ((temp_val.shortcutsmode) && (temp_val.Available_Shortcut_Count > 1)) {
          temp_val.activ_shortcut = 0;
          temp_val.old_sel_shortcut = 4; 
          temp_val.flagdir = false;
          temp_val.current_shortcut_0 =  Get_Available_Shortcut(0);
          temp_val.current_shortcut_1 =  Get_Available_Shortcut(1);
        }
    }
  #endif

  void CrealityDWINClass::Update_Status(const char * const text) {
    #if HAS_STATUS_MESSAGE_TIMEOUT
      ui.set_status_reset_fn();
      ui.status_message_expire_ms = millis() + (STATUS_MESSAGE_TIMEOUT_SEC) * 1000UL;
    #endif
    if (strncmp_P(text, PSTR("<F>"), 3) == 0) {
      LOOP_L_N(i, _MIN((size_t)LONG_FILENAME_LENGTH, strlen(text))) filename[i] = text[i + 3];
      filename[_MIN((size_t)LONG_FILENAME_LENGTH - 1, strlen(text))] = '\0';
      Draw_Print_Filename(true);
    }
    else {
      LOOP_L_N(i, _MIN((size_t)128, strlen(text))) statusmsg[i] = text[i];
      statusmsg[_MIN((size_t)128, strlen(text))] = '\0';
    }
  }

  void CrealityDWINClass::Start_Print(bool sd) {
    temp_val.sdprint = sd;
    if (!temp_val.printing) {
      temp_val.printing = true;
      temp_val.flag_stop_print = false;
      statusmsg[0] = '\0';
      if (sd) {
        #if ENABLED(POWER_LOSS_RECOVERY)
          if (recovery.valid()) {
            MediaFile *diveDir = nullptr;
            const char * const fname = card.diveToFile(true, diveDir, recovery.info.sd_filename);
            card.selectFileByName(fname);
            #if HAS_GCODE_PREVIEW
              #if HAS_LIGHT_GCODE_PREVIEW
                bool has_preview = Has_Preview();
                temp_val.file_preview = has_preview;
                if (!has_preview)  gcode.process_subcommands_now(F("M117 Preview not found"));
              #else
                uint16_t image_address;
                bool has_preview = find_and_decode_gcode_preview(card.filename, Thumnail_Preview, &image_address);
                temp_val.file_preview = has_preview;
                if (has_preview)   file_preview_image_address = image_address;
                else  gcode.process_subcommands_now(F("M117 Preview not found"));
              #endif
            #endif
          }
        #endif
        strcpy(filename, card.longest_filename());
      }
      else
        strcpy(filename, Hostfilename);
      TERN_(SET_PROGRESS_PERCENT, ui.set_progress(0));
      TERN_(SET_REMAINING_TIME, ui.set_remaining_time(0));
      Draw_Print_Screen();
    }
  }

  void CrealityDWINClass::DWIN_Hostheader(const char *text = nullptr) {
  if (text) {
    const int8_t size = _MIN((unsigned) LONG_FILENAME_LENGTH, strlen_P(text));
    LOOP_L_N(i, size) Hostfilename[i] = text[i];
    Hostfilename[size] = '\0';
    }
  }

  void CrealityDWINClass::Stop_Print() {
    temp_val.printing = false;
    temp_val.sdprint = false;
    thermalManager.cooldown();
    duration_t printing_time = print_job_timer.duration();
    temp_val.flag_resetstatus = false;
    sprintf_P(cmd, PSTR("%s: %02dh %02dm %02ds"), GET_TEXT(MSG_INFO_PRINT_TIME), (uint8_t)(printing_time.value / 3600), (uint8_t)((printing_time.value / 60) %60), (uint8_t)(printing_time.value %60));
    #if HAS_GCODE_PREVIEW
      if (!temp_val.file_preview) Update_Status(cmd);
    #else
      Update_Status(cmd);
    #endif
    TERN_(SET_PROGRESS_PERCENT, ui.set_progress(100 * (PROGRESS_SCALE)));
    TERN_(SET_REMAINING_TIME, ui.set_remaining_time(0));
    #if HAS_MESH
      if(HMI_datas.leveling_active) set_bed_leveling_enabled(HMI_datas.leveling_active);
    #endif
    Draw_Print_confirm();
  }

  void CrealityDWINClass::Update() {
    State_Update();
    Screen_Update();
    temp_val.sd_item_flag = false;
    switch (temp_val.process) {
      case Main:      Main_Menu_Control();    break;
      case Menu:      Menu_Control();         break;
      case Value:     Value_Control();        break;
      case Option:    Option_Control();       break;
      case File:      temp_val.sd_item_flag = true;  File_Control();         break;
      case Print:     Print_Screen_Control(); break;
      case Popup:     Popup_Control();        break;
      case Confirm:   Confirm_Control();      break;
      #if HAS_GRAPHICAL_LEVELMANUAL && EXTJYERSUI
        case Selectvalue_Mesh: Selectvalue_Control();     break;
        case Editvalue_Mesh: Editvalue_Control();       break;
      #endif
      #if HAS_HOSTACTION_MENUS
        case Keyboard:   Keyboard_Control();     break;
      #endif
      #if HAS_SCREENLOCK
        case Locked:     HMI_ScreenLock();       break;
      #endif
      #if HAS_SHORTCUTS
        case Short_cuts :  HMI_Move_Z();       break;
      #endif
      #if HAS_HOTEND && HAS_E_CALIBRATION
        case eCalibration:  HMI_E_Calibration(); break;
      #endif
    }
  }

  void MarlinUI::update() { CrealityDWIN.Update(); }

  #if HAS_LCD_BRIGHTNESS
    void MarlinUI::_set_brightness() { DWIN_LCD_Brightness(backlight ? brightness : 0); }
  #endif

  void CrealityDWINClass::State_Update() {
    if ((print_job_timer.isRunning() || print_job_timer.isPaused()) != temp_val.printing) {
      if (!temp_val.printing) Start_Print(card.isFileOpen() || TERN0(POWER_LOSS_RECOVERY, recovery.valid()));
      else Stop_Print();
    }
    if (print_job_timer.isPaused() != temp_val.paused) {
      temp_val.paused = print_job_timer.isPaused();
      if (temp_val.process == Print) Print_Screen_Icons();
      if (temp_val.process == Wait && !temp_val.paused) Redraw_Menu(true, true);
    }
    if (wait_for_user && !(temp_val.process == Confirm) && !print_job_timer.isPaused())
      Confirm_Handler(UserInput);
    #if ENABLED(ADVANCED_PAUSE_FEATURE)
      if (temp_val.process == Popup && temp_val.popup == PurgeMore) {
        if (pause_menu_response == PAUSE_RESPONSE_EXTRUDE_MORE)
          Popup_Handler(FilChange);
        else if (pause_menu_response == PAUSE_RESPONSE_RESUME_PRINT) {
          if (temp_val.printing) Popup_Handler(Resuming);
          else {
            if (temp_val.flag_chg_fil) Popup_Handler(FilChange, true);
            else Redraw_Menu(true, true, (temp_val.active_menu==PreheatHotend));
          }
        }
      }
    #endif
    #if HAS_FILAMENT_SENSOR
      static bool ranout = false;
      if (runout.filament_ran_out != ranout) {
        ranout = runout.filament_ran_out;
        if (ranout) Popup_Handler(Runout);
      }
    #endif
  }

  void CrealityDWINClass::Screen_Update() {
    const millis_t ms = millis();

    #if HAS_BACKLIGHT_TIMEOUT
      if (ui.lcd_backlight_timeout) {
        temp_val.backlight_timeout_disable = false;
        switch (temp_val.process) { 
          case Popup:
          case Confirm:
            if (temp_val.flag_stop_print)  break; 
          case Wait:
          case Locked:
          #if HAS_GRAPHICAL_LEVELMANUAL && EXTJYERSUI
            case Selectvalue_Mesh:
            case Editvalue_Mesh:
          #endif
          case Keyboard:
            temp_val.backlight_timeout_disable = true;
            ui.refresh_backlight_timeout();
            break;
          default:
            break;
        }
      }
      else temp_val.backlight_timeout_disable = true;
      if (!temp_val.backlight_timeout_disable && ui.backlight_off_ms && ELAPSED(ms, ui.backlight_off_ms)) {
        ui.set_brightness(0);
        ui.backlight_off_ms = 0;
        #ifdef NEOPIXEL_BKGD_INDEX_FIRST
          neo.set_background_off();
          neo.show();
        #endif
      }
    #endif

    static millis_t scrltime = 0;
    if (ELAPSED(ms, scrltime)) {
      scrltime = ms + 200;
      if (temp_val.process != Keyboard) Update_Status_Bar();
      if (temp_val.process == Print) Draw_Print_Filename();
    }

    static millis_t statustime = 0;
    if (ELAPSED(ms, statustime) && temp_val.process != Keyboard) {
      statustime = ms + 500;
      if (!temp_val.flag_viewmesh) Draw_Status_Area();
      #if HAS_ES_DIAG
        if (temp_val.process == Confirm && temp_val.popup == endsdiag) EndSDiag.Update_ends_diag();
      #endif
      
      #if HAS_STATUS_MESSAGE_TIMEOUT
        bool did_expire = ui.status_reset_callback && (*ui.status_reset_callback)();
        did_expire |= ui.status_message_expire_ms && ELAPSED(ms, ui.status_message_expire_ms);
        if ((did_expire) && (temp_val.flag_resetstatus) && (!temp_val.printing)) ui.reset_status();
      #endif
    }

    #if ENABLED(EXTJYERSUI, PID_GRAPH)
      static millis_t pidstatustime = 0;
      if (ELAPSED(ms, pidstatustime) && temp_val.process != Keyboard) {
        pidstatustime = ms + 1000;
        if (temp_val.process == Wait && (temp_val.popup == PIDWaitH || temp_val.popup == MPCWaitH))
          plot.UpdateP(thermalManager.wholeDegHotend(0));
        if (temp_val.process == Wait && temp_val.popup == PIDWaitB)
          plot.UpdateP(thermalManager.wholeDegBed());
        #if HAS_TEMP_SENSOR && !HAS_RC_CPU
          if (temp_val.process == Confirm && temp_val.popup  == Plottemp)
            plot.UpdateP(temp_val.htemp ? thermalManager.wholeDegHotend(0) : thermalManager.wholeDegBed());
        #endif
      }
    #endif

    static millis_t printtime = 0;
    if (ELAPSED(ms, printtime)) {
      printtime = ms + 1000;
      if (temp_val.process == Print) {
        Draw_Print_ProgressBar();
        Draw_Print_ProgressElapsed();
        TERN_(SET_REMAINING_TIME, Draw_Print_ProgressRemain());
      }
    }

    static bool mounted = card.isMounted();
    if (mounted != card.isMounted()) {
      mounted = card.isMounted();
      #if HAS_GCODE_PREVIEW && HAS_NORMAL_GCODE_PREVIEW
        image_cache.clear();
      #endif
      if (temp_val.process == File) {
        #if ALL(SDSUPPORT, SDCARD_SORT_ALPHA, SDSORT_GCODE)
          DWIN_Sort_SD(card.isMounted());
        #endif
        temp_val.sd_item_flag = true;
        Draw_SD_List();
      }
      else temp_val.sd_item_flag = false;
    }

    #if ENABLED(CPU_STM32G0B1)
      static millis_t refreshtime = 0;
      if (ELAPSED(ms, refreshtime)) {
        refreshtime = ms + SEC_TO_MS(temp_val.refreshtimevalue);
        if (temp_val.process == Print) Redraw_Screen();
      }
    #endif

    #if HAS_GCODE_PREVIEW && HAS_NORMAL_GCODE_PREVIEW
      if (HMI_datas.show_gcode_thumbnails && ELAPSED(millis(), thumbtime)) {
        uint16_t cColor = GetColor(HMI_datas.cursor_color, Rectangle_Color);
        thumbtime = millis() + 60000;
        if (temp_val.process == File) {
          temp_val.sd_item_flag = true;
          if (temp_val.selection-temp_val.scrollpos > MROWS) temp_val.scrollpos = temp_val.selection - MROWS;
          LOOP_L_N(i, _MIN(card.get_num_Files() + 1, TROWS)) {
            if (Encoder_ReceiveAnalyze() != ENCODER_DIFF_NO) break;
            if (i + temp_val.scrollpos == 0) {
              if (card.flag.workDirIsRoot)
                Draw_Menu_Item(0, ICON_Back, GET_TEXT_F(MSG_BACK));
              else
                Draw_Menu_Item(0, ICON_Back, "..");
            } else {
              card.getfilename_sorted(SD_ORDER(i + temp_val.scrollpos - 1, card.get_num_Files()));
              Draw_Menu_Item(i, card.flag.filenameIsDir ? ICON_More : ICON_File);
            }
            DWIN_UpdateLCD();
          }
          if ((cColor == TERN(HAS_TJC_DISPLAY, Color_Bg_Black, GetColor(HMI_datas.background, Color_Bg_Black))) || ((cColor == Color_Black) && (HMI_datas.background = 0)))
            DWIN_Draw_Rectangle(0, GetColor(HMI_datas.items_menu_text, Color_White), 0, MBASE(temp_val.selection - temp_val.scrollpos) - 18, 8, MBASE(temp_val.selection - temp_val.scrollpos) + 31);
          else
            DWIN_Draw_Rectangle(1, cColor, 0, MBASE(temp_val.selection - temp_val.scrollpos) - 18, 8, MBASE(temp_val.selection - temp_val.scrollpos) + 31);
        }
        else
          temp_val.sd_item_flag = false;
      }
    #endif

    #if HAS_HOTEND
      static int16_t hotendtarget = -1;
    #endif
    #if HAS_HEATED_BED
      static int16_t bedtarget = -1;
    #endif
    #if HAS_FAN
      static int16_t fanspeed = -1;
    #endif

    #if HAS_ZOFFSET_ITEM
      static float lastzoffset = temp_val.zoffsetvalue;
      if (temp_val.zoffsetvalue != lastzoffset && !temp_val.printing) {
        lastzoffset = temp_val.zoffsetvalue;
        #if HAS_BED_PROBE
          probe.offset.z = temp_val.zoffsetvalue;
        #else
          set_home_offset(Z_AXIS, -temp_val.zoffsetvalue);
        #endif
      }

      #if HAS_BED_PROBE
        if (probe.offset.z != lastzoffset)
          temp_val.zoffsetvalue = lastzoffset = probe.offset.z;
      #else
        if (-home_offset.z != lastzoffset)
          temp_val.zoffsetvalue = lastzoffset = -home_offset.z;
      #endif
    #endif // HAS_ZOFFSET_ITEM

    if (temp_val.process == Menu || temp_val.process == Value) {

      switch (temp_val.active_menu) {
        case TempMenu:
          #if HAS_HOTEND
            if (thermalManager.temp_hotend[0].target != hotendtarget) {
              hotendtarget = thermalManager.temp_hotend[0].target;
              if (temp_val.scrollpos <= TEMP_HOTEND && TEMP_HOTEND <= temp_val.scrollpos + MROWS) {
                if (temp_val.process != Value || temp_val.selection != TEMP_HOTEND - temp_val.scrollpos)
                  Draw_Float(thermalManager.temp_hotend[0].target, TEMP_HOTEND - temp_val.scrollpos, false, 1);
              }
            }
          #endif
          #if HAS_HEATED_BED
            if (thermalManager.temp_bed.target != bedtarget) {
              bedtarget = thermalManager.temp_bed.target;
              if (temp_val.scrollpos <= TEMP_BED && TEMP_BED <= temp_val.scrollpos + MROWS) {
                if (temp_val.process != Value || temp_val.selection != TEMP_HOTEND - temp_val.scrollpos)
                  Draw_Float(thermalManager.temp_bed.target, TEMP_BED - temp_val.scrollpos, false, 1);
              }
            }
          #endif
          #if HAS_FAN
            if (thermalManager.fan_speed[0] != fanspeed) {
              fanspeed = thermalManager.fan_speed[0];
              if (temp_val.scrollpos <= TEMP_FAN && TEMP_FAN <= temp_val.scrollpos + MROWS) {
                if (temp_val.process != Value || temp_val.selection != TEMP_HOTEND - temp_val.scrollpos) {
                  temp_val.fan_speed = Set_fan_speed_value();
                  Draw_Float(temp_val.fan_speed, TEMP_FAN - temp_val.scrollpos, false, 1);
                }
              }
            }
          #endif
          break;
        case Tune:
          #if HAS_HOTEND
            if (thermalManager.temp_hotend[0].target != hotendtarget) {
              hotendtarget = thermalManager.temp_hotend[0].target;
              if (temp_val.scrollpos <= TUNE_HOTEND && TUNE_HOTEND <= temp_val.scrollpos + MROWS) {
                if (temp_val.process != Value || temp_val.selection != TEMP_HOTEND - temp_val.scrollpos)
                  Draw_Float(thermalManager.temp_hotend[0].target, TUNE_HOTEND - temp_val.scrollpos, false, 1);
              }
            }
          #endif
          #if HAS_HEATED_BED
            if (thermalManager.temp_bed.target != bedtarget) {
              bedtarget = thermalManager.temp_bed.target;
              if (temp_val.scrollpos <= TUNE_BED && TUNE_BED <= temp_val.scrollpos + MROWS) {
                if (temp_val.process != Value || temp_val.selection != TEMP_HOTEND - temp_val.scrollpos)
                  Draw_Float(thermalManager.temp_bed.target, TUNE_BED - temp_val.scrollpos, false, 1);
              }
            }
          #endif
          #if HAS_FAN
            if (thermalManager.fan_speed[0] != fanspeed) {
              fanspeed = thermalManager.fan_speed[0];
              if (temp_val.scrollpos <= TUNE_FAN && TUNE_FAN <= temp_val.scrollpos + MROWS) {
                if (temp_val.process != Value || temp_val.selection != TEMP_HOTEND - temp_val.scrollpos) {
                  temp_val.fan_speed = Set_fan_speed_value();
                  Draw_Float(temp_val.fan_speed, TUNE_FAN - temp_val.scrollpos, false, 1);
                }
              }
            }
          #endif
          break;
      }
    }
  }

  void CrealityDWINClass::AudioFeedback(const bool success/*=true*/) {
    if (ui.sound_on)
    DONE_BUZZ(success);
    else
      Update_Status(success ? GET_TEXT(MSG_SUCCESS) : GET_TEXT(MSG_FAILED));
  }

  void CrealityDWINClass::ManualLevel_Init() {
    if (axes_should_home()) {
      Popup_Handler(Home);
      gcode.home_all_axes(true);
    }
    #if HAS_LEVELING
      level_state = planner.leveling_active;
      set_bed_leveling_enabled(false);
    #endif    
    Draw_Menu(ManualLevel);
  }

  #if HAS_ZOFFSET_ITEM
    void CrealityDWINClass::ZOffset_Init() {
      #if HAS_LEVELING
        level_state = planner.leveling_active;
        #if NONE(Z_MIN_PROBE_USES_Z_MIN_ENDSTOP_PIN, USE_PROBE_FOR_Z_HOMING)
          set_bed_leveling_enabled(true);
        #else
          set_bed_leveling_enabled(false);
        #endif
      #endif
      #if !HAS_BED_PROBE
        gcode.process_subcommands_now(F("M211 S0"));
      #endif
      Draw_Menu(ZOffset);
    }
  #endif

  #if HAS_LEVELING_HEAT
      void CrealityDWINClass::HeatBeforeLeveling() {
        Popup_Handler(Heating);
        Update_Status(GET_TEXT(MSG_HEATING));
        #if HAS_HOTEND
          if (!HMI_datas.ena_LevelingTemp_hotend) thermalManager.setTargetHotend(0, 0);
          else if (thermalManager.degTargetHotend(0) < HMI_datas.LevelingTemp_hotend)
                  thermalManager.setTargetHotend(HMI_datas.LevelingTemp_hotend, 0);
        #endif
        #if HAS_HEATED_BED
          if (!HMI_datas.ena_LevelingTemp_bed) thermalManager.setTargetBed(0);
          else if (thermalManager.degTargetBed() < HMI_datas.LevelingTemp_bed)
              thermalManager.setTargetBed(HMI_datas.LevelingTemp_bed); 
        #endif
        if (HMI_datas.ena_LevelingTemp_hotend) TERN_(HAS_HOTEND, thermalManager.wait_for_hotend(0));
        if (HMI_datas.ena_LevelingTemp_bed) TERN_(HAS_HEATED_BED, thermalManager.wait_for_bed_heating());
      }
  #endif

  #if EITHER(PIDTEMP, PIDTEMPBED)
    void CrealityDWINClass::SetPID(celsius_t t, heater_id_t h) {
      #if ENABLED(PIDTEMP) && !HAS_RC_CPU_UBL_IS
        if (h == H_E0) {
          temp_val.last_process = temp_val.process;
          temp_val.last_selection = temp_val.selection;
          Popup_Handler(MoveWait);
          sprintf_P(cmd, PSTR("G28OXY\nG0Z5F300\nG0X%sY%sF5000\nM84\nM400"),
          dtostrf(X_CENTER, 1, 1, str_1),
          dtostrf(Y_CENTER, 1, 1, str_2)
        );
        gcode.process_subcommands_now(cmd);
        }
      #endif
      thermalManager.PID_autotune(t, h, temp_val.PID_cycles, true);
    }
  #endif

  #if EXTJYERSUI && PID_GRAPH && ANY(HAS_PID_HEATING, MPC_AUTOTUNE)
      void CrealityDWINClass::DWIN_Draw_PID_MPC_Popup() {
        const uint16_t color_popup_txt = GetColor(HMI_datas.popup_txt, Popup_Text_Color);
        celsius_t _maxtemp, _target;
        constexpr frame_rect_t gfrm = {40, 160, DWIN_WIDTH - 80, 150};
        Clear_Screen(2);
        DWIN_Draw_Rectangle(1, GetColor(HMI_datas.popup_bg, Color_Bg_Window), 14, 60, 258, 330);
        DWIN_Draw_Rectangle(0, GetColor(HMI_datas.popup_highlight, Color_White), 14, 60, 258, 330);
        char nstr[10];
        
        #if ENABLED(MPC_AUTOTUNE)
          switch (temp_val.pidresult) {
            case MPCTEMP_START:
              _maxtemp = thermalManager.hotend_maxtemp[0];
              //_target = 200;
              _target = temp_val.MPCtargettemp;
              JYERSUI::Draw_CenteredString(color_popup_txt, 80, GET_TEXT_F(MSG_MPC_AUTOTUNE));
              JYERSUI::Draw_String(color_popup_txt, gfrm.x, gfrm.y - JYERSUI::fontHeight() - 4, F("MPC target:    Celsius"));
              JYERSUI::Draw_CenteredString(color_popup_txt, 100, F("for Nozzle is running."));
              break;
            #if ENABLED(PIDTEMPBED)
              case PID_BED_START:
                _maxtemp = BED_MAXTEMP;
                _target = temp_val.PID_bed_temp;
                JYERSUI::Draw_CenteredString(color_popup_txt, 80, GET_TEXT_F(MSG_PID_AUTOTUNE));
                JYERSUI::Draw_String(color_popup_txt, gfrm.x, gfrm.y - JYERSUI::fontHeight() - 4, F("PID target:    Celsius"));
                JYERSUI::Draw_CenteredString(color_popup_txt, 100, F("for BED is running."));
                break;
            #endif
            default:
              return;
          }
        #else
          #if HAS_PID_HEATING
            JYERSUI::Draw_CenteredString(color_popup_txt, 80, GET_TEXT_F(MSG_PID_AUTOTUNE));
            JYERSUI::Draw_String(color_popup_txt, gfrm.x, gfrm.y - JYERSUI::fontHeight() - 4, F("PID target:    Celsius"));
          #endif
          switch (temp_val.pidresult) {
            #if ENABLED(PIDTEMP)
              case PID_EXTR_START:
                _maxtemp = thermalManager.hotend_maxtemp[0];
                _target = temp_val.PID_e_temp;
                JYERSUI::Draw_CenteredString(color_popup_txt, 100, F("for Nozzle is running."));
                break;
            #endif
            #if ENABLED(PIDTEMPBED)
              case PID_BED_START:
                _maxtemp = BED_MAXTEMP;
                _target = temp_val.PID_bed_temp;
                JYERSUI::Draw_CenteredString(color_popup_txt, 100, F("for BED is running."));
                break;
            #endif
            default:
              return;
          }
        #endif
        #if EXTJYERSUI
          plot.Draw(gfrm, _maxtemp, _target);
        #endif
        sprintf_P(nstr, PSTR("%*i"), 3, _target);
        DWIN_Draw_String(false, JYERSUI::font, color_popup_txt, GetColor(HMI_datas.popup_bg, Color_Bg_Window), gfrm.x + 90, gfrm.y - JYERSUI::fontHeight() - 4, nstr);
      }
  #endif

  #if EXTJYERSUI && PID_GRAPH && (HAS_TEMP_SENSOR || HAS_HEATED_BED) && !HAS_RC_CPU
    void CrealityDWINClass::DWIN_Draw_Plot_Nozzle_Bed(const bool htemp/*=true*/) {
        char nstr[10];
        temp_val.htemp = htemp;
        temp_val.last_process = temp_val.process; temp_val.last_selection = temp_val.selection; temp_val.process = Confirm; temp_val.popup = Plottemp;
        const uint16_t color_popup_txt = GetColor(HMI_datas.popup_txt, Popup_Text_Color);
        const uint16_t color_popup_bg = GetColor(HMI_datas.popup_bg, Color_Bg_Window);
        celsius_t _maxtemp, _target;
        constexpr frame_rect_t gfrm = {40, 130, DWIN_WIDTH - 80, 150};
        Clear_Screen(2);
        DWIN_Draw_Rectangle(1, color_popup_bg, 14, 60, 258, 350);
        DWIN_Draw_Rectangle(0, GetColor(HMI_datas.popup_highlight, Color_White), 14, 60, 258, 350);
        JYERSUI::Draw_CenteredString(color_popup_txt, 80, htemp ? F("Nozzle Temperature") : F("Bed Temperature"));
        JYERSUI::Draw_String(color_popup_txt, gfrm.x, gfrm.y - JYERSUI::fontHeight() - 4, F("Target ->      Celsius"));
        _maxtemp = htemp ? thermalManager.hotend_maxtemp[0] : BED_MAXTEMP;
        _target = htemp ? thermalManager.temp_hotend[0].target : thermalManager.temp_bed.target;
        plot.Draw(gfrm, _maxtemp, _target);
        sprintf_P(nstr, PSTR("%*i"), 3, _target);
        DWIN_Draw_String(false, JYERSUI::font, color_popup_txt, color_popup_bg, gfrm.x + 90, gfrm.y - JYERSUI::fontHeight() - 4, nstr);
        DWIN_Draw_Rectangle(1, (HMI_datas.pack_red)? Color_Red : Confirm_Color, 87, 295, 186, 332);
        DWIN_Draw_Rectangle(0, Color_White, 86, 294, 187, 333);
        DWIN_Draw_Rectangle(0, Color_White, 85, 293, 188, 334);
        DWIN_Draw_String(false, DWIN_FONT_STAT, Color_White, Color_Bg_Window, 107, 305, F("Return"));
        DWIN_UpdateLCD();
    }
  #endif

  #if HAS_PID_HEATING
      void CrealityDWINClass::PidTuning(pidresult_t result) {
        temp_val.pidresult = result;
        switch (result) { 
          case PID_STARTED:  break;
          #if PID_GRAPH && EXTJYERSUI
            #if ENABLED(PIDTEMP)
              case PID_EXTR_START: 
                #if HAS_RC_CPU_UBL_IS
                  temp_val.last_process = temp_val.process; temp_val.last_selection = temp_val.selection;
                #endif
                temp_val.process = Wait; temp_val.popup = PIDWaitH; DWIN_Draw_PID_MPC_Popup(); break;
            #endif
            #if ENABLED(PIDTEMPBED)
              case PID_BED_START: temp_val.last_process = temp_val.process; temp_val.last_selection = temp_val.selection; temp_val.process = Wait; temp_val.popup = PIDWaitB; DWIN_Draw_PID_MPC_Popup(); break;
            #endif
          #else
            #if ENABLED(PIDTEMP)
              case PID_EXTR_START:  Popup_Handler(PIDWait); break;
            #endif
            #if ENABLED(PIDTEMPBED)
              case PID_BED_START:  Popup_Handler(PIDWait, true); break;
            #endif
          #endif
          #if ENABLED(PIDTEMP)
            case PID_BAD_HEATER_ID:  Confirm_Handler(BadextruderNumber);  break;
          #endif
          case PID_TEMP_TOO_HIGH:  Confirm_Handler(TemptooHigh);  break;
          case PID_TUNING_TIMEOUT:  Confirm_Handler(PIDTimeout);  break;
          case PID_DONE: Confirm_Handler(PIDDone);  break;
          default: break;
        }
      }
  #endif

#if BOTH(MPC_AUTOTUNE, MPC_AUTOTUNE_MENU)
    void CrealityDWINClass::MPCTuning(pidresult_t result) {
      temp_val.pidresult = result;
      switch (result) {
        case MPCTEMP_START:
          #if PID_GRAPH && EXTJYERSUI
            temp_val.last_selection = temp_val.selection; temp_val.process = Wait; temp_val.popup = MPCWaitH; DWIN_Draw_PID_MPC_Popup(); break;
          #else
            Popup_Handler(MPCWait); break;
          #endif
        case MPC_TEMP_ERROR:
          Confirm_Handler(MPC_Temperror);
          ui.reset_alert_level();
          break;
        case MPC_INTERRUPTED:
          Confirm_Handler(MPC_Interrupted);
          ui.reset_alert_level();
          break;
        case MPC_DONE:
          Confirm_Handler(MPC_Done);
          ui.reset_alert_level();
          break;
        default:
          ui.reset_alert_level();
          break;
      }
    }

    void CrealityDWINClass::ApplyMPCTarget() {
      switch (temp_val.MPC_targetmode){
        case 0 : temp_val.MPCtargettemp = 200; break;
        case 1 : temp_val.MPCtargettemp = 220; break;
        default: break;
      }
    }

    #if ENABLED(MPC_EDIT_MENU)
      void CrealityDWINClass::ApplyFilHeatCapacity() {
        switch (temp_val.filheatcap) {
          case 0: mpc.filament_heat_capacity_permm = 0; break;
          case 1: mpc.filament_heat_capacity_permm = 5.6e-3f; break;
          case 2: mpc.filament_heat_capacity_permm = 3.6e-3f; break;
          case 3:
          default: break;
        }
      }
    #endif

#endif

  #if ENABLED(FILAMENT_LOAD_UNLOAD_GCODES)
    void CrealityDWINClass::DWIN_Load_Unload(const bool load/*=false*/) {
      temp_val.flag_chg_fil = load;
      if (thermalManager.temp_hotend[0].is_below_target(-2)) {
        Update_Status(GET_TEXT(MSG_FILAMENT_CHANGE_HEATING));
        Popup_Handler(Heating);
        thermalManager.wait_for_hotend(0);
      }
      Popup_Handler(FilLoad, load);
      if (load) Update_Status(GET_TEXT(MSG_FILAMENT_CHANGE_UNLOAD));
      else Update_Status(GET_TEXT(MSG_FILAMENT_CHANGE_LOAD));
      gcode.process_subcommands_now(F(load ? "M702" : "M701"));
      planner.synchronize();
    }
  #endif

  #if ENABLED(LED_CONTROL_MENU, HAS_COLOR_LEDS)
    void CrealityDWINClass::ApplyLEDColor() { 
      HMI_datas.Led_Color = TERN0(HAS_WHITE_LED, (leds.color.w << 24)) | (leds.color.r << 16) | (leds.color.g << 8) | leds.color.b;
    }
  #endif

  void CrealityDWINClass::Save_Settings(char *buff) {
    TERN_(AUTO_BED_LEVELING_UBL, HMI_datas.tilt_grid_size = BedLevelTools.tilt_grid - 1);
    #if ENABLED(HAS_BED_PROBE, AUTO_BED_LEVELING_UBL) && !HAS_NO_AUTOTILT_AFTERNPRINT
      HMI_datas.N_Printed = temp_val.NPrinted;
    #endif
    HMI_datas.corner_pos = temp_val.corner_pos * 10;
    #if HAS_SHORTCUTS
      HMI_datas.shortcuts_ui = temp_val.shortcutsmode;
      LOOP_L_N(i, NB_Shortcuts) {
        HMI_datas.shortcut_[i] = temp_val.shortcut[i];
      }
    #endif
    #if HAS_LEVELING && HAS_LEVELING_HEAT
      HMI_datas.TempmodeLeveling = temp_val.LevelingTempmode;
    #endif
    #if ENABLED(HOST_ACTION_COMMANDS, HAS_HOSTACTION_MENUS)
      HMI_datas.host_action_label_1 = Encode_String(action1);
      HMI_datas.host_action_label_2 = Encode_String(action2);
      HMI_datas.host_action_label_3 = Encode_String(action3);
    #endif
    #if HAS_MESH
      HMI_datas.leveling_active = planner.leveling_active;
    #endif
    #if ENABLED(DWIN_CREALITY_LCD_CUSTOM_ICONS)
      HMI_datas.main_icon = temp_val.iconmain;
    #endif
    #if HAS_HOTEND && BOTH(MPC_AUTOTUNE, MPC_AUTOTUNE_MENU)
      HMI_datas.MPCtargetmode = temp_val.MPC_targetmode;
      #if ENABLED(MPC_EDIT_MENU)
        HMI_datas.filheatcapacity = temp_val.filheatcap;
      #endif
    #endif
    #if HAS_AUTOTEMP_MENU
      HMI_datas.enable_autotemp = temp_val.autotemp_enable;
    #endif
    #if ENABLED(CPU_STM32G0B1)
      HMI_datas.refreshDtime = temp_val.refreshtimevalue;
    #endif
    TERN_(DEBUG_DWIN, SERIAL_ECHOLNPGM("Save_Settings"));
    TERN_(DEBUG_DWIN, SERIAL_ECHOLNPGM("HMI_datas: ", sizeof(HMI_datas_t)));
    memcpy(buff, &HMI_datas, _MIN(sizeof(HMI_datas), eeprom_data_size));
  }

  void CrealityDWINClass::Load_Settings(const char *buff) {
    TERN_(DEBUG_DWIN, SERIAL_ECHOLNPGM("Load_Settings"));
    memcpy(&HMI_datas, buff, _MIN(sizeof(HMI_datas), eeprom_data_size));
    JYERSUI::textcolor = GetColor(HMI_datas.items_menu_text, Color_White);
    JYERSUI::backcolor = TERN(HAS_TJC_DISPLAY, Color_Bg_Black, GetColor(HMI_datas.background, Color_Bg_Black));

    #if HAS_EXTRUDERS
      planner.set_flow(0, 100);
    #endif

    #if HAS_AUTOTEMP_MENU
      temp_val.autotemp_enable = HMI_datas.enable_autotemp;
    #endif
   
    #if HAS_HOTEND && BOTH(MPC_AUTOTUNE, MPC_AUTOTUNE_MENU)
      temp_val.MPC_targetmode = HMI_datas.MPCtargetmode;
      ApplyMPCTarget();
      #if ENABLED(MPC_EDIT_MENU)
        temp_val.filheatcap = HMI_datas.filheatcapacity;
        ApplyFilHeatCapacity();
      #endif
    #endif

    #if ENABLED(DWIN_CREALITY_LCD_CUSTOM_ICONS)
      temp_val.iconmain = HMI_datas.main_icon;
    #endif

    #if HAS_MESH
      if(HMI_datas.leveling_active) set_bed_leveling_enabled(HMI_datas.leveling_active);
    #endif
    TERN_(AUTO_BED_LEVELING_UBL, BedLevelTools.tilt_grid = HMI_datas.tilt_grid_size + 1);
    #if ENABLED(HAS_BED_PROBE, AUTO_BED_LEVELING_UBL) && !HAS_NO_AUTOTILT_AFTERNPRINT
      temp_val.NPrinted = HMI_datas.N_Printed;
    #endif
    if (HMI_datas.corner_pos == 0) HMI_datas.corner_pos = 325;
    temp_val.corner_pos = HMI_datas.corner_pos / 10.0f;
    #if HAS_FILAMENT_SENSOR
      Get_Rsensormode(runout.mode[0]);
    #endif

    #if ENABLED(BAUD_RATE_GCODE)
      SetBaudRate();
    #endif

    #if ALL(SDSUPPORT, SDCARD_SORT_ALPHA, SDSORT_GCODE)
      temp_val.old_sdsort = !HMI_datas.sdsort_alpha;
    #endif

    #if BOTH(LED_CONTROL_MENU, HAS_COLOR_LEDS)
      leds.set_color(
            (HMI_datas.Led_Color >> 16) & 0xFF,
            (HMI_datas.Led_Color >>  8) & 0xFF,
            (HMI_datas.Led_Color >>  0) & 0xFF
            OPTARG(HAS_WHITE_LED, (HMI_datas.Led_Color >> 24) & 0xFF)
          );
    #endif

    #if EXTJYERSUI && HAS_MESH
      if (HMI_datas.grid_max_points == 0) HMI_datas.grid_max_points = DEF_GRID_MAX_POINTS;
      _GridxGrid = Get_GridxGrid(HMI_datas.grid_max_points);
    #endif
    #if HAS_SHORTCUTS
      temp_val.shortcutsmode = HMI_datas.shortcuts_ui;
      temp_val.activ_shortcut = 0; 
      temp_val.old_sel_shortcut = 4;
      temp_val.flagdir = false;
      LOOP_L_N(i, NB_Shortcuts) {
        temp_val.shortcut[i] = HMI_datas.shortcut_[i];
      }
      Set_Current_Shortcuts();
    #endif

    #if ENABLED(FWRETRACT)
      temp_val.auto_fw_retract = fwretract.autoretract_enabled;
    #endif

    #if HAS_LEVELING && HAS_LEVELING_HEAT
      temp_val.LevelingTempmode = HMI_datas.TempmodeLeveling;
    #endif

    #if ENABLED(HOST_ACTION_COMMANDS, HAS_HOSTACTION_MENUS)
      Decode_String(HMI_datas.host_action_label_1, action1);
      Decode_String(HMI_datas.host_action_label_2, action2);
      Decode_String(HMI_datas.host_action_label_3, action3);
    #endif

    #if ENABLED(CPU_STM32G0B1)
      temp_val.refreshtimevalue = HMI_datas.refreshDtime;
    #endif

    Redraw_Screen();
    #if ENABLED(POWER_LOSS_RECOVERY)
      static bool init = true;
      if (init) {
        init = false;
        queue.inject(F("M1000 S"));
      }
    #endif
  }

  void CrealityDWINClass::Reset_Settings() {
    #if HAS_EXTRUDERS
      planner.set_flow(0, 100);
    #endif

    HMI_datas.time_format_textual = false;
    HMI_datas.fan_percent = false;
    HMI_datas.rev_encoder_dir = false;
    HMI_datas.pack_red = TERN(PACK_ICON_RED, true, false);
    TERN_(HAS_LEV_BLINKING, HMI_datas.lev_blinking = false);
    TERN_(HAS_SHOW_REAL_POS_MENU, HMI_datas.show_real_pos = true);

    #if ENABLED(BLTOUCH_VALIDATE_ON_HOMING)
      HMI_datas.bltouch_validate_ui = true;
    #endif

    #if ENABLED(ADAPTIVE_STEP_SMOOTHING)
      HMI_datas.ass = true;
    #endif

    temp_val.flag_stop_print = false;
    HMI_datas.reprint_on = false;
    TERN_(AUTO_BED_LEVELING_UBL, HMI_datas.tilt_grid_size = 0);
    HMI_datas.corner_pos = 325;

    #if ENABLED(DWIN_CREALITY_LCD_CUSTOM_ICONS)
      HMI_datas.main_icon = 0;
      temp_val.iconmain = 0;
    #endif

    #if ENABLED(CPU_STM32G0B1)
      temp_val.refreshtimevalue = 30;
      HMI_datas.refreshDtime = temp_val.refreshtimevalue;
    #endif

    #if HAS_AUTOTEMP_MENU
      temp_val.autotemp_enable = false;
      HMI_datas.enable_autotemp = temp_val.autotemp_enable;
    #endif

    #if HAS_HOTEND && BOTH(MPC_AUTOTUNE, MPC_AUTOTUNE_MENU)
      HMI_datas.MPCtargetmode = 0;
      temp_val.MPC_targetmode = 0;
      temp_val.MPCtargettemp = 200;
      #if ENABLED(MPC_EDIT_MENU)
        HMI_datas.filheatcapacity = 1;
        temp_val.filheatcap = 1;
      #endif
    #endif

    HMI_datas.cursor_color = TERN0(Ext_Config_JyersUI, Def_cursor_color);
    HMI_datas.menu_split_line = TERN0(Ext_Config_JyersUI, Def_menu_split_line);
    HMI_datas.items_menu_text = TERN0(Ext_Config_JyersUI, Def_items_menu_text);
    HMI_datas.icons_menu_text = TERN0(Ext_Config_JyersUI, Def_icons_menu_text);
    #if !HAS_TJC_DISPLAY
      HMI_datas.background = TERN0(Ext_Config_JyersUI, Def_background);
    #else
      HMI_datas.background = 0;
    #endif
    HMI_datas.menu_top_bg = TERN0(Ext_Config_JyersUI, Def_menu_top_bg);
    HMI_datas.menu_top_txt = TERN0(Ext_Config_JyersUI, Def_menu_top_txt);
    HMI_datas.select_txt = TERN0(Ext_Config_JyersUI, Def_select_txt);
    HMI_datas.select_bg = TERN0(Ext_Config_JyersUI, Def_select_bg);
    HMI_datas.highlight_box = TERN0(Ext_Config_JyersUI, Def_highlight_box);
    HMI_datas.popup_highlight = TERN0(Ext_Config_JyersUI, Def_popup_highlight);
    HMI_datas.popup_txt = TERN0(Ext_Config_JyersUI, Def_popup_txt);
    HMI_datas.popup_bg = TERN0(Ext_Config_JyersUI, Def_popup_bg);
    HMI_datas.ico_confirm_txt = TERN0(Ext_Config_JyersUI, Def_ico_confirm_txt);
    HMI_datas.ico_confirm_bg = TERN0(Ext_Config_JyersUI, Def_ico_confirm_bg);
    HMI_datas.ico_cancel_txt = TERN0(Ext_Config_JyersUI, Def_ico_cancel_txt);
    HMI_datas.ico_cancel_bg = TERN0(Ext_Config_JyersUI, Def_ico_cancel_bg);
    HMI_datas.ico_continue_txt = TERN0(Ext_Config_JyersUI, Def_ico_continue_txt);
    HMI_datas.ico_continue_bg = TERN0(Ext_Config_JyersUI, Def_ico_continue_bg);
    HMI_datas.print_screen_txt = TERN0(Ext_Config_JyersUI, Def_print_screen_txt);
    HMI_datas.print_filename = TERN0(Ext_Config_JyersUI, Def_print_filename);
    HMI_datas.progress_bar = TERN0(Ext_Config_JyersUI, Def_progress_bar);
    HMI_datas.progress_percent = TERN0(Ext_Config_JyersUI, Def_progress_percent);
    HMI_datas.remain_time = TERN0(Ext_Config_JyersUI, Def_remain_time);
    HMI_datas.elapsed_time = TERN0(Ext_Config_JyersUI, Def_elapsed_time);
    HMI_datas.status_bar_text = TERN0(Ext_Config_JyersUI, Def_status_bar_text);
    HMI_datas.status_area_text = TERN0(Ext_Config_JyersUI, Def_status_area_text);
    HMI_datas.status_area_percent = TERN0(Ext_Config_JyersUI, Def_status_area_percent);
    HMI_datas.coordinates_text = TERN0(Ext_Config_JyersUI, Def_coordinates_text);
    HMI_datas.coordinates_split_line = TERN0(Ext_Config_JyersUI, Def_coordinates_split_line);

    #if ENABLED(HOST_ACTION_COMMANDS, HAS_HOSTACTION_MENUS)
      HMI_datas.host_action_label_1 = 0;
      HMI_datas.host_action_label_2 = 0;
      HMI_datas.host_action_label_3 = 0;
      action1[0] = action2[0] = action3[0] = '-';
    #endif

    TERN_(AUTO_BED_LEVELING_UBL, BedLevelTools.tilt_grid = HMI_datas.tilt_grid_size + 1);
    temp_val.corner_pos = HMI_datas.corner_pos / 10.0f;
    TERN_(SOUND_MENU_ITEM, ui.sound_on = true);
    
    #if ENABLED(HAS_BED_PROBE, AUTO_BED_LEVELING_UBL) && !HAS_NO_AUTOTILT_AFTERNPRINT
      temp_val.NPrinted = 0;
    #endif

    #if ALL(SDSUPPORT, SDCARD_SORT_ALPHA, SDSORT_GCODE)
      HMI_datas.sdsort_alpha = true;
    #endif

    #if BOTH(LED_CONTROL_MENU, HAS_COLOR_LEDS)
      leds.setup();
      #if ENABLED(LED_COLOR_PRESETS)
        leds.set_default();
        ApplyLEDColor();
      #endif
      HMI_datas.Led_Color = Def_Leds_Color;
      leds.set_color(
            (HMI_datas.Led_Color >> 16) & 0xFF,
            (HMI_datas.Led_Color >>  8) & 0xFF,
            (HMI_datas.Led_Color >>  0) & 0xFF
            OPTARG(HAS_WHITE_LED, (HMI_datas.Led_Color >> 24) & 0xFF)
          );
    #endif

    #if HAS_FILAMENT_SENSOR
      Get_Rsensormode(runout.mode[0]);
    #endif

    #if EXTJYERSUI && HAS_MESH
      if (HMI_datas.grid_max_points == 0) HMI_datas.grid_max_points = DEF_GRID_MAX_POINTS;
      _GridxGrid = Get_GridxGrid(DEF_GRID_MAX_POINTS);
    #endif

    #if HAS_SHORTCUTS
      #ifdef Def_Mode_Shortcut
        temp_val.shortcutsmode = Def_Mode_Shortcut;
      #else
        temp_val.shortcutsmode = false;
      #endif
      HMI_datas.shortcuts_ui = temp_val.shortcutsmode;
      temp_val.activ_shortcut = 0; 
      temp_val.old_sel_shortcut = 4;
      temp_val.flagdir = false;
      #define _Def_Shortcut(V) \
        HMI_datas.shortcut_[V] = TERN(Ext_Config_JyersUI, Def_Shortcut_##V, 0);
      REPEAT(NB_Shortcuts, _Def_Shortcut)
      LOOP_L_N(i, NB_Shortcuts) {
        temp_val.shortcut[i] = HMI_datas.shortcut_[i];
      }
      Set_Current_Shortcuts();
    #endif

    #if ENABLED(FWRETRACT)
      temp_val.auto_fw_retract = fwretract.autoretract_enabled;
    #endif

    #if ENABLED(BAUD_RATE_GCODE)
      HMI_datas.baudratemode = (BAUDRATE == 115200);
      SetBaudRate();
    #endif

    #if HAS_LEVELING_HEAT
      HMI_datas.ena_LevelingTemp_hotend = true;
      HMI_datas.ena_LevelingTemp_bed = true;
      HMI_datas.TempmodeLeveling = 0;
      HMI_datas.LevelingTemp_hotend = TERN(PREHEAT_BEFORE_PROBING, PROBING_NOZZLE_TEMP, LEVELING_NOZZLE_TEMP);
      HMI_datas.LevelingTemp_bed = TERN(PREHEAT_BEFORE_PROBING, PROBING_BED_TEMP, LEVELING_BED_TEMP);
    #endif

    #if EXTJYERSUI   
      HMI_datas.invert_dir_extruder = INVERT_E0_DIR;
      DWIN_Invert_Extruder();
      #if ENABLED(NOZZLE_PARK_FEATURE)
        HMI_datas.Park_point = DEF_NOZZLE_PARK_POINT;
      #endif
      #if HAS_AUTO_FAN_0
        HMI_datas.extruder_auto_fan_temperature = DEF_EXTRUDER_AUTO_FAN_TEMPERATURE;
      #endif
      #if HAS_PHYSICAL_MENUS
        HMI_datas.x_bed_size = DEF_X_BED_SIZE;
        HMI_datas.y_bed_size = DEF_Y_BED_SIZE;
        HMI_datas.x_min_pos  = DEF_X_MIN_POS;
        HMI_datas.y_min_pos  = DEF_Y_MIN_POS;
        HMI_datas.x_max_pos  = DEF_X_MAX_POS;
        HMI_datas.y_max_pos  = DEF_Y_MAX_POS;
        HMI_datas.z_max_pos  = DEF_Z_MAX_POS;
      #endif
      #if HAS_MESH
        HMI_datas.probing_margin = DEF_PROBING_MARGIN;
        HMI_datas.mesh_min_x = DEF_MESH_MIN_X;
        HMI_datas.mesh_max_x = DEF_MESH_MAX_X;
        HMI_datas.mesh_min_y = DEF_MESH_MIN_Y;
        HMI_datas.mesh_max_y = DEF_MESH_MAX_Y;
      #endif
      #if HAS_BED_PROBE
        HMI_datas.zprobefeedfast = DEF_Z_PROBE_FEEDRATE_FAST;
        HMI_datas.zprobefeedslow = DEF_Z_PROBE_FEEDRATE_SLOW;
      #endif
      #if ENABLED(ADVANCED_PAUSE_FEATURE)
        HMI_datas.fil_unload_feedrate = DEF_FILAMENT_CHANGE_UNLOAD_FEEDRATE;
        HMI_datas.fil_fast_load_feedrate = DEF_FILAMENT_CHANGE_FAST_LOAD_FEEDRATE;
        #if !HAS_RC_CPU
          HMI_datas.park_nozzle_timeout = DEF_PAUSE_PARK_NOZZLE_TIMEOUT;
        #endif
      #endif

    #endif

    TERN_(HAS_NORMAL_GCODE_PREVIEW, HMI_datas.show_gcode_thumbnails = true);

    #if HAS_MESH
      HMI_datas.leveling_active = planner.leveling_active;
    #endif

    Redraw_Screen();
  }

  void CrealityDWINClass::DWIN_Invert_Extruder() {
    stepper.disable_e_steppers();
    current_position.e = 0;
    sync_plan_position_e();
  }

  #if HAS_FILAMENT_SENSOR
    // Filament Runout temp_val.process
    void CrealityDWINClass::DWIN_Filament_Runout(const uint8_t extruder) { LCD_MESSAGE(MSG_RUNOUT_SENSOR); }

    void CrealityDWINClass::Get_Rsensormode(uint8_t Rsmode) {
      switch (Rsmode) {
           case RM_NONE: temp_val.rsensormode = 0; break; // None 
           case RM_OUT_ON_HIGH: temp_val.rsensormode = 1; break; // mode HIGH
           case RM_OUT_ON_LOW: temp_val.rsensormode = 2; break; // mode LOW
           case RM_RESERVED3: break;
           case RM_RESERVED4: break;
           case RM_RESERVED5: break;
           case RM_RESERVED6: break;
           case RM_MOTION_SENSOR: temp_val.rsensormode = 3; break; // mode MOTION
          }
    }
  #endif

  #if EXTJYERSUI && HAS_MESH
    uint8_t CrealityDWINClass::Get_Grid_Max_XY(uint8_t valuegrid) {
        #if ENABLED(AUTO_BED_LEVELING_BILINEAR)
          return (valuegrid + 3);
        #elif ENABLED(AUTO_BED_LEVELING_UBL)
          return (TERN(HAS_BED_PROBE, valuegrid + 7, valuegrid + 3));
        #endif
    }

    uint8_t CrealityDWINClass::Get_GridxGrid(uint8_t datavalue) {
        #if ENABLED(AUTO_BED_LEVELING_BILINEAR)
          return (datavalue - 3);
        #elif ENABLED(AUTO_BED_LEVELING_UBL)
          return (TERN(HAS_BED_PROBE, datavalue - 7, datavalue - 3));
        #endif
   }

    void CrealityDWINClass::MaxMeshArea() {
      #if HAS_BED_PROBE
        HMI_datas.mesh_min_x = _MAX(PROBING_MARGIN, probe.offset.x);
        HMI_datas.mesh_max_x = _MIN(X_BED_SIZE - PROBING_MARGIN, X_MAX_POS + probe.offset.x);
        HMI_datas.mesh_min_y = _MAX(PROBING_MARGIN, probe.offset.y);
        HMI_datas.mesh_max_y = _MIN(Y_BED_SIZE - PROBING_MARGIN, Y_MAX_POS + probe.offset.y);
      #else
        HMI_datas.mesh_min_x = PROBING_MARGIN;  // PROBING_MARGIN_LEFT;
        HMI_datas.mesh_max_x = X_BED_SIZE - PROBING_MARGIN;  // PROBING_MARGIN_RIGHT;
        HMI_datas.mesh_min_y = PROBING_MARGIN;  // PROBING_MARGIN_FRONT;
        HMI_datas.mesh_max_y = Y_BED_SIZE - PROBING_MARGIN;  // PROBING_MARGIN_BACK;
      #endif
      ApplyMeshLimits();
      Redraw_Menu(false);
    }

    void CrealityDWINClass::CenterMeshArea() {
      float max = HMI_datas.mesh_min_x;
      if (max < X_BED_SIZE - HMI_datas.mesh_max_x) max = X_BED_SIZE - HMI_datas.mesh_max_x;
      if (max < HMI_datas.mesh_min_y) max = HMI_datas.mesh_min_y;
      if (max < Y_BED_SIZE - HMI_datas.mesh_max_y) max = Y_BED_SIZE - HMI_datas.mesh_max_y;
      HMI_datas.mesh_min_x = max;
      HMI_datas.mesh_max_x = X_BED_SIZE - max;
      HMI_datas.mesh_min_y = max;
      HMI_datas.mesh_max_y = Y_BED_SIZE - max;
      ApplyMeshLimits();
      Redraw_Menu(false);
    }

    void CrealityDWINClass::ApplyMeshLimits() {
      set_bed_leveling_enabled(false);
      reset_bed_level();
    }
  #endif
    
  #if HAS_LEVELING
    void CrealityDWINClass::Dis_steppers_and_cooldown() {
      queue.inject(F("M84"));
      thermalManager.cooldown();
    }
  #endif
  
  #if HAS_BED_PROBE
    void CrealityDWINClass::Bed_Tramming_init_P() {
      tram_points_P[0].x = PROBE_X_MIN, tram_points_P[0].y = PROBE_Y_MIN ;
      tram_points_P[1].x = PROBE_X_MIN, tram_points_P[1].y = PROBE_Y_MAX ;
      tram_points_P[2].x = PROBE_X_MAX, tram_points_P[2].y = PROBE_Y_MAX ;
      tram_points_P[3].x = PROBE_X_MAX, tram_points_P[3].y = PROBE_Y_MIN ;
      tram_points_P[4].x = (X_BED_SIZE) / 2.0f - probe.offset.x, tram_points_P[4].y = (Y_BED_SIZE) / 2.0f - probe.offset.y ;
    }

    void CrealityDWINClass::Bed_Tramming_wizard(bool wizard/*=false*/) {
      temp_val.flag_busy = true;
      Bed_Tramming_init_P();
      if (!wizard) Popup_Handler(Level);
      do_z_clearance(Z_HOMING_HEIGHT);
      bed_mesh_t bt_zval = {0};

      if (wizard) { Draw_Title(F("Tramming Bed")); TrammingTool.DrawMeshGrid(); }
      LOOP_S_LE_N(c, 1, mlev_cycles) {
        uint8_t count_loop = 0;
        uint8_t _j;
        temp_val.flag_unreachable = false;
        temp_val.corner_avg = 0.0f;

        //if (wizard) TrammingTool.DrawMeshGrid(2, 2);

        LOOP_L_N(i, 2) {
              LOOP_L_N(j, 2) { 
                _j = (i == 0) ? j : 1 - j;
                bt_zval[i][_j] = probe.probe_at_point(tram_points_P[count_loop].x, tram_points_P[count_loop].y, c == mlev_cycles && i == 1 && _j == 0 ? PROBE_PT_STOW : PROBE_PT_RAISE, 0, false);
                if (isnan(bt_zval[i][_j])) {
                    temp_val.flag_unreachable = true;
                    Update_Status(GET_TEXT(MSG_ZPROBE_UNREACHABLE));
                    temp_val.flag_busy = false; 
                    Redraw_Menu(true, true);
                    return;
                }
                temp_val.corner_avg += bt_zval[i][_j];
                if (wizard && (count_loop == 0)) TrammingTool.DrawMeshGrid();
                count_loop++;
                if (wizard) TrammingTool.DrawMeshPoint(i, _j, bt_zval[i][_j]);
                }
        }
      }
      temp_val.corner_avg /= 4.0f;

      if (!wizard) { temp_val.flag_busy = false; Redraw_Menu(true, true); return; }

      JYERSUI::Draw_CenteredString(140, F("Calculating average"));
      JYERSUI::Draw_CenteredString(160, F("and relative heights"));
      safe_delay(1500);
      
      LOOP_L_N(x, 2) LOOP_L_N(y, 2) bt_zval[x][y] -= temp_val.corner_avg;
      TrammingTool.DrawMesh(bt_zval);
      ui.reset_status();

      if (ABS(TrammingTool.max - TrammingTool.min) < 0.05) {
        JYERSUI::Draw_CenteredString(140,F("Corners leveled"));
        JYERSUI::Draw_CenteredString(160,F("Tolerance achieved!"));
      }
      else {
        uint8_t cp = 0;
        float max = 0;
        FSTR_P plabel;
        bool s = true;
        LOOP_L_N(x, 2) LOOP_L_N(y, 2) {
          const float d = ABS(bt_zval[x][y]);
          if (max < d) {
            s = (bt_zval[x][y] >= 0);
            max = d;
            cp = x + 2 * y;
          }
        }
        switch (cp) {
          case 0b00 : plabel = GET_TEXT_F(MSG_LEVBED_FL); break;
          case 0b01 : plabel = GET_TEXT_F(MSG_LEVBED_FR); break;
          case 0b10 : plabel = GET_TEXT_F(MSG_LEVBED_BL); break;
          case 0b11 : plabel = GET_TEXT_F(MSG_LEVBED_BR); break;
          default   : plabel = F(""); break;
        }
        JYERSUI::Draw_CenteredString(120, F("Corners not leveled"));
        JYERSUI::Draw_CenteredString(140, F("Knob adjustment required"));
        JYERSUI::Draw_CenteredString(s ? Color_Green : Color_Red, 160, s ? F("Down") : F("Up")); 
        JYERSUI::Draw_CenteredString(s ? Color_Green : Color_Red, 180, plabel);
      }
      DWIN_Draw_Rectangle(1, GetColor(HMI_datas.ico_confirm_bg, (HMI_datas.pack_red)? Color_Red : Confirm_Color), 26, 305, 125, 342);
      DWIN_Draw_Rectangle(1, GetColor(HMI_datas.ico_cancel_bg , Cancel_Color), 146, 305, 245, 342);
      DWIN_Draw_String(false, DWIN_FONT_STAT, GetColor(HMI_datas.ico_confirm_txt, Color_White), GetColor(HMI_datas.ico_confirm_bg, (HMI_datas.pack_red)? Color_Red : Confirm_Color), 34, 315, GET_TEXT_F(MSG_BUTTON_CONTINUE));
      DWIN_Draw_String(false, DWIN_FONT_STAT, GetColor(HMI_datas.ico_cancel_txt, Color_White), GetColor(HMI_datas.ico_cancel_bg , Cancel_Color), 165, 315, GET_TEXT_F(MSG_BUTTON_CANCEL));
      temp_val.flag_busy = false; 
      Popup_Select(true);
      DWIN_UpdateLCD();
    }

  #endif

  void CrealityDWINClass::Bed_Tramming_init() {
    tram_points[0].x = temp_val.corner_pos, tram_points[0].y = temp_val.corner_pos ;
    tram_points[1].x = temp_val.corner_pos, tram_points[1].y = (Y_BED_SIZE) - temp_val.corner_pos ;
    tram_points[2].x = (X_BED_SIZE) - temp_val.corner_pos, tram_points[2].y = (Y_BED_SIZE) - temp_val.corner_pos ;
    tram_points[3].x = (X_BED_SIZE) - temp_val.corner_pos, tram_points[3].y = temp_val.corner_pos ;
    tram_points[4].x = (X_BED_SIZE) / 2.0f, tram_points[4].y = (Y_BED_SIZE) / 2.0f ;
  }

  void CrealityDWINClass::Bed_Tramming(uint8_t point, float z_pos, bool pb/*=false*/) {
    Bed_Tramming_init();
    TERN_(HAS_BED_PROBE, Bed_Tramming_init_P());
    Keep_selection();
    Popup_Handler(MoveWait);
    if (pb) {
      #if HAS_BED_PROBE
        sprintf_P(cmd, PSTR("G0 F4000\nG0 Z10\nG0 X%s Y%s"), dtostrf(tram_points_P[point].x, 1, 3, str_1), dtostrf(tram_points_P[point].y, 1, 3, str_2));
        gcode.process_subcommands_now(cmd);
        planner.synchronize();
        Popup_Handler(ManualProbing);
      #endif
    }
    else {
      sprintf_P(cmd, PSTR("G0 F4000\nG0 Z10\nG0 X%s Y%s\nG0 F300 Z%s"), dtostrf(tram_points[point].x, 1, 3, str_1), dtostrf(tram_points[point].y, 1, 3, str_2), dtostrf(z_pos, 1, 3, str_3));
      gcode.process_subcommands_now(cmd);
      planner.synchronize();
      Redraw_Menu(true, true);
    }  
  }


  #if ENABLED(HAS_BED_PROBE, AUTO_BED_LEVELING_UBL)

    void CrealityDWINClass::Autotilt_Common() {
      #if HAS_LEVELING_HEAT
        HeatBeforeLeveling();
      #endif
        Update_Status("");
        Popup_Handler(Home);
        gcode.home_all_axes(true);
        Popup_Handler(Level);
        if (BedLevelTools.tilt_grid > 1) {
          sprintf_P(cmd, PSTR("G29 J%i"), BedLevelTools.tilt_grid);
          gcode.process_subcommands_now(cmd);
        }
        else gcode.process_subcommands_now(F("G29 J"));
        planner.synchronize();
    }

    #if !HAS_NO_AUTOTILT_AFTERNPRINT
      void CrealityDWINClass::Autotilt_AfterNPrint(uint16_t NPrints) {
        printStatistics prdatas = print_job_timer.getStats();
        uint16_t _NPrints = prdatas.totalPrints;
        if ((NPrints > 0) && ( _NPrints >= NPrints)) {
            if (bedlevel.storage_slot < 0) { Popup_Handler(MeshSlot); return; }
            else {
              Autotilt_Common();
              #if ENABLED(EEPROM_SETTINGS)
                gcode.process_subcommands_now(F("G29 S"));
                AudioFeedback();  
                planner.synchronize();
              #endif
            }
          }  
        }
    #endif
  #endif

  #if ENABLED(BAUD_RATE_GCODE)
    void CrealityDWINClass::SetBaudRate() {
      gcode.process_subcommands_now(HMI_datas.baudratemode ? F("M575 P0 B115") : F("M575 P0 B250"));
    }
  #endif

  void CrealityDWINClass::RebootPrinter() {                   
    wait_for_heatup = wait_for_user = false;    // Stop waiting for heating/user
    thermalManager.disable_all_heaters();
    planner.finish_and_disable();
    DWIN_RebootScreen();
    hal.reboot();
    #if ENABLED(BAUD_RATE_GCODE)
      SetBaudRate();
    #endif
  }

  void CrealityDWINClass::DWIN_RebootScreen() {
    DWIN_Frame_Clear(Color_Bg_Black);
    DWIN_JPG_ShowAndCache(0);
    JYERSUI::Draw_CenteredString(TERN(AQUILA, Color_Bg_Black, Color_White), 210, GET_TEXT_F(MSG_PLEASE_WAIT_REBOOT));
    for (uint16_t t = 0; t <= 100; t += 2) {
      #ifdef BOOTPERSO
        DRAW_IconWB(ICON, ICON_Bar, 15, 170);
        DWIN_Draw_Rectangle(1, Color_Bg_Black, 15 + t * 242 / 100, 170, 257, 190);
      #else
        DRAW_IconWB(ICON, ICON_Bar, 15, 260);
        DWIN_Draw_Rectangle(1, Color_Bg_Black, 15 + t * 242 / 100, 260, 257, 280);
      #endif
    }
    DWIN_UpdateLCD();
    delay(500);
  }
  
  #if HAS_ES_DIAG
	  void CrealityDWINClass::DWIN_Init_diag_endstops() {
	    temp_val.last_process = temp_val.process;
	    temp_val.last_selection = temp_val.selection;
	    temp_val.process = Confirm;
	    temp_val.popup = endsdiag;
	  }
  #endif

  #if HAS_HOTEND
      void CrealityDWINClass::Check_E_Move_Mintemp(const bool calib/*=false*/) {
        if (thermalManager.temp_hotend[0].target < thermalManager.extrude_min_temp) {
          Popup_Handler(ETemp);
        }
        else {
          if (thermalManager.temp_hotend[0].is_below_target(-2)) {
            Popup_Handler(Heating);
            Update_Status(GET_TEXT(MSG_HEATING));
            thermalManager.wait_for_hotend(0);
            Update_Status("");
            Redraw_Menu();
          }
          current_position.e = 0;
          sync_plan_position();
          if (!calib)
            Modify_Value(current_position.e, -1000, 1000, 10);
          #if HAS_E_CALIBRATION
            else Draw_Menu(MoveE100);
          #endif
        }
      }

    #if HAS_E_CALIBRATION
      void CrealityDWINClass::DWIN_Init_EC_Stage1() {
        temp_val.last_process = temp_val.process;
        temp_val.last_selection = temp_val.selection;
        temp_val.old_last_selection = temp_val.selection;
        temp_val.old_process = temp_val.process;
        temp_val.process = Popup;
        temp_val.popup = EC_Stage1;
        temp_val.popup_EC = temp_val.popup;
        temp_val.selection = 0;
      }
    
      void CrealityDWINClass::DWIN_Init_EC_Stage2() {
        temp_val.process = Wait;
        wait_for_user = false;
        temp_val.popup = EC_Stage2;
        temp_val.popup_EC = temp_val.popup;
      }
    
      void CrealityDWINClass::DWIN_Init_EC_Stage3() {
        temp_val.process = eCalibration;
        temp_val.popup = EC_Stage3;
        temp_val.popup_EC = temp_val.popup;
      }
    
      void CrealityDWINClass::DWIN_Init_EC_Stage4() {
        temp_val.process = Confirm;
        temp_val.popup = EC_Stage4;
        temp_val.popup_EC = temp_val.popup;
      }
    #endif
  #endif

 

  #if HAS_GRAPHICAL_LEVELMANUAL && EXTJYERSUI
    void CrealityDWINClass::Draw_EditMesh(const bool init/*=false*/) {
      Clear_Screen(3);
      Draw_Title(F("View Edit Mesh"));
      temp_val.last_process = Menu;
      temp_val.last_selection = temp_val.selection;
      temp_val.process = Selectvalue_Mesh;
      const uint16_t color_bg = GetColor(HMI_datas.popup_bg, Color_Bg_Window);
      const uint16_t color_popup_highlight = GetColor(HMI_datas.popup_highlight, Color_White);
      DWIN_Draw_Rectangle(1, color_bg, 0, 10 + TITLE_HEIGHT, DWIN_WIDTH - 1, 351);
      DWIN_Draw_Rectangle(0, color_popup_highlight, 0, 10 + TITLE_HEIGHT, DWIN_WIDTH - 1, 351);
      ExtJyersui.Draw_EditGrid(init);
      JYERSUI::last_backcolor = JYERSUI::backcolor;
      JYERSUI::backcolor = color_bg;
      Update_Status("Turn knob to select - Click to edit value");
    }

    void CrealityDWINClass::Selectvalue_Control() {
      temp_val.evenvalue = (GRID_MAX_POINTS_X % 2 == 0);
      EncoderState encoder_diffState = Encoder_ReceiveAnalyze();
      if (encoder_diffState == ENCODER_DIFF_NO) return;
        BedLevelTools.onEncoderS(encoder_diffState);
    }
  
    void CrealityDWINClass::Editvalue_Control() {
      EncoderState encoder_diffState = Encoder_ReceiveAnalyze();
      if (encoder_diffState == ENCODER_DIFF_NO) return;
        BedLevelTools.onEncoderV(encoder_diffState);
    }
#endif

  #if HAS_CANCEL_OBJECTS
    uint8_t CrealityDWINClass::Get_nb_canceled_objects() {
      temp_val.count_canceled = 0;
      for (int i = 0; i < cancelable.object_count; i++)
        if (TEST(cancelable.canceled, i)) temp_val.count_canceled += 1;
      return temp_val.count_canceled;
    }

    void CrealityDWINClass::DWIN_Cancel_object(int8_t object) {
      if (Get_nb_canceled_objects() < (cancelable.object_count - 1)) {
        if (object != -1) {
          if (!cancelable.is_canceled(object)) {
            cancelable.cancel_object(object);
            ui.status_printf(0, F("Object %i canceled"), object);
          }
          else { ui.status_printf(0, F("Object %i already cancelled !"), object); }
        }
        else ui.set_status("Object not cancelable !");
      }
      else ui.set_status("You must have at least 1 object !");
      Redraw_Menu(false);
    }
  #endif


  //=============================================================================
  // Extended G-CODES Cn
  //=============================================================================

  void CrealityDWINClass::DWIN_CError() {
    SERIAL_ECHO_START();
    Update_Status("This G-Code or parameter is not implemented in firmware");
    SERIAL_ECHOLNPGM("This G-Code or parameter is not implemented in firmware");
  }

  // Cancel a Wait for User without an Emergecy Parser
  void CrealityDWINClass::DWIN_C108() { 
    #if DEBUG_DWIN
      SERIAL_ECHOLNPGM(F("Wait for user was "), wait_for_user);
      SERIAL_ECHOLNPGM(F("Process was "), temp_val.process);
    #endif
    wait_for_user = false;
    AudioFeedback();
  }

  // lock/unlock screen
  #if HAS_SCREENLOCK
    void CrealityDWINClass::DWIN_C510() {
      if (!parser.seen_any()) return CrealityDWIN.DWIN_CError();
      if (parser.seenval('U') && parser.value_int()) DWIN_ScreenUnLock();
      else DWIN_ScreenLock();
    }
  #endif

  #if HAS_LEVELING && HAS_LEVELING_HEAT && !HAS_NO_DWIN_C109
    void CrealityDWINClass::DWIN_C109() {
      if (!parser.seen("BHP")) return DWIN_C109_report();
        if (parser.seenval('H')) {
          const celsius_t hotendtemp_value = parser.value_int();
          if (!WITHIN(hotendtemp_value, MIN_E_TEMP, MAX_E_TEMP)) return CrealityDWIN.DWIN_CError();
          else HMI_datas.LevelingTemp_hotend = hotendtemp_value;
        }
        if (parser.seenval('B')) {
          const celsius_t bedtempvalue = parser.value_int();
          if (!WITHIN(bedtempvalue, MIN_BED_TEMP, MAX_BED_TEMP)) return CrealityDWIN.DWIN_CError();
          else HMI_datas.LevelingTemp_bed = bedtempvalue;
        }
        if (parser.seenval('P')) {
          const uint8_t temp_mode = parser.value_int();
          if (WITHIN(temp_mode, 0, 3)) { temp_val.LevelingTempmode = temp_mode, HMI_datas.TempmodeLeveling = temp_mode; }
          else return CrealityDWIN.DWIN_CError();
          HMI_datas.ena_LevelingTemp_hotend = false;
          HMI_datas.ena_LevelingTemp_bed = false;
          switch (temp_mode) {
            case 0: HMI_datas.ena_LevelingTemp_hotend = true; HMI_datas.ena_LevelingTemp_bed = true; break;
            case 1: HMI_datas.ena_LevelingTemp_hotend = true ; break;
            case 2: HMI_datas.ena_LevelingTemp_bed = true; break;
            default: break;
            }
        }  
    }
    
    void CrealityDWINClass::DWIN_C109_report() {
        SERIAL_ECHOLNPGM(
          "  C109"
          " H", HMI_datas.LevelingTemp_hotend
        , " D", HMI_datas.LevelingTemp_bed
        , " P", HMI_datas.TempmodeLeveling
        );
    }

  #endif

  #if DEBUG_DWIN
    void CrealityDWINClass::DWIN_C997() {
      #if ENABLED(POWER_LOSS_RECOVERY)
        if (temp_val.printing && recovery.enabled) {
          planner.synchronize();
          recovery.save(true);
        }
      #endif
      DWIN_RebootScreen();
      Update_Status("Simulating a printer freeze");
      SERIAL_ECHOLNPGM("Simulating a printer freeze");
      while (1) {};
    }
  #endif

  // Special Creality DWIN GCodes
  void CrealityDWINClass::DWIN_Gcode(const int16_t codenum) {
    switch(codenum) {
      case 108: DWIN_C108(); break;           // Cancel a Wait for User without an Emergecy Parser
      #if HAS_SCREENLOCK
        case 510: DWIN_C510(); break;           // lock screen
      #endif
      #if DEBUG_DWIN
        case 997: DWIN_C997(); break;         // Simulate a printer freeze
      #endif
      #if HAS_LEVELING && HAS_LEVELING_HEAT && !HAS_NO_DWIN_C109
        case 109: DWIN_C109(); break;         // Set mode and Preheat Before Probing/Leveling
      #endif
      #if EXTJYERSUI
        #if HAS_MESH
          case 29: ExtJyersui.C29(); break;       // Set amount of grid points of the mesh leveling
          case 852: ExtJyersui.C852(); break;     // Set probing margin
        #endif
        #if HAS_PHYSICAL_MENUS
          case 100: ExtJyersui.C100(); break;       // Change Physical minimums
          case 101: ExtJyersui.C101(); break;       // Change Physical maximums
          case 102: ExtJyersui.C102(); break;       // Change Bed size
        #endif
        #if ENABLED(NOZZLE_PARK_FEATURE)
          case 125: ExtJyersui.C125(); break;      // Set park position
        #endif
        case 562: ExtJyersui.C562(); break;        // Invert Extruder
        #if HAS_BED_PROBE
          case 851: ExtJyersui.C851(); break;      // Set z feed rate of the probe mesh leveling
        #endif
      #endif
      default: DWIN_CError(); break;
    }
  }

  //=============================================================================

  void MarlinUI::init_lcd() {
    #if BOTH(CPU_STM32G0B1, AUTO_BED_LEVELING_UBL)
      safe_delay(1600);
    #else
      delay(800);
    #endif
    SERIAL_ECHOPGM("\nDWIN handshake ");
    if (DWIN_Handshake()) SERIAL_ECHOLNPGM("ok."); else SERIAL_ECHOLNPGM("error.");
    DWIN_Frame_SetDir(1); // Orientation 90°
    DWIN_UpdateLCD();     // Show bootscreen (first image)
    Encoder_Configuration();
    for (uint16_t t = 0; t <= 100; t += 2) {
      #ifdef BOOTPERSO
        DRAW_IconWB(ICON, ICON_Bar, 15, 170);
        DWIN_Draw_Rectangle(1, Color_Bg_Black, 15 + t * 242 / 100, 170, 257, 190);
      #else
        DRAW_IconWB(ICON, ICON_Bar, 15, 260);
        DWIN_Draw_Rectangle(1, Color_Bg_Black, 15 + t * 242 / 100, 260, 257, 280);
      #endif
      DWIN_UpdateLCD();
      delay(20);
    }
    JYERSUI::cursor.x = 0;
    JYERSUI::cursor.y = 0;
    JYERSUI::pencolor = Color_White;
    JYERSUI::textcolor = CrealityDWIN.GetColor(HMI_datas.items_menu_text, Color_White);
    JYERSUI::backcolor = TERN(HAS_TJC_DISPLAY, Color_Bg_Black, CrealityDWIN.GetColor(HMI_datas.background, Color_Bg_Black));
    JYERSUI::buttoncolor = RGB( 0, 23, 16);
    JYERSUI::font = font8x16;
    DWIN_JPG_CacheTo1(Language_English);
    TERN(SHOW_BOOTSCREEN,,DWIN_Frame_Clear(Color_Bg_Black));
    DWIN_JPG_ShowAndCache(3);
    #if BOTH(CPU_STM32G0B1, AUTO_BED_LEVELING_UBL)
      safe_delay(500);
      DWIN_UpdateLCD();
    #else
      DWIN_UpdateLCD();
    #endif
    CrealityDWIN.Redraw_Screen();
  }


  #if ENABLED(ADVANCED_PAUSE_FEATURE)
    void MarlinUI::pause_show_message(const PauseMessage message, const PauseMode mode/*=PAUSE_MODE_SAME*/, const uint8_t extruder/*=active_extruder*/) {
      switch (message) {
        case PAUSE_MESSAGE_INSERT:  CrealityDWIN.Confirm_Handler(FilInsert);  break;
        case PAUSE_MESSAGE_PURGE: break;
        case PAUSE_MESSAGE_OPTION: 
          pause_menu_response = PAUSE_RESPONSE_WAIT_FOR;
          CrealityDWIN.Popup_Handler(PurgeMore);
          break;
        case PAUSE_MESSAGE_HEAT:    CrealityDWIN.Confirm_Handler(HeaterTime); break;
        case PAUSE_MESSAGE_WAITING:
          if (temp_val.flag_tune_submenu) { CrealityDWIN.Draw_Menu(temp_val.active_menu); break; }
          else if (temp_val.flag_tune_menu) { CrealityDWIN.Draw_Menu(Tune); break; }
              else CrealityDWIN.Draw_Print_Screen();
        break;
        default: break;
      }
    }
  #endif

#endif // DWIN_CREALITY_LCD_JYERSUI
