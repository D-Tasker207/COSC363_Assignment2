/*==================================================================================
* COSC 363  Computer Graphics
* Department of Computer Science and Software Engineering, University of Canterbury.
*
* A basic ray tracer
* See Lab07.pdf   for details.
*===================================================================================
*/

#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#endif

#include <iostream>
#include <mutex>
#include <thread>
#include <cmath>
#include <sys/time.h>
#include <vector>
#include <glm/glm.hpp>
#include <GL/freeglut.h>
#include "FilePath.h"
#include "SceneObject.h"
#include "Plane.h"
#include "Cylinder.h"
#include "Sphere.h"
#include "RayBatchFactory.h"
#include "Ray.h"
#include "BVH.h"
using namespace std;

const int NUM_THREADS = 25;
const bool ENABLE_BVH = false;
const bool PRINT_RAY_DEBUG = false; // enabling this will increase frame draw time significantly due to thread synchronization
const bool PRINT_FRAME_TIME = true;
const float EDIST = 25.0;
const int NUMDIV = 800;
const long TOTAL_RAYS = NUMDIV * NUMDIV;
const int MAX_STEPS = 5;
const float XMIN = -10.0;
const float XMAX = 10.0;
const float YMIN = -10.0;
const float YMAX = 10.0;

int frameCount = 0;
float frameTime = 0.0f;
struct timeval lastTime;

std::vector<int> numRayIntersections;

std::mutex numRayIntersectionsMutex;

std::vector<SceneObject*> sceneObjects;
BVH *bvh;

int raysPerThread = TOTAL_RAYS / NUM_THREADS;
std::thread threads[NUM_THREADS];
RayBatches *rayBatches;

// Function to calculate time difference in milliseconds
float getTimeDifference(struct timeval *start, struct timeval *end) {
    return (end->tv_sec - start->tv_sec) * 1000.0f + (end->tv_usec - start->tv_usec) / 1000.0f;
}

//---The most important function in a ray tracer! ---------------------------------- 
//   Computes the colour value obtained by tracing a ray and finding its 
//     closest point of intersection with objects in the scene.
//----------------------------------------------------------------------------------
glm::vec3 trace(Ray ray, int eta_1, int step) {
	glm::vec3 backgroundCol(0);						//Background colour = (0,0,0)
	glm::vec3 lightPos(10, 30, -3);					//Light's position
	glm::vec3 color(0);
	glm::vec3 shadowColor(0);
	glm::vec3 reflectedColor(0);
	glm::vec3 transmissiveColor(0);

	SceneObject* obj;

	bool isShadow = false;
	int numIntersections = 0;

	//If number of objects in scene is greater than threshold, 
	// use BVH to find closest intersection instead of linear search
	if(ENABLE_BVH)
		numIntersections += ray.closestPt(*bvh);
	else
    	numIntersections += ray.closestPt(sceneObjects);
    if(ray.index == -1) return backgroundCol;		//no intersection
	obj = sceneObjects[ray.index];					//object on which the closest point of intersection is found

	//Object's colour
	LightingResult result = obj->lighting(lightPos, -ray.dir, ray.hit);
	color = result.ambient + result.diffuse;

	// Shadow calculation
	// OK so the current problem is that the shadow ray is only intersecting the first surface it contacts with
	// regardless of transparency whereas if an object is transparent the shadow ray should be attenuated and then
	// continue to the next object and so on until it reaches the light source or is stopped by a completely opaque object
	// SO change this to recursivey trace the shadow ray until it reaches the light source or is stopped by an opaque object
	// or the maximum number of steps is reached
	// Because shadows are subtractive, tracing in either direction would yield the same color value hence we can start the shadow
	// color as the light source's color and attenuate it as we go along so the final ray color as it has a clean shot to the light
	// is the shadow color
	
	glm::vec3 lightVec = lightPos - ray.hit;
	Ray shadowRay(ray.hit, lightVec);
	if(ENABLE_BVH)
		numIntersections += shadowRay.closestPt(*bvh);
	else
    	numIntersections += shadowRay.closestPt(sceneObjects);

	if(shadowRay.index > -1 && shadowRay.dist < glm::length(lightVec)) {
		isShadow = true;
		SceneObject* shadowObj = sceneObjects[shadowRay.index];
		if(shadowObj->isTransparent() && !(shadowRay.index == ray.index)) {
			float shadowAlpha = (1 - shadowObj->getTransparencyCoeff());
			color = (0.2f + 0.7f * shadowAlpha) * color;
		} else if(shadowObj->isRefractive() && !(shadowRay.index == ray.index)){
			float shadowAlpha = (1 - shadowObj->getRefractionCoeff());
			color = (0.2f + 0.7f * shadowAlpha) * color;
		} else {
			color = result.ambient;
		}
	}

	if (obj->isReflective() && step < MAX_STEPS) {
		// Reflection calculation
		float rho = obj->getReflectionCoeff();
		glm::vec3 normalVec = obj->normal(ray.hit);
		if(glm::dot(ray.dir, normalVec) < 0.0f){
			glm::vec3 reflectedDir = glm::reflect(ray.dir, normalVec);
			Ray reflectedRay(ray.hit, reflectedDir);
			reflectedColor = trace(reflectedRay, obj->getRefractiveIndex(), step + 1);
			color = (1-rho) * color + rho * reflectedColor;
		}
	}

	if(obj->isTransparent() && step < MAX_STEPS) {
		// Transparency calculation
		float alpha = obj->getTransparencyCoeff();
		Ray transparencyRay(ray.hit, ray.dir);
		transmissiveColor = trace(transparencyRay, obj->getRefractiveIndex(), step + 1);
		color = (alpha * color) + ((1 - alpha) * transmissiveColor);
	} else if(obj->isRefractive() && step < MAX_STEPS) {
		// Refraction calculation
		float alpha = obj->getRefractionCoeff();
		float eta_2 = obj->getRefractiveIndex();
		glm::vec3 n = obj->normal(ray.hit);
		(glm::dot(ray.dir, n) > 0) ? n = -n : n = n;
		glm::vec3 g = glm::refract(glm::normalize(ray.dir), n, eta_1 / eta_2);
		Ray refractedRay(ray.hit, g);
		transmissiveColor = trace(refractedRay, eta_2, step + 1);
		color = (alpha * color) + ((1 - alpha) * transmissiveColor);
	}

	if(PRINT_RAY_DEBUG){
		// Lock the mutex before accessing the shared data
		std::lock_guard<std::mutex> lock(numRayIntersectionsMutex);
		numRayIntersections.push_back(numIntersections);
	}

	// don't add specular component if object is in shadow
	// adding it after all other calculatinos ensures it's brightness 
	// is preseved through transparency and reflection calculations
	return (isShadow) ? color : color + result.specular;
}

void printFrameTime() {
	// Increment frame count
    frameCount++;

	// Calculate frame time
    struct timeval currentTime;
    gettimeofday(&currentTime, NULL);
    float deltaTime = getTimeDifference(&lastTime, &currentTime);
    frameTime += deltaTime;
    lastTime = currentTime;
    
    // Update frame time every second
    if (frameTime >= 1000.0f) {
        printf("Average frame time: %.2f ms\n", frameTime / frameCount);
        frameTime = 0.0f;
        frameCount = 0;
    }
}

void printRayDebug() {
	// Calculate average number of ray intersections
	int totalIntersections = 0;
	for (int i = 0; i < numRayIntersections.size(); i++) {
		totalIntersections += numRayIntersections[i];
	}
	cout << "Total Intersection Tests per Frame: " << totalIntersections << endl;
	cout << "Average Intersection Tests per Ray per Frame: " << static_cast<float>(totalIntersections / numRayIntersections.size()) << endl;
	numRayIntersections.clear();
}

void rayTraceThread(RayWrapper *rays, size_t numRays) {
	for(int i = 0; i < numRays; i++) {
		rays[i].col = trace(*(rays[i].ray), 1, 1); //Trace the primary ray and get the colour value
	}
	return;
}

//---The main display module -----------------------------------------------------------
// In a ray tracing application, it just displays the ray traced image by drawing
// each cell as a quad.
//---------------------------------------------------------------------------------------
void display() {
	float xp, yp;  //grid point
	float cellX = (XMAX - XMIN) / NUMDIV;  //cell width
	float cellY = (YMAX - YMIN) / NUMDIV;  //cell height
	glm::vec3 eye(0., 0., 0.);

	glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

	for (int i = 0; i < NUMDIV; i++)	//Scan every cell of the image plane
	{
		xp = XMIN + i * cellX;
		for (int j = 0; j < NUMDIV; j++)
		{
			yp = YMIN + j * cellY;

			glm::vec3 dir(xp + 0.5 * cellX, yp + 0.5 * cellY, -EDIST);	//direction of the primary ray

			RayWrapper* wrappedRay = rayBatches[(i * NUMDIV + j) / raysPerThread].rays + ((i * NUMDIV + j) % raysPerThread);

			wrappedRay->xp = xp;
			wrappedRay->yp = yp;
			wrappedRay->ray = new Ray(eye, dir);
		}
	}

	for (size_t i = 0; i < NUM_THREADS; i++) {
		threads[i] = std::thread(rayTraceThread, rayBatches[i].rays, rayBatches[i].numRays);
	}

	glBegin(GL_QUADS);  //Each cell is a tiny quad.

	for (size_t i = 0; i < NUM_THREADS; i++) {
		threads[i].join();
		for (int j = 0; j < rayBatches[i].numRays; j++){
			glColor3f(rayBatches[i].rays[j].col.r, rayBatches[i].rays[j].col.g, rayBatches[i].rays[j].col.b);
			glVertex2f(rayBatches[i].rays[j].xp, rayBatches[i].rays[j].yp);
			glVertex2f(rayBatches[i].rays[j].xp + cellX, rayBatches[i].rays[j].yp);
			glVertex2f(rayBatches[i].rays[j].xp + cellX, rayBatches[i].rays[j].yp + cellY);
			glVertex2f(rayBatches[i].rays[j].xp, rayBatches[i].rays[j].yp + cellY);
		}
	}

	glEnd();
    glutSwapBuffers();

	if(PRINT_FRAME_TIME) printFrameTime();
	if(PRINT_RAY_DEBUG) printRayDebug();
}

void drawCircles(const int numSpheres, const bool useRandomPlacement) {
	for(int i = 0; i < numSpheres; i++) {
		if(useRandomPlacement) {
			float x = (rand() % 20) - 10;
			float y = (rand() % 20) - 10;
			float z = (rand() % 20) - 10;
			float r = static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * 5.0f;
			Sphere *sphere = new Sphere(glm::vec3(x, y, -70 + z), r);
			sphere->setId(i);
			sphere->setColor(glm::vec3((rand() % 100) / 100.0f, (rand() % 100) / 100.0f, (rand() % 100) / 100.0f));
			sceneObjects.push_back(sphere);
		}else{
			const int rows = ceil(sqrt(numSpheres));
			const int cols = floor(sqrt(numSpheres));
			const float xSpacing = (XMAX - XMIN) / (cols + 1);
			const float ySpacing = (YMAX - YMIN) / (rows + 1);

			//space spheres out evenly and set size so all are visible
			int rowIndex = i / cols;
    		int colIndex = i % cols;

			float x = XMIN + (colIndex + 1) * xSpacing;
    		float y = YMIN + (rowIndex + 1) * ySpacing;
			float z = -40;
			float r = 0.5f;
			Sphere *sphere = new Sphere(glm::vec3(x, y, z), r);
			sphere->setId(i);
			sphere->setColor(glm::vec3((rand() % 100) / 100.0f, (rand() % 100) / 100.0f, (rand() % 100) / 100.0f));
			sceneObjects.push_back(sphere);
		}
	}
}

//---This function initializes the scene ------------------------------------------- 
//   Specifically, it creates scene objects (spheres, planes, cones, cylinders etc)
//     and add them to the list of scene objects.
//   It also initializes the OpenGL 2D orthographc projection matrix for drawing the
//     the ray traced image.
//----------------------------------------------------------------------------------
void initialize() {
	gettimeofday(&lastTime, NULL);
	rayBatches = createRayBatches(NUMDIV, NUM_THREADS);
	if (rayBatches == nullptr) {
		cout << "Unable to allocate memory for RayBatches array. Exiting..." << endl;
  		exit(1);
	}

    glMatrixMode(GL_PROJECTION);
    gluOrtho2D(XMIN, XMAX, YMIN, YMAX);

    glClearColor(0, 0, 0, 1);

	// Objects
	Sphere *sphere1 = new Sphere(glm::vec3(-5.0, 0.0, -90.0), 15.0);
	sphere1->setColor(glm::vec3(0, 0, 1));   //Set colour to blue
	sphere1->setReflectivity(true, 0.5);
	sceneObjects.push_back(sphere1);		 //Add sphere to scene objects

	Sphere *sphere2 = new Sphere(glm::vec3(10, 10, -60), 3.0);
	sphere2->setColor(glm::vec3(0, 1, 1));   //Set colour to cyan
	sphere2->setShininess(5);
	sceneObjects.push_back(sphere2);		 //Add sphere to scene objects

	Sphere *sphere3 = new Sphere(glm::vec3(5, 5, -70), 5.0);
	sphere3->setColor(glm::vec3(1, 0, 0));   //Set colour to red
	sphere3->setShininess(100);
	sphere3->setTransparency(true, 0.3);
	sceneObjects.push_back(sphere3);		 //Add sphere to scene objects

	Sphere *sphere4 = new Sphere(glm::vec3(5, -10, -60), 5.0);
	sphere4->setColor(glm::vec3(0, 1, 0));   //Set colour to green
	sphere4->setSpecularity(false);
	sphere4->setRefractivity(true, 0.1, 1.1);
	sceneObjects.push_back(sphere4);		 //Add sphere to scene objects

	Cylinder *cylinder = new Cylinder(glm::vec3(10, -15, -40), 2, 10);
	cylinder->setColor(glm::vec3(0, 1, 1));
	cylinder->setReflectivity(true, 0.9);
	sceneObjects.push_back(cylinder);

	// Walls
	Plane *floor = new Plane(glm::vec3(-40., -15, 20), //Point A
							  glm::vec3(40., -15, 20), //Point B
							  glm::vec3(40., -15, -200), //Point C
							  glm::vec3(-40., -15, -200)); //Point D
	floor->setColor(glm::vec3(0.8, 0.8, 0));
	floor->setSpecularity(false);
	floor->setStripe(true, 5, glm::vec3(0, 0, 1), {glm::vec3(0, 1, 0), glm::vec3(1, 1, 0.5)});
	floor->setTextured(true);
	floor->setTexArea(glm::vec2(-15, -60), glm::vec2(5, -90));
	floor->setTexture(TextureBMP(getFilePath("Butterfly.bmp").c_str()));
	sceneObjects.push_back(floor);

	Plane *backWall = new Plane(glm::vec3(-40., -15, -200), //Point A
								glm::vec3(40., -15, -200), //Point B
								glm::vec3(40., 40, -200), //Point C
								glm::vec3(-40., 40, -200)); //Point D
	backWall->setColor(glm::vec3(0.8, 0.8, 0.8));
	backWall->setSpecularity(false);
	backWall->setCheckered(true, 2, glm::vec3(0, 0, 0), glm::vec3(1, 1, 1));
	sceneObjects.push_back(backWall);

	Plane *ceiling = new Plane(glm::vec3(-50, 40, 20), //Point A
							   glm::vec3(-50, 40, -200), //Point B
							   glm::vec3(50, 40, -200), //Point C
							   glm::vec3(50, 40, 20)); //Point D
	ceiling->setColor(glm::vec3(0.8, 0.8, 0.8));
	ceiling->setSpecularity(false);
	ceiling->setReflectivity(true, 0.7);
	sceneObjects.push_back(ceiling);

	Plane *leftWall = new Plane(glm::vec3(-40., -15, 20), //Point A
								glm::vec3(-40., -15, -200), //Point B
								glm::vec3(-40., 40, -200), //Point C
								glm::vec3(-40., 40, 20)); //Point D
	leftWall->setColor(glm::vec3(1, 0, 0));
	leftWall->setSpecularity(false);
	sceneObjects.push_back(leftWall);

	Plane *rightWall = new Plane(glm::vec3(40., -15, 20), //Point A
								glm::vec3(40., 40, 20), //Point B
								glm::vec3(40., 40, -200), //Point C
								glm::vec3(40., -15, -200)); //Point D
	rightWall->setColor(glm::vec3(0, 0.5, 1));
	rightWall->setSpecularity(false);
	sceneObjects.push_back(rightWall);

	Plane *frontWall = new Plane(glm::vec3(-40., -15, 20), //Point A
								glm::vec3(-40., 40, 20), //Point B
								glm::vec3(40., 40, 20), //Point C
								glm::vec3(40., -15, 20)); //Point D
	frontWall->setColor(glm::vec3(0.8, 0.8, 0.8));
	frontWall->setSpecularity(false);
	sceneObjects.push_back(frontWall);

	bvh = new BVH(&sceneObjects);
}

void keyHandler(unsigned char key, int x, int y){
    if(key == 27){
		void freeRayBatches(RayBatches* rayBatches);
		exit(0);
	}
}

int main(int argc, char *argv[]) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(800, 800);
    glutInitWindowPosition(20, 20);
    glutCreateWindow("Raytracing");

	glutKeyboardFunc(keyHandler);
    glutDisplayFunc(display);
	glutIdleFunc(display);
    initialize();

    glutMainLoop();
    return 0;
}
