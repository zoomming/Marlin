/**
 * Mesh Tool
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

#include "../../../inc/MarlinConfigPre.h"

#if BOTH(DWIN_CREALITY_LCD_JYERSUI, HAS_MESH)

#include "../../../core/types.h"
#include "../../marlinui.h"
#include "dwin_lcd.h"
#include "jyersui.h"
#include "dwin.h"
#include "../../../feature/bedlevel/bedlevel.h"
#include "trammingtool.h"

TrammingToolClass TrammingTool;

uint8_t rmax;                               // Maximum radius

#define margin 25                           // XY Margins
#define rmin 5                              // Minimum radius
#define zmin -20                            // rmin at z=-0.20
#define zmax  20                            // rmax at z= 0.20
#define width DWIN_WIDTH - 2 * margin 
#define rmax _MIN(margin - 2, 0.5*(width))
#define r(z) ((z-zmin)*(rmax-rmin)/(zmax-zmin)+rmin)
#define px(xp) (margin + (xp)*(width))
#define py(yp) (30 + DWIN_WIDTH - margin - (yp)*(width))


void TrammingToolClass::DrawMeshGrid() {
  min = 100;
  max = -100;
  const uint16_t linecolor = CrealityDWIN.GetColor(HMI_datas.menu_split_line, Line_Color, true);
  JYERSUI::ClearMenuArea();
  DWIN_Draw_Rectangle(0, linecolor, px(0), py(0), px(1), py(1));
}

void TrammingToolClass::DrawMeshPoint(const uint8_t x, const uint8_t y, const float z) {
  const bool Nan = isnan(z);
  int16_t v = Nan ? 0 : round(z * 100);

  LIMIT(v, zmin, zmax);
  NOLESS(max, z);
  NOMORE(min, z);

  const int8_t radio = r(abs(v));
  const uint16_t color = JYERSUI::RainbowInt(v, zmin, zmax);

  JYERSUI::Draw_FillCircle(color, px(x), py(y), radio);

  if (!Nan) {
    if (v == 0) JYERSUI::Draw_Float(meshfont, 1, 2, px(x) - 2*fontsize, py(y) - fontsize, 0);
    else JYERSUI::Draw_Signed_Float(meshfont, 1, 2, px(x) - 3*fontsize, py(y) - fontsize, z);
  }
}

void TrammingToolClass::DrawMesh(bed_mesh_t zval) {
  DrawMeshGrid();
  LOOP_L_N(y, 2) {
    hal.watchdog_refresh();
    LOOP_L_N(x, 2) DrawMeshPoint(x, y, zval[x][y]);
  }
}


#endif
