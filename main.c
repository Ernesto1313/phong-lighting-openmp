#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include "vectors.h"
#include "polygons.h"

#define CAMERA_POSITION  {8.0f, 0.0f, 0.0f}

#define DEFAULT_AMBIENT_LIGHT {1.0f, 1.0f, 1.0f}

#define RED {1.0f, 0.2f, 0.2f}
#define BLUE {0.2f, 0.2f, 1.0f}
#define WHITE {1.0f, 1.0f, 1.0f}
#define BLACK {0.0f, 0.0f, 0.0f}

#define PI  3.14159265358979323846

#define NUM_SPHERES_DEBUG 2
#define NUM_LIGHTS_DEBUG 2

#define FILE_OK 2
#define DEBUG_MODE 0
#define EXPERIMENT_MODE 1

Vec3 ambientLight = DEFAULT_AMBIENT_LIGHT;


/*
 * * Función toFile
 * * Función  guarda los vértices y los colores de las esferas
 * *          en archivo salida.txt
 * *-------------------------
 * * Parámetros
 * * sphere: array de esferas
 * * numSpheres : número de esferas
 * */
int toFile(Sphere *sphere, int numSpheres) {
	// Abrir el archivo para escritura
    FILE *file = fopen("salida.txt", "w");
    if (file == NULL) {
        perror("Error al abrir el archivo");
        return EXIT_FAILURE;
    }
    
    for (int s=0; s<numSpheres; s++) { 
    	for (int t=0; t<sphere[s].numTriangles; t++) {
    		for (int v=0; v<3; v++) {			
    			Vec3 p = sphere[s].triangles[t].v[v];
    			Vec3 c = sphere[s].triangles[t].c[v];
    			fprintf(file, "%.2f %.2f %.2f", p.x, p.y, p.z);
    			fprintf(file, " ");
    			fprintf(file, "%.2f %.2f %.2f", c.x, c.y, c.z);
    			fprintf(file, " ");
    		}
    		fprintf(file, "\n");
	}
    }
	
	fclose(file);
	
	return FILE_OK;	
}

/*
 * * Función createShadowMatrix
 * * Función  que crea la matriz de sombras
 * *-------------------------
 * * Parámetros
 * * numSpheres : número de esferas
 * * numLights: número de luces
 * */

bool*** createShadowMatrix(int numSpheres, int numLights){
    // Reservar memoria para la primera dimensión (i)
    bool*** shadowMatrix = (bool ***)malloc(numSpheres * sizeof(bool **));
    if (shadowMatrix == NULL) {
        printf("Error al reservar memoria.\n");
        return NULL;
    }
    // Reservar memoria para la segunda dimensión (j)
    for (int i = 0; i < numSpheres; i++) {
         shadowMatrix[i] = (bool **)malloc(numSpheres * sizeof(bool *));
         if (shadowMatrix[i] == NULL) {
             printf("Error al reservar memoria.\n");
             return NULL;
         }
         // Reservar memoria para la tercera dimensión (k)
         for (int j = 0; j < numLights; j++) {
              shadowMatrix[i][j] = (bool *)malloc(numLights * sizeof(bool));
              if (shadowMatrix[i][j] == NULL) {
                  printf("Error al reservar memoria.\n");
                  return NULL;
              }
         }
    }
                                                                                                               
    return shadowMatrix;
};

/*
 * * Función createDebugData
 * * Función  que crea las esferas y luces en modo debug
 * *-------------------------
 * * Parámetros
 * * sphere : matriz con las esferas de la escena
 * * light: matriz con las luces de la escena
 * */

void createDebugData(Sphere *sphere, Light *light){
	
   Vec3 center[NUM_SPHERES_DEBUG] = {{0.0f, 1.0f, 0.0f}, {0.0f, 6.0f, 0.0f}};
   float radius[NUM_SPHERES_DEBUG] = {1.0f, 1.25f};
   int freq[NUM_SPHERES_DEBUG] = {8, 16};
   Vec3 color[NUM_SPHERES_DEBUG] = {RED, BLUE};

   Vec3 lightPos[NUM_LIGHTS_DEBUG] = {{0.0f, -2.0f, 0.0f},{0.0f, 2.5f, 0.0f}};
   Vec3 lightCol[NUM_LIGHTS_DEBUG] = {WHITE, WHITE};
   float lightIntensity[NUM_LIGHTS_DEBUG] = {1.0f, 1.0f};
	
   // Create the spheres
   for (int i=0; i<NUM_SPHERES_DEBUG; i++) {
    	sphere[i] = createSphere(center[i], radius[i], freq[i], color[i]);   
   }
    	
   // Create the lights
   for (int l=0; l<NUM_LIGHTS_DEBUG; l++) {
      light[l] = createLight(lightPos[l], lightCol[l], lightIntensity[l]);
   }		
}


int main(int argc, char** argv) {
	
    int mode = DEBUG_MODE;

    int numSpheres;
    int numLights;
	
    if (mode==DEBUG_MODE) {
	numSpheres = NUM_SPHERES_DEBUG;
	numLights = NUM_LIGHTS_DEBUG;
    }
	
    // Matriz de sombras entre objetos y luces
    bool ***shadowMatrix = createShadowMatrix(numSpheres, numLights);
    Sphere *sphere = (Sphere *) malloc(sizeof(Sphere)*numSpheres); 
    Light *light = (Light* )malloc(sizeof(Light)*numLights); 
	
    Vec3 camera = CAMERA_POSITION;	
    
    // Create the spheres & the lights
    if (mode==DEBUG_MODE)
    	createDebugData( sphere, light);
    
    // Calculate la normal de los triángulos de las esferas
    // ...
    	
    // Calcular la bounding box
    // ...
    
    // Calcular la matriz de sombres
    // .... 
    
    // Calcular la iluminación Phong sobre las esferas  
    // teniendo en cuenta las sombras
    // ....
    	
    // Guardar los vertices y colores en un archivo
    if (toFile(sphere, numSpheres) == FILE_OK)
	printf("El archivo se ha creado.\n");
    else
	printf("El archivo NO se ha creado\n");
    
    // Liberar memoria
    // ....
    
    return 0;
}
