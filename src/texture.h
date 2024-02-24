#ifndef TEXTURE_H
#define TEXTURE_H

#include "common.h"

#include "tool/image.h"
#include "tool/noise.h"

class texture {
public:
    virtual ~texture() = default;

    virtual color value(double u, double v, const point3& p) const = 0;
};

class solid_color : public texture {
public:
    solid_color(color c) : color_value(c) {}

    solid_color(double red, double green, double blue) : solid_color(color(red, green, blue)) {}

    color value(double u, double v, const point3& p) const override {
        return color_value;
    }

private:
    color color_value;
};

class checker_texture : public texture {
public:
    checker_texture(double _scale, shared_ptr<texture> _even, shared_ptr<texture> _odd)
        : inv_scale(1.0 / _scale), even(_even), odd(_odd) {}

    checker_texture(double _scale, color c1, color c2)
        : inv_scale(1.0 / _scale),
        even(make_shared<solid_color>(c1)),
        odd(make_shared<solid_color>(c2))
    {}

    color value(double u, double v, const point3& p) const override {
        auto xInteger = static_cast<int>(std::floor(inv_scale * p.x()));
        auto yInteger = static_cast<int>(std::floor(inv_scale * p.y()));
        auto zInteger = static_cast<int>(std::floor(inv_scale * p.z()));

        bool isEven = (xInteger + yInteger + zInteger) % 2 == 0;

        return isEven ? even->value(u, v, p) : odd->value(u, v, p);
    }

private:
    double inv_scale;
    shared_ptr<texture> even;
    shared_ptr<texture> odd;
};

class image_texture : public texture {
public:
    image_texture(const char* filename) : image(filename){}

    color value(double u, double v, const point3& p) const override {
        // If we have no texture data, then return solid cyan as a debugging aid.
        if (image.height() <= 0) return color(0, 1, 1);
        
        auto pixelColor = image.pixel_color(u, v);
        //std::cout << pixelColor[0]<< " " <<  pixelColor[1] << " " << pixelColor[2] << std::endl;
        return color(pixelColor[0], pixelColor[1], pixelColor[2]);
    }

private:
    Image image;

};

class NoiseTexture : public texture {
public:

    NoiseTexture(double _freq, double _scale) : freq(_freq), scale(_scale) {}

    color value(double u, double v, const point3& p) const override {
        auto s = freq * p;
        return color(1, 1, 1) * scale * (1 + sin(s.z() + 10 * noise.turb(s)));
    }

private:
    perlin noise;
    double scale;
    double freq;
};
#endif