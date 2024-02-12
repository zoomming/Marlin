/**
 * Marlin 3D Printer Firmware
 * Copyright (c) 2022 MarlinFirmware [https://github.com/MarlinFirmware/Marlin]
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

/*****************************************************************************
  * @file     lcd/e3v2/common/limits.h
  * @brief    Limits for UI values
  ****************************************************************************/

#include "../../../inc/MarlinConfig.h"

#if HAS_TRINAMIC_CONFIG && !HAS_RC_CPU
  #define TMC_EDIT_MIN_CURRENT 400
  #define TMC_EDIT_MAX_CURRENT 1500
#endif


#define MIN_FEEDRATE_EDIT_VALUE 1
#define MIN_ACCELERATION_EDIT_VALUE 1
#define MIN_JERK_EDIT_VALUE 0.1
#define MIN_STEPS_EDIT_VALUE 1

// If max edit values are not specified use default *DEFAULT_MAX_MULTIPLIER
#define DEFAULT_MAX_MULTIPLIER 2
#if ENABLED(DWIN_CREALITY_LCD_JYERSUI)
  #if defined(CUSTOM_E_STEPS) || HAS_SPRITE_PRO
    #define DEFAULT_MAX_FEEDRATE_MULTIPLIER 8
  #else
    #define DEFAULT_MAX_FEEDRATE_MULTIPLIER DEFAULT_MAX_MULTIPLIER
  #endif
  #define DEFAULT_MAX_JERK_MULTIPLIER 100
  #define DEFAULT_MAX_XY_ACC_MULTIPLIER 8
  #define DEFAULT_MAX_Z_ACC_MULTIPLIER 9
  #define DEFAULT_MAX_E_ACC_MULTIPLIER 3
  #define DEFAULT_MAX_XYZ_STEPS_MULTIPLIER 4
  #define DEFAULT_MAX_E_STEPS 2000
#endif

/* feedrate limits */

constexpr float min_feedrate_edit_values[] = {
  [X_AXIS] = MIN_FEEDRATE_EDIT_VALUE,
  [Y_AXIS] = MIN_FEEDRATE_EDIT_VALUE,
  [Z_AXIS] = MIN_FEEDRATE_EDIT_VALUE,
  [E_AXIS] = MIN_FEEDRATE_EDIT_VALUE
};
constexpr float default_max_feedrate[] = DEFAULT_MAX_FEEDRATE;
constexpr float max_feedrate_edit_values[] =
  #ifdef MAX_FEEDRATE_EDIT_VALUES
    MAX_FEEDRATE_EDIT_VALUES
  #else
    {
      [X_AXIS] = default_max_feedrate[X_AXIS] * DEFAULT_MAX_MULTIPLIER,
      [Y_AXIS] = default_max_feedrate[Y_AXIS] * DEFAULT_MAX_MULTIPLIER,
      [Z_AXIS] = default_max_feedrate[Z_AXIS] * DEFAULT_MAX_MULTIPLIER,
      [E_AXIS] = default_max_feedrate[E_AXIS] * DEFAULT_MAX_FEEDRATE_MULTIPLIER
    }
  #endif
;

/* acceleration limits */

constexpr float min_acceleration_edit_values[] = {
  [X_AXIS] = MIN_ACCELERATION_EDIT_VALUE,
  [Y_AXIS] = MIN_ACCELERATION_EDIT_VALUE,
  [Z_AXIS] = MIN_ACCELERATION_EDIT_VALUE,
  [E_AXIS] = MIN_ACCELERATION_EDIT_VALUE
};
constexpr float default_max_acceleration[] = DEFAULT_MAX_ACCELERATION;
constexpr float max_acceleration_edit_values[] =
  #ifdef MAX_ACCEL_EDIT_VALUES
    MAX_ACCEL_EDIT_VALUES
  #elif ENABLED(DWIN_CREALITY_LCD_JYERSUI)
    {
      [X_AXIS] = default_max_acceleration[X_AXIS] * DEFAULT_MAX_XY_ACC_MULTIPLIER,
      [Y_AXIS] = default_max_acceleration[Y_AXIS] * DEFAULT_MAX_XY_ACC_MULTIPLIER,
      [Z_AXIS] = default_max_acceleration[Z_AXIS] * DEFAULT_MAX_Z_ACC_MULTIPLIER,
      [E_AXIS] = default_max_acceleration[E_AXIS] * DEFAULT_MAX_E_ACC_MULTIPLIER
    }
  #else 
    {
      [X_AXIS] = default_max_acceleration[X_AXIS] * DEFAULT_MAX_MULTIPLIER,
      [Y_AXIS] = default_max_acceleration[Y_AXIS] * DEFAULT_MAX_MULTIPLIER,
      [Z_AXIS] = default_max_acceleration[Z_AXIS] * DEFAULT_MAX_MULTIPLIER,
      [E_AXIS] = default_max_acceleration[E_AXIS] * DEFAULT_MAX_MULTIPLIER
    }
  #endif
;

/* max jerk limits */

#if HAS_CLASSIC_JERK
  constexpr float min_jerk_edit_values[] = {
    [X_AXIS] = MIN_JERK_EDIT_VALUE,
    [Y_AXIS] = MIN_JERK_EDIT_VALUE,
    [Z_AXIS] = MIN_JERK_EDIT_VALUE,
    [E_AXIS] = MIN_JERK_EDIT_VALUE
  };
  constexpr float max_jerk_edit_values[] =
    #ifdef MAX_JERK_EDIT_VALUES
      MAX_JERK_EDIT_VALUES
    #elif ENABLED(DWIN_CREALITY_LCD_JYERSUI)
      {
        [X_AXIS] = DEFAULT_XJERK * DEFAULT_MAX_JERK_MULTIPLIER,
        [Y_AXIS] = DEFAULT_YJERK * DEFAULT_MAX_JERK_MULTIPLIER,
        [Z_AXIS] = DEFAULT_ZJERK * DEFAULT_MAX_JERK_MULTIPLIER,
        [E_AXIS] = DEFAULT_EJERK * DEFAULT_MAX_JERK_MULTIPLIER
      }
    #else
      {
        [X_AXIS] = DEFAULT_XJERK * DEFAULT_MAX_MULTIPLIER,
        [Y_AXIS] = DEFAULT_YJERK * DEFAULT_MAX_MULTIPLIER,
        [Z_AXIS] = DEFAULT_ZJERK * DEFAULT_MAX_MULTIPLIER,
        [E_AXIS] = DEFAULT_EJERK * DEFAULT_MAX_MULTIPLIER
      }
    #endif
  ;
#endif

/* steps per mm limits */

constexpr float min_steps_edit_values[] = {
  [X_AXIS] = MIN_STEPS_EDIT_VALUE,
  [Y_AXIS] = MIN_STEPS_EDIT_VALUE,
  [Z_AXIS] = MIN_STEPS_EDIT_VALUE,
  [E_AXIS] = MIN_STEPS_EDIT_VALUE,
};
constexpr float default_steps[] = DEFAULT_AXIS_STEPS_PER_UNIT;
constexpr float max_steps_edit_values[] =
  #if ENABLED(DWIN_CREALITY_LCD_JYERSUI)
    {
      [X_AXIS] = default_steps[X_AXIS] * DEFAULT_MAX_XYZ_STEPS_MULTIPLIER,
      [Y_AXIS] = default_steps[Y_AXIS] * DEFAULT_MAX_XYZ_STEPS_MULTIPLIER,
      [Z_AXIS] = default_steps[Z_AXIS] * DEFAULT_MAX_XYZ_STEPS_MULTIPLIER,
      [E_AXIS] = DEFAULT_MAX_E_STEPS

    };
  #else
  {
    [X_AXIS] = default_steps[X_AXIS] * DEFAULT_MAX_MULTIPLIER,
    [Y_AXIS] = default_steps[Y_AXIS] * DEFAULT_MAX_MULTIPLIER,
    [Z_AXIS] = default_steps[Z_AXIS] * DEFAULT_MAX_MULTIPLIER,
    [E_AXIS] = default_steps[E_AXIS] * DEFAULT_MAX_MULTIPLIER
  };
  #endif

