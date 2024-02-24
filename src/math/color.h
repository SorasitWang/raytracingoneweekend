#pragma once
#ifndef COLOR_H
#define COLOR_H

#include <iostream>

#include "vec3.h"

using color = vec3;

inline double linear_to_gamma(double linearComponent)
{
    return pow(linearComponent, 0.45);
}

inline void write_color(std::ostream& out, color pixelColor) {
    
    // Apply the linear to gamma transform.
    double r = linear_to_gamma(pixelColor.x() );
    double g = linear_to_gamma(pixelColor.y() );
    double b = linear_to_gamma(pixelColor.z() );

    // Write the translated [0,255] value of each color component.
    static const interval intensity(0.000, 0.999);
    out << static_cast<int>(255.999 * intensity.clamp( r ) ) << ' '
        << static_cast<int>(255.999 * intensity.clamp( g ) ) << ' '
        << static_cast<int>(255.999 * intensity.clamp( b ) ) << '\n';
}

#endif