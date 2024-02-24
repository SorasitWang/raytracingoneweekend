#ifndef HITTABLE_LIST_H
#define HITTABLE_LIST_H
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

#include <memory>
#include <vector>

#include "hittable.h"
#include "aabb.h"

class HittableList : public Hittable {
public:
    std::vector<shared_ptr<Hittable>> objects;

    HittableList() {}
    HittableList(shared_ptr<Hittable> object) { add(object); }

    void clear() { objects.clear(); }

    void add(shared_ptr<Hittable> object) {
        objects.push_back(object);
        bbox = aabb(bbox, object->bounding_box());
    }

    bool hit(const ray& r, interval rayT, HitRecord& rec) const override {
        HitRecord tempRec;
        auto hitAnything = false;
        auto currentClosest = rayT.max;

        for (const auto& object : objects) {
            if (object->hit(r, interval(rayT.min, currentClosest), tempRec)) {
                hitAnything = true;
                currentClosest = tempRec.t;
                rec = tempRec;
            }
        }

        return hitAnything;
    }

    aabb bounding_box() const override { return bbox; }

private:
    aabb bbox;
};


#endif