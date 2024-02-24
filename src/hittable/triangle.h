#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "../common.h"

#include "hittable.h"
#include "../math/plane.h"

class Triangle : public Hittable {
public:

    Triangle( point3 a, point3 b, point3 c, shared_ptr<material> _material)
        : mat(_material) {

        // a-b-c order in counter-clockwise
        vertices[0] = a;
        vertices[1] = b;
        vertices[2] = c;

        vec3 normal = unit_vector( cross(b-a, c-a) );
        plane = Plane( a, -1*normal );

        constructAabb();
    }

    Triangle(point3 _vertices[3], vec3 normal, shared_ptr<material> _material)
        : mat(_material) {

        // a-b-c order in counter-clockwise
        vertices[0] = _vertices[0];
        vertices[1] = _vertices[1];
        vertices[2] = _vertices[2];

        plane = Plane(vertices[0], -1 *normal);
        constructAabb();
    }

    Triangle(point3 _vertices[3], shared_ptr<material> _material)
        : mat(_material) {

        // a-b-c order in counter-clockwise
        vertices[0] = _vertices[0];
        vertices[1] = _vertices[1];
        vertices[2] = _vertices[2];

        vec3 normal = unit_vector(cross(vertices[1] - vertices[0], vertices[2] - vertices[0]));
        plane = Plane(vertices[0], -1 * normal);
        constructAabb();
    }

    void constructAabb() {

        point3 minPoint = point3(fmin(fmin(vertices[0].x(), vertices[1].x()), vertices[2].x()),
            fmin(fmin(vertices[0].y(), vertices[1].y()), vertices[2].y()),
            fmin(fmin(vertices[0].z(), vertices[1].z()), vertices[2].z())
        );

        point3 maxPoint = point3(fmax(fmax(vertices[0].x(), vertices[1].x()), vertices[2].x()),
            fmax(fmax(vertices[0].y(), vertices[1].y()), vertices[2].y()),
            fmax(fmax(vertices[0].z(), vertices[1].z()), vertices[2].z())
        );

        bbox = aabb(minPoint, maxPoint);
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

    static Triangle create_equilaterial_triangle(point3 center, double length, shared_ptr<material> material) {

        point3 points[3];

        points[0] = point3(0, 0.577 * length, 0) + center;
        points[1] = point3(-length / 2, -length * 0.289, 0) + center;
        points[2] = point3(length / 2, -length * 0.289, 0) + center;

        Triangle result(points[0], points[1], points[2], material);

        return result;
    };

private:

    shared_ptr<material> mat;
    point3 vertices[3];
    Plane plane;
    aabb bbox;
};

#endif