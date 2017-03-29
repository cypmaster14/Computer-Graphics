#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "glut.h"
#include <map>

using namespace std;


class GrilaCarteziana {
public:
	int numarLinii;
	int numarColoane;
	double cx;
	double cy;
	double dc;
	double de;
	double epsilon;
	double radius;
	GrilaCarteziana(int numarLinii, int numarColoane);
	void deseneazaLinii();
	void deseneazaColoane();
	void writePixel(double i, double j);
	void afisareSegmentDreapta3(double x0, double y0, double xn, double yn);

	void afisareSegmentDreapta3_1(double x0, double y0, double xn, double yn);


	void deseneaaCerc(double x, double y, double r, int numberOfSegments);
	void afisareCerc4();
	void afisarePuncteCerc3(double x, double y3);
	void cerc(double i, double j);
};
