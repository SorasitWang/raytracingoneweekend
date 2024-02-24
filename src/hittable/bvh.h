#ifndef BVH_H
#define BVH_H

#include <algorithm>

#include "../common.h"

#include "Hittable.h"
#include "HittableList.h"


class bvhNode : public Hittable {
public:
    bvhNode(const HittableList& list) : bvhNode(list.objects, 0, list.objects.size()) {}

    bvhNode(const std::vector<shared_ptr<Hittable>>& srcObjectVec, size_t start, size_t end) {
        auto objects = srcObjectVec; // Create a modifiable array of the source scene objects

        int axis = Util::random_int(0, 2);
        auto comparator = (axis == 0) ? box_x_compare
            : (axis == 1) ? box_y_compare
            : box_z_compare;

        size_t object_span = end - start;

        if (object_span == 1) {
            left = right = objects[start];
        }
        else if (object_span == 2) {
            if (comparator(objects[start], objects[start + 1])) {
                left = objects[start];
                right = objects[start + 1];
            }
            else {
                left = objects[start + 1];
                right = objects[start];
            }
        }
        else {
            std::sort(objects.begin() + start, objects.begin() + end, comparator);

            auto mid = start + object_span / 2;
            left = make_shared<bvhNode>(objects, start, mid);
            right = make_shared<bvhNode>(objects, mid, end);
        }

        bbox = aabb(left->bounding_box(), right->bounding_box());
    }

    bool hit(const ray& r, interval rayT, HitRecord& rec) const override {
        if (!bbox.hit(r, rayT))
            return false;
        
        bool hit_left = left->hit(r, rayT, rec);

        bool hit_right = right->hit(r, interval(rayT.min, hit_left ? rec.t : rayT.max), rec);

        return hit_left || hit_right;
    }

    aabb bounding_box() const override { return bbox; }

private:
    shared_ptr<Hittable> left;
    shared_ptr<Hittable> right;
    aabb bbox;

    static bool box_compare(
        const shared_ptr<Hittable> a, const shared_ptr<Hittable> b, int axisIndex
    ) {
        return a->bounding_box().axis(axisIndex).min < b->bounding_box().axis(axisIndex).min;
    }

    static bool box_x_compare(const shared_ptr<Hittable> a, const shared_ptr<Hittable> b) {
        return box_compare(a, b, 0);
    }

    static bool box_y_compare(const shared_ptr<Hittable> a, const shared_ptr<Hittable> b) {
        return box_compare(a, b, 1);
    }

    static bool box_z_compare(const shared_ptr<Hittable> a, const shared_ptr<Hittable> b) {
        return box_compare(a, b, 2);
    }
};

#endif