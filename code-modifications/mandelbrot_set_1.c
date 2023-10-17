#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>

# define NPOINTS 1000
# define MAXITER 1000

void testpoint(void);

struct d_complex {
   double r;
   double i;
};

struct d_complex c;
int numoutside = 0;

int main() {
   int i, j;
   double area, error, eps = 1.0e-5;

   // Loop over grid of points in the complex plane which contains the Mandelbrot set,
   // testing each point to see whether it is inside or outside the set.

   #pragma omp parallel for default(shared) private(c, eps) reduction(+:numoutside)
   for (i = 0; i < NPOINTS; i++) {
     for (j = 0; j < NPOINTS; j++) {
       c.r = -2.0 + 2.5 * (double)(i) / (double)(NPOINTS) + eps;
       c.i = 1.125 * (double)(j) / (double)(NPOINTS) + eps;
       testpoint();
     }
   }

   // Calculate area of set and error estimate and output the results
   area = 2.0 * 2.5 * 1.125 * (double)(NPOINTS * NPOINTS - numoutside) / (double)(NPOINTS * NPOINTS);
   error = area / (double)NPOINTS;

   printf("Area of Mandelbrot set = %12.8f +/- %12.8f\n", area, error);
   printf("Correct answer should be around 1.510659\n");
}

void testpoint(void) {
   // Does the iteration z = z * z + c, until |z| > 2 when the point is known to be outside the set
   // If loop count reaches MAXITER, the point is considered to be inside the set

   struct d_complex z;
   int iter;
   double temp;

   z = c;
   for (iter = 0; iter < MAXITER; iter++) {
     temp = (z.r * z.r) - (z.i * z.i) + c.r;
     z.i = z.r * z.i * 2 + c.i;
     z.r = temp;
     if ((z.r * z.r + z.i * z.i) > 4.0) {
       numoutside++;
       break;
     }
   }
}