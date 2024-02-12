/**
 * ExtJyersUI extenions
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
 * For commercial applications additional licences can be requested
 */

/**
 * Modify and enhanced by Tititopher68-dev (Christophe L.) since 28/11/2021 !
 * build v3.0.0a was public at 28/05/2022 !
 */

#pragma once

#include "../../../inc/MarlinConfigPre.h"
#include "../../../core/types.h"


#ifndef LOW
  #define LOW  0x0
#endif
#ifndef HIGH
  #define HIGH 0x1
#endif

#if EXTJYERSUI

  #define X_BED_MIN 150
  #define Y_BED_MIN 150

  #if HAS_PHYSICAL_MENUS
    constexpr int16_t DEF_X_BED_SIZE = X_BED_SIZE;
    constexpr int16_t DEF_Y_BED_SIZE = Y_BED_SIZE;
    constexpr int16_t DEF_X_MIN_POS = X_MIN_POS;
    constexpr int16_t DEF_Y_MIN_POS = Y_MIN_POS;
    constexpr int16_t DEF_X_MAX_POS = X_MAX_POS;
    constexpr int16_t DEF_Y_MAX_POS = Y_MAX_POS;
    constexpr int16_t DEF_Z_MAX_POS = Z_MAX_POS;
  #endif

  #if HAS_MAG_MOUNTED_PROBE
    constexpr int16_t DEF_MAG_MOUNTED_POSX = X_BED_SIZE + 15;
    constexpr int16_t DEF_MAG_MOUNTED_POSY = (Y_BED_SIZE / 2);
    constexpr int16_t DEF_MAG_MOUNTED_POSZ = 20;
  #endif

  #if ENABLED(NOZZLE_PARK_FEATURE)
    //#define DEF_NOZZLE_PARK_POINT {240, 220, 20}
    constexpr xyz_int_t DEF_NOZZLE_PARK_POINT = NOZZLE_PARK_POINT;
    #define MIN_PARK_POINT_Z 10
  #endif

  #if (E0_AUTO_FAN_PIN != -1)
      constexpr uint8_t DEF_EXTRUDER_AUTO_FAN_TEMPERATURE = EXTRUDER_AUTO_FAN_TEMPERATURE;
  #endif

  #if HAS_MESH
    #if EITHER(AUTO_BED_LEVELING_BILINEAR, AUTO_BED_LEVELING_UBL)
      #if HAS_BED_PROBE
          #define GRID_LIMIT 15
      #else
        #define GRID_LIMIT 5
      #endif
      #if (GRID_MAX_POINTS_X) > (GRID_LIMIT)
        constexpr int8_t DEF_GRID_MAX_POINTS = GRID_LIMIT;
      #else
        constexpr int8_t DEF_GRID_MAX_POINTS = GRID_MAX_POINTS_X;
      #endif
    #endif
  #endif


  #if !EITHER(MESH_BED_LEVELING, AUTO_BED_LEVELING_UBL)       
    #define MESH_INSET 15
    #define MESH_MIN_X MESH_INSET
    #define MESH_MIN_Y MESH_INSET
    #define MESH_MAX_X X_BED_SIZE - (MESH_INSET)
    #define MESH_MAX_Y Y_BED_SIZE - (MESH_INSET)
  #endif

  #if HAS_MESH
    constexpr int16_t DEF_MESH_MIN_X = MESH_MIN_X;
    constexpr int16_t DEF_MESH_MAX_X = MESH_MAX_X;
    constexpr int16_t DEF_MESH_MIN_Y = MESH_MIN_Y;
    constexpr int16_t DEF_MESH_MAX_Y = MESH_MAX_Y;
    #define MIN_MESH_INSET 5
    #define MAX_MESH_INSET X_BED_SIZE

    #if HAS_MAG_MOUNTED_PROBE
      constexpr int16_t DEF_PROBING_MARGIN = 15;
    #else
      constexpr int16_t DEF_PROBING_MARGIN = PROBING_MARGIN;
    #endif
    #define MIN_PROBE_MARGIN 5
    #define MAX_PROBE_MARGIN 60
  #endif

  #if HAS_BED_PROBE
    #define MIN_Z_PROBE_FEEDRATE 60
    #define MAX_Z_PROBE_FEEDRATE 1200
    constexpr int16_t DEF_Z_PROBE_FEEDRATE_FAST = Z_PROBE_FEEDRATE_FAST;
    constexpr int16_t DEF_Z_PROBE_FEEDRATE_SLOW = (Z_PROBE_FEEDRATE_FAST / 2);
  #endif

  #if ENABLED(ADVANCED_PAUSE_FEATURE)
    #define MIN_FIL_CHANGE_FEEDRATE 1
    #define MAX_FIL_CHANGE_FEEDRATE 60
    constexpr int8_t DEF_FILAMENT_CHANGE_UNLOAD_FEEDRATE = FILAMENT_CHANGE_UNLOAD_FEEDRATE;
    constexpr int8_t DEF_FILAMENT_CHANGE_FAST_LOAD_FEEDRATE = FILAMENT_CHANGE_FAST_LOAD_FEEDRATE;
    #if !HAS_RC_CPU
      constexpr int8_t DEF_PAUSE_PARK_NOZZLE_TIMEOUT = PAUSE_PARK_NOZZLE_TIMEOUT;
    #endif
  #endif

  #define DEF_INVERT_E0_DIR false

  

  // Class
  class ExtJyersuiClass {
  public:
  
    #if ENABLED(AUTO_BED_LEVELING_BILINEAR)
      static void abl_extrapolate();
    #endif

    #if HAS_MESH
      static void C29();
      static void C29_report(const bool forReplay=true);
      static void C852();
      static void C852_report(const bool forReplay=true);
    #endif
    #if HAS_PHYSICAL_MENUS
      static void UpdateAxis(const AxisEnum axis);
      static void ApplyPhySet();
      static void C100();
      static void C100_report(const bool forReplay=true);
      static void C101();
      static void C101_report(const bool forReplay=true);
      static void C102();
      static void C102_report(const bool forReplay=true);
    #endif
    #if ENABLED(NOZZLE_PARK_FEATURE)
      static void C125();
      static void C125_report(const bool forReplay=true);
    #endif
      static void C562();
      static void C562_report(const bool forReplay=true);
    #if HAS_BED_PROBE
      static void C851();
      static void C851_report(const bool forReplay=true);
    #endif
    
    #if ENABLED(AUTO_BED_LEVELING_BILINEAR)
      static float getZvalues(const uint8_t sx, const uint8_t x, const uint8_t y, const float *values);
    #endif

    #if HAS_LIVEMESH
      static void Draw_LiveMeshGrid();
      static void MeshUpdate(const uint8_t x, const uint8_t y, const float z);
      static void MeshSelectPoint(const uint8_t x, const uint8_t y, const bool flag);
    #endif

    #if HAS_GRAPHICAL_LEVELMANUAL && HAS_LEVELING
      static void Draw_EditGrid(const bool start = false);
    #endif

  };

  extern ExtJyersuiClass ExtJyersui;

#endif


