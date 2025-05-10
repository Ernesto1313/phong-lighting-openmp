#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include "vectors.h"
#include "polygons.h"
#include <time.h>
#include <mpi.h>

#define CAMERA_POSITION  {12.0f, 0.0f, 0.0f}

#define DEFAULT_AMBIENT_LIGHT {1.0f, 1.0f, 1.0f}

const Vec3 RED = {1.0f, 0.2f, 0.2f};
const Vec3 BLUE = {0.2f, 0.2f, 1.0f};
const Vec3 WHITE = {1.0f, 1.0f, 1.0f};
const Vec3 BLACK = {0.0f, 0.0f, 0.0f};

#define PI  3.14159265358979323846

#define NUM_SPHERES_DEBUG 2
#define NUM_LIGHTS_DEBUG 2

#define FILE_OK 2
#define DEBUG_MODE 0
#define EXPERIMENT_MODE 1

Vec3 ambientLight = DEFAULT_AMBIENT_LIGHT;

// Contador de FLOPs
long flop_count = 0;  // Contador global de FLOPs

// Función para contar FLOPs
void count_flops(int ops) {
    flop_count += ops;
}

// Definición del tipo MPI_Triangle
void create_mpi_triangle(MPI_Datatype* MPI_Triangle) {
    int blocklengths[3] = {3, 3, 1};  // v[3], c[3], normal
    MPI_Datatype types[3] = {MPI_FLOAT, MPI_FLOAT, MPI_FLOAT};
    MPI_Aint offsets[3];
    offsets[0] = offsetof(Triangle, v);
    offsets[1] = offsetof(Triangle, c);
    offsets[2] = offsetof(Triangle, normal);
    MPI_Type_create_struct(3, blocklengths, offsets, types, MPI_Triangle);
    MPI_Type_commit(MPI_Triangle);
}


/*
 * * Funci�n toFile
 * * Funci�n  guarda los v�rtices y los colores de las esferas
 * *          en archivo salida.txt
 * *-------------------------
 * * Par�metros
 * * sphere: array de esferas
 * * numSpheres : n�mero de esferas
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
 * * Funci�n createShadowMatrix
 * * Funci�n  que crea la matriz de sombras
 * *-------------------------
 * * Par�metros
 * * numSpheres : n�mero de esferas
 * * numLights: n�mero de luces
 * */

 bool*** createShadowMatrix(int numSpheres, int numLights) {
    if (numSpheres <= 0 || numLights <= 0) {
        printf("Error: numSpheres (%d) o numLights (%d) no válido.\n", numSpheres, numLights);
        return NULL;
    }

    bool*** shadowMatrix = (bool ***)malloc(numSpheres * sizeof(bool **));
    if (shadowMatrix == NULL) {
        printf("Error al reservar memoria para shadowMatrix.\n");
        return NULL;
    }

    for (int i = 0; i < numSpheres; i++) {
        shadowMatrix[i] = (bool **)malloc(numLights * sizeof(bool *));
        if (shadowMatrix[i] == NULL) {
            printf("Error al reservar memoria para shadowMatrix[%d].\n", i);
            return NULL;
        }

        for (int j = 0; j < numLights; j++) {
            shadowMatrix[i][j] = (bool *)malloc(numLights * sizeof(bool));
            if (shadowMatrix[i][j] == NULL) {
                printf("Error al reservar memoria para shadowMatrix[%d][%d].\n", i, j);
                return NULL;
            }

            // Inicializar la memoria asignada para evitar valores basura
            for (int k = 0; k < numLights; k++) {
                shadowMatrix[i][j][k] = false;
            }
        }
    }

    return shadowMatrix;
}
;

/*
 * * Funci�n createDebugData
 * * Funci�n  que crea las esferas y luces en modo debug
 * *-------------------------
 * * Par�metros
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

    MPI_Init(&argc, &argv); 

    int rank, numprocs;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
	
    if (mode==DEBUG_MODE) {
	numSpheres = NUM_SPHERES_DEBUG;
	numLights = NUM_LIGHTS_DEBUG;
    }
	
     // Leer argumentos de línea de comandos
     if (argc > 1) {
        numSpheres = atoi(argv[1]);
        if (numSpheres > 0) {
            mode = EXPERIMENT_MODE;
            numLights = numSpheres + 1; // Luces intercaladas + luz central
        } else {
            numSpheres = NUM_SPHERES_DEBUG;
            numLights = NUM_LIGHTS_DEBUG;
        }
    }
    
    printf("Modo: %s\n", mode == DEBUG_MODE ? "Depuración" : "Experimentación");
    printf("Número de esferas: %d\n", numSpheres);
    printf("Número de luces: %d\n", numLights);
    
    // Matriz de sombras entre objetos y luces
    printf("numSpheres = %d, numLights = %d\n", numSpheres, numLights);
    bool ***shadowMatrix = createShadowMatrix(numSpheres, numLights);
    if (shadowMatrix == NULL) {
        printf("Error: No se pudo asignar memoria para shadowMatrix.\n");
        return EXIT_FAILURE;
    }
    Sphere *sphere = (Sphere *) malloc(sizeof(Sphere)*numSpheres); 
    Light *light = (Light* )malloc(sizeof(Light)*numLights); 
	
    Vec3 camera = CAMERA_POSITION;	

    if (mode == DEBUG_MODE) {
        createDebugData(sphere, light);
    } else {
        
        float radius = 0.7f / sinf(PI / numSpheres);
        for (int i = 0; i < numSpheres; i++) {
            float angle = (2.0f * PI / (2 * numSpheres)) * (2 * i);
            Vec3 position = {radius * cos(angle), 0.0f, radius * sin(angle)};
            int freq = 4 + (i * 2);
            sphere[i] = createSphere(position, 0.5f, freq, RED);
        }
        
        for (int i = 0; i < numSpheres; i++) {
            float angle = (2.0f * PI / (2 * numSpheres)) * (2 * i + 1);
            Vec3 position = {radius * cos(angle), 0.5f, radius * sin(angle)};
            light[i] = createLight(position, WHITE, 0.13f);
        }
        
        light[numSpheres] = createLight((Vec3){0.0f, 0.0f, 0.0f}, WHITE, 0.1f);
    }
    
    // Medición del tiempo de ejecución: Inicio
    clock_t start_time = clock();
    // Función para calcular la normal de un triángulo
    // Calcular la normal de los triángulos de las esferas
    for (int s = 0; s < numSpheres; s++) {
        for (int t = 0; t < sphere[s].numTriangles; t++) {
            Triangle *triangle = &sphere[s].triangles[t];
            Vec3 v0 = triangle->v[0];
            Vec3 v1 = triangle->v[1];
            Vec3 v2 = triangle->v[2];
            
            Vec3 edge1 = vec3_sub(v1, v0);  // 1 FLOP (subtraction)
                count_flops(1);
            Vec3 edge2 = vec3_sub(v2, v0);  // 1 FLOP (subtraction)
                count_flops(1);
            
            Vec3 normal = {
                edge1.y * edge2.z - edge1.z * edge2.y,  // Componente x
                edge1.z * edge2.x - edge1.x * edge2.z,  // Componente y
                edge1.x * edge2.y - edge1.y * edge2.x   // Componente z
            };
            // 9 FLOPS = 3*(2 Mults + 1 Subs)
            count_flops(9);

            normal = normalize(normal); // 8 FLOPS Sqrt
                count_flops(8);
            normal = vec3_scale(normal, -1);
                count_flops(1);
            triangle->normal = normal;
        }
    }


    	
    // Calcular la bounding box
    Vec3 minBound = {INFINITY, INFINITY, INFINITY};
    Vec3 maxBound = {-INFINITY, -INFINITY, -INFINITY};

    for (int s = 0; s < numSpheres; s++) {
        for (int t = 0; t < sphere[s].numTriangles; t++) {
            for (int v = 0; v < 3; v++) {

                Vec3 p = sphere[s].triangles[t].v[v];
                if (p.x < minBound.x) minBound.x = p.x;
                if (p.y < minBound.y) minBound.y = p.y;
                if (p.z < minBound.z) minBound.z = p.z;
                if (p.x > maxBound.x) maxBound.x = p.x;
                if (p.y > maxBound.y) maxBound.y = p.y;
                if (p.z > maxBound.z) maxBound.z = p.z;
            }
        }
    }
    
    // Calcular la matriz de sombres
    for (int i = 0; i < numSpheres; i++) {
        for (int j = 0; j < numSpheres; j++) {
            for (int k = 0; k < numLights; k++) {
                // Cálculo de las distancias
                float dLA = distance(sphere[i].center, light[k].position); // 8 FLOPS Sqrt
                    count_flops(8);
                float dAB = distance(sphere[i].center, sphere[j].center); // 8 FLOPS Sqrt
                    count_flops(8);
                float dLB = distance(light[k].position, sphere[j].center); // 8 FLOPS Sqrt
                    count_flops(8);

                // Si la esfera A no está entre L y B, no bloquea la luz
                if ((dLA + dAB) > dLB) {
                    shadowMatrix[i][j][k] = false;
                } else {
                    // Cálculo de la tangente del cono de sombra
                    float tanShadow = sphere[i].radius / dLA; // 4 FLOPS Div
                        count_flops(4);
                    float shadowRadius = sphere[i].radius + dAB * tanShadow;  // 2 FLOPS Add + Mult 
                        count_flops(2);
                    // Verificar si el shadowRadius de A es mayor que el radio de B
                    if (shadowRadius > sphere[j].radius) {
                        shadowMatrix[i][j][k] = true;  // A hace sombra sobre B
                    } else {
                        shadowMatrix[i][j][k] = false; // A no hace sombra sobre B
                    }
                }
            }
        }
    }
    
    
    // Calcular la iluminaci�n Phong sobre las esferas teniendo en cuenta las sombras
    for (int s = 0; s < numSpheres; s++) {
        for (int t = 0; t < sphere[s].numTriangles; t++) {
            Triangle *triangle = &sphere[s].triangles[t];
            Vec3 color = {0.0f, 0.0f, 0.0f}; // Color final del triángulo
            
            for (int l = 0; l < numLights; l++) {
                // Verificar si la esfera A está en sombra por alguna esfera en la matriz de sombras
                if (!shadowMatrix[s][s][l]) {
                    // Vector de luz y vector de visión
                    Vec3 L = normalize(vec3_sub(light[l].position, triangle->v[0])); // Desde el vértice hacia la luz
                        count_flops(8); // 8 FLOPs Sqrt
                    Vec3 V = normalize(vec3_sub(camera, triangle->v[0])); // Desde el vértice hacia la cámara
                        count_flops(8); // 8 FLOPs Sqrt
                    // Componente difusa: max(0, dot(N, L))
                    float NL = dotProduct(triangle->normal, L);
                        count_flops(1);
                    if (NL < 0) NL = 0;
                    Vec3 D = vec3_scale(sphere[s].material.diffuse, NL * light[l].intensity);
                        count_flops(3);

                    // Componente especular: calcular el reflejo
                    Vec3 R = normalize(vec3_sub(vec3_scale(triangle->normal, 2 * NL), L));
                        count_flops(9);
                    float RV = dotProduct(R, V);
                        count_flops(1);
                    if (RV < 0) RV = 0;
                    float specFactor = pow(RV, sphere[s].material.shininess);
                        count_flops(8);
                    Vec3 S = vec3_scale(sphere[s].material.specular, specFactor * light[l].intensity);
                        count_flops(3);

                    // Multiplicar las componentes difusa y especular por el color de la luz
                    D = vec3_scale(D, light[l].color.x);
                        count_flops(1);
                    S = vec3_scale(S, light[l].color.x);
                        count_flops(1);

                    // Sumar las contribuciones de la luz
                    color = vec3_add(color, vec3_add(D, S));
                        count_flops(2);
                }
            }
            
            // Asignar color al color de cada vértice del triángulo
            for (int v = 0; v < 3; v++) {
                triangle->c[v] = color;
            }
        }
    }

    // Medición del tiempo de ejecución: Fin
    clock_t end_time = clock();
    double time_taken = ((double)(end_time - start_time)) / CLOCKS_PER_SEC;  // Conversion de ticks a segundos
    printf("Tiempo de ejecucion: %.5f segundos\n", time_taken);
    printf("Numero de esferas: %d, FLOPs totales: %ld\n", numSpheres, flop_count);

    	
    // Guardar los vertices y colores en un archivo
    if (rank == 0) {
    if (toFile(sphere, numSpheres) == FILE_OK)
        printf("El archivo se ha creado.\n");
    else
        printf("El archivo NO se ha creado\n");
}

    
    

    
    // Liberar la memoria de la matriz de sombras
    for (int i = 0; i < numSpheres; i++) {
        for (int j = 0; j < numLights; j++) {
            free(shadowMatrix[i][j]); // Liberar cada fila de la tercera dimensión
        }
        free(shadowMatrix[i]); // Liberar cada fila de la segunda dimensión
    }
    free(shadowMatrix); // Liberar la primera dimensión

    // Liberar la memoria de las esferas
    for (int s = 0; s < numSpheres; s++) {
        for (int t = 0; t < sphere[s].numTriangles; t++) {
            free(sphere[s].triangles[t].v);  // Liberar memoria de los vértices si es necesario
            free(sphere[s].triangles[t].c);  // Liberar memoria de los colores si es necesario
        }
        free(sphere[s].triangles); // Liberar la memoria de los triángulos
    }
    free(sphere); // Liberar el array de esferas

    // Liberar la memoria de las luces
    free(light); // Liberar el array de luces



    MPI_Finalize();

    return 0;
}
