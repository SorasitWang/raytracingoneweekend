#ifndef MEDIUM_H
#define MEDIUM_H

#include "../common.h"

#include "Hittable.h"
#include "../material.h"
#include "../texture.h"

class ConstantMedium : public Hittable {
public:
    ConstantMedium(shared_ptr<Hittable> b, double d, shared_ptr<texture> a)
        : boundary(b), negInvDensity(-1 / d), phaseFunction(make_shared<Isotropic>(a))
    {}

    ConstantMedium(shared_ptr<Hittable> b, double d, color c)
        : boundary(b), negInvDensity(-1 / d), phaseFunction(make_shared<Isotropic>(c))
    {}

    bool hit(const ray& r, interval ray_t, HitRecord& rec) const override {
        // Print occasional samples when debugging. To enable, set enableDebug true.
        const bool enableDebug = false;
        const bool debugging = enableDebug && Util::random_double() < 0.000001;

        HitRecord rec1, rec2;

        if (!boundary->hit(r, interval(-Util::infinity, Util::infinity), rec1))
            return false;

        if (!boundary->hit(r, interval(rec1.t + 0.0001, Util::infinity), rec2))
            return false;

        if (debugging) std::clog << "\nray_tmin=" << rec1.t << ", ray_tmax=" << rec2.t << '\n';

        //  Clamp in ray interval
        if (rec1.t < ray_t.min) rec1.t = ray_t.min;
        if (rec2.t > ray_t.max) rec2.t = ray_t.max;

        if (rec1.t >= rec2.t)
            return false;

        if (rec1.t < 0)
            rec1.t = 0;

        auto rayLength = r.direction().length();
        auto distanceInsideBoundary = (rec2.t - rec1.t) * rayLength;
        //  Random'y determine the scatter point along the ray
        //  C * delta L
        auto hitDistance = negInvDensity * log(Util::random_double());

        if (hitDistance > distanceInsideBoundary)
            return false;

        rec.t = rec1.t + hitDistance / rayLength;
        rec.p = r.at(rec.t);

        if (debugging) {
            std::clog << "hit_distance = " << hitDistance << '\n'
                << "rec.t = " << rec.t << '\n'
                << "rec.p = " << rec.p << '\n';
        }

        rec.normal = vec3(1, 0, 0);  // arbitrary
        rec.frontFace = true;     // also arbitrary
        rec.mat = phaseFunction;

        return true;
    }

    aabb bounding_box() const override { return boundary->bounding_box(); }

private:
    shared_ptr<Hittable> boundary;
    double negInvDensity;
    shared_ptr<material> phaseFunction;
};

#endif