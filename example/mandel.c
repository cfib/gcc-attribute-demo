/*
 *  Copyright (c) Christian Fibich, 2017 <christian.fibich@gmail.com>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *  --------------------------------------------------------------------
 *  Demo program to demonstrate GCC plugin
 *  --------------------------------------------------------------------
 */


#include <stdio.h>
#include <math.h>

#define WIDTH 640
#define HEIGHT 480
#define ITER 64
#define XMIN -2.5
#define XMAX 1.0
#define YMAX ((XMAX-XMIN)*(HEIGHT/(double)WIDTH)/2)
#define YMIN (-YMAX)

int __attribute__((ATTRIBUTE_DEMO)) l;
int __attribute__((ATTRIBUTE_DEMO)) c;
    
unsigned char __attribute__((ATTRIBUTE_DEMO)) iterate(int  __attribute__((ATTRIBUTE_DEMO))  Px, int Py) {
    double x0 = (XMAX-XMIN)/WIDTH*Px+XMIN;
    double y0 = (YMAX-YMIN)/HEIGHT*Py+YMIN;
    double __attribute__((ATTRIBUTE_DEMO)) x = 0.0;
    double y = 0.0;
    int iteration = 0;
    l_iterate : __attribute__((ATTRIBUTE_DEMO)) while((pow(x,2) + pow(y,2)) < 4 && (iteration < ITER)) {
        double xtemp = pow(x,2) - pow(y,2) + x0;
        y = 2*x*y + y0;
        x = xtemp;
        iteration++;
    }
    return iteration<<2;
}

int main(void) {
    printf("P2\n%d %d\n%d\n",WIDTH,HEIGHT,ITER-1);
    for (l = 0; l < HEIGHT; l++) {
        for (c = 0; c < WIDTH; c++) {
            unsigned char px = iterate(c,l);
            printf("%d\n",px);
        }
    }
}
