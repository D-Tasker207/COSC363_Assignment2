/*----------------------------------------------------------
* COSC363  Ray Tracer
*
*  The Plane class
*  This is a subclass of SceneObject, and hence implements the
*  methods intersect() and normal().
-------------------------------------------------------------*/

#include "Plane.h"
#include <math.h>

/**
* Plane's intersection method.  The input is a ray (p0, dir).
* See slide Lec09-Slide 31
*/
float Plane::intersect(glm::vec3 p0, glm::vec3 dir) {
	glm::vec3 n = normal(p0);
	glm::vec3 vdif = a_ - p0;
	float d_dot_n = glm::dot(dir, n);
	if(fabs(d_dot_n) < 1.e-4) return -1;   //Ray parallel to the plane

    float t = glm::dot(vdif, n)/d_dot_n;
	if(t < 0) return -1;

	glm::vec3 q = p0 + dir*t; //Point of intersection
	if( isInside(q) ) return t; //Inside the plane
	else return -1; //Outside
}

/**
* Returns the unit normal vector at a given point.
* Assumption: The input point p lies on the plane.
*/
glm::vec3 Plane::normal(glm::vec3 p) {
	glm::vec3 v1 = c_-b_;
	glm::vec3 v2 = a_-b_;
	glm::vec3 n = glm::cross(v1, v2);
	n = glm::normalize(n);
    return n;
}

/**
* 
* Checks if a point q is inside the current polygon
* See slide Lec09-Slide 33
*/
bool Plane::isInside(glm::vec3 q) {
	glm::vec3 n = normal(q);     //Normal vector at the point of intersection
	glm::vec3 ua = b_ - a_, ub = c_ - b_, uc = d_ - c_, ud = a_ - d_;
	glm::vec3 va = q - a_, vb = q - b_, vc = q - c_, vd = q - d_;
	if (nverts_ == 3) uc = a_ - c_;
	float ka = glm::dot(glm::cross(ua, va), n);
	float kb = glm::dot(glm::cross(ub, vb), n);
	float kc = glm::dot(glm::cross(uc, vc), n);
	float kd;
	if (nverts_ == 4)
		kd = glm::dot(glm::cross(ud, vd), n);
	else
		kd = ka;
	if (ka > 0 && kb > 0 && kc > 0 && kd > 0) return true;
	if (ka < 0 && kb < 0 && kc < 0 && kd < 0) return true;
	else return false;
}

//Getter function for number of vertices
int  Plane::getNumVerts() {
	return nverts_;
}

// Set the axis-aligned bounding box for the sphere
void Plane::calculateAABB() {
	glm::vec3 minPoint, maxPoint;
	switch(nverts_) {
		case 3:
			minPoint = glm::min(a_, glm::min(b_, c_));
			maxPoint = glm::max(b_, glm::max(b_, c_));
			break;
		case 4:
			minPoint = glm::min(a_, glm::min(b_, glm::min(c_, d_)));
			maxPoint = glm::max(a_, glm::max(b_, glm::max(c_, d_)));
			break;
	}

	// check if any of the components are the same
	// if they are, add a small value to the min and max
	// to ensure the AABB is a valid 3d object
	if (minPoint.x == maxPoint.x) {
		minPoint.x -= 0.0001;
		maxPoint.x += 0.0001;
	}
	if (minPoint.y == maxPoint.y) {
		minPoint.y -= 0.0001;
		maxPoint.y += 0.0001;
	}
	if (minPoint.z == maxPoint.z) {
		minPoint.z -= 0.0001;
		maxPoint.z += 0.0001;
	}

    aabb_ = AABB(minPoint, maxPoint);
}

glm::vec3 Plane::getColor(glm::vec3 hit) {
	glm::vec3 color = color_;
	if (stripe_){	
		for(auto stripe : stripes_){
			float projection = glm::dot(hit, stripe.stripeDirection);
			int stripeIndex = ceil(projection / stripe.stripeWidth);
			int colorIndex = stripeIndex % stripe.stripeColors.size();
			if(colorIndex < 0) colorIndex += stripe.stripeColors.size();
			color = stripe.stripeColors[colorIndex];
		}
	}
	if (tex_) {
		float u = (hit.x - texA_.x) / (texB_.x - texA_.x);
		float v = (hit.z - texA_.y) / (texB_.y - texA_.y);
		if(u > 0 && u < 1 &&
		v > 0 && v < 1)
		{
			color=texture_.getColorAt(u, v);
		}
	}

	return color;
}

void Plane::setStripe(bool flag) {
	stripe_ = flag;
}

void Plane::addStripe(stripe_t stripe) {
	stripes_.push_back(stripe);
}

void Plane::setTextured(bool flag) {
	tex_ = flag;
}

void Plane::setTexture(TextureBMP file) {
	texture_ = file;
}

void Plane::setTexArea(glm::vec2 a, glm::vec2 b) {
	texA_ = a;
	texB_ = b;
}