# Ray Tracing One Weekend
Additional implementation from Peter Shirley : "Ray tracing in one weekend" and "The next week"

## New
1. Primitive geomertry class : **Plane** and **Triangle** for computing ray hit in **Hittable**.
2. **Polygon** hittable class contructed by reading .obj file by **objectReader**.
3. **Cube** and **Triangle** hittable class .


### Improvement
- Use transform matrix with *scale*, *transform* and *rotate* interface instead of wrapping Object with **Transfrom** Hittable.
- Actual write .ppm file.
- More sampling texture method : *Normal*, *SuperSampling* and *AdaptiveSuperSampling*.
- More interpolation method for noise generator : *Perlin*.
- More image...
    - wrap method : *Repeat*, *MirroedRepea*t, *ClampToEdge*, *ClampToBorder*.
    - interpolation method : *Nearest*, *Linear*.
- Implement 2 rays scattering in **Dielectric** material, with coefficient for each ray.