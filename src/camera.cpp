#include "camera.h"

void Camera::render(std::ostream& out, const Hittable& world) {
    initialize();

    out << "P3\n" << image_width << ' ' << image_height << "\n255\n";

    for (int j = 0; j < image_height; ++j) {
        std::clog << "\rScanlines remaining: " << (image_height - j) << ' ' << std::flush;
        for (int i = 0; i < image_width; ++i) {
            color pixel_color(0, 0, 0);
            //  TODO : other super sampling method
            //      Adaptive sampling, uniform jittered
            if (samplingMethod == SamplingMethod::Normal)
                normalSampling(world, i, j, pixel_color);
            else if (samplingMethod == SamplingMethod::SuperSampling)
                superSampling(world, i, j, pixel_color);
            else if (samplingMethod == SamplingMethod::AdaptiveSuperSampling)
                adaptiveSuperSampling(world, i, j, pixel_color);
            write_color(out, pixel_color);
        }
    }
}

void Camera::initialize() {
    image_height = static_cast<int>(image_width / aspect_ratio);
    image_height = (image_height < 1) ? 1 : image_height;

    center = lookfrom;

    // Determine viewport dimensions.
    auto theta = Util::degrees_to_radians(vfov);
    auto h = tan(theta / 2);
    auto viewport_height = 2 * h * focus_dist;
    auto viewport_width = viewport_height * (static_cast<double>(image_width) / image_height);

    // Calculate the u,v,w unit basis vectors for the camera coordinate frame.
    w = unit_vector(lookfrom - lookat);
    u = unit_vector(cross(vup, w));
    v = cross(w, u);

    // Calculate the vectors across the horizontal and down the vertical viewport edges.
    vec3 viewport_u = viewport_width * u;    // Vector across viewport horizontal edge
    vec3 viewport_v = viewport_height * -v;  // Vector down viewport vertical edge

    // Calculate the horizontal and vertical delta vectors from pixel to pixel.
    pixel_delta_u = viewport_u / image_width;
    pixel_delta_v = viewport_v / image_height;

    // Calculate the location of the upper left pixel.
    auto viewport_upper_left = center - (focus_dist * w) - viewport_u / 2 - viewport_v / 2;
    pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);

    // Calculate the camera defocus disk basis vectors.
    auto defocus_radius = focus_dist * tan(Util::degrees_to_radians(defocus_angle / 2));
    defocus_disk_u = u * defocus_radius;
    defocus_disk_v = v * defocus_radius;
}

color Camera::rayColor(const ray& r, int depth, const Hittable& world) const {
    HitRecord rec;

    if (depth <= 0)
        return color(1.0, 1.0, 1.0);

    if (world.hit(r, interval(0.001, Util::infinity), rec)) {
        ScatteredRays scattered;
        color attenuation;
        color colorFromEmission = rec.mat->emitted(rec.u, rec.v, rec.p);

        if (rec.mat->scatter(r, rec, attenuation, scattered)) {
            color accuColor;
            for (unsigned int i = 0; i < scattered.size(); i++) {
                ScatteredRay s = scattered[i];
                color tmp = rayColor(s.r, depth - 1, world);
                accuColor += s.coeff * tmp;
            }
 
            accuColor = accuColor * attenuation;
            return accuColor + colorFromEmission;
        }
        return colorFromEmission;
    }
    else {
        return background;
    }
}

point3 Camera::defocus_disk_sample() const {
    // Returns a random point in the camera defocus disk.
    auto p = random_in_unit_disk();
    return center + (p[0] * defocus_disk_u) + (p[1] * defocus_disk_v);
}

vec3 Camera::pixel_sample_square() const {
    // Returns a random point in the square surrounding a pixel at the origin.
    auto px = -0.5 + Util::random_double();
    auto py = -0.5 + Util::random_double();
    return (px * pixel_delta_u) + (py * pixel_delta_v);
}

void Camera::normalSampling(const Hittable& world, unsigned int i, unsigned int j, color& pixelColor) {

    ray r = getRay(i, j);
    pixelColor = rayColor(r, max_depth, world);

}

void Camera::superSampling(const Hittable& world, unsigned int i, unsigned int j, color& pixelColor) {

    for (int sample = 0; sample < samples_per_pixel; ++sample) {
        ray r = getRay(i, j);
        pixelColor += rayColor(r, max_depth, world);
    }
    pixelColor = pixelColor / samples_per_pixel;
}

void Camera::adaptiveSuperSamplingRecur(const Hittable& world, const point3(&corners)[4], unsigned int depth, color& pixelColor,
    const color(&cornersColor)[4]) {

    double threshold = 0.2;
    bool isSigDiff = false;

    //  If any corners has diff color, split this region to quadrant
    for (unsigned int i = 0; i < 3; ++i) {

        if ((cornersColor[i] - cornersColor[i + 1]).length() > threshold) {
            isSigDiff = true;
            if (depth != 0)
                break;
        }
    }
    if (!isSigDiff || depth == 0) {
        pixelColor = (cornersColor[0] + cornersColor[1] + cornersColor[2] + cornersColor[3]) / 4;
        return;
    }

    /*

       0....[0]....1
       .       .
       [1]...c...[2]
       .       .
       2....[3]....3


   */
    point3 center = (corners[0] + corners[1] + corners[2] + corners[3]) / 4;
    color centerColor = rayColor(getRayWithSamplePos(center), max_depth, world);

    point3 newCorners[4] = { (corners[0] + corners[1]) / 2,
                            (corners[0] + corners[2]) / 2,
                            (corners[1] + corners[3]) / 2,
                            (corners[2] + corners[3]) / 2
    };
    color newCornersColor[4] = { rayColor(getRayWithSamplePos(newCorners[0]), max_depth, world),
                                    rayColor(getRayWithSamplePos(newCorners[1]), max_depth, world),
                                    rayColor(getRayWithSamplePos(newCorners[2]), max_depth, world),
                                    rayColor(getRayWithSamplePos(newCorners[3]), max_depth, world),

    };
    //  Split to quadrant
    color color1, color2, color3, color4;

    point3 q1[4] = { corners[0] , newCorners[0], newCorners[1] , center };
    color c1[4] = { cornersColor[0], newCornersColor[0], newCornersColor[1], centerColor };
    adaptiveSuperSamplingRecur(world, q1, depth - 1, color1, c1);

    point3 q2[4] = { newCorners[0], corners[1] , center , newCorners[2] };
    color c2[4] = { newCornersColor[0], cornersColor[1], centerColor, newCornersColor[2] };
    adaptiveSuperSamplingRecur(world, q2, depth - 1, color2, c2);

    point3 q3[4] = { newCorners[1] , center, corners[2] , newCorners[3] };
    color c3[4] = { newCornersColor[1], centerColor, cornersColor[2], newCornersColor[3] };
    adaptiveSuperSamplingRecur(world, q3, depth - 1, color3, c3);

    point3 q4[4] = { center , newCorners[2], newCorners[3] , corners[3] };
    color c4[4] = { centerColor, newCornersColor[2], newCornersColor[3], cornersColor[3] };
    adaptiveSuperSamplingRecur(world, q4, depth - 1, color4, c4);

    pixelColor = (color1 + color2 + color3 + color4) / 4.0;

}

void Camera::adaptiveSuperSampling(const Hittable& world, unsigned int i, unsigned int j, color& pixelColor) {

    unsigned int quadrant = 1, depth = 5;

    point3 pixelCenter = pixel00_loc + (i * pixel_delta_u) + (j * pixel_delta_v);
    // UL, UR, LL, LR
    point3 corners[4] = { pixelCenter - pixel_delta_u / 2 - pixel_delta_v / 2 ,
                            pixelCenter + pixel_delta_u / 2 - pixel_delta_v / 2 ,
                            pixelCenter - pixel_delta_u / 2 + pixel_delta_v / 2 ,
                            pixelCenter + pixel_delta_u / 2 + pixel_delta_v / 2
    };
    color cornersColor[4] = { rayColor(getRayWithSamplePos(corners[0]), max_depth, world),
                                    rayColor(getRayWithSamplePos(corners[1]), max_depth, world),
                                    rayColor(getRayWithSamplePos(corners[2]), max_depth, world),
                                    rayColor(getRayWithSamplePos(corners[3]), max_depth, world) };

    adaptiveSuperSamplingRecur(world, corners, depth, pixelColor, cornersColor);

}