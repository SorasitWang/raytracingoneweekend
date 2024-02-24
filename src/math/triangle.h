#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "../common.h"

#include "plane.h"

class Triangle  {
public:

    point3 vertices[3];
    vec3 normal;
    Plane plane;

    Triangle(point3 a, point3 b, point3 c) {

        // a-b-c order in counter-clockwise
        vertices[0] = a;
        vertices[1] = b;
        vertices[2] = c;

        normal = unit_vector(cross(b - a, c - a));
        plane = Plane(a, normal);
    }

    Triangle(point3 v[3], vec3 _normal) : normal(_normal){

        // a-b-c order in counter-clockwise
        vertices[0] = v[0];
        vertices[1] = v[1];
        vertices[2] = v[2];

        plane = Plane(vertices[0], normal);
    }

    Triangle(point3 v[3]){

        // a-b-c order in counter-clockwise
        vertices[0] = v[0];
        vertices[1] = v[1];
        vertices[2] = v[2];

        normal = unit_vector(cross(vertices[1] - vertices[0], vertices[2] - vertices[0]));
        plane = Plane(vertices[0], normal);
    }

    ~Triangle() {
        delete[] vertices;
    }
    bool hit_triangle(const ray& r, interval ray_t, double& t) const {
        // Test if ray not (almost) parallel with triangle
        double _t;
        if (!plane.hit_plane(r, ray_t, _t))
            return false;

        point3 intersectPoint = r.at(_t);

        // Test if intersect point is inside triangle
        intersectPoint.approx_zero();

        if (dot(cross(vertices[1] - vertices[0], intersectPoint - vertices[0]), plane.normal) >= 0
            && dot(cross(vertices[2] - vertices[1], intersectPoint - vertices[1]), plane.normal) >= 0
            && dot(cross(vertices[0] - vertices[2], intersectPoint - vertices[2]), plane.normal) >= 0) {

            t = _t;
            return true;
        }
        return false;
    };

    static Triangle create_equilaterial_triangle(point3 center, double length) {

        point3 points[3];

        points[0] = point3(0, 0.577 * length, 0) + center;
        points[1] = point3(-length / 2, -length * 0.289, 0) + center;
        points[2] = point3(length / 2, -length * 0.289, 0) + center;

        Triangle result(points[0], points[1], points[2]);

        return result;
    };
};

#endif