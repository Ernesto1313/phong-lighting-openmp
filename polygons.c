#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "vectors.h"
#include "polygons.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Valores por defecto de los vectores de componente ambiente, 
// especular y shininess de material de la esfera
Vec3 DEFAULT_AMBIENT = {0.1f, 0.1f, 0.1f};
Vec3 DEFAULT_SPECULAR = {1.0f, 1.0f, 1.0f};
float DEFAULT_SHININESS = 10.0f;

// Vector con el color por defecto de cada esfera 
Vec3 INITIAL_COLOR = {0.0f, 0.0f, 0.0f};

/*
 * Función: generateSphereMeshTriangles
 * --------------------------------------
 * Genera una malla triangular para una esfera.
 *
 * Parámetros:
 *   - center: centro de la esfera.
 *   - radius: radio de la esfera.
 *   - color : color de la esfera.
 *   - frequency: número de subdivisiones en latitud (se usará 2*frequency en longitud).
 *   - outTriangleCount: puntero a entero donde se almacenará el número total de triángulos generados.
 *
 * Retorna:
 *   - Un arreglo dinámico de Triangle que contiene la malla de la esfera.
 *     El arreglo debe liberarse con free() cuando ya no se use.
 */
Triangle* generateSphereMeshTriangles(const Vec3 center, float radius, const Vec3 color, int frequency, int *outTriangleCount) {
    if (frequency < 2) {
        fprintf(stderr, "La frecuencia debe ser al menos 2.\n");
        exit(EXIT_FAILURE);
    }
    
    int latSteps = frequency;         // subdivisiones en la dirección polar
    int lonSteps = frequency * 2;       // subdivisiones en la dirección azimutal
    int totalTriangles = 2 * lonSteps * (latSteps - 1);
    
    Triangle* triangles = (Triangle*) malloc(totalTriangles * sizeof(Triangle));
    if (!triangles) {
        fprintf(stderr, "Error: No se pudo asignar memoria.\n");
        exit(EXIT_FAILURE);
    }
    
    float dTheta = M_PI / latSteps;         // paso en latitud
    float dPhi = (2.0f * M_PI) / lonSteps;    // paso en longitud
    int tIndex = 0;
    
    // Recorrer cada banda en latitud
    for (int i = 0; i < latSteps; i++) {
        float theta1 = i * dTheta;
        float theta2 = (i + 1) * dTheta;
        
        // Recorrer cada segmento en longitud
        for (int j = 0; j < lonSteps; j++) {
            float phi1 = j * dPhi;
            float phi2 = (j + 1) * dPhi;
            
            // Calcular los 4 vértices de la celda en la esfera (en coordenadas esféricas)
            // v0: en (theta1, phi1)
            Vec3 v0 = { radius * sinf(theta1) * cosf(phi1),
                        radius * cosf(theta1),
                        radius * sinf(theta1) * sinf(phi1) };
            // v1: en (theta2, phi1)
            Vec3 v1 = { radius * sinf(theta2) * cosf(phi1),
                        radius * cosf(theta2),
                        radius * sinf(theta2) * sinf(phi1) };
            // v2: en (theta2, phi2)
            Vec3 v2 = { radius * sinf(theta2) * cosf(phi2),
                        radius * cosf(theta2),
                        radius * sinf(theta2) * sinf(phi2) };
            // v3: en (theta1, phi2)
            Vec3 v3 = { radius * sinf(theta1) * cosf(phi2),
                        radius * cosf(theta1),
                        radius * sinf(theta1) * sinf(phi2) };
            
            // Trasladar cada vértice por el centro de la esfera
            v0 = vec3_add(v0, center);
            v1 = vec3_add(v1, center);
            v2 = vec3_add(v2, center);
            v3 = vec3_add(v3, center);
            
            triangles[tIndex].c[0] = color;
            triangles[tIndex].c[1] = color;
            triangles[tIndex].c[2] = color;
            
            // En la banda superior (i == 0) y la inferior (i == latSteps-1) se generan triángulos "degenerados"
            // que en realidad forman un solo triángulo por celda.
            if (i == 0) {
                // Polo norte: v0 y v3 son casi el mismo punto (norte)
                // Solo se genera un triángulo: (v1, v2, v0)
                triangles[tIndex].v[0] = v1;
                triangles[tIndex].v[1] = v2;
                triangles[tIndex].v[2] = v0;
                tIndex++;
            } else if (i == latSteps - 1) {
                // Polo sur: v1 y v2 son casi el mismo (sur)
                // Se genera un triángulo: (v0, v1, v3)
                triangles[tIndex].v[0] = v0;
                triangles[tIndex].v[1] = v1;
                triangles[tIndex].v[2] = v3;
                tIndex++;
            } else {
                // Banda intermedia: se generan 2 triángulos por celda.
                // Triángulo 1: (v0, v1, v2)
                triangles[tIndex].v[0] = v0;
                triangles[tIndex].v[1] = v1;
                triangles[tIndex].v[2] = v2;
                tIndex++;
            	
				triangles[tIndex].c[0] = color;
            	triangles[tIndex].c[1] = color;
            	triangles[tIndex].c[2] = color;
                
				// Triángulo 2: (v0, v2, v3)
                triangles[tIndex].v[0] = v0;
                triangles[tIndex].v[1] = v2;
                triangles[tIndex].v[2] = v3;
                tIndex++;
            }                        
        }
    }
    
    if (outTriangleCount) {
        *outTriangleCount = tIndex;
    }
    
    return triangles;
}

// Función que crea el material por defecto de la esfera en función del color
Material createDefaultMaterial(Vec3 color){
	Material mat;
	
	mat.diffuse=color;
	mat.ambient=DEFAULT_AMBIENT;
	mat.specular = DEFAULT_SPECULAR;
	mat.shininess = DEFAULT_SHININESS;
	
	return mat;
}

/*
* Función createSphere
* Función  que crea una esfera
*-------------------------
* Parámetros
* center : centro de la esfera
* radius: radio de la esfera
* freq : frecuencia de polígonos de la esfera
* color : color de la esfera
*/
 Sphere createSphere(Vec3 center, float radius, int freq, Vec3 color) {
	Sphere sphere;
	sphere.center = center;
	sphere.radius = radius;
	sphere.color = INITIAL_COLOR;
	sphere.triangles = generateSphereMeshTriangles(sphere.center, sphere.radius, sphere.color, freq, &sphere.numTriangles);
	sphere.material = createDefaultMaterial(color);
	
	return sphere;
}

/*
* Función createLight
* Función  que crea una luz
*-------------------------
* Parámetros
* position : posición de la luz
* color: color de la luz
* intensity : intensidad de la luz ( de 0 a 1) 
*/
Light createLight(Vec3 position, Vec3 color, float intensity){
	Light light;
	
	light.position = position;
	light.color = color;
	light.intensity = intensity;
	
	return light;
}



