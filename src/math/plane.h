#ifndef PLANE_H
#define PLANE_H

#include "../common.h"

class Plane {

public:

    point3 point;
    vec3 normal;

    Plane() {};

    Plane(point3 _point, vec3 _normal) : point(_point), normal(_normal) {}

    ~Plane() {};

    bool hit_plane(const ray& r, interval rayT, double& t) const {

        // assuming vectors are all normalized
        float denom = dot(this->normal, r.direction());
       
        if (fabs(denom) > Util::epsilon) {
            vec3 p0l0 = this->point - r.origin();
            t = dot(p0l0, this->normal) / denom;
            return rayT.surrounds(t);
        }

        return false;
    }
};

#endif