#ifndef HITTABLE_H
#define HITTABLE_H
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

#include "../common.h"
#include "aabb.h"
#include "math/transform.h"
class material;


class HitRecord {
public:
    point3 p;
    vec3 normal;
    shared_ptr<material> mat;
    double t;
    double u, v;
    bool frontFace;
 
    void set_face_normal(const ray& r, const vec3& outwardNormal) {
        // Sets the hit record normal vector.
        // NOTE: the parameter `outwardNormal` is assumed to have unit length.

        frontFace = dot(r.direction(), outwardNormal) < 0;
        normal = frontFace ? outwardNormal : -outwardNormal;
    }
};


class Hittable {
public:
    virtual ~Hittable() = default;

    virtual bool hit(const ray& r, interval rayT, HitRecord& rec) const = 0;

    virtual aabb bounding_box() const = 0;
};


#endif