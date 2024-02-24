#ifndef SPHERE_H
#define SPHERE_H
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

#include "hittable.h"

class Sphere : public Hittable {
public:
    Sphere(point3 _center, double _radius, shared_ptr<material> _material)
        : radius(_radius), mat(_material), isMoving(false) {

        centers.push_back(_center);
        auto rVec = vec3(radius, radius, radius);
        bbox = aabb(_center - rVec, _center + rVec);
    }

    // Moving Sphere
    Sphere(std::vector<point3> _centers, double _radius, shared_ptr<material> _material)
        : centers(_centers), radius(_radius), mat(_material), isMoving(true) {
    
        auto rVec = vec3(radius, radius, radius);
        bbox = aabb(_centers[0] - rVec, _centers[0] + rVec);

        for (unsigned int i = 1; i < _centers.size(); ++i) {
            bbox = aabb(bbox, aabb(_centers[i] - rVec, _centers[i] + rVec));
        }    
    }

    bool hit(const ray& r, interval rayT, HitRecord& rec) const override {

        // Convert to local space
        ray rLocal = worldToLocal(r);

        point3 center = isMoving ? this->center(rLocal.time()) : centers[0];
        vec3 oc = rLocal.origin() - center;
        auto a = rLocal.direction().length_squared();
        auto half_b = dot(oc, rLocal.direction());
        auto c = oc.length_squared() - radius * radius;

        auto discriminant = half_b * half_b - a * c;
        if (discriminant < 0)
            return false;

        // Find the nearest root that lies in the acceptable range.
        auto sqrtd = sqrt(discriminant);
        auto root = (-half_b - sqrtd) / a;
        // convert root to world space

        if (!rayT.surrounds(root)) {
            root = (-half_b + sqrtd) / a;
            if (!rayT.surrounds(root))
                return false;
        }

        rec.t = root;
        rec.p = rLocal.at(rec.t);
        vec3 outward_normal = (rec.p - center) / radius;
        rec.set_face_normal(rLocal, outward_normal);
        rec.mat = mat;
        get_sphere_uv(outward_normal, rec.u, rec.v);

        return true;
    }

    aabb bounding_box() const override { return bbox; }

    static void get_sphere_uv(const point3& p, double& u, double& v) {
        // p: a given point on the sphere of radius one, centered at the origin.
        // u: returned value [0,1] of angle around the Y axis from X=-1.
        // v: returned value [0,1] of angle from Y=-1 to Y=+1.
        //     <1 0 0> yields <0.50 0.50>       <-1  0  0> yields <0.00 0.50>
        //     <0 1 0> yields <0.50 1.00>       < 0 -1  0> yields <0.50 0.00>
        //     <0 0 1> yields <0.25 0.50>       < 0  0 -1> yields <0.75 0.50>

        auto theta = acos(-p.y());
        auto phi = atan2(-p.z(), p.x()) + Util::pi;

        u = phi * Util::invPi / 2.0;
        v = theta * Util::invPi;
    }

private:

    double radius;
    shared_ptr<material> mat;
    bool isMoving;
    std::vector<point3> centers;
    aabb bbox;
    TransformMatrix worldToLocal, localToWorld;

    point3 center(double time) const {
        // Linearly interpolate from center1 to center2 according to time, where t=0 yields
        // center1, and t=1 yields center2.
        if (time >= centers.size()-1)
            return centers.at(centers.size() - 1);
        else if (time < 0)
            return centers.at(0);
        unsigned int timeInt = floor(time);
       
        return centers[timeInt] + (time - timeInt) * ( centers[timeInt + 1]- centers[timeInt]);
    }
};


#endif