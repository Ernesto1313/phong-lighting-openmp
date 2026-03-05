**# Parallel Phong Lighting (OpenMP)

This project implements the **Phong lighting algorithm** in C and explores its **parallelization using OpenMP** as part of a Computer Architecture course.

The goal of the project is to analyze the performance difference between a **sequential implementation** and a **parallel implementation**, measuring execution times and computational performance.

## Features

- Sequential implementation of the Phong lighting algorithm
- Parallel implementation using OpenMP
- Performance comparison
- Execution time measurements
- MFLOPS analysis
- Experimental results with different problem sizes

## Technologies

- C
- OpenMP
- GCC
- Linux

## Project Structure

src/ -> Source code of the implementation
report/ -> Project report and documentation
results/ -> Experimental results and performance graphs
Makefile -> Compilation instructions


## Compilation

Example compilation using GCC with OpenMP support:
  gcc -O2 -fopenmp phong.c -o phong
To compile without optimization:
  gcc -fopenmp phong.c -o phong

## Execution

Run the program with:
  ./phong <problem_size>
The program computes the illumination of a scene using the Phong lighting model and measures execution time for performance evaluation.

## Experiments

The project includes experiments comparing:
- Sequential vs parallel execution
- Different problem sizes
- Optimized vs non-optimized compilation (`-O2`)
Performance is evaluated using **execution time and MFLOPS**.

## Academic Context

This project was developed for the **Computer Architecture Laboratory** at the **University of Valencia** as part of a performance analysis assignment.

## Author

Ernesto  
Computer Engineering Student**
