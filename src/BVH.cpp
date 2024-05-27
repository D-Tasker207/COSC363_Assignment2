#include "BVH.h"
#include "UniqueStack.h"
#include "BVHNode.h"
#include <glm/glm.hpp>
#include <iostream>
using namespace std;

BVH::BVH(std::vector<SceneObject*> *sceneObjects) : sceneObjects(sceneObjects) {
    // nodes = new BVHNode[sceneObjects->size() * 2];

    head = new BVHNode(0, (*sceneObjects).size());
    AABB worldBBox;

    worldBBox = (*sceneObjects)[0]->getBBox();
    for (int i = 1; i < sceneObjects->size(); i++) {
            worldBBox = unionAABB(worldBBox, (*sceneObjects)[i]->getBBox());
    }

    head->setAABB(worldBBox);
    // head.setIndex(0);
    // head.setNumObjects((*sceneObjects).size());

    buildRecursive(head);
}

void BVH::buildRecursive(BVHNode *node) {
    // set number of objects in nodeidx and index of first object
    unsigned int lidx = node->getIndex();
    unsigned int ridx = node->getIndex() + node->getNumObjects();

    if(ridx - lidx <= LEAF_OBJ_THRESHOLD) {
        // if number of objects between lidx and ridx is less than or equal to LEAF_OBJ_THRESHOLD, make nodeidx a leaf node
        node->makeLeaf();
        // printNode(nodeidx);
        return;
    } else{
        node->setLeft(new BVHNode());
        node->setRight(new BVHNode());
    }

    // find largest axis for node's bounding box
    glm::vec3 min = node->getBBox().getMin();
    glm::vec3 max = node->getBBox().getMax();
    glm::vec3 size = max - min;
    int axis = 0;
    if (size.y > size.x) axis = 1;
    if (size.z > size.y && size.z > size.x) axis = 2;

    // sort scene objects between lidx and ridx based on axis
    std::stable_sort((*sceneObjects).begin() + lidx, (*sceneObjects).begin() + ridx, [axis](SceneObject* a, SceneObject* b) {
        return a->getBBox().getCenter()[axis] < b->getBBox().getCenter()[axis];
    });
    
    // find midpoint of axis, then use binary search to find the first object that has a center greater than the midpoint
    float midpoint = (min[axis] + max[axis]) / 2;
    unsigned int mid = lidx;
    unsigned int left = lidx;
    unsigned int right = ridx - 1;
    while (left <= right) {
        int mid = (left + right) / 2;
        if ((*sceneObjects)[mid]->getBBox().getCenter()[axis] > midpoint) {
            right = mid - 1;
        } else {
            left = mid + 1;
        }
    }
    mid = left;

    // if mid is equal to lidx or ridx, then all objects are on one side of the midpoint and use midpoint as the split
    if (mid == lidx || mid == ridx) {
        mid = (lidx + ridx) / 2;
    }   

    // calculate bounding box for left and right splits
    AABB leftBBox = (*sceneObjects)[lidx]->getBBox();
    AABB rightBBox = (*sceneObjects)[mid]->getBBox();
    for (int i = lidx + 1; i < mid; i++) {
        leftBBox = unionAABB(leftBBox, (*sceneObjects)[i]->getBBox());
    }
    for (int i = mid + 1; i < ridx; i++) {
        rightBBox = unionAABB(rightBBox, (*sceneObjects)[i]->getBBox());
    }

    // printNode(nodeidx);
    // set bounding boxes for left and right nodes
    node->getLeft()->setAABB(leftBBox);
    node->getRight()->setAABB(rightBBox);

    // set index and number of objects for left and right nodes
    node->getLeft()->setIndex(lidx);
    node->getLeft()->setNumObjects(mid - lidx);
    node->getRight()->setIndex(mid);
    node->getRight()->setNumObjects(ridx - mid);

    // recursively build left and right nodes using left and right splits
    buildRecursive(node->getLeft());
    buildRecursive(node->getRight());
}

struct RayHit BVH::intersect(glm::vec3 p0, glm::vec3 dir) {
    UniqueStack<BVHNode*> stack; // stack of node indices to check collision for
    stack.push(head);

    struct RayHit hit;

    int numIntersections = 0;
    while (!stack.empty()) {
        // cout << "Stack size: " << stack.size() << endl;
        BVHNode* node = stack.pop();

        float bboxIntersection = node->getBBox().intersect(p0, dir);
        numIntersections++;

        // if the ray does not intersect the current node's bounding box, skip the node
        // or if closestHit is closer than the current node's bounding box, skip the node
        if((bboxIntersection < 0) || (hit.dist >= 0 && bboxIntersection > hit.dist)){
            continue;
        }

        // if the node is not a leaf node, add the left and right children to the stack
        if (!node->isLeaf()) {
            stack.push(node->getLeft());
            stack.push(node->getRight());
            continue;
        }

        // if the node is a leaf node, check for intersection with each object in the node
        float tmin = 1.e+6;
        for (size_t i = node->getIndex(); i < node->getIndex() + node->getNumObjects(); i++) {
            float t = (*sceneObjects)[i]->intersect(p0, dir);
            numIntersections++;
            if (t > 0 && t < tmin) {
                tmin = t;
                hit.dist = t;
                hit.objIdx = i;
                hit.hit = p0 + dir * t;
            }
        }
    }

    hit.numIntersections = numIntersections;
    
    return hit;
}

AABB BVH::unionAABB(const AABB &a, const AABB &b) {
    glm::vec3 min = glm::min(a.getMin(), b.getMin());
    glm::vec3 max = glm::max(a.getMax(), b.getMax());
    return AABB(min, max);
}

void BVH::printNode(BVHNode *node, unsigned int &index) {
    std::cout << "Node " << index++ << ": ";
    if (node->isLeaf()) {
        std::cout << "Leaf node with " << node->getNumObjects() << " objects ";
        std::cout << "Min: (" << node->getBBox().getMin().x << ", " << node->getBBox().getMin().y << ", " << node->getBBox().getMin().z << "), ";
        std::cout << "Max: (" << node->getBBox().getMax().x << ", " << node->getBBox().getMax().y << ", " << node->getBBox().getMax().z << ") ";
        std::cout << "Contains Objects from: " << node->getIndex() << " to " << node->getIndex() + node->getNumObjects() - 1 << std::endl;
    } else {
        std::cout << "Internal node with bounding box: ";
        std::cout << "Min: (" << node->getBBox().getMin().x << ", " << node->getBBox().getMin().y << ", " << node->getBBox().getMin().z << "), ";
        std::cout << "Max: (" << node->getBBox().getMax().x << ", " << node->getBBox().getMax().y << ", " << node->getBBox().getMax().z << ") ";
        std::cout << "Contains Objects from: " << node->getIndex() << " to " << node->getIndex() + node->getNumObjects() - 1 << std::endl;
        printNode(node->getLeft(), index);
        printNode(node->getRight(), index);
    }
}

void BVH::printNodes() {
    unsigned int index = 0;
    printNode(head, index);
}