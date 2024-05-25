#ifndef AABB_H
#define AABB_H

#include <glm/glm.hpp>

class AABB {
    public:
        AABB() : min(glm::vec3(0)), max(glm::vec3(0)) {}
        AABB(glm::vec3 min, glm::vec3 max) : min(min), max(max) {}
        float intersect(glm::vec3 p0, glm::vec3 dir);
        void setAABB(glm::vec3 min, glm::vec3 max) { this->min = min; this->max = max; }
        glm::vec3 getMin() const { return min; }
        glm::vec3 getMax() const { return max; }
        glm::vec3 getCenter() const { return (min + max) / 2.0f; }
    private:
        glm::vec3 min;
        glm::vec3 max;
};

#endif