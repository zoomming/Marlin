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

#pragma once

#include "jyersui.h"

class PlotClass {
public:
  void Draw(const frame_rect_t frame, const celsius_t max, const_float_t ref = 0);
  void UpdateP(const_float_t value);
private:
  frame_rect_t grphframe;
  float scale;
  uint16_t grphpoints, r, x2, y2, x1, y1;
};

extern PlotClass plot;

