/**
 * JyersUI extensions
 * Author: tititopher68-dev (Christophe LEVEQUE LChristophe68)
 * Version: 1.0
 * Date: 2022/02/12
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

#include "../../../inc/MarlinConfig.h"
#include "../../../core/types.h"


#if EXTJYERSUI

#include "dwin.h"
#include "../../../gcode/gcode.h"

#include "extjyersui.h"
#include "../../../module/motion.h"
#include "../../../module/endstops.h"

#if HAS_LEVELING
  #include "bedlevel_tools.h"
#endif


#if HAS_BED_PROBE
    #include "../../../module/probe.h"

    float Probe::_min_x(const xy_pos_t &probe_offset_xy) {
        return _MAX((X_MIN_BED) + (HMI_datas.mesh_min_x), (X_MIN_POS) + probe_offset_xy.x);
    }
    float Probe::_max_x(const xy_pos_t &probe_offset_xy) {
        return _MIN((HMI_datas.mesh_max_x), (X_MAX_POS) + probe_offset_xy.x);
    }
    float Probe::_min_y(const xy_pos_t &probe_offset_xy) {
        return _MAX((Y_MIN_BED) + (HMI_datas.mesh_min_y), (Y_MIN_POS) + probe_offset_xy.y);
    }
    float Probe::_max_y(const xy_pos_t &probe_offset_xy) {
        return _MIN((HMI_datas.mesh_max_y), (Y_MAX_POS) + probe_offset_xy.y);
    }
#endif


#if ENABLED(AUTO_BED_LEVELING_UBL)
  #include "../../../feature/bedlevel/bedlevel.h"
  #include "../../../feature/bedlevel/ubl/ubl.h"

  float unified_bed_leveling::get_mesh_x(const uint8_t i) {
      return (HMI_datas.mesh_min_x) + i * (float(HMI_datas.mesh_max_x - (HMI_datas.mesh_min_x)) / (HMI_datas.grid_max_points - 1));
  }
  float unified_bed_leveling::get_mesh_y(const uint8_t i) {
      return (HMI_datas.mesh_min_y) + i * (float(HMI_datas.mesh_max_y - (HMI_datas.mesh_min_y)) / (HMI_datas.grid_max_points - 1));
  } 

  
  typedef struct { int sx, ex, sy, ey; bool yfirst; } smart_fill_info;

  void unified_bed_leveling::smart_fill_mesh() {
      static const smart_fill_info
      info0 PROGMEM = { 0, HMI_datas.grid_max_points,      0, HMI_datas.grid_max_points - 2,  false },  // Bottom of the mesh looking up
      info1 PROGMEM = { 0, HMI_datas.grid_max_points,      HMI_datas.grid_max_points - 1, 0,  false },  // Top of the mesh looking down
      info2 PROGMEM = { 0, HMI_datas.grid_max_points - 2,  0, HMI_datas.grid_max_points,      true  },  // Left side of the mesh looking right
      info3 PROGMEM = { HMI_datas.grid_max_points - 1, 0,  0, HMI_datas.grid_max_points,      true  };  // Right side of the mesh looking left
    static const smart_fill_info * const info[] PROGMEM = { &info0, &info1, &info2, &info3 };

    LOOP_L_N(i, COUNT(info)) {
      const smart_fill_info *f = (smart_fill_info*)pgm_read_ptr(&info[i]);
      const int8_t sx = pgm_read_byte(&f->sx), sy = pgm_read_byte(&f->sy),
                  ex = pgm_read_byte(&f->ex), ey = pgm_read_byte(&f->ey);
      if (pgm_read_byte(&f->yfirst)) {
        const int8_t dir = ex > sx ? 1 : -1;
        for (uint8_t y = sy; y != ey; ++y)
          for (uint8_t x = sx; x != ex; x += dir)
            if (smart_fill_one(x, y, dir, 0)) break;
      }
      else {
        const int8_t dir = ey > sy ? 1 : -1;
        for (uint8_t x = sx; x != ex; ++x)
          for (uint8_t y = sy; y != ey; y += dir)
            if (smart_fill_one(x, y, 0, dir)) break;
      }
    }
  }

#endif

#if ENABLED(AUTO_BED_LEVELING_BILINEAR)
    #include "../../../feature/bedlevel/bedlevel.h"
    #include "../../../feature/bedlevel/abl/bbl.h"
#endif

#if ENABLED(AUTO_BED_LEVELING_BILINEAR)
    void ExtJyersuiClass::abl_extrapolate() {
        #ifdef HALF_IN_X
        uint8_t ctrx2 = 0, xend = HMI_datas.grid_max_points - 1;
        #else
        uint8_t ctrx1 = (HMI_datas.grid_max_points - 1) / 2, // left-of-center
                ctrx2 = (HMI_datas.grid_max_points - 1) / 2,  // right-of-center
                xend = ctrx1;
        #endif

        #ifdef HALF_IN_Y
        uint8_t ctry2 = 0, yend = HMI_datas.grid_max_points - 1;
        #else
        uint8_t ctry1 = (HMI_datas.grid_max_points - 1) / 2, // top-of-center
                ctry2 = (HMI_datas.grid_max_points - 1) / 2,  // bottom-of-center
                yend = ctry1;
        #endif

        LOOP_LE_N(xo, xend)
            LOOP_LE_N(yo, yend) {
                uint8_t x2 = ctrx2 + xo, y2 = ctry2 + yo;
                #ifndef HALF_IN_X
                    uint8_t x1 = ctrx1 - xo;
                #endif
                #ifndef HALF_IN_Y
                    uint8_t y1 = ctry1 - yo;
                    #ifndef HALF_IN_X
                        bedlevel.extrapolate_one_point(x1, y1, +1, +1);   //  left-below + +
                    #endif
                    bedlevel.extrapolate_one_point(x2, y1, -1, +1);     // right-below - +
                #endif
                #ifndef HALF_IN_X
                    bedlevel.extrapolate_one_point(x1, y2, +1, -1);     //  left-above + -
                #endif
                bedlevel.extrapolate_one_point(x2, y2, -1, -1);       // right-above - -
            }
    }

    float ExtJyersuiClass::getZvalues(const uint8_t sx, const uint8_t x, const uint8_t y, const float *values) {
        const uint8_t factmult = ((GRID_LIMIT) - (HMI_datas.grid_max_points));
        #if ENABLED(ABL_BILINEAR_SUBDIVISION)
            if (sx > HMI_datas.grid_max_points) return values[(x * sx) + y + (x * (factmult) * (BILINEAR_SUBDIVISIONS))];
            else return values[(x * sx) + y + (x * (factmult))];
        #else
            return values[(x * sx) + y + (x * (factmult))];
        #endif 
    }

#endif

#if HAS_PHYSICAL_MENUS
    void ExtJyersuiClass::UpdateAxis(const AxisEnum axis) {
        const xyz_float_t _Valuemin = { (float)HMI_datas.x_min_pos, (float)HMI_datas.y_min_pos, Z_MIN_POS };
        const xyz_float_t _Valuemax = { (float)HMI_datas.x_max_pos, (float)HMI_datas.y_max_pos, (float)HMI_datas.z_max_pos };
        if (axis == NO_AXIS_ENUM || axis == ALL_AXES_ENUM) {
        LOOP_L_N(i,2) {
            soft_endstop.min[i] = _Valuemin[i];
            soft_endstop.max[i] = _Valuemax[i];
            #if HAS_WORKSPACE_OFFSET
                workspace_offset[i] = home_offset[i] + position_shift[i];
            #endif
        }
        }
        else {
        soft_endstop.min[axis] = _Valuemin[axis];
        soft_endstop.max[axis] = _Valuemax[axis];
        }
    }

    void ExtJyersuiClass::ApplyPhySet() {
        update_software_endstops(temp_val.axis);
    }

    void ExtJyersuiClass::C100() {
        //if (!parser.seen_any()) return CrealityDWIN.DWIN_CError();
        if (!parser.seen("XY")) return C100_report();
        xy_int_t xymin_value;
        if (parser.seenval('X')) {
            xymin_value.x = parser.value_int();
            if (!WITHIN(xymin_value.x, -100, 500)) return CrealityDWIN.DWIN_CError();
            else {
              TERN_(NOZZLE_PARK_FEATURE, temp_val.last_pos = HMI_datas.x_min_pos);
              HMI_datas.x_min_pos = xymin_value.x;
              #if ENABLED(NOZZLE_PARK_FEATURE)
                if (HMI_datas.Park_point.x < temp_val.last_pos / 2) 
                  HMI_datas.Park_point.x = HMI_datas.Park_point.x + (HMI_datas.x_min_pos - temp_val.last_pos);
              #endif
              ApplyPhySet();
            }
        }
        if (parser.seenval('Y')) {
            xymin_value.y = parser.value_int();
            if (!WITHIN(xymin_value.y, -100, 500)) return CrealityDWIN.DWIN_CError();
            else {
              TERN_(NOZZLE_PARK_FEATURE, temp_val.last_pos = HMI_datas.y_min_pos);
              HMI_datas.y_min_pos = xymin_value.y;
              #if ENABLED(NOZZLE_PARK_FEATURE)
                if (HMI_datas.Park_point.y < temp_val.last_pos / 2) 
                  HMI_datas.Park_point.y = HMI_datas.Park_point.y + (HMI_datas.y_min_pos - temp_val.last_pos);
              #endif
              ApplyPhySet();
            }
        }
    }

    void ExtJyersuiClass::C100_report(const bool forReplay/*=true*/) {
        gcode.report_heading(forReplay, F("X and Y MIN POS (X<-500 to 500mm> Y<-500 to 500mm>)"));
        SERIAL_ECHOLNPGM_P(
            PSTR("  C100 X"), HMI_datas.x_min_pos
          , PSTR(" Y"), HMI_datas.y_min_pos
        );
    }

    void ExtJyersuiClass::C101() {
        //if (!parser.seen_any()) return CrealityDWIN.DWIN_CError();
        if (!parser.seen("XYZ")) return C101_report();
        xyz_int_t xyzmax_value;
        if (parser.seenval('X')) {
            xyzmax_value.x = parser.value_int();
            if (!WITHIN(xyzmax_value.x, X_BED_MIN, 999)) return CrealityDWIN.DWIN_CError();
            else {
              TERN_(NOZZLE_PARK_FEATURE, temp_val.last_pos = HMI_datas.x_max_pos);
              HMI_datas.x_max_pos = xyzmax_value.x;
              #if ENABLED(NOZZLE_PARK_FEATURE)
                if (HMI_datas.Park_point.x > temp_val.last_pos / 2) 
                  HMI_datas.Park_point.x = HMI_datas.Park_point.x + (HMI_datas.x_max_pos - temp_val.last_pos);
              #endif
              ApplyPhySet();
            }
        }
        if (parser.seenval('Y')) {
            xyzmax_value.y = parser.value_int();
            if (!WITHIN(xyzmax_value.y, Y_BED_MIN, 999)) return CrealityDWIN.DWIN_CError();
            else {
              TERN_(NOZZLE_PARK_FEATURE, temp_val.last_pos = HMI_datas.y_max_pos);
              HMI_datas.y_max_pos = xyzmax_value.y;
              #if ENABLED(NOZZLE_PARK_FEATURE)
                if (HMI_datas.Park_point.y > temp_val.last_pos / 2) 
                  HMI_datas.Park_point.y = HMI_datas.Park_point.y + (HMI_datas.y_max_pos - temp_val.last_pos);
              #endif
              ApplyPhySet();
            }
        }
        if (parser.seenval('Z')) {
            xyzmax_value.z = parser.value_int();
            if (!WITHIN(xyzmax_value.z, 100, 999)) return CrealityDWIN.DWIN_CError();
            else {
              HMI_datas.z_max_pos = xyzmax_value.z;
              ApplyPhySet();
            }
        }
    }

    void ExtJyersuiClass::C101_report(const bool forReplay/*=true*/) {
        gcode.report_heading(forReplay, F("X,Y,Z MAX POS (X<150 to 999mm> Y<150 to 999mm>) Z<100 to 999mm>)"));
        SERIAL_ECHOLNPGM_P(
            PSTR("  C101 X"), HMI_datas.x_max_pos
          , PSTR(" Y"), HMI_datas.y_max_pos
          , PSTR(" Z"), HMI_datas.z_max_pos
        );
    }

    void ExtJyersuiClass::C102() {
        //if (!parser.seen_any()) return CrealityDWIN.DWIN_CError();
        if (!parser.seen("XY")) return C102_report();
        xy_int_t xysize_value;
        if (parser.seenval('X')) {
            xysize_value.x = parser.value_int();
            if (!WITHIN(xysize_value.x, X_BED_MIN, X_MAX_POS)) return CrealityDWIN.DWIN_CError();
            else { HMI_datas.x_bed_size = xysize_value.x; HMI_datas.x_max_pos = HMI_datas.x_bed_size;
            ApplyPhySet(); }
        }
        if (parser.seenval('Y')) {
            xysize_value.y = parser.value_int();
            if (!WITHIN(xysize_value.y, Y_BED_MIN, Y_MAX_POS)) return CrealityDWIN.DWIN_CError();
            else { 
              HMI_datas.y_bed_size = xysize_value.y; HMI_datas.y_max_pos = HMI_datas.y_bed_size;
              ApplyPhySet();
            }
        }
    }

    void ExtJyersuiClass::C102_report(const bool forReplay/*=true*/) {
        gcode.report_heading(forReplay, F("X_BED_SIZE (X<150 to 999mm> Y<150 to 999mm>)"));
        SERIAL_ECHOLNPGM_P(
            PSTR("  C102 X"), HMI_datas.x_bed_size
          , PSTR(" Y"), HMI_datas.y_bed_size
        );
    }
#endif

#if ENABLED(NOZZLE_PARK_FEATURE)
    void ExtJyersuiClass::C125() {
        xyz_int_t park_value = HMI_datas.Park_point;

        //if (!parser.seen_any()) return CrealityDWIN.DWIN_CError();
        if (!parser.seen("XYZ")) return C125_report();
        if (parser.seenval('X')) {
            park_value.x = parser.value_int();
            if (!WITHIN(park_value.x, X_MIN_POS, X_MAX_POS)) return CrealityDWIN.DWIN_CError();
            else HMI_datas.Park_point.x = park_value.x;
            }
        if (parser.seenval('Y')) {
            park_value.y = parser.value_int();
            if (!WITHIN(park_value.y, Y_MIN_POS, Y_MAX_POS)) return CrealityDWIN.DWIN_CError();
            else HMI_datas.Park_point.y = park_value.y;
            }
        if (parser.seenval('Z')) {
            park_value.z = parser.value_int();
            if (!WITHIN(park_value.z, MIN_PARK_POINT_Z, Z_MAX_POS)) return CrealityDWIN.DWIN_CError();
            else HMI_datas.Park_point.z = park_value.z;
            }
    }

    void ExtJyersuiClass::C125_report(const bool forReplay/*=true*/) {
        gcode.report_heading(forReplay, F("Park Head Position (X<mm> Y<mm>) Z<mm>)"));
        SERIAL_ECHOLNPGM_P(
            PSTR("  C125 X"), HMI_datas.Park_point.x
          , PSTR(" Y"), HMI_datas.Park_point.y
          , PSTR(" Z"), HMI_datas.Park_point.z
        );
    }
#endif  // Set park position

void ExtJyersuiClass::C562() {
    //if (!parser.seen_any()) return CrealityDWIN.DWIN_CError();
    if (!parser.seen("E")) return C562_report();
        if (parser.seenval('E')) {
            HMI_datas.invert_dir_extruder = parser.value_bool() ? !HMI_datas.invert_dir_extruder : HMI_datas.invert_dir_extruder;
            CrealityDWIN.DWIN_Invert_Extruder();
        }
    }      // Invert Extruder

void ExtJyersuiClass::C562_report(const bool forReplay/*=true*/) {
        gcode.report_heading(forReplay, F("Invert Direction Extruder (E<0> -> Off or E<1> -> On>)"));
        SERIAL_ECHOLNPGM_P(
            PSTR("  C562 E"), HMI_datas.invert_dir_extruder
        );
    }

#if HAS_MESH
    void ExtJyersuiClass::C29() {
       //if (!parser.seen_any()) return CrealityDWIN.DWIN_CError();
       if (!parser.seen("LRFBN")) return C29_report();
        if (parser.seenval('N')) {
            #if ENABLED(AUTO_BED_LEVELING_BILINEAR)
                if WITHIN(((uint8_t)parser.value_byte() - 3), 0, GRID_LIMIT - 3) 
                    HMI_datas.grid_max_points = (uint8_t)parser.value_byte();
                else return CrealityDWIN.DWIN_CError();
            #elif ENABLED(AUTO_BED_LEVELING_UBL)
                if WITHIN(((uint8_t)parser.value_byte() - TERN(HAS_BED_PROBE, 7, 3)), 0, TERN(HAS_BED_PROBE, 8, 2))
                    HMI_datas.grid_max_points = (uint8_t)parser.value_byte();
                else return CrealityDWIN.DWIN_CError();
            #endif
            CrealityDWIN._GridxGrid = CrealityDWIN.Get_GridxGrid(HMI_datas.grid_max_points);
        }
        if (parser.seenval('L')) {
            HMI_datas.mesh_min_x = parser.value_float();
            LIMIT(HMI_datas.mesh_min_x, MIN_MESH_INSET, MAX_MESH_INSET);
            }
        if (parser.seenval('R')) {
            HMI_datas.mesh_max_x = parser.value_float();
            LIMIT(HMI_datas.mesh_max_x, MIN_MESH_INSET, MAX_MESH_INSET);
            }
        if (parser.seenval('F')) {
            HMI_datas.mesh_min_y = parser.value_float();
            LIMIT(HMI_datas.mesh_min_y, MIN_MESH_INSET, MAX_MESH_INSET);
            }
        if (parser.seenval('B')) {
            HMI_datas.mesh_max_y = parser.value_float();
            LIMIT(HMI_datas.mesh_max_y, MIN_MESH_INSET, MAX_MESH_INSET);
            }
        ApplyPhySet();
    }

    void ExtJyersuiClass::C29_report(const bool forReplay/*=true*/) {
        gcode.report_heading(forReplay, F("Bed Leveling Grid NxN (N<value>) - Mesh Insets L(mm) R(mm) F(mm) B(mm)"));
        SERIAL_ECHOLNPGM_P(
            PSTR("  C29 N"), HMI_datas.grid_max_points,
            PSTR("  C29 L"), HMI_datas.mesh_min_x,
            PSTR(" R"), HMI_datas.mesh_max_x,
            PSTR(" F"), HMI_datas.mesh_min_y,
            PSTR(" B"), HMI_datas.mesh_max_y
        );
    }

    void ExtJyersuiClass::C852() {
        //if (!parser.seen_any()) return CrealityDWIN.DWIN_CError();
        if (!parser.seen("M")) return C852_report();
        int16_t margin_parser;
        if (parser.seenval('M')) {
            margin_parser = parser.value_int();
            if (!WITHIN(margin_parser, MIN_PROBE_MARGIN, MAX_PROBE_MARGIN)) return CrealityDWIN.DWIN_CError();
            else HMI_datas.probing_margin = (float)margin_parser;
            }
    }

    void ExtJyersuiClass::C852_report(const bool forReplay/*=true*/) {
        gcode.report_heading(forReplay, F("Probing Margin (M<value 5 to 60mm>)"));
        SERIAL_ECHOLNPGM_P(
            PSTR("  C852 M"), HMI_datas.probing_margin
        );
    }

#endif

#if HAS_BED_PROBE
    void ExtJyersuiClass::C851() {
        //if (!parser.seen_any()) return CrealityDWIN.DWIN_CError();
        if (!parser.seen("FS")) return C851_report();
        uint16_t z_fast_feedrate_parser = HMI_datas.zprobefeedfast ;
        uint16_t z_slow_feedrate_parser = HMI_datas.zprobefeedslow;
     
        if (parser.seenval('F')) {
            z_fast_feedrate_parser = parser.value_int();
            if (!WITHIN(z_fast_feedrate_parser, (MIN_Z_PROBE_FEEDRATE * 2), MAX_Z_PROBE_FEEDRATE)) return CrealityDWIN.DWIN_CError();
            else HMI_datas.zprobefeedfast = z_fast_feedrate_parser;
            }
        if (parser.seenval('S')) {
            z_slow_feedrate_parser = parser.value_int();
            if  (!WITHIN(z_slow_feedrate_parser, MIN_Z_PROBE_FEEDRATE, MAX_Z_PROBE_FEEDRATE)) return CrealityDWIN.DWIN_CError();
            else HMI_datas.zprobefeedslow = z_slow_feedrate_parser;
            }          
    }

    void ExtJyersuiClass::C851_report(const bool forReplay/*=true*/) {
        gcode.report_heading(forReplay, F("Z Probe Fast and Slow Feedrate (F<mm/min> S<mm/min>)"));
        SERIAL_ECHOLNPGM_P(
            PSTR("  C851 F"), HMI_datas.zprobefeedfast
          , PSTR(" S"), HMI_datas.zprobefeedslow
        );
    }
#endif  // Set probing margin and z feed rate of the probe mesh leveling

#if HAS_LIVEMESH
    void ExtJyersuiClass::Draw_LiveMeshGrid() {
      BedLevelTools.Draw_Grid_Mesh();
    }

    void ExtJyersuiClass::MeshUpdate(const uint8_t x, const uint8_t y, const float z) {
      BedLevelTools.DrawMeshPoint(x, y, z, true);
    }

    void ExtJyersuiClass::MeshSelectPoint(const uint8_t x, const uint8_t y, const bool flag /*=false*/) {
      BedLevelTools.DrawSelectPoint(x, y, flag);
    }
#endif

#if HAS_GRAPHICAL_LEVELMANUAL && HAS_LEVELING
    void ExtJyersuiClass::Draw_EditGrid(const bool start/*=false*/) {
      BedLevelTools.Draw_Bed_Mesh(true, start);
      BedLevelTools.DrawSelectPoint(BedLevelTools.mesh_x, BedLevelTools.mesh_y, start, true);
      BedLevelTools.LevelManual_flag = false;
      BedLevelTools.manual_mesh_move();
    }
#endif

#endif