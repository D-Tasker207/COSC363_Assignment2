/*----------------------------------------------------------
* COSC363  Ray Tracer
*
*  The Plane class
*  This is a subclass of SceneObject, and hence implements the
*  methods intersect() and normal().
-------------------------------------------------------------*/

#ifndef H_PLANE
#define H_PLANE

#include <glm/glm.hpp>
#include "TextureBMP.h"
#include "SceneObject.h"

typedef struct {
	int stripeWidth;
	glm::vec3 stripeDirection;
	std::vector<glm::vec3> stripeColors;
} stripe_t;

class Plane : public virtual SceneObject
{
private:
	glm::vec3 a_ = glm::vec3(0);   //The vertices of the quad
	glm::vec3 b_ = glm::vec3(0);
	glm::vec3 c_ = glm::vec3(0);
	glm::vec3 d_ = glm::vec3(0);
	int nverts_ = 4;				//Number of vertices (3 or 4)

	bool stripe_ = false; //stripe pattern: true/false
	std::vector<stripe_t> stripes_; // stripe struct vector

	bool tex_ = false;
	glm::vec2 texA_ = glm::vec3(0);   //The texture coordinates of the quad
	glm::vec2 texB_ = glm::vec3(0);
	TextureBMP texture_;
protected:
	void calculateAABB() override;

public:	
	Plane() = default;
	
	Plane(glm::vec3 pa, glm::vec3 pb, glm::vec3 pc, glm::vec3 pd) : 
		a_(pa), b_(pb), c_(pc), d_(pd), nverts_(4) { calculateAABB(); }

	Plane(glm::vec3 pa, glm::vec3 pb, glm::vec3 pc) :
		a_(pa), b_(pb), c_(pc),  nverts_(3) { calculateAABB(); }


	bool isInside(glm::vec3 pt);
	int getNumVerts();
	
	float intersect(glm::vec3 posn, glm::vec3 dir) override;
	glm::vec3 normal(glm::vec3 pt) override;
	glm::vec3 getColor(glm::vec3 hit) override;

	void setStripe(bool flag);
	void addStripe(int stripeWidth, glm::vec3 stripeDirection, std::vector<glm::vec3> stripeColors);
	bool isStripe() { return stripe_; }

	void setTextured(bool flag);
	void setTexture(TextureBMP file);
	void setTexArea(glm::vec2 a, glm::vec2 b);
	bool isTextured() { return tex_; }

};

#endif //!H_PLANE
