// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

#pragma once

#define _USE_MATH_DEFINES
#include <cmath>

#ifndef M_PI
#define M_PI (3.14159265358979323846)
#endif

/// Utility functions for angle conversions.
namespace AngleUtils
{
    /// Converts an angle from degrees to radians.
    /// \param angle_in_degrees The angle in degrees.
    /// \returns The angle in radians.
    inline double DegreesToRadians(double angle_in_degrees)
    {
        return angle_in_degrees / 180.0 * M_PI;
    }

    /// Converts an angle from radians to degrees.
    /// \param angle_in_radians The angle in radians.
    /// \returns The angle in degrees.
    inline double RadiansToDegrees(double angle_in_radians)
    {
        return angle_in_radians / M_PI * 180.0;
    }
}
