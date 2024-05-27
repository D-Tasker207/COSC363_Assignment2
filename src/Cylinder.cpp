#include "Cylinder.h"

float Cylinder::intersect(glm::vec3 p0, glm::vec3 dir) {
    // if (aabb_.intersect(p0, dir) == -1.0) return -1.0;  // No intersection if ray does not intersect AABB

    glm::vec3 d = p0 - center;
    float a = dir.x * dir.x + dir.z * dir.z;
    float b = 2 * (dir.x * d.x + dir.z * d.z);
    float c = d.x * d.x + d.z * d.z - radius * radius;
    float disc = b * b - 4 * a * c;

    if (disc < 0) return -1.0;  // No intersection if discriminant is less than 0

    float t0 = (-b - sqrt(disc)) / (2 * a);
    float t1 = (-b + sqrt(disc)) / (2 * a);

    if (t0 > t1) std::swap(t0, t1);

    if (dir.y > 0) {
        // Bottom cap
        float tBottom = (center.y - p0.y) / dir.y;
        glm::vec3 pBottom = p0 + tBottom * dir;
        if (tBottom > 0 && (pBottom.x - center.x) * (pBottom.x - center.x) + (pBottom.z - center.z) * (pBottom.z - center.z) <= radius * radius) {
            return tBottom;
        }
    } else if (dir.y < 0) {
        // Top cap
        float tTop = (center.y + height - p0.y) / dir.y;
        glm::vec3 pTop = p0 + tTop * dir;
        if (tTop > 0 && (pTop.x - center.x) * (pTop.x - center.x) + (pTop.z - center.z) * (pTop.z - center.z) <= radius * radius) {
            return tTop;
        }
    }

    // Check intersection with cylinder caps
    if (t0 > 0) {
        glm::vec3 p = p0 + t0 * dir;
        if (p.y >= center.y && p.y <= center.y + height) {
            return t0;
        }
    }

    if (t1 > 0) {
        glm::vec3 p = p0 + t1 * dir;
        if (p.y >= center.y && p.y <= center.y + height) {
            return t1;
        }
    }

    // If no valid intersection, return -1
    return -1.0;
}

glm::vec3 Cylinder::normal(glm::vec3 p) {
    float epsilon = 1e-6; // Small value to handle floating-point precision issues

    // Check if the point is on the top or bottom cap
    if (fabs(p.y - (center.y + height)) < epsilon) {
        // Top cap normal
        return glm::vec3(0, 1, 0);
    } else if (fabs(p.y - center.y) < epsilon) {
        // Bottom cap normal
        return glm::vec3(0, -1, 0);
    } else {
        // Side surface normal
        glm::vec3 n = p - center;
        n.y = 0;
        return glm::normalize(n);
    }
}

void Cylinder::calculateAABB() {
    aabb_ = AABB(center - glm::vec3(radius, 0, radius), center + glm::vec3(radius, height, radius));
}