#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "glut.h"

// dimensiunea ferestrei in pixeli
#define dim 300

unsigned char prevKey;

// concoida lui Nicomede (concoida dreptei)
// $x = a + b \cdot cos(t), y = a \cdot tg(t) + b \cdot sin(t)$. sau
// $x = a - b \cdot cos(t), y = a \cdot tg(t) - b \cdot sin(t)$. unde
// $t \in (-\pi / 2, \pi / 2)$
void Display1() {
	double xmax, ymax, xmin, ymin;
	double a = 1, b = 2;
	double pi = 4 * atan(1.0);
	double ratia = 0.05;
	double t;

	// calculul valorilor maxime/minime ptr. x si y
	// aceste valori vor fi folosite ulterior la scalare
	xmax = a - b - 1;
	xmin = a + b + 1;
	ymax = ymin = 0;
	for (t = -pi / 2 + ratia; t < pi / 2; t += ratia) {
		double x1, y1, x2, y2;
		x1 = a + b * cos(t);
		xmax = (xmax < x1) ? x1 : xmax;
		xmin = (xmin > x1) ? x1 : xmin;

		x2 = a - b * cos(t);
		xmax = (xmax < x2) ? x2 : xmax;
		xmin = (xmin > x2) ? x2 : xmin;

		y1 = a * tan(t) + b * sin(t);
		ymax = (ymax < y1) ? y1 : ymax;
		ymin = (ymin > y1) ? y1 : ymin;

		y2 = a * tan(t) - b * sin(t);
		ymax = (ymax < y2) ? y2 : ymax;
		ymin = (ymin > y2) ? y2 : ymin;
	}

	xmax = (fabs(xmax) > fabs(xmin)) ? fabs(xmax) : fabs(xmin);
	ymax = (fabs(ymax) > fabs(ymin)) ? fabs(ymax) : fabs(ymin);
	xmax *= 1.1;
	ymax *= 1.1;

	// afisarea punctelor propriu-zise precedata de scalare
	glColor3f(1, 0.1, 0.1); // rosu
	glBegin(GL_LINE_STRIP);
	for (t = -pi / 2 + ratia; t < pi / 2; t += ratia) {
		double x1, y1, x2, y2;
		x1 = (a + b * cos(t)) / xmax;
		x2 = (a - b * cos(t)) / xmax;
		y1 = (a * tan(t) + b * sin(t)) / ymax;
		y2 = (a * tan(t) - b * sin(t)) / ymax;

		glVertex2f(x1, y1);
	}
	glEnd();

	glBegin(GL_LINE_STRIP);
	for (t = -pi / 2 + ratia; t < pi / 2; t += ratia) {
		double x1, y1, x2, y2;
		x1 = (a + b * cos(t)) / xmax;
		x2 = (a - b * cos(t)) / xmax;
		y1 = (a * tan(t) + b * sin(t)) / ymax;
		y2 = (a * tan(t) - b * sin(t)) / ymax;

		glVertex2f(x2, y2);
	}
	glEnd();
}

// graficul functiei 
// $f(x) = \bar sin(x) \bar \cdot e^{-sin(x)}, x \in \langle 0, 8 \cdot \pi \rangle$, 
void Display2() {
	double pi = 4 * atan(1.0);
	double xmax = 8 * pi;
	double ymax = exp(1.1);
	double ratia = 0.05;

	// afisarea punctelor propriu-zise precedata de scalare
	glColor3f(1, 0.1, 0.1); // rosu
	glBegin(GL_LINE_STRIP);
	for (double x = 0; x < xmax; x += ratia) {
		double x1, y1;
		x1 = x / xmax;
		y1 = (fabs(sin(x)) * exp(-sin(x))) / ymax;

		glVertex2f(x1, y1);
	}
	glEnd();
}

double nearestInteger(double number) {
	double fractionalPart = number - int(number);
	if (fractionalPart > 0.5) {
		return fabs(1.0 - fractionalPart);
	}
	return fractionalPart;
}

//Graficul functiei f(x)
void Display3() {

	double ymax = 1;
	double xmax = 100;
	double ratia = 0.05;

	ymax *= 1.05;
	xmax *= 1.1;

	glColor3f(1, 0.1, 0.1); // rosu
	glBegin(GL_LINE_STRIP);
	glVertex2f(0 / xmax, 1 / ymax);
	for (double x = 0 + ratia; x <= 100; x += ratia) {
		double distance = nearestInteger(x);
		glVertex2f(x / xmax, distance / x / ymax);
	}
	glEnd();
}


//Melcul lui Pascal
void Display4(double a, double b) {

	double xmax, ymax;
	double pi = 4 * atan(1.0);
	double ratia = 0.05;

	xmax = 2 * (a + b) - 1;
	ymax = 2 * (a + b) - 1;

	double x, y;
	for (double t = -pi + ratia; t < pi; t += ratia) {
		x = 2 * (a*cos(t) + b)*cos(t);
		y = 2 * (a*cos(t) + b)*sin(t);
		xmax = (xmax > x) ? xmax : x;
		ymax = (ymax > y) ? ymax : y;
	}

	printf("Xmax:%f\tYmax:%f\n", xmax, ymax);


	xmax = xmax* 1.1;
	//depinde de a si b
	ymax = ymax * 1.7;

	glColor3f(1, 0.1, 0.1); // rosu
	glBegin(GL_LINE_LOOP);

	for (double t = -pi + ratia; t <= pi ; t += ratia) {
		x = 2 * (a*cos(t) + b)*cos(t);
		y = 2 * (a*cos(t) + b)*sin(t);
		glVertex2f(x / xmax, y / ymax);
	}
	glEnd();

}

//Trisectoarea lui Longchamps: 
void Display5(double a) {
	double pi = 4 * atan(1);
	double ratia = 0.0055;
	double x, y;

	int count = 0;
	double xx[1000], yy[1000];
	for (double t = -pi / 2 + ratia; t < -pi / 6; t += ratia) {
		if (fabs(t) == pi / 6) {
			continue;
		}
		x = 0.2 / (4 * cos(t) * cos(t) - 3);
		y = 0.2 * tan(t) / (4 * cos(t) * cos(t) - 3);
		if (fabs(x) > 1 || fabs(y) > 1)
			continue;
		xx[count] = x;
		yy[count] = y;
		//printf("X:%f\tY:%f\t\n", x, y);
		count++;
	}

	//extremitatile
	double x1, y1;
	x1 = xx[count-2]; //se sare peste ultimul punct ca sa fie ca in figura, mai la dreapta
	y1 = yy[0];

	glShadeModel(GL_FLAT);
	glBegin(GL_TRIANGLE_FAN);
	glColor3f(1, 0.1, 0.1);
	glVertex2f(x1, y1);
	printf("X:%f\tY:%f\t\n", x1, y1);
	int paint = 2;
	for (int i = 0; i < count; i++) {
		if (fabs(yy[i] < 0.25)) {
			paint = 0;
			continue;
		}
		if (i % 3 == paint) {
			glColor3f(1, 0.1, 0.1);
		}
		else {
			glColor3f(1, 1, 1);
		}
		glVertex2f(xx[i], yy[i]);
		printf("X:%f\tY:%f\tI:%d\n", xx[i], yy[i],i);

	}
	glEnd();

	glBegin(GL_LINE_LOOP);
	glColor3f(0.2, 0.15, 0.88); // albastru
	glVertex2f(x1, y1);
	for (int i = 0; i < count - 1; i++) {
		glVertex2f(xx[i], yy[i]);
	}
	glEnd();

}

//Cicloida
void Display6(double a, double b) {

	double xmax, ymax;
	double ratia = 0.05;

	xmax = a*-1 - b + 1;
	ymax = a - b + 1;
	double x, y;
	for (double t = -10 + ratia; t < 10; t += ratia) {
		x = a*t - b*sin(t);
		y = a - b*cos(t);
		xmax = (xmax > x) ? xmax : x;
		ymax = (ymax > y) ? ymax : y;
	}

	xmax = xmax*1.1;
	ymax = ymax*1.1;

	glColor3f(1.0, 0.1, 0.1);
	glBegin(GL_LINE_STRIP);

	for (double t = -10 + ratia; t < 10; t += ratia) {
		x = a*t - b*sin(t);
		y = a - b*cos(t);
		glVertex2f(x / xmax, y / ymax);
	}

	glEnd();
}

//Epicicloida
void Display7(double R, double r) {

	double xmax, ymax;
	double x, y;
	double ratia = 0.05;
	double pi = 4 * atan(1.0);

	xmax = R + r - r + 1;
	ymax = R + r - r + 1;

	for (double t = 0; t <= 2 * pi; t += ratia) {
		x = (R + r)*cos(r / R*t) - r*cos(t + r / R*t);
		y = (R + r)*sin(r / R*t) - r*sin(t + r / R*t);

		xmax = (xmax > x) ? xmax : x;
		ymax = (ymax > y) ? ymax : y;
	}

	printf("Xmax:%f\t Ymax:%f\n", xmax, ymax);

	xmax = xmax*1.1;
	ymax = ymax*1.1;

	glColor3f(1.0, 0.1, 0.1);
	glBegin(GL_LINE_STRIP);
	for (double t = 0; t <= 2 * pi; t += ratia) {
		x = (R + r)*cos(r / R*t) - r*cos(t + r / R*t);
		y = (R + r)*sin(r / R*t) - r*sin(t + r / R*t);
		glVertex2f(x / xmax, y / ymax);
	}

	glEnd();

}

//Hipocicloida
void Display8(double R, double r) {

	double xmax, ymax;
	double x, y;
	double ratia = 0.05;
	double pi = 4 * atan(1.0);

	xmax = R - r - r + 1;
	ymax = R - r - r + 1;


	for (double t = 0; t <= 2 * pi; t += ratia) {
		x = (R - r)*cos(r / R*t) - r*cos(t - r / R*t);
		y = (R - r)*sin(r / R*t) - r*sin(t - r / R*t);

		xmax = (xmax > x) ? xmax : x;
		ymax = (ymax > y) ? ymax : y;
	}

	printf("Xmax:%f\t Ymax:%f\n", xmax, ymax);

	xmax = xmax*2;
	ymax = ymax*2;

	glColor3f(1.0, 0.1, 0.1);
	glBegin(GL_LINE_STRIP);
	for (double t = 0; t <= 2 * pi; t += ratia) {
		x = (R - r)*cos(r / R*t) - r*cos(t - r / R*t);
		y = (R - r)*sin(r / R*t) - r*sin(t - r / R*t);
		glVertex2f(x / xmax, y / ymax);
	}

	glEnd();

}

//Bernoulli
void Display9(double a) {
	double xmax, ymax, xmin, ymin, rmax, rmin, r1, r2;
	double pi = 4 * atan(1.0);
	double ratia = 0.001;
	double t;
	double r;

	rmax = a * sqrt(2) - 1;
	for (t = -pi / 4 + ratia; t < pi / 4; t += ratia) {
		r = a * sqrt(2 * cos(2 * t));
		rmax = (r > rmax) ? r : rmax;
	}

	rmax *= 1.1;

	glColor3f(1, 0.1, 0.1); // rosu

	glBegin(GL_LINE_STRIP);
	for (t = pi / 4 - ratia; t > -pi / 4; t -= ratia) {
		double x1, y1;
		r1 = a*sqrt(2 * cos(2 * t)) / rmax;

		x1 = r1*cos(t);
		y1 = r1*sin(t);
		glVertex2f(x1, y1);
	}

	for (t = -pi / 4 + ratia; t < pi / 4; t += ratia) {
		double x2, y2;
		r2 = -a * sqrt(2 * cos(2 * t)) / rmax;

		x2 = r2 * cos(t);
		y2 = r2 * sin(t);
		glVertex2f(x2, y2);
	}
	glEnd();
}

void Display0(double a) {
	double  rmax, r;
	double ratia = 0.05;
	double t, tmax;

	tmax = 2.95;
	rmax = a * exp(exp(1) + 1) - 1;

	for (t = 0 + ratia; t < tmax; t += ratia) {
		r = a * exp(1 + t);
		rmax = (r > rmax) ? r : rmax;
	}

	rmax *= 1.1;

	glColor3f(1, 0.1, 0.1); // rosu
	glBegin(GL_LINE_STRIP);
	for (t = 0 + ratia; t < tmax; t += ratia) {
		double x, y;

		r = a * exp(1 + t) / rmax;
		x = r * cos(t);
		y = r * sin(t);
		glVertex2f(x, y);
	}
	glEnd();
}

void Init(void) {

	glClearColor(1.0, 1.0, 1.0, 1.0);

	glLineWidth(1);

	//   glPointSize(4);

	glPolygonMode(GL_FRONT, GL_LINE);
}

void Display(void) {
	glClear(GL_COLOR_BUFFER_BIT);

	switch (prevKey) {
	case '1':
		Display1();
		break;
	case '2':
		Display2();
		break;
	case '3':
		Display3();
		break;
	case '4':
		Display4(0.3, 0.2);
		break;
	case '5':
		Display5(0.2);
		break;
	case '6':
		Display6(0.1, 0.2);
		break;
	case '7':
		Display7(0.1, 0.3);
		break;
	case '8':
		Display8(0.1, 0.3);
		break;
	case '9':
		Display9(0.4);
		break;
	case '0':
		Display0(0.02);
		break;

	default:
		break;
	}

	glFlush();
}

void Reshape(int w, int h) {
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
}

void KeyboardFunc(unsigned char key, int x, int y) {
	prevKey = key;
	if (key == 27) // escape
		exit(0);
	glutPostRedisplay();
}

void MouseFunc(int button, int state, int x, int y) {
}

int main(int argc, char** argv) {

	glutInit(&argc, argv);

	glutInitWindowSize(dim, dim);

	glutInitWindowPosition(100, 100);

	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);

	glutCreateWindow(argv[0]);

	Init();

	glutReshapeFunc(Reshape);

	glutKeyboardFunc(KeyboardFunc);

	glutMouseFunc(MouseFunc);

	glutDisplayFunc(Display);

	glutMainLoop();

	return 0;
}