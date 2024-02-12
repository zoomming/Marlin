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
#pragma once

#include "../../../inc/MarlinConfigPre.h"
#include "../common/encoder.h"


class BedLevelToolsClass {
public:
  static bool viewer_asymmetric_range;
  static bool viewer_print_value;
  static bool goto_mesh_value;
  static uint8_t mesh_x;
  static uint8_t mesh_y;
  #if HAS_GRAPHICAL_LEVELMANUAL
    static bool LevelManual_flag;
  #endif
  
  #if ENABLED(AUTO_BED_LEVELING_UBL)
    static uint8_t tilt_grid;
    static void manual_value_update(bool undefined=false);
    static bool create_plane_from_mesh();
  #else
    static void manual_value_update();
  #endif
  static void manual_mesh_move(const bool zmove=false);
  static void next_point(const bool graph_edit = false);
  static void prev_point(const bool graph_edit = false);
  static float get_max_value();
  static float get_min_value();
  static void Init_Grid_Mesh(uint16_t _padding_y_top =  86 /*40 + 53 - 7*/);
  static void Cell_Grid_Mesh(const uint8_t px, const uint8_t py);
  #if HAS_LIVEMESH
    static void Draw_Grid_Mesh();
  #endif
  #if HAS_LIVEMESH || HAS_GRAPHICAL_LEVELMANUAL
    static void DrawSelectPoint(const uint8_t x, const uint8_t y, const bool startprobing = false, const bool edition = false);
  #endif
  #if HAS_GRAPHICAL_LEVELMANUAL
    static void DrawMeshPoint(const uint8_t x, const uint8_t y, const float v, const bool livemesh = false, const bool edition = false);
  #else
    static void DrawMeshPoint(const uint8_t x, const uint8_t y, const float z, const bool livemesh = false);
  #endif
  #if HAS_GRAPHICAL_LEVELMANUAL
    static void Draw_Bed_Mesh(const bool editpoint = false, const bool start = false);
    static void value_up();
    static void value_down();
    static void update_popup_editvalue();
    static void popup_editvalue();
    static void onEncoderS(EncoderState encoder_diffState);
    static void onEncoderV(EncoderState encoder_diffState);
  #else
    static void Draw_Bed_Mesh();
  #endif
  static void Set_Mesh_Viewer_Status();
};

extern BedLevelToolsClass BedLevelTools;
