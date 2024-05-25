#include "BVH.h"
#include "UniqueStack.h"
#include "BVHNode.h"
#include <glm/glm.hpp>
#include <iostream>
using namespace std;

BVH::BVH(std::vector<SceneObject*> *sceneObjects) : sceneObjects(sceneObjects) {
    nodes = new BVHNode[sceneObjects->size() * 2 - 1];

    AABB worldBBox;

    for (int i = 0; i < sceneObjects->size(); i++) {
        if (i == 0) {
            worldBBox = (*sceneObjects)[i]->getBBox();
        } else {
            worldBBox = unionAABB(worldBBox, (*sceneObjects)[i]->getBBox());
        }
    }

    nodes[0].setAABB(worldBBox);
    nodes[0].setIndex(0);
    nodes[0].setNumObjects((*sceneObjects).size());

    buildRecursive(0, 0, (*sceneObjects).size());
}

void BVH::buildRecursive(unsigned int nodeidx, unsigned int lidx, unsigned int ridx) {
    // if number of objects between lidx and ridx is less than or equal to LEAF_OBJ_THRESHOLD, make nodeidx a leaf node
    if(ridx - lidx <= LEAF_OBJ_THRESHOLD) {
        nodes[nodeidx].makeLeaf(lidx, ridx - lidx);
        return;
    }

    // set number of objects in nodeidx and index of first object
    nodes[nodeidx].setIndex(lidx);
    nodes[nodeidx].setNumObjects(ridx - lidx);

    // find largest axis for nodeidx's bounding box
    glm::vec3 min = nodes[nodeidx].getBBox().getMin();
    glm::vec3 max = nodes[nodeidx].getBBox().getMax();
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

    // set bounding boxes for left and right nodes
    nodes[nodeidx * 2 + 1].setAABB(leftBBox);
    nodes[nodeidx * 2 + 2].setAABB(rightBBox);

    // recursively build left and right nodes using left and right splits
    buildRecursive(nodeidx * 2 + 1, lidx, mid);
    buildRecursive(nodeidx * 2 + 2, mid, ridx);
}

struct RayHit BVH::intersect(glm::vec3 p0, glm::vec3 dir) {
    UniqueStack<unsigned int> stack; // stack of node indices to check collision for
    stack.push(0);

    struct RayHit hit;

    int numIntersections = 0;
    while (!stack.empty()) {
        // cout << "Stack size: " << stack.size() << endl;
        unsigned int idx = stack.pop();
        BVHNode &node = nodes[idx];

        float bboxIntersection = node.getBBox().intersect(p0, dir);
        numIntersections++;

        // if the ray does not intersect the current node's bounding box, skip the node
        // or if closestHit is closer than the current node's bounding box, skip the node
        if((bboxIntersection < 0) || (hit.dist >= 0 && bboxIntersection > hit.dist)){
            continue;
        }

        // if the node is not a leaf node, add the left and right children to the stack
        if (!node.isLeaf()) {
            stack.push(idx * 2 + 1);
            stack.push(idx * 2 + 2);
            continue;
        }

        // if the node is a leaf node, check for intersection with each object in the node
        float tmin = 1.e+6;
        for (size_t i = node.getIndex(); i < node.getIndex() + node.getNumObjects(); i++) {
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

void BVH::printNode(int index) {
    std::cout << "Node " << index << ": ";
    if (nodes[index].isLeaf()) {
        std::cout << "Leaf node with " << nodes[index].getNumObjects() << " objects ";
        glm::vec3 min = nodes[index].getBBox().getMin();
        glm::vec3 max = nodes[index].getBBox().getMax();
        std::cout << "Min: (" << min.x << ", " << min.y << ", " << min.z << "), ";
        std::cout << "Max: (" << max.x << ", " << max.y << ", " << max.z << ") ";
        std::cout << "Contains Objects from: " << nodes[index].getIndex() << " to " << nodes[index].getIndex() + nodes[index].getNumObjects() - 1 << std::endl;
    } else {
        std::cout << "Internal node with bounding box: ";
        glm::vec3 min = nodes[index].getBBox().getMin();
        glm::vec3 max = nodes[index].getBBox().getMax();
        std::cout << "Min: (" << min.x << ", " << min.y << ", " << min.z << "), ";
        std::cout << "Max: (" << max.x << ", " << max.y << ", " << max.z << ") ";
        std::cout << "Contains Objects from: " << nodes[index].getIndex() << " to " << nodes[index].getIndex() + nodes[index].getNumObjects() - 1 << std::endl;
    }
}

void BVH::printNodes() {
    for (int i = 0; i < (*sceneObjects).size() * 2 - 1; i++) {
        if(nodes[i].getNumObjects() > 0)
            printNode(i);
    }
}

void BVH::printGraph(){
    for (int i = 0; i < (*sceneObjects).size() * 2 -1; i++){
        if(nodes[i].getNumObjects() == 0)
            continue;

        if (!nodes[i].isLeaf()){
            cout << "\"" << nodes[i].getIndex() << "-" << nodes[i].getNumObjects() + nodes[i].getIndex() - 1 << "\"" << " -> ";
            cout << "\"" << nodes[i * 2 + 1].getIndex() << "-" << nodes[i * 2 + 1].getNumObjects() + nodes[i * 2 + 1].getIndex() - 1 << "\"" << endl;
            cout << "\"" << nodes[i].getIndex() << "-" << nodes[i].getNumObjects() + nodes[i].getIndex() - 1 << "\"" << " -> ";
            cout << "\"" << nodes[i * 2 + 2].getIndex() << "-" << nodes[i * 2 + 2].getNumObjects() + nodes[i * 2 + 2].getIndex() - 1 << "\"" << endl;
        }
    }
}