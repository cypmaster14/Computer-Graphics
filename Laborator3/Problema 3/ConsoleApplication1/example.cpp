#define _CRT_SECURE_NO_WARNINGS

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <assert.h>
#include <float.h>

#include "glut.h"

// dimensiunea ferestrei in pixeli
#define dim 300
// numarul maxim de iteratii pentru testarea apartenentei la mult.Julia-Fatou
#define NRITER_JF 5000
// modulul maxim pentru testarea apartenentei la mult.Julia-Fatou
#define MODMAX_JF 10000000
// ratii ptr. CJuliaFatou
#define RX_JF 0.01
#define RY_JF 0.01

unsigned char prevKey;
int nivelPerron = 0;
int nivelSierpinski = 0;
int nivelPatratele = 0;

class C2coord
{
public:
	C2coord()
	{
		m.x = m.y = 0;
	}

	C2coord(double x, double y)
	{
		m.x = x;
		m.y = y;
	}

	C2coord(const C2coord &p)
	{
		m.x = p.m.x;
		m.y = p.m.y;
	}

	C2coord &operator=(C2coord &p)
	{
		m.x = p.m.x;
		m.y = p.m.y;
		return *this;
	}

	int operator==(C2coord &p)
	{
		return ((m.x == p.m.x) && (m.y == p.m.y));
	}

protected:
	struct SDate
	{
		double x, y;
	} m;
};

class CPunct : public C2coord
{
public:
	CPunct() : C2coord(0.0, 0.0)
	{}

	CPunct(double x, double y) : C2coord(x, y)
	{}

	CPunct &operator=(const CPunct &p)
	{
		m.x = p.m.x;
		m.y = p.m.y;
		return *this;
	}

	void getxy(double &x, double &y)
	{
		x = m.x;
		y = m.y;
	}

	int operator==(CPunct &p)
	{
		return ((m.x == p.m.x) && (m.y == p.m.y));
	}

	void marcheaza()
	{
		glBegin(GL_POINTS);
		glVertex2d(m.x, m.y);
		glEnd();
	}

	void print(FILE *fis)
	{
		fprintf(fis, "(%+f,%+f)", m.x, m.y);
	}

};

class CVector : public C2coord
{
public:
	CVector() : C2coord(0.0, 0.0)
	{
		normalizare();
	}

	CVector(double x, double y) : C2coord(x, y)
	{
		normalizare();
	}

	CVector &operator=(CVector &p)
	{
		m.x = p.m.x;
		m.y = p.m.y;
		return *this;
	}

	int operator==(CVector &p)
	{
		return ((m.x == p.m.x) && (m.y == p.m.y));
	}

	CPunct getDest(CPunct &orig, double lungime)
	{
		double x, y;
		orig.getxy(x, y);
		CPunct p(x + m.x * lungime, y + m.y * lungime);
		return p;
	}

	void rotatie(double grade)
	{
		double x = m.x;
		double y = m.y;
		double t = 2 * (4.0 * atan(1.0)) * grade / 360.0;
		m.x = x * cos(t) - y * sin(t);
		m.y = x * sin(t) + y * cos(t);
		normalizare();
	}

	void deseneaza(CPunct p, double lungime)
	{
		double x, y;
		p.getxy(x, y);
		glColor3f(1.0, 0.1, 0.1);
		glBegin(GL_LINE_STRIP);
		glVertex2d(x, y);
		glVertex2d(x + m.x * lungime, y + m.y * lungime);
		glEnd();
	}

	void print(FILE *fis)
	{
		fprintf(fis, "%+fi %+fj", C2coord::m.x, C2coord::m.y);
	}

private:
	void normalizare()
	{
		double d = sqrt(C2coord::m.x * C2coord::m.x + C2coord::m.y * C2coord::m.y);
		if (d != 0.0)
		{
			C2coord::m.x = C2coord::m.x * 1.0 / d;
			C2coord::m.y = C2coord::m.y * 1.0 / d;
		}
	}
};

class CComplex {
public:
	CComplex() : re(0.0), im(0.0) {}
	CComplex(double re1, double im1) : re(re1 * 1.0), im(im1 * 1.0) {}
	CComplex(const CComplex &c) : re(c.re), im(c.im) {}
	~CComplex() {}

	CComplex &operator=(const CComplex &c)
	{
		re = c.re;
		im = c.im;
		return *this;
	}

	double getRe() { return re; }
	void setRe(double re1) { re = re1; }

	double getIm() { return im; }
	void setIm(double im1) { im = im1; }

	double getModul() { return sqrt(re * re + im * im); }

	int operator==(CComplex &c1)
	{
		return ((re == c1.re) && (im == c1.im));
	}

	CComplex pow2()
	{
		CComplex rez;
		rez.re = powl(re * 1.0, 2) - powl(im * 1.0, 2);
		rez.im = 2.0 * re * im;
		return rez;
	}

	friend CComplex operator+(const CComplex &c1, const CComplex &c2);
	friend CComplex operator*(CComplex &c1, CComplex &c2);

	void print(FILE *f)
	{
		fprintf(f, "%.20f%+.20f i", re, im);
	}

private:
	double re, im;
};

CComplex operator+(const CComplex &c1, const CComplex &c2)
{
	CComplex rez(c1.re + c2.re, c1.im + c2.im);
	return rez;
}

CComplex operator*(CComplex &c1, CComplex &c2)
{
	CComplex rez(c1.re * c2.re - c1.im * c2.im,
		c1.re * c2.im + c1.im * c2.re);
	return rez;
}

class Mandelbrot
{
public:
	Mandelbrot()
	{
		// m.c se initializeaza implicit cu 0+0i

		m.nriter = NRITER_JF;
		m.modmax = MODMAX_JF;
	}

	Mandelbrot(CComplex &c)
	{
		m.c = c;
		m.nriter = NRITER_JF;
		m.modmax = MODMAX_JF;
	}

	~Mandelbrot() {}

	void setmodmax(double v) { assert(v <= MODMAX_JF); m.modmax = v; }
	double getmodmax() { return m.modmax; }

	void setnriter(int v) { assert(v <= NRITER_JF); m.nriter = v; }
	int getnriter() { return m.nriter; }

	int isIn(CComplex &x, double &nrIt)
	{
		int rez = 1;
		int i;

		CComplex z0, z1;

		z0 = CComplex(0, 0);

		for (i = 1; i < m.nriter; i++)
		{
			z1 = z0 * z0 + x;

			if (z1 == z0)
			{
				rez = -1;
				break;
			}

			if (z1.getModul() > 2)
			{
				rez = 0;
				break;
			}

			z0 = z1;
		}

		nrIt = i;
		return rez;
	}

	void display(double xmin, double ymin, double xmax, double ymax)
	{
		glPushMatrix();
		glLoadIdentity();

		//    glTranslated((xmin + xmax) * 1.0 / (xmin - xmax), (ymin + ymax)  * 1.0 / (ymin - ymax), 0);
		//    glScaled(1.0 / (xmax - xmin), 1.0 / (ymax - ymin), 1);
		// afisarea propriu-zisa
		glBegin(GL_POINTS);
		double nrIt;
		for (double x = xmin; x <= xmax; x += RX_JF)
		for (double y = ymin; y <= ymax; y += RY_JF)
		{
			CComplex z(x, y);
			int r = isIn(z, nrIt);

			if (r == 1)
			{
				glColor3f(1.0, 0.1, 0.1);
			}
			else
			{
				glColor3f(0.1*nrIt, 0.1, 0.1);
			}

			glVertex3d(x / 2.0, y / 2.0, 0);
		}
		fprintf(stdout, "STOP\n");
		glEnd();

		glPopMatrix();
	}
private:
	struct SDate {
		CComplex c;
		// nr. de iteratii
		int nriter;
		// modulul maxim
		double modmax;
	} m;
};

void deseneazaPatrat(CPunct p1, double lungime)
{

	double x1, y1;
	p1.getxy(x1, y1);
	CVector v(0, -1);
	CPunct p = p1;
	v.rotatie(90);
	v.deseneaza(p, lungime);

	p = v.getDest(p, lungime);
	v.rotatie(90);
	v.deseneaza(p, lungime);

	p = v.getDest(p, lungime);
	v.rotatie(90);
	v.deseneaza(p, lungime);

	p = v.getDest(p, lungime);
	v.rotatie(90);
	v.deseneaza(p, lungime);
}

void deseneazaPatratele(CPunct p1, int nivel, double lungime)
{
	if (nivel == 0){
		return;
	}
	double x1, y1;
	p1.getxy(x1, y1);
	deseneazaPatratele(CPunct(x1, y1), nivel - 1, lungime / 3);
	deseneazaPatratele(CPunct(x1 + lungime / 3, y1), nivel - 1, lungime / 3);
	deseneazaPatratele(CPunct(x1 + 2 * lungime / 3, y1), nivel - 1, lungime / 3);
	deseneazaPatratele(CPunct(x1, y1 + lungime / 3), nivel - 1, lungime / 3);
	deseneazaPatrat(CPunct(x1 + lungime / 3, y1 + lungime / 3), lungime / 3);

	deseneazaPatratele(CPunct(x1 + 2 * lungime / 3, y1 + lungime / 3), nivel - 1, lungime / 3);
	deseneazaPatratele(CPunct(x1, y1 + 2 * lungime / 3), nivel - 1, lungime / 3);
	deseneazaPatratele(CPunct(x1 + lungime / 3, y1 + 2 * lungime / 3), nivel - 1, lungime / 3);
	deseneazaPatratele(CPunct(x1 + 2 * lungime / 3, y1 + 2 * lungime / 3), nivel - 1, lungime / 3);


}

void drawImg1(int nivel)
{
	deseneazaPatrat(CPunct(-0.5, -0.5), 1);

	deseneazaPatratele(CPunct(-0.5, -0.5), nivel, 1);
}

class CArborePerron
{
public:

	void arborePerron(double lungime,
		int nivel,
		double factordiviziune,
		CPunct p,
		CVector v)
	{
		assert(factordiviziune != 0);
		CPunct p1, p2, p3;
		if (nivel == 0)
		{
		}
		else
		{
			v.rotatie(-45);
			v.deseneaza(p, lungime);
			p1 = v.getDest(p, lungime);
			arborePerron(lungime * factordiviziune, nivel - 1, factordiviziune, p1, v);

			v.rotatie(90);
			v.deseneaza(p, lungime);
			p1 = v.getDest(p, lungime);
			p2 = p1;
			p3 = p1;

			v.rotatie(-45);
			v.deseneaza(p1, lungime);
			p1 = v.getDest(p1, lungime);
			p2 = p1;

			lungime /= 2;
			v.rotatie(-90);
			v.deseneaza(p1, lungime);
			p1 = v.getDest(p1, lungime);
			arborePerron(lungime * factordiviziune, nivel - 1, factordiviziune, p1, v);

			p1 = p2;
			v.rotatie(120);
			v.deseneaza(p1, lungime);
			p1 = v.getDest(p1, lungime);
			arborePerron(lungime * factordiviziune, nivel - 1, factordiviziune, p1, v);

			lungime *= 2;
			p1 = p3;
			v.rotatie(30);
			v.deseneaza(p1, lungime);
			p1 = v.getDest(p1, lungime);
			arborePerron(lungime * factordiviziune, nivel - 1, factordiviziune, p1, v);

		}
	}

	void afisare(double lungime, int nivel)
	{
		CVector v(0.0, -1.0);
		CPunct p(0.0, 3.0);

		v.deseneaza(p, 0.25);
		p = v.getDest(p, 0.25);
		arborePerron(lungime, nivel, 0.4, p, v);
	}
};

class CSierpinskiArrowheadCurve
{
public:

	void sierpinskiArrowheadCurve(double lungime, int nivel, CPunct p, CVector v, int d)
	{
		if (nivel == 0)
		{
			v.deseneaza(p, lungime);
		}
		else
		{

			//p = v.getDest(p, lungime / 2);
			v.rotatie(d * 60);

			sierpinskiArrowheadCurve(lungime / 2, nivel - 1, p, v, -d);

			p = v.getDest(p, lungime / 2);
			v.rotatie(-d * 60);

			sierpinskiArrowheadCurve(lungime / 2, nivel - 1, p, v, d);

			p = v.getDest(p, lungime / 2);
			v.rotatie(-d * 60);

			sierpinskiArrowheadCurve(lungime / 2, nivel - 1, p, v, -d);
			p = v.getDest(p, lungime / 2);
		}
	}

	void afisare(double lungime, int nivel)
	{
		CVector v(1.0, 0.55);
		CPunct p(0.0, 0.0);

		// If order is even we can just draw the curve.
		if (0 == (nivel & 1))
		{
			sierpinskiArrowheadCurve(lungime, nivel, p, v, 1);
		}
		else /* order is odd */
		{
			v.rotatie(60);
			sierpinskiArrowheadCurve(lungime, nivel, p, v, -1);
		}
	}
};

void Display1() {
	CComplex c(-0.12375, 0.056805);
	Mandelbrot cjf(c);

	cjf.setnriter(20);
	cjf.display(-2.0, -
		2.0, 2.0, 2.0);
}

void Display2() {
	CArborePerron cap;

	char c[3];
	sprintf(c, "%2d", nivelPerron);
	glRasterPos2d(-0.98, -0.98);
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'N');
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'i');
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'v');
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'e');
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'l');
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, '=');
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, c[0]);
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, c[1]);

	glRasterPos2d(-1.0, -0.9);
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'a');
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'r');
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'b');
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'o');
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'r');
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'e');
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, ' ');
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'P');
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'e');
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'r');
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'r');
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'o');
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'n');

	glPushMatrix();
	glLoadIdentity();
	glScaled(0.4, 0.4, 1);
	glTranslated(-0.5, -0.5, 0.0);
	cap.afisare(1, nivelPerron);
	glPopMatrix();
	nivelPerron++;
}

void Display3() {
	CSierpinskiArrowheadCurve cch;
	cch.afisare(0.5, nivelSierpinski);

	char c[3];
	sprintf(c, "%2d", nivelSierpinski);
	glRasterPos2d(-0.98, -0.98);
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'N');
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'i');
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'v');
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'e');
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'l');
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, '=');
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, c[0]);
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, c[1]);

	glRasterPos2d(-1.0, -0.9);
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'c');
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'u');
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'r');
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'b');
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'a');
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, ' ');
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'H');
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'i');
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'l');
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'b');
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'e');
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'r');
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 't');
	nivelSierpinski++;
}

void Display4() {
	drawImg1(nivelPatratele++);
	char c[3];
	sprintf(c, "%2d", nivelPatratele);
	glRasterPos2d(-0.98, -0.98);
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'N');
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'i');
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'v');
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'e');
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'l');
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, '=');
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, c[0]);
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, c[1]);
}

void Init(void) {

	glClearColor(1.0, 1.0, 1.0, 1.0);

	glLineWidth(1);

	glPointSize(3);

	glPolygonMode(GL_FRONT, GL_LINE);
}

void Display(void)
{
	switch (prevKey)
	{
	case '0':
		glClear(GL_COLOR_BUFFER_BIT);
		nivelPerron = nivelPatratele = nivelSierpinski = 0;
		break;
	case '1':
		glClear(GL_COLOR_BUFFER_BIT);
		Display1();
		break;
	case '2':
		glClear(GL_COLOR_BUFFER_BIT);
		Display2();
		break;
	case '3':
		glClear(GL_COLOR_BUFFER_BIT);
		Display3();
		break;
	case '4':
		glClear(GL_COLOR_BUFFER_BIT);
		Display4();
		break;
	default:
		break;
	}

	glFlush();
}

void Reshape(int w, int h)
{
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
}

void KeyboardFunc(unsigned char key, int x, int y)
{
	prevKey = key;
	if (key == 27) // escape
		exit(0);
	glutPostRedisplay();
}

void MouseFunc(int button, int state, int x, int y)
{
}

int main(int argc, char** argv)
{
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