#ifndef CAMERA_H
#define CAMERA_H
//==============================================================================================
// Originally written in 2016 by Peter Shirley <ptrshrl@gmail.com>
//
// To the extent possible under law, the author(s) have dedicated all copyright and related and
// neighboring rights to this software to the public domain worldwide. This software is
// distributed without any warranty.
//
// You should have received a copy (see file COPYING.txt) of the CC0 Public Domain Dedication
// along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.
//==============================================================================================

#include <iostream>

#include "common.h"
#include "material.h"

#include "hittable/hittable.h"

enum class SamplingMethod { Normal, SuperSampling, AdaptiveSuperSampling };

class Camera {
public:
    double aspect_ratio = 1.0;  // Ratio of image width over height
    int    image_width = 100;  // Rendered image width in pixel count
    int    samples_per_pixel = 10;   // Count of random samples for each pixel
    int    max_depth = 10;   // Maximum number of ray bounces into scene
    color  background;               // Scene background color

    double vfov = 90;              // Vertical view angle (field of view)
    point3 lookfrom = point3(0, 0, -1);  // Point camera is looking from
    point3 lookat = point3(0, 0, 0);   // Point camera is looking at
    vec3   vup = vec3(0, 1, 0);     // Camera-relative "up" direction

    double defocus_angle = 0;  // Variation angle of rays through each pixel
    double focus_dist = 10;    // Distance from camera lookfrom point to plane of perfect focus

    double shutter_duration = 0.0;

    SamplingMethod samplingMethod = SamplingMethod::SuperSampling;

    void render(std::ostream& out, const Hittable& world);

private:
    int    image_height;   // Rendered image height
    point3 center;         // Camera center
    point3 pixel00_loc;    // Location of pixel 0, 0
    vec3   pixel_delta_u;  // Offset to pixel to the right
    vec3   pixel_delta_v;  // Offset to pixel below
    vec3   u, v, w;        // // Camera frame basis vectors

    vec3   defocus_disk_u;  // Defocus disk horizontal radius
    vec3   defocus_disk_v;  // Defocus disk vertical radius


    void initialize();

    color rayColor(const ray& r, int depth, const Hittable& world) const;

    ray getRayWithSamplePos(point3 pixel_sample) const {
       
        auto ray_origin = (defocus_angle <= 0) ? center : defocus_disk_sample();

        auto ray_direction = pixel_sample - ray_origin;

        auto ray_time = shutter_duration * Util::random_double();

        return ray(ray_origin, ray_direction, ray_time);
    }

    ray getRay(int i, int j) const {
        // Get a randomly-sampled camera ray for the pixel at location i,j, originating from
        // the camera defocus disk.

        point3 pixel_center = pixel00_loc + (i * pixel_delta_u) + (j * pixel_delta_v);
        point3 pixel_sample = pixel_center + pixel_sample_square();

        return getRayWithSamplePos(pixel_sample);
    }

    point3 defocus_disk_sample() const;

    vec3 pixel_sample_square() const;

    void normalSampling(const Hittable& world, unsigned int i, unsigned int j, color& pixelColor);

    void superSampling(const Hittable& world, unsigned int i, unsigned int j, color& pixelColor);

    void adaptiveSuperSamplingRecur(const Hittable& world, const point3(&corners)[4], unsigned int depth, color& pixelColor,
        const color(&cornersColor)[4]);

    void adaptiveSuperSampling(const Hittable& world, unsigned int i, unsigned int j, color& pixelColor);
};


#endif