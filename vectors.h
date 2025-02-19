#ifndef VECTORS_H
#define VECTORS_H

/* Definición de un vector 3D */
typedef struct {
    float x, y, z;
} Vec3;

/* Funciones auxiliares para operaciones con Vec3 */
Vec3 vec3_add(Vec3 a, Vec3 b);
Vec3 vec3_sub(Vec3 a, Vec3 b);
Vec3 vec3_scale(Vec3 v, float s);

// Funcioón para calcular el módulo de un vector
float module(Vec3 v);

// Función para calcular la distancia en a y b
float distance(Vec3 a, Vec3 b);

/* Función para imprimir por consola un vecto */
void display(Vec3 v);

// Función para normalizar un vector
Vec3 normalize(Vec3 v);

// Función para calcular el producto escalar entre dos vectores
float dotProduct(Vec3 a, Vec3 b);

#endif
