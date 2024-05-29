#include "Cone.h"

float Cone::intersect(glm::vec3 p0, glm::vec3 dir) {
    // if(aabb_.intersect(p0, dir) == -1.0) return -1.0;
    dir = normalize(dir);

    float tanThetaSq = (radius / height) * (radius / height);
    glm::vec3 d = p0 - center;
    float a = dir.x * dir.x + dir.z * dir.z - dir.y * dir.y * tanThetaSq;
    float b = 2 * (dir.x * d.x + dir.z * d.z - dir.y * d.y * tanThetaSq);
    float c = d.x * d.x + d.z * d.z - d.y * d.y * tanThetaSq;
    float disc = b * b - 4 * a * c;

    if (disc < 1e-6) return -1.0;

    float t0 = (-b - sqrt(disc)) / (2 * a);
    float t1 = (-b + sqrt(disc)) / (2 * a);

    if (t0 > t1) std::swap(t0, t1);

    if (dir.y > 1e-6) {
        // Base Cap
        glm::vec3 cap_center = center - glm::vec3(0, height, 0);
        float t = (cap_center.y - p0.y) / dir.y;
        glm::vec3 p = p0 + t * dir;
        if (t > 0 && glm::distance(glm::vec2(p.x, p.z), glm::vec2(cap_center.x, cap_center.z)) <= radius) {
            return t;
        }
    }

    if (t0 > 0) {
        glm::vec3 p = p0 + t0 * dir;
        if (p.y <= center.y && p.y >= center.y - height) {
            return t0;
        }
    }

    if (t1 > 0) {
        glm::vec3 p = p0 + t1 * dir;
        if (p.y <= center.y && p.y >= center.y - height) {
            return t1;
        }
    }

    return -1.0;
}

glm::vec3 Cone::normal(glm::vec3 p) {
    float epsilon = 1e-6;

    // Check if the point is on the top cap
    if (fabs(p.y - center.y) < epsilon) {
        return glm::vec3(0, -1, 0);
    }

    glm::vec3 cp = p - center;
    float r = sqrt(cp.x * cp.x + cp.z * cp.z);
    glm::vec3 n = glm::vec3(cp.x, r * (radius / height), cp.z);

    return glm::normalize(n);
}


    


    

void Cone::calculateAABB() {
    aabb_ = AABB(glm::vec3(center.x - radius, center.y, center.z - radius), 
    glm::vec3(center.x + radius, center.y + height, center.z + radius));
}