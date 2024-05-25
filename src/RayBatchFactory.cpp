#include "RayBatchFactory.h"

RayBatches* createRayBatches(const int NUMDIV,const int NUM_THREADS) {
    const long TOTAL_RAYS = NUMDIV * NUMDIV;
    const long raysPerThread = TOTAL_RAYS / NUM_THREADS;    
    
    // Allocate memory for the RayBatches array
    RayBatches* rayBatches = (RayBatches*)malloc(sizeof(RayBatches) * NUM_THREADS);
    
    if (rayBatches == nullptr) {
        // Handle allocation failure
        return nullptr;
    }

    // Populate the RayBatches array
    for(int i = 0; i < NUM_THREADS; i++) {
        rayBatches[i].numRays = std::min(raysPerThread, (TOTAL_RAYS) - (i * raysPerThread));
        rayBatches[i].rays = (RayWrapper*)malloc(sizeof(RayWrapper) * rayBatches[i].numRays);
        
        if (rayBatches[i].rays == nullptr) {
            // Handle allocation failure
            // Free previously allocated memory to prevent memory leaks
            for (int j = 0; j < i; j++) {
                free(rayBatches[j].rays);
            }
            free(rayBatches);
            return nullptr;
        }
    }
    
    return rayBatches;
}

void freeRayBatches(RayBatches* rayBatches, const int NUM_THREADS){
    for (size_t i = 0; i < NUM_THREADS; i++) {
        RayWrapper* rays = rayBatches[i].rays;
        for (int i = 0; i < rayBatches[i].numRays; i++){
            free(rays+i);
        }
        free(rays);
    }
}