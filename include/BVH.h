#ifndef BVH_H
#define BVH_H

#include <vector>
#include "SceneObject.h"
#include "BVHNode.h"

#define LEAF_OBJ_THRESHOLD 2

struct RayHit{
    int objIdx = -1;
    float dist = -1.0f;
    glm::vec3 hit; 
    int numIntersections;
};

class BVH {
    public:
        BVH(std::vector<SceneObject*> *sceneObjects);
        struct RayHit intersect(glm::vec3 p0, glm::vec3 dir);

        void printNodes();
        // void printGraph();
    private:
        void buildRecursive(BVHNode *node);
        AABB unionAABB(const AABB& a, const AABB& b);
        void printNode(BVHNode *node, unsigned int &index);
        
        std::vector<SceneObject*> *sceneObjects;
        BVHNode *head;
};

#endif