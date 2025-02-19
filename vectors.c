#include "vectors.h"
#include <stdio.h>
#include <math.h>

/* Funciones auxiliares para operaciones con Vec3 */
Vec3 vec3_add(Vec3 a, Vec3 b) {
	    Vec3 result = { a.x + b.x, a.y + b.y, a.z + b.z };
	        return result;
}

Vec3 vec3_sub(Vec3 a, Vec3 b){
    Vec3 result = { a.x - b.x, a.y - b.y, a.z - b.z };
    return result;	
}

Vec3 vec3_scale(Vec3 v, float s) {
	    Vec3 result = { v.x * s, v.y * s, v.z * s };
	        return result;
}

void display(Vec3 v){
	printf("( %.2f, %.2f, %.2f )", v.x, v.y, v.z);
}

// Función para normalizar un vector
Vec3 normalize(Vec3 v) {
    float length = sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
    Vec3 result = {v.x / length, v.y / length, v.z / length};
    return result;
}

// Funcioón para calcular el módulo de un vector
float module(Vec3 v) {
	return sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}

// Función para calcular la distancia en a y b
float distance(Vec3 a, Vec3 b){
	Vec3 sub = {b.x-a.x, b.y-a.y, b.z-a.z};
	
	return module(sub);
}

// Función para calcular el producto escalar entre dos vectores
float dotProduct(Vec3 a, Vec3 b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}
