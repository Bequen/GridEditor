#include "Math.h"

bool Math::ray_box_intersection(const Ray* ray, glm::vec3 begin, glm::vec3 end) {
    // X Axis
    float txmin = (begin.x - ray->origin.x) / ray->direction.x;
    float txmax = (end.x - ray->origin.x) / ray->direction.x;

    if(txmin > txmax)
        std::swap(txmin, txmax);

    // Y Axis
    float tymin = (begin.y - ray->origin.y) / ray->direction.y;
    float tymax = (end.y - ray->origin.y) / ray->direction.y;

    if(tymin > tymax)
        std::swap(tymin, tymax);

    if((txmin > tymax) || (tymin > txmax)) 
        return false;

    float tmin = 0.0f, tmax = 0.0f;
    if(tymin > txmin)
        tmin = tymin;
    else
        tmin = txmin;

    if(tymax < txmax)
        tmax = tymax;
    else
        tmax = txmax;

    float tzmin = (begin.z - ray->origin.z) / ray->direction.z;
    float tzmax = (end.z - ray->origin.z) / ray->direction.z;

    if(tzmin > tzmax)
        std::swap(tzmin, tzmax);

    if((tmin > tzmax) || (tzmin > tmax))
        return false;

    if(tzmin > tmin)
        tmin = tzmin;
    
    if(tzmax < tmax)
        tmax = tzmax;
    
    return true;
}