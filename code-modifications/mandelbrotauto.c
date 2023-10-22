/*
**  PROGRAM: Mandelbrot area
**
**  PURPOSE: Program to compute the area of a  Mandelbrot set.
**           Correct answer should be around 1.510659
**           WARNING: this program may contain errors
**
**  USAGE:   Program runs without input ... just run the executable
**
**  HISTORY: Written:  (Mark Bull, August 2011)
**           Changed "comples" to "d_comples" to avoid collsion with
**           math.h complex type (Tim Mattson, September 2011)
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>

#define NPOINTS 1000
#define MAXITER 1000

struct d_complex
{
    double r;
    double i;
};

void testpoint(struct d_complex);

struct d_complex c;
int numoutside = 0;

int main(int argc, char *argv[])
{
    int num_threads = atoi(argv[1]);

    int i, j;
    double area, error, eps = 1.0e-5;
    double start_time = omp_get_wtime();
    omp_set_num_threads(num_threads);

    // Loop over grid of points in the complex plane which contains the Mandelbrot set,
    // testing each point to see whether it is inside or outside the set.

#pragma omp parallel for default(shared) firstprivate(eps) private(c, j) schedule(auto)
    for (i = 0; i < NPOINTS; i++)
    {
        for (j = 0; j < NPOINTS; j++)
        {
            c.r = -2.0 + 2.5 * (double)(i) / (double)(NPOINTS) + eps;
            c.i = 1.125 * (double)(j) / (double)(NPOINTS) + eps;
            testpoint(c);
        }
    }

    // Calculate area of set and error estimate and output the results
    area = 2.0 * 2.5 * 1.125 * (double)(NPOINTS * NPOINTS - numoutside) / (double)(NPOINTS * NPOINTS);
    error = area / (double)NPOINTS;

    double run_time = omp_get_wtime() - start_time;
    printf("Time of execution: %lf seconds\n", run_time);
    printf("Area of Mandelbrot set = %12.8f +/- %12.8f\n", area, error);
    printf("Correct answer should be around 1.510659\n");
}

void testpoint(struct d_complex c)
{
    // Does the iteration z = z * z + c, until |z| > 2 when the point is known to be outside the set
    // If loop count reaches MAXITER, the point is considered to be inside the set

    struct d_complex z;
    int iter;
    double temp;

    z = c;
    for (iter = 0; iter < MAXITER; iter++)
    {
        temp = (z.r * z.r) - (z.i * z.i) + c.r;
        z.i = z.r * z.i * 2 + c.i;
        z.r = temp;
        if ((z.r * z.r + z.i * z.i) > 4.0)
        {
#pragma omp atomic
            numoutside++;
            break;
        }
    }
}