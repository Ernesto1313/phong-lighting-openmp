#ifndef POLYGONS_H
#define POLYGONS_H

#include "vectors.h"

/* Definición de un triángulo: 3 vértices y 3 colores (r,g,b) */
typedef struct {
    Vec3 v[3];
    Vec3 c[3];
    Vec3 normal;
} Triangle;

typedef struct {
	Vec3 max;
	Vec3 min;
} BoundingBox;

// Estructura para representar materiales
typedef struct {
    Vec3 ambient, diffuse, specular;
    float shininess;
} Material;


/* Definición de una Esfera */
typedef struct {
    Vec3 center;
    float radius;
    Vec3 color;
    Triangle* triangles;
    int numTriangles;
    Material material;
    BoundingBox bBox;
} Sphere;

// Estructura para representar luces
typedef struct {
    Vec3 position, color;
    float intensity;
} Light;




Sphere createSphere(Vec3 center, float radius, int freq, Vec3 color);
Light createLight(Vec3 position, Vec3 color, float intensity);

#endif
