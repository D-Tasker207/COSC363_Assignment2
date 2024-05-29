#ifndef CONE_H
#define CONE_H

#include "SceneObject.h"
#include <glm/glm.hpp>

class Cone : virtual public SceneObject {
private:
    glm::vec3 center;
    float radius;
    float height;
protected:
    void calculateAABB() override;
public:
    Cone() : center(glm::vec3(0)), radius(1), height(1) { calculateAABB(); };
    Cone(glm::vec3 c, float r, float h) : center(c), radius(r), height(h) { calculateAABB(); };

    float intersect(glm::vec3 p0, glm::vec3 dir) override;
    glm::vec3 normal(glm::vec3 p) override;
    
};

#endif