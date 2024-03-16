#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <ctime>

#define STB_IMAGE_IMPLEMENTATION

#include "common.h"
#include "camera.h"
#include "material.h"

#include "hittable/sphere.h"
#include "hittable/cube.h"
#include "hittable/hittableList.h"
#include "hittable/bvh.h"
#include "hittable/translate.h"
#include "hittable/medium.h"
#include "tool/objectReader.h"

#include "math/transform.h"

#include "texture.h"

Camera cam;

void setupCamera() {
    cam.vup = vec3(0, 1, 0);
    cam.background = color(0.70, 0.80, 1.00);

    cam.aspect_ratio = 16.0 / 9.0;
    cam.image_width = 400;
    cam.samples_per_pixel = 10;
}


void earth(std::ofstream& out ) {
    auto earth_texture = make_shared<image_texture>("textures/earthmap.jpg");
    auto earth_surface = make_shared<lambertian>(earth_texture);
    auto globe = make_shared<Sphere>(point3(0, 0, 0), 2, earth_surface);

    cam.max_depth = 10;
    cam.vfov = 20;
    cam.lookfrom = point3(0, 0, 12);
    cam.lookat = point3(0, 0, 0);

    cam.render(out, HittableList(globe));
}


void randomSpheres( std::ofstream &out) {

    HittableList world;

    auto material_ground = make_shared<lambertian>(color(0.8, 0.8, 0.0));
    auto material_center = make_shared<lambertian>(color(0.1, 0.2, 0.5));
    auto material_left = make_shared<dielectric>(1.5);
    auto material_right = make_shared<metal>(color(0.8, 0.6, 0.2), 0.0);

    try {
        Polygon polygon = Reader::read_obj_file("./res/cube1.obj", material_center);
        TransformMatrix tMat;
        tMat.translate(vec3(0, 1, 0.2));
        tMat.rotate_y(0.5);
        tMat.scale(vec3(1.2, 1.2, 1.2));
        world.add(make_shared<trs>(make_shared<Polygon>(polygon), tMat));
    }
    catch (std::runtime_error error) {
        std::cout << error.what() << std::endl;
        return;
    }

    auto checker = make_shared<checker_texture>(0.32, color(.2, .3, .1), color(.9, .9, .9));
    world.add(make_shared<Sphere>(point3(0, -1000, 0), 1000, make_shared<lambertian>(checker)));
    world.add(make_shared<Sphere>(point3(0.0, -102, -1.0), 100.0, material_ground));
    Sphere s = Sphere(point3(0.0, 0.0, -1.0), 0.5, material_center);
    TransformMatrix tMat;
    tMat.translate(vec3(0, 1, 0));
   
    world.add(make_shared<trs>(make_shared<Sphere>(s), tMat));
    world.add(make_shared<Sphere>(point3(0.0, 0.0, -1.0), 0.5, material_center));

    world.add(make_shared<Sphere>(point3(-1.0, 0.0, -1.0), 0.5, material_left));
    world.add(make_shared<Sphere>(point3(-1.0, 0.0, -1.0), -0.4, material_left));
    world.add(make_shared<Sphere>(point3(1.0, 0.0, -1.0), 0.5, material_right));

    for (int a = -5; a < 5; a++) {
        for (int b = -1; b < 1; b++) {
            auto choose_mat = Util::random_double();
            point3 center(a + 0.9 * Util::random_double(), 0.2, b + 0.9 * Util::random_double());

            if ((center - point3(4, 0.2, 0)).length() > 0.9) {
                shared_ptr<material> sphere_material;

                if (choose_mat < 0.5) {
                    // diffuse
                    auto albedo = color::random() * color::random();
                    sphere_material = make_shared<lambertian>(albedo);
                    auto center2 = center + vec3(0, Util::random_double(0, .5), 0);
                    world.add(make_shared<Sphere>(std::vector<point3>({ center, center2 }), 0.2, sphere_material));
                }
                else {
                    // metal
                    sphere_material = make_shared<metal>(color::random(),0.3);
                    auto center2 = center + vec3(0, Util::random_double(0, .5), 0);
                    world.add(make_shared<Sphere>(std::vector<point3>({ center, center2 }), 0.2, sphere_material));
                }
            }
        }
    }

    //  Render

    cam.max_depth = 2;
    cam.samplingMethod = SamplingMethod::SuperSampling;

    cam.vfov = 90;
    cam.lookfrom = point3(1.5, 2, 2);
    cam.lookat = point3(0, 0, -1);

    cam.focus_dist = 3.4;
    cam.shutter_duration = 2;

    cam.render(out, world);
}


void twoSpheres(std::ofstream& out) {

    HittableList world;

    auto material_ground = make_shared<lambertian>(color(0.8, 0.8, 0.0));
    auto material_center = make_shared<lambertian>(color(0.1, 0.2, 0.5));
    auto material_left = make_shared<dielectric>(1.5);
    auto material_right = make_shared<metal>(color(0.8, 0.6, 0.2), 0.0);

    try {
        Polygon polygon = Reader::read_obj_file("./res/cube1.obj", material_center);
        world.add(make_shared<Polygon>(polygon));
    }
    catch (std::runtime_error error) {
        std::cout << error.what() << std::endl;
        return;
    }

    auto checker = make_shared<checker_texture>(0.32, color(.2, .3, .1), color(.9, .9, .9));
    world.add(make_shared<Sphere>(point3(0, -1000, 0), 1000, make_shared<lambertian>(checker)));
 
    cam.max_depth = 2;

    cam.vfov = 90;
    cam.lookfrom = point3(1.5, 2, 2);
    cam.lookat = point3(0, 0, -1);

    cam.focus_dist = 3.4;
    cam.shutter_duration = 2;

    cam.render(out, world);

}

void two_perlin_spheres(std::ofstream& out) {
    HittableList world;

    auto pertext = make_shared<NoiseTexture>(4,0.5);
    world.add(make_shared<Sphere>(point3(0, -1000, 0), 1000, make_shared<lambertian>(pertext)));
    world.add(make_shared<Sphere>(point3(0, 2, 0), 2, make_shared<lambertian>(pertext)));
    
    cam.max_depth = 20;

    cam.vfov = 20;
    cam.lookfrom = point3(13, 2, 3);
    cam.lookat = point3(0, 0, 0);

    cam.render(out,world);
}

void cornell_smoke(std::ofstream& out) {
    HittableList world;

    auto red = make_shared<lambertian>(color(.65, .05, .05));
    auto white = make_shared<lambertian>(color(.73, .73, .73));
    auto green = make_shared<lambertian>(color(.12, .45, .15));
    //auto light = make_shared<diffuse_light>(color(7, 7, 7));

    /*world.add(make_shared<quad>(point3(555, 0, 0), vec3(0, 555, 0), vec3(0, 0, 555), green));
    world.add(make_shared<quad>(point3(0, 0, 0), vec3(0, 555, 0), vec3(0, 0, 555), red));
    world.add(make_shared<quad>(point3(113, 554, 127), vec3(330, 0, 0), vec3(0, 0, 305), light));
    world.add(make_shared<quad>(point3(0, 555, 0), vec3(555, 0, 0), vec3(0, 0, 555), white));
    world.add(make_shared<quad>(point3(0, 0, 0), vec3(555, 0, 0), vec3(0, 0, 555), white));
    world.add(make_shared<quad>(point3(0, 0, 555), vec3(555, 0, 0), vec3(0, 555, 0), white));*/

    shared_ptr<Hittable> box1 = make_shared<Cube>(vec3(265,0,295), point3(165, 330, 165), green);
 
    shared_ptr<Hittable> box2 = make_shared<Cube>(point3(20, 0, 0), point3(5,5,5), white);

    //world.add(make_shared<Cube>(point3(0, 0, 0), point3(1000, 1000, 1000), red));
    world.add(make_shared<ConstantMedium>(box1, 0.1, color(1, 1, 1)));
    //world.add(make_shared<ConstantMedium>(box2, 0.01, color(1, 1, 1)));

    cam.max_depth = 40;

    cam.vfov = 40;
    cam.lookfrom = point3(278, 278, -800);
    cam.lookat = point3(278, 278, 0);

    cam.render(out, world);
}

void simple_light(std::ofstream& out) {
    HittableList world;

    auto pertext = make_shared<NoiseTexture>(4,3);
    world.add(make_shared<Sphere>(point3(0, -1000, 0), 1000, make_shared<lambertian>(pertext)));
    world.add(make_shared<Sphere>(point3(0, 2, 0), 2, make_shared<lambertian>(pertext)));

    auto difflight = make_shared<DiffuseLight>(color(4, 4, 4));
    world.add(make_shared<Cube>(point3(3, 1, -2), vec3(2, 2, 2), difflight));

    cam.max_depth = 20;

    cam.vfov = 20;
    cam.lookfrom = point3(26, 3, 6);
    cam.lookat = point3(0, 2, 0);

    cam.render(out, world);
}

int main() {

    string imageNameList[] = {
        "twoSpheres",
        "randomSpheres",
        "earth",
        "two_perlin_spheres",
        "simple_light",
        "cornell_smoke"
    };
    unsigned int numImage = 6;

    for (unsigned int i = 1; i <= numImage; i++) {
        std::cout << i << " : " << imageNameList[i-1] << std::endl;
    }

    unsigned int id = 1;
    string input;
    std::cin >> input;

    id = stoi(input);

    if ( id < 1 || id > numImage)
        throw std::runtime_error( "Image ID out of range" );
    
    string imagePath = "./output/" + imageNameList[id-1] + ".ppm";

    std::ofstream out(imagePath);

    time_t start = time(NULL);

    setupCamera();

    switch (id) {
        case 1: twoSpheres(out);            break;
        case 2: randomSpheres(out);         break;
        case 3: earth(out);                 break;
        case 4: two_perlin_spheres(out);    break;
        case 5: simple_light(out);          break;
        case 6: cornell_smoke(out);         break;
    }
    
    out.close();
   
    time_t spendTime = time(NULL) - start;

    std::cout << "Total time used " << spendTime << std::endl;

}
