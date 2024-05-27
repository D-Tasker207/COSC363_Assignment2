#ifndef BVHNODE_H
#define BVHNODE_H

#include "AABB.h"
#include "SceneObject.h"

class BVHNode {
    public:
        BVHNode() : leaf(false), index(0), numObjects(0) {}
        BVHNode(unsigned int index, unsigned int numObjects) : leaf(false), index(index), numObjects(numObjects) {}
        void setAABB(const AABB& bbox) { this->bbox = bbox; }
        void setIndex(unsigned int index) { this->index = index; }
        void setNumObjects(unsigned int numObjects) { this->numObjects = numObjects; }
        void makeLeaf() { leaf = true; }
        unsigned int getNumObjects() { return numObjects; }
        unsigned int getIndex() { return index; }
        AABB getBBox() { return bbox; }
        bool isLeaf() { return leaf; }
        BVHNode* getLeft() { return left; }
        BVHNode* getRight() { return right; }
        void setLeft(BVHNode* left) { this->left = left; }
        void setRight(BVHNode* right) { this->right = right; }
    private:
        AABB bbox;
        bool leaf;
        unsigned int numObjects;
        unsigned int index;

        BVHNode *left;
        BVHNode *right;
};

#endif