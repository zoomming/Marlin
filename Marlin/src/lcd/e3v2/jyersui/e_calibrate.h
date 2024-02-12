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
 *
 * Based on the original code provided by Creality under GPL
 */

/**
 * Modify and enhanced by Tititopher68-dev (Christophe L.) since 28/11/2021 !
 * build v3.0.0a was public at 28/05/2022 !
 */

#include "../../../core/types.h"
#include "../common/encoder.h"
#include "dwin.h"

#include <stdint.h>

#if HAS_HOTEND && HAS_E_CALIBRATION

    class E_calibrateClass {
    public:

    //static float real_value;
    //static float control_value;
    //static float temp_value;

    static bool done;
    static void initStage1();
    
    static void Draw_Stage1();
    static void Draw_Stage2();
    static void Draw_Stage3();
    static void Draw_Stage4();

    static void onEncoderE(EncoderState encoder_diffState);
    static bool isDone_E() { return done; }
    };

    extern E_calibrateClass e_calibrate;

#endif