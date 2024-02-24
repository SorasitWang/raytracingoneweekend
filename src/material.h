#ifndef MATERIAL_H
#define MATERIAL_H

#include <vector>

#include "common.h"
#include "texture.h"

#include "hittable/hittableList.h"

class HitRecord;

struct ScatteredRay {
    double coeff;
    ray r;

    ScatteredRay(ray _r) : coeff(1.0), r(_r) {};

    ScatteredRay(double _coeff, ray _r) : coeff(_coeff), r(_r) {};
};

typedef std::vector < ScatteredRay > ScatteredRays;

class material {
public:
    virtual ~material() = default;

    virtual bool scatter(
        const ray& r_in, const HitRecord& rec, color& attenuation, ScatteredRays& scattered) const = 0;

    virtual color emitted(double u, double v, const point3& p) const {
        return color(0, 0, 0);
    }
};

class plain : public material {
public:
    plain(const color& a) : plainColor(a) {}

    bool scatter(const ray& r_in, const HitRecord& rec, color& attenuation, ScatteredRays& scattered) const override {

        attenuation = plainColor;

        return false;
    }

private:
    color plainColor;

};

class lambertian : public material {
  public:
    lambertian(const color& a) : albedo(make_shared<solid_color>(a)) {}
    lambertian(shared_ptr<texture> a) : albedo(a) {}

    bool scatter(const ray& r_in, const HitRecord& rec, color& attenuation, ScatteredRays& scattered)
    const override {
        auto scatter_direction = rec.normal + random_unit_vector();

        // Catch degenerate scatter direction
        if (scatter_direction.near_zero())
            scatter_direction = rec.normal;

        scattered = ScatteredRays{ ScatteredRay(ray(rec.p, scatter_direction, r_in.time())) };
        attenuation = albedo->value(rec.u, rec.v, rec.p);
        return true;
    }

  private:
    shared_ptr<texture> albedo;
};

class metal : public material {
public:
    metal(const color& a, double f) : albedo(a), fuzz(f < 1 ? f : 1) {}

    bool scatter(const ray& r_in, const HitRecord& rec, color& attenuation, ScatteredRays& scattered)
        const override {
        vec3 reflected = reflect(unit_vector(r_in.direction()), rec.normal);
        //  TODO : another method other than fuzz?
        vec3 direction = reflected + fuzz * random_unit_vector();
        scattered = ScatteredRays{ ScatteredRay(ray(rec.p, direction, r_in.time())) };
        attenuation = albedo;
        //  Check if scattered direction is inside surface ( according to fuzz direction )
        return (dot(direction, rec.normal) > 0);
    }

private:
    color albedo;
    float fuzz;
};

class dielectric : public material {
public:
    dielectric(double index_of_refraction) : ir(index_of_refraction), isRoughlyScattering(true) {}

    bool scatter(const ray& r_in, const HitRecord& rec, color& attenuation, std::vector<ScatteredRay>& scattered)
        const override {
        attenuation = color(1.0, 1.0, 1.0);
        //  Why?
        double refraction_ratio = rec.frontFace ? (1.0 / ir) : ir;

        vec3 unit_direction = unit_vector(r_in.direction());
        double cos_theta = fmin(dot(-unit_direction, rec.normal), 1.0);
        double sin_theta = sqrt(1.0 - cos_theta * cos_theta);

        bool canRefract = refraction_ratio * sin_theta <= 1.0;

        double fresnelReflectance = schlickApprox(cos_theta, refraction_ratio);

        if (isRoughlyScattering) {
            vec3 direction;
            if (!canRefract || fresnelReflectance > 0.5)
                direction = reflect(unit_direction, rec.normal);
            else
                direction = refract(unit_direction, rec.normal, refraction_ratio);

            scattered = ScatteredRays{ ScatteredRay(ray(rec.p, direction, r_in.time())) };
        }
        else {
            vec3 reflectDirection = reflect(unit_direction, rec.normal);
            scattered = ScatteredRays{ ScatteredRay(!canRefract? 1.0 : fresnelReflectance, 
                ray(rec.p, reflectDirection, r_in.time())) };

            if (canRefract) {
                vec3 refractDirection = refract(unit_direction, rec.normal, refraction_ratio);
                scattered.push_back( ScatteredRay(1.0 - fresnelReflectance, ray(rec.p, refractDirection,
                    r_in.time())) );
            }
        }
       
        return true;
    }

private:
    double ir; // Index of Refraction
    bool isRoughlyScattering ;

    static double schlickApprox(double cosine, double ref_idx) {
        // Use Schlick's approximation for reflectance.
        auto r0 = (1 - ref_idx) / (1 + ref_idx);
        r0 = r0 * r0;
        //  1 is the index's refraction of medium
        double oneMinusCos = 1 - cosine;
        return r0 + (1 - r0) * oneMinusCos * oneMinusCos * oneMinusCos * oneMinusCos * oneMinusCos;
    }
};

/*
    Direction invariant, equally scatter in all direction (represent by random vector)
*/
class Isotropic : public material {
public:
    Isotropic(color c) : albedo(make_shared<solid_color>(c)) {}
    Isotropic(shared_ptr<texture> a) : albedo(a) {}

    bool scatter(const ray& r_in, const HitRecord& rec, color& attenuation, std::vector<ScatteredRay>& scattered)
        const override {
        scattered = ScatteredRays{ ScatteredRay(ray(rec.p, random_unit_vector(), r_in.time())) };
        attenuation = albedo->value(rec.u, rec.v, rec.p);
        return true;
    }

private:
    shared_ptr<texture> albedo;
};

class DiffuseLight : public material {
public:
    DiffuseLight(shared_ptr<texture> a) : emit(a) {}
    DiffuseLight(color c) : emit(make_shared<solid_color>(c)) {}

    bool scatter(const ray& r_in, const HitRecord& rec, color& attenuation, ScatteredRays& scattered)
        const override {
        return false;
    }

    color emitted(double u, double v, const point3& p) const override {
        return emit->value(u, v, p);
    }

private:
    shared_ptr<texture> emit;
};

#endif