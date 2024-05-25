#include "AABB.h"
#include "glm/glm.hpp"
#include <vector>

/*
 * Returns the cloest, positive 
 * intersection of the ray with the AABB
*/
float AABB::intersect(glm::vec3 p0, glm::vec3 dir) {
    float t1 = (min.x - p0.x) / dir.x;
    float t2 = (max.x - p0.x) / dir.x;
    float t3 = (min.y - p0.y) / dir.y;
    float t4 = (max.y - p0.y) / dir.y;
    float t5 = (min.z - p0.z) / dir.z;
    float t6 = (max.z - p0.z) / dir.z;

    float tmin = std::max(std::max(std::min(t1, t2), std::min(t3, t4)), std::min(t5, t6));
    float tmax = std::min(std::min(std::max(t1, t2), std::max(t3, t4)), std::max(t5, t6));

    if (tmax < 0 || tmin > tmax) {
        return -1.0f; // No intersection
    }

    return (tmin < 0) ? tmax : tmin;
}
