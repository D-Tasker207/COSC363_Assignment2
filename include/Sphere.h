/*----------------------------------------------------------
* COSC363  Ray Tracer
*
*  The sphere class
*  This is a subclass of SceneObject, and hence implements the
*  methods intersect() and normal().
-------------------------------------------------------------*/

#ifndef H_SPHERE
#define H_SPHERE
#include <glm/glm.hpp>
#include "SceneObject.h"

/**
 * Defines a simple Sphere located at 'center'
 * with the specified radius
 */
class Sphere : public virtual SceneObject {
private:
    glm::vec3 center = glm::vec3(0);
    float radius = 1;
protected:
	void calculateAABB() override;
public:
	Sphere() { calculateAABB(); };  //Default constructor creates a unit sphere
	Sphere(glm::vec3 c, float r) : center(c), radius(r) { calculateAABB(); };

	float intersect(glm::vec3 p0, glm::vec3 dir) override;
	glm::vec3 normal(glm::vec3 p) override;

};

#endif //!H_SPHERE
