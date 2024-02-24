#ifndef QUAD_H
#define QUAD_H

#include "../common.h"

#include "hittable.h"
#include "../math/plane.h"

class Quad : public Hittable {
public:

    Quad(point3 center, double w, double h, shared_ptr<material> _material)
        : mat(_material) {

        // a-b-c order in counter-clockwise
        vertices[0] = a;
        vertices[1] = b;
        vertices[2] = c;

        vec3 normal = unit_vector(cross(b - a, c - a));
        plane = Plane(a, -1 * normal);
    }

    bool hit(const ray& r, interval rayT, HitRecord& rec) const override {
        // Test if ray not (almost) parallel with triangle
        double t;
        if (!plane.hit_plane(r, rayT, t))
            return false;

        point3 intersectPoint = r.at(t);

        // Round those closed to 0
        intersectPoint.approx_zero();
        // Test if intersect point is inside triangle
        if (dot(cross(vertices[1] - vertices[0], intersectPoint - vertices[0]), plane.normal) <= 0
            && dot(cross(vertices[2] - vertices[1], intersectPoint - vertices[1]), plane.normal) <= 0
            && dot(cross(vertices[0] - vertices[2], intersectPoint - vertices[2]), plane.normal) <= 0) {

            rec.t = t;
            rec.p = intersectPoint;
            vec3 outwardNormal = plane.normal;
            rec.set_face_normal(r, outwardNormal);
            rec.mat = mat;

            return true;
        }
        return false;
    };

    aabb bounding_box() const override { return bbox; }


private:

    shared_ptr<material> mat;
    point3 vertices[3];
    Plane plane;
    aabb bbox;
};

#endif