#include "GrilaCarteziana.h"
#include <stdlib.h>
#include <math.h>
#include "glut.h"



GrilaCarteziana::GrilaCarteziana(int numarLinii, int numarColoane) {
	this->numarLinii = numarLinii;
	this->numarColoane = numarColoane;
	this->epsilon = 0.1;
	this->cx = -1 + this->epsilon;
	this->cy = -1 + this->epsilon;
	this->dc = (2 - 2 * this->epsilon) / (numarColoane - 1);
	this->de = (2 - 2 * this->epsilon) / (numarLinii - 1);

	this->radius = this->dc / 3;
	this->deseneazaColoane();
	this->deseneazaLinii();
}


void GrilaCarteziana::deseneazaColoane() {
	glColor3f(0.0, 0.0, 0.0);
	glBegin(GL_LINES);
	double p_x = this->cx;
	double p1_y = this->cy;
	double p2_y = 1 - this->epsilon;
	for (int i = 1; i <= this->numarColoane; i++) {
		glVertex2f(p_x, p1_y);
		glVertex2f(p_x, p2_y);
		p_x += this->dc;
	}
	glEnd();
}

void GrilaCarteziana::deseneazaLinii() {
	glColor3f(0.0, 0.0, 0.0);
	glBegin(GL_LINES);
	double p1_x = this->cx;
	double p_y = this->cy;
	double p2_x = -this->cx;
	for (int i = 1; i <= this->numarLinii; i++) {
		glVertex2f(p1_x, p_y);
		glVertex2f(p2_x, p_y);
		p_y += this->de;
	}
	glEnd();
}


void GrilaCarteziana::deseneaaCerc(double x, double y, double r, int numberOfSegments) {

	double pi = 4 * atan(1.0);

	glColor3ub(71, 71, 71);
	glBegin(GL_TRIANGLE_FAN);
	glVertex2f(x, y);

	for (int i = 0; i < numberOfSegments; i++) {
		float x_aux = x + (this->radius*cos(i * 2 * pi / numberOfSegments));
		float y_aux = y + (this->radius*sin(i * 2 * pi / numberOfSegments));
		glVertex2f(x_aux, y_aux);
	}

	glEnd();
	
}



void GrilaCarteziana::afisarePuncteCerc3(double x, double y) {
	map<double, double> m;
	m[x] = y;
	m[-x] = -y;
	m[-x] = y;
	m[x] = -y;
	if (x != y) {
		m[y] = x;
		m[-y] = -x;
		m[-y] = x;
		m[y] = -x;
	}
}

void GrilaCarteziana::afisareCerc4() {
	int x = 0;
	float y = this->radius;
	int d = 1 - this->radius;
	int dE = 3, dSE = 2 - this->radius + 5;
	
	map<double, double> m;
	m[x] = y;
	m[-x] = -y;
	m[-x] = y;
	m[x] = -y;
	if (x != y) {
		m[y] = x;
		m[-y] = -x;
		m[-y] = x;
		m[y] = -x;
	}



	while (y > x) {
		if (d < 0) {
			d += dE;
			dE += 2;
			dSE += 2;
		}
		else {
			d += dSE;
			dE += 2;
			dSE += 4;
			y--;
		}
		x++;
		


		m[x] = y;
		m[-x] = -y;
		m[-x] = y;
		m[x] = -y;
		if (x != y) {
			m[y] = x;
			m[-y] = -x;
			m[-y] = x;
			m[y] = -x;
		}

	}

	glColor3f(10, 0.1, 0.1);
	glLineWidth(4);
	glBegin(GL_LINES);

	map<double, double> ::iterator it;
	for (it = m.begin(); it != m.end(); it++) {
		glVertex2f(this->cx+it->first*this->dc, this->cy+it->second*this->de);
	}
	glEnd();

}


void GrilaCarteziana::writePixel(int i, int j) {
	double x = this->cx + i*this->dc;
	double y = this->cy + j*this->de;
	deseneaaCerc(x, y, radius, 10000);
	//afisareCerc4();
	printf("[WritePixel]X:%f Y:%f\n", x, y);
}



void GrilaCarteziana::afisareSegmentDreapta3(int x0, int y0, int xn, int yn) {
	double dx, dy;
	//panta dreptei m = rise/run (cat urc/cobor pe linii supra cat merg la stg/dreapta pe coloane)
	dx = abs(xn - x0);//run
	dy = abs(yn - y0);//rise
	

	
	int d = 2 * dy - dx;
	int dE = 2 * dy;
	int dNE = 2 * (dy - dx);
	int x = x0, y = y0;
	map<int, int> m;
	m[x] = y;
	printf("X:%d Y:%d\n", x, y);
	if (y < this->numarLinii) {
		this->writePixel(x, y);
	}
	while (x < xn) {
		printf("d:%d\n",d);
		if (d <= 0) {
			d += dE;
			x++;
		}else{
			d += dNE;
			x++;
			if (x0 < xn && y0 < yn) {
				y++;
			}
			else {
				y--;
			}
		}
		printf("X:%d Y:%d\n", x, y);
		if (y < this->numarLinii) {
			this->writePixel(x, y);
		}
		m[x] = y;
	}



	//Desenez segmentul de dreapta
	//glColor3f(1.0, 0.1, 0.1);
	//glBegin(GL_LINES); 
	//double x1, y1;
	//x1 = this->cx + x0 * this->dc;
	//y1 = this->cy + y0 * this->de;
	//glVertex2f(x1, y1);
	//x1 = this->cx + xn * this->dc;
	//y1 = this->cy + yn * this->de;
	//glVertex2f(x1, y1);
	//glEnd();

}

void GrilaCarteziana::deseneazaSegmentMartor(int x0, int y0, int xn, int yn) {
	glColor3f(1.0, 0.1, 0.1);
	glBegin(GL_LINES); 
	double x1, y1;
	x1 = this->cx + x0 * this->dc;
	y1 = this->cy + y0 * this->de;
	glVertex2f(x1, y1);
	x1 = this->cx + xn * this->dc;
	y1 = this->cy + yn * this->de;
	glVertex2f(x1, y1);
	glEnd();
}

void GrilaCarteziana::afisareSegmentDreapta3_1(int x0, int y0, int xn, int yn) {
	double dx, dy;
	if (x0 < xn && y0 < yn) {
		dx = xn - x0;
		dy = yn - y0;
	}
	else {
		dx = abs(xn - x0);
		dy = abs(yn - y0);
	}
	int d = 2 * dy - dx;
	int dE = 2 * dy;
	int dNE = 2 * (dy - dx);
	int x = x0, y = y0;
	map<int, int> m;
	m[x] = y;
	printf("X:%d Y:%d\n", x, y);	
	this->writePixel(x, y);
	while (x> xn) {
		printf("d:%d\n", d);
		if (d <= 0) {
			d += dE;
			x++;
		}
		else {
			d += dNE;
			x++;
			if (x0 < xn && y0 < yn) {
				y++;
			}
			else {
				y--;
			}
		}
		printf("X:%d Y:%d\n", x, y);
		this->writePixel(x, y);
		m[x] = y;
	}




	glColor3f(1.0, 0.1, 0.1);
	glBegin(GL_LINES); // 0 15 15 10
	double x1, y1;
	x1 = this->cx + x0 * this->dc;
	y1 = this->cy + y0 * this->de;
	glVertex2f(x1, y1);
	x1 = this->cx + xn * this->dc;
	y1 = this->cy + yn * this->de;
	glVertex2f(x1, y1);
	glEnd();

}
