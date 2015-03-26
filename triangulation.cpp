#include <GL/glut.h>
#include <cassert>
#include <iostream>
#include <cstdio>
#include <algorithm>
#include <vector>
#include <set>
#include <map>
#include <ctime>
#include <cmath>
using namespace std;

struct point {
	double x, y, z;
	point () {}
	point (double X, double Y, double Z) {x = X, y = Y, z = Z; }
	bool operator < (const point& other) const {
		if (x < other.x)
			return true;               
		if (x == other.x && y < other.y)
			return true;
		if (x == other.x && y == other.y && z < other.z)
			return true;
		return false;	
	}
};

struct Edge {
	double dist;
    int u, v;
    Edge () {}
    Edge (double D, int U, int V) {dist = D, u = U, v = V; }		
	
	bool operator < (const Edge& other) const {
		return dist < other.dist;
	}
};

int n;
#define MaxN 1000
point P[MaxN];
vector <Edge> E;
int g[MaxN][MaxN];

void init () {
	glClearColor (0.0, 0.0, 0.0, 0.0);
	glMatrixMode (GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-1.0, 101.0, -1.0, 101.0, -1.0, 1.0);
	
/*	glOrtho (-10.0, 10.0, -10.0, 10.0, -10.0, 10.0);	         
	gluLookAt(2., 2., 5., 0., 0., -100., 0., 1., 0.);
	GLfloat mat_specular[]={1.0,1.0,0.0,1.0};
	GLfloat mat_shininess[]={80.0};
	GLfloat light_position[]={2.0,1.0,2.0,0.0};
	GLfloat white_light[]={1.0,1.0,0.0,1.0};
	glClearColor(0.0,0.0,0.0,0.0);
	glShadeModel(GL_SMOOTH);
	glMaterialfv(GL_FRONT,GL_SPECULAR,mat_specular);
	glMaterialfv(GL_FRONT,GL_SHININESS,mat_shininess);

	glLightfv(GL_LIGHT0,GL_POSITION,light_position);
	glLightfv(GL_LIGHT0,GL_DIFFUSE,white_light);
	glLightfv(GL_LIGHT0,GL_SPECULAR,white_light);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
*/    	

}
        
GLfloat get () {
	return (rand() % 100) / 100.;
}

void display () {
	glClear (GL_COLOR_BUFFER_BIT);
	glBegin(GL_TRIANGLES); // of the color cube
    for (int i = 0; i < n; i++)
    	for (int j = i + 1; j < n; j++)
    		for (int k = j + 1; k < n; k++) {
    			if (g[i][j]==g[i][k] && g[i][k]==g[j][k] && g[i][j]) {
				    glColor3f(get(), get(), get());
    				glVertex3f(P[i].x, P[i].y, .0);
    				glVertex3f(P[j].x, P[j].y, .0);
    				glVertex3f(P[k].x, P[k].y, .0);
    			}
    		}
	glEnd(); // of the color cube
	glFlush();
}       
      
void reshape (int w, int h) {
	glViewport (0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode (GL_PROJECTION);
	glLoadIdentity();
	glFrustum (-1., 1., -1., 1., 1.5, 20.);
	glMatrixMode (GL_MODELVIEW);
}
    
inline double calcDist (int i, int j) {
	return ((P[i].x-P[j].x)*(P[i].x-P[j].x)+(P[i].y-P[j].y)*(P[i].y-P[j].y));	  	
}

bool isIntersect (int i, int j) {
	double a[3];                                             
	double d1=calcDist (E[i].u, E[i].v),d2=calcDist(E[j].u, E[j].v),d3=calcDist (E[i].u, E[j].v),d4=calcDist(E[j].u, E[i].v),d5=calcDist (E[i].u, E[j].u),d6=calcDist(E[j].v, E[i].v);
	a[0] = d1 + d2;
	a[1] = d3 + d4;
	a[2] = d5 + d6;
	sort (a,a+3);
	double maxDist = a[2];	
	double eps=0;
	bool res = (std::abs(E[i].dist + E[j].dist - maxDist) == eps) && d1 > eps && d2 > eps && d3 > eps  && d4 > eps && d5 > eps && d6 > eps ;
/*	string ans = "";
	if (!res)
		ans = " doesnt";	
	cout << E[i].u + 1 << " " << E[i].v + 1 << " with " << E[j].u + 1 << " " << E[j].v + 1 << ans << " intersect ";
	cout << "because maxDist = " << maxDist << " but current dist = " << E[i].dist + E[j].dist << " and abs() = " << abs (E[i].dist + E[j].dist - maxDist);
	printf (" \n");
*/	return res;
}

int main (int argc, char**argv) {

	freopen ("small_input.txt", "r", stdin);
	freopen ("output.txt", "w", stdout);
	cin >> n;
	cout << "n = " << n << endl;
	for (int i = 0; i < n; i++) {
		double x, y, z;
		cin >> x >> y >> z;
		assert(x <= 100 && x >= 0 && y <= 100 && y >= 0 && z <= 100 && z >= 0);
		P[i] = point (x, y, z);
	}
	for (int i = 0; i < n; i++)
		for (int j = i + 1; j < n; j++)
			E.push_back(Edge(calcDist (i, j), i, j));
	sort (E.begin(), E.end());
	vector <int> res;
	for (int i = 0; i < E.size(); i++) {
		bool ok = true;
		for (int j = 0; j < res.size(); j++)
			if (isIntersect(i, res[j])) {
				ok = false;
				break;				
			}
		if (ok) {
			res.push_back(i);
			g[E[i].u][E[i].v] = g[E[i].v][E[i].u] = 1;
			cout << E[i].u + 1 << " " << E[i].v + 1 << endl;
		}
	}
	
	cout << res.size() << endl;

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE|GLUT_RGBA);
	glutInitWindowSize(500, 500);
	glutInitWindowPosition(100, 100);
	glutCreateWindow ("hello world! ");
	init ();
	glutDisplayFunc (display);
	glutMainLoop();			
	
	return 0;
}
