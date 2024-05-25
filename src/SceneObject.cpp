/*--------------------------------------------------------------
* COSC363  Ray Tracer
*
*  The SceneObject class
*  This is a generic type for storing objects in the scene
*  Sphere, Plane etc. must be defined as subclasses of SceneObject.
*  Being an abstract class, this class cannot be instantiated.
-----------------------------------------------------------------*/

#include "SceneObject.h"

glm::vec3 SceneObject::getColor(glm::vec3 hit) {
	return color_;
}

LightingResult SceneObject::lighting(glm::vec3 lightPos, glm::vec3 viewVec, glm::vec3 hit) {
	float ambient = 0.2;
	float specular = 0;
	glm::vec3 normalVec = normal(hit);
	glm::vec3 lightVec = lightPos - hit;
	lightVec = glm::normalize(lightVec);
	float lDotn = glm::dot(lightVec, normalVec);
	if (spec_) {
		glm::vec3 reflVec = glm::reflect(-lightVec, normalVec);
		float rDotv = glm::dot(reflVec, viewVec);
		if (rDotv > 0) specular = pow(rDotv, shin_);
	}
	glm::vec3 color = this->getColor(hit);

	glm::vec3 ambientColor = ambient * color;
	glm::vec3 diffuseColor = lDotn * color;
	glm::vec3 specularColor = specular * glm::vec3(1);

	return LightingResult{ ambientColor, diffuseColor, specularColor };
}

float SceneObject::getReflectionCoeff() {
	return reflc_;
}

float SceneObject::getRefractionCoeff() {
	return refrc_;
}

float SceneObject::getTransparencyCoeff() {
	return tranc_;
}

float SceneObject::getRefractiveIndex() {
	return refri_;
}

float SceneObject::getShininess() {
	return shin_;
}

bool SceneObject::isReflective() {
	return refl_;
}

bool SceneObject::isRefractive() {
	return refr_;
}


bool SceneObject::isSpecular() {
	return spec_;
}


bool SceneObject::isTransparent() {
	return tran_;
}

void SceneObject::setColor(glm::vec3 col) {
	color_ = col;
}

void SceneObject::setReflectivity(bool flag) {
	refl_ = flag;
}

void SceneObject::setReflectivity(bool flag, float refl_coeff) {
	refl_ = flag;
	reflc_ = refl_coeff;
}

void SceneObject::setRefractivity(bool flag) {
	refr_ = flag;
}

void SceneObject::setRefractivity(bool flag, float refr_coeff, float refr_index) {
	refr_ = flag;
	refrc_ = refr_coeff;
	refri_ = refr_index;
}

void SceneObject::setShininess(float shininess) {
	shin_ = shininess;
}

void SceneObject::setSpecularity(bool flag) {
	spec_ = flag;
}

void SceneObject::setTransparency(bool flag) {
	tran_ = flag;
}

void SceneObject::setTransparency(bool flag, float tran_coeff) {
	tran_ = flag;
	tranc_ = tran_coeff;
}

AABB SceneObject::getBBox() {
	return aabb_;
}

float SceneObject::intersectAABB(glm::vec3 p0, glm::vec3 dir) {
	return aabb_.intersect(p0, dir);
}
