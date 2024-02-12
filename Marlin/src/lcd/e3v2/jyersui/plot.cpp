/**
 * DWIN Graph PID
 * 
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

#if BOTH(DWIN_CREALITY_LCD_JYERSUI, PID_GRAPH)
#include "plot.h"
#include "../../../core/types.h"
#include "dwin_lcd.h"
#include "jyersui.h"
#include "dwin.h"


PlotClass plot;

void PlotClass::Draw(const frame_rect_t frame, const celsius_t max, const_float_t ref/*=0*/) {
  grphframe = frame;
  grphpoints = 0;
  scale = frame.h / (float)max;
  x2 = frame.x + frame.w - 1;
  y2 = frame.y + frame.h - 1;
  r = round((y2) - ref * scale);
  JYERSUI::Draw_Box(1, Color_Bg_Black, frame);
  for (uint8_t i = 1; i < 4; i++)  if (i*50 < frame.w) DWIN_Draw_VLine(Line_Color, i*50 + frame.x, frame.y, frame.h);
  JYERSUI::Draw_Box(0, CrealityDWIN.GetColor(HMI_datas.popup_highlight, Color_White), JYERSUI::ExtendFrame(frame, 1));
  DWIN_Draw_HLine(Color_Red, frame.x, r, frame.w);
}
 
void PlotClass::UpdateP(const_float_t value) {
  if (!scale) return;
  uint16_t y = round((y2) - value * scale);
  if (grphpoints < grphframe.w) {
    if (grphpoints == 0) { x1 = grphframe.x; y1 = y; }
    else x1 = grphpoints + grphframe.x - 1;
    //DWIN_Draw_Point(Color_Yellow, 1, 1, grphpoints + grphframe.x, y);
    DWIN_Draw_Line(Color_Yellow, x1, y1, grphpoints + grphframe.x, y);
  }
  else {
    DWIN_Frame_AreaMove(1, 0, 1, Color_Bg_Black, grphframe.x, grphframe.y, x2, y2);
    if ((grphpoints % 50) == 0) DWIN_Draw_VLine(Line_Color, x2 - 1, grphframe.y + 1, grphframe.h - 2);
    DWIN_Draw_Point(Color_Red, 1, 1, x2 - 1, r);
    //DWIN_Draw_Point(Color_Yellow, 1, 1, x2 - 1, y);
    DWIN_Draw_Line(Color_Yellow, x2 - 2, y1, x2 - 1, y);
  }
  grphpoints++;
  y1 = y;
}

#endif