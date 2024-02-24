#ifndef TRANSLATE_H
#define TRANSLATE_H

#include "Hittable.h"
#include "../common.h"
#include "../math/transform.h"

class trs : public Hittable {
public:
    trs(shared_ptr<Hittable> p, const TransformMatrix& m)
        : object(p), localToWorld(m)
    {
        localToWorld.inverse(worldToLocal);
        bbox = localToWorld(object->bounding_box());
    }

    trs(shared_ptr<Hittable> p)
        : object(p), localToWorld(), worldToLocal()
    {
        bbox = object->bounding_box();
    }

    bool hit(const ray& r, interval rayT, HitRecord& rec) const override {
        // Move the ray backwards by the offset
        ray rLocal = worldToLocal(ray(r.origin(), r.direction(), r.time()));
 
        // Determine where (if any) an intersection occurs along the offset ray
        if (!object->hit(rLocal, rayT, rec))
            return false;

        // Move the intersection point forwards by the offset
        rec.p = localToWorld(rec.p);

        return true;
    }

    aabb bounding_box() const override { return bbox; }

private:
    shared_ptr<Hittable> object;
    TransformMatrix worldToLocal, localToWorld;
    aabb bbox;
};

#endif