/*--------------------------------------------------------------
* COSC363  Ray Tracer
* CSSE, University of Canterbury.
*
*  The SceneObject class
*  This is a generic type for storing objects in the scene.
*  Being an abstract class, this class cannot be instantiated.
*  Sphere, Plane etc, must be defined as subclasses of SceneObject
*      and provide implementations for the virtual functions
*      intersect()  and normal().
-----------------------------------------------------------------*/

#ifndef H_SOBJECT
#define H_SOBJECT
#include <glm/glm.hpp>
#include <vector>
#include "AABB.h"

typedef struct {
	glm::vec3 ambient, diffuse, specular;
} LightingResult;

class SceneObject {
protected:
	virtual void calculateAABB() = 0;

	int id_;

	AABB aabb_; //Axis-aligned bounding box
	glm::vec3 color_ = glm::vec3(1);  //material color
	bool refl_ = false;  //reflectivity: true/false
	bool refr_ = false;  //refractivity: true/false
	bool spec_ = true;   //specularity: true/false
	bool tran_ = false;  //transparency: true/false
	float reflc_ = 0.0;  //coefficient of reflection
	float refrc_ = 1.0;  //coefficient of refraction
	float tranc_ = 1.0;  //coefficient of transparency
	float refri_ = 1.0;  //refractive index
	float shin_ = 50.0; //shininess



public:
	SceneObject() {}
    virtual float intersect(glm::vec3 p0, glm::vec3 dir) = 0;
	virtual glm::vec3 normal(glm::vec3 pos) = 0;
	virtual ~SceneObject() {}

	virtual LightingResult lighting(glm::vec3 lightPos, glm::vec3 viewVec, glm::vec3 hit);
	virtual glm::vec3 getColor(glm::vec3 hit);
	
	void setId(int id) { id_ = id; }
	int getId() { return id_; }
	float intersectAABB(glm::vec3 p0, glm::vec3 dir);
	void setColor(glm::vec3 col);
	void setReflectivity(bool flag);
	void setReflectivity(bool flag, float refl_coeff);
	void setRefractivity(bool flag);
	void setRefractivity(bool flag, float refr_coeff, float refr_indx);
	void setShininess(float shininess);
	void setSpecularity(bool flag);
	void setTransparency(bool flag);
	void setTransparency(bool flag, float tran_coeff);
	void setStripe(bool flag);
	void addStripeColor(glm::vec3 color);
	void setStripeWidth(int width);
	void setStripeDirection(glm::vec3 direction);
	AABB getBBox();
	float getReflectionCoeff();
	float getRefractionCoeff();
	float getTransparencyCoeff();
	float getRefractiveIndex();
	float getShininess();
	bool isStripe();
	bool isReflective();
	bool isRefractive();
	bool isSpecular();
	bool isTransparent();
};

#endif