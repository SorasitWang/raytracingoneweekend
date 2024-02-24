#ifndef UTIL_H
#define UTIL_H

#include <limits>
#include <iostream>
// Constants

namespace Util {

    const double infinity = std::numeric_limits<double>::infinity();
    const double pi = 3.1415926535897932385;
    const double invPi = 1 / pi;
    const double epsilon = 1e-6;

    // Utility Functions

    inline double degrees_to_radians(double degrees) {
        return degrees * pi / 180.0;
    }

    inline double random_double() {
        // Returns a random real in [0,1).
        return rand() / (RAND_MAX + 1.0);
    }

    inline double random_double(double min, double max) {
        // Returns a random real in [min,max).
        return min + (max - min) * random_double();
    }

    inline int random_int(int min, int max) {
        // Returns a random integer in [min,max].
        return static_cast<int>(random_double(min, max + 1));
    }

    inline int clamp(int x, int low, int high) {
        // Return the value clamped to the range [low, high].
        if (x < low) return low;
        if (x < high) return x;
        return high;
    }

    inline double clamp(double x, double low, double high) {
        // Return the value clamped to the range [low, high].
        if (x < low) return low;
        if (x < high) return x;
        return high;
    }

    inline void print(std::string a) {
        std::cout << a << std::endl;
    }
    inline void print(std::string a, std::string b) {
        std::cout << a << " " << b << std::endl;
    }
    inline void print(std::string a, std::string b, std::string c) {
        std::cout << a << " " << b << " " << c << std::endl;
    }
}

#endif