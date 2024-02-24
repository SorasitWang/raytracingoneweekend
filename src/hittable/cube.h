#ifndef CUBE_H
#define CUBE_H

#include "hittable.h"
#include "../math/plane.h"

class Cube : public Hittable {
public:
    Cube(point3 _center, vec3 _size, shared_ptr<material> _material)
        : center(_center), size(_size), mat(_material) {

        //  planes = [ +x, -x, +y, -y, +z, -z ]
        planes[0] = Plane(center + vec3(size.x() / 2, 0, 0), vec3(-1, 0, 0));
        planes[1] = Plane(center + vec3(-size.x() / 2, 0, 0), vec3(1, 0, 0));

        planes[2] = Plane(center + vec3(0, size.y() / 2, 0), vec3(0, -1, 0));
        planes[3] = Plane(center + vec3(0, -size.y() / 2, 0), vec3(0, 1, 0));

        planes[4] = Plane(center + vec3(0, 0, size.z() / 2), vec3(0, 0, -1));
        planes[5] = Plane(center + vec3(0, 0, -size.z() / 2), vec3(0, 0, 1));

        bbox = aabb(_center - _size/2, _center + _size/2);
    }

    ~Cube() {
        std::cout << "del" << std::endl;
        //delete[] planes;
        std::cout << "dell" << std::endl;
    }
    bool hit(const ray& r, interval rayT, HitRecord& rec) const override {

        //  For each planes, check if ray is intersect or not
        double tMax = rayT.max;
        int hitPlaneIndex = -1;
        point3 intersectPoint;
        for (unsigned int i = 0; i < 6; ++i) {
            double t;
            if (! planes[i].hit_plane(r, interval(rayT.min, tMax), t))
                continue;
          
            intersectPoint = r.at(t);

            // if intersectPoint is on cube surface
            if (intersectPoint.x() <= planes[0].point.x() + Util::epsilon
                && intersectPoint.x() >= planes[1].point.x() - Util::epsilon
                && intersectPoint.y() <= planes[2].point.y() + Util::epsilon
                && intersectPoint.y() >= planes[3].point.y() - Util::epsilon
                && intersectPoint.z() <= planes[4].point.z() + Util::epsilon
                && intersectPoint.z() >= planes[5].point.z() - Util::epsilon) {
               
                hitPlaneIndex = i;
                //  Clamp tMin so that we can get the first (nearest) planes that ray inersects
                tMax = t;
            }
        }

        if (hitPlaneIndex == -1)
            return false;
        
        rec.t = tMax;
        rec.p = intersectPoint;
        vec3 outward_normal = planes[hitPlaneIndex].normal;
        rec.set_face_normal(r, outward_normal);
        rec.mat = mat;

        return true;
    }

    aabb bounding_box() const override { return bbox; }

private:
    point3 center;
    vec3 size;
    shared_ptr<material> mat;
    Plane planes[6];
    aabb bbox;
};


#endif