#ifndef POLYGON_H
#define POLYGON_H

#include <vector>

#include "hittable.h"
#include "../math/triangle.h"

class Polygon : public Hittable {
public:
    Polygon(std::vector<Triangle> _faces, shared_ptr<material> _material)
        : faces(_faces), mat(_material) {

        double minX = 10000 ,minY = 10000, minZ = 10000;
        double maxX = -minX, maxY = -minY, maxZ = -minZ;

        for (unsigned int i = 0; i < faces.size();++i) {
            for (unsigned int j = 0; j < 3; ++j) {
                minX = fmin(faces[i].vertices[j].x(), minX);
                maxX = fmax(faces[i].vertices[j].x(), maxX);

                minY = fmin(faces[i].vertices[j].y(), minY);
                maxY = fmax(faces[i].vertices[j].y(), maxY);

                minZ = fmin(faces[i].vertices[j].z(), minZ);
                maxZ = fmax(faces[i].vertices[j].z(), maxZ);
            }
        }
        bbox = aabb(point3(minX, minY, minZ), point3(maxX, maxY, maxZ));
    }

    bool hit(const ray& r, interval rayT, HitRecord& rec) const override {

        //  For each faces, check if ray is intersect or not
        double tMax = rayT.max;
        int hitFacesIdx = -1;

        for (unsigned int i = 0; i < faces.size(); ++i) {
            double t;
            if (!faces[i].hit_triangle(r, interval(rayT.min, tMax), t))
                continue;
            tMax = t;
            hitFacesIdx = i;
            
        }
        if (hitFacesIdx==-1)
            return false;

        rec.t = tMax;
        rec.p = r.at(tMax);
        vec3 outwardNormal = faces[hitFacesIdx].normal;
        rec.set_face_normal(r, outwardNormal);
        rec.mat = mat;

        return true;
    }

    aabb bounding_box() const override { return bbox; }

private:
    std::vector<Triangle> faces;
    shared_ptr<material> mat;
    aabb bbox;
};


#endif