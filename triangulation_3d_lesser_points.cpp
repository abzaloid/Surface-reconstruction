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
#include <bitset>
using namespace std;
#define stripeImageWidth 32
GLubyte stripeImage[4*stripeImageWidth];
static GLuint texName;
void makeStripeImage(void) {
   for (int j = 0; j < stripeImageWidth; j++) {
      stripeImage[4*j] = (GLubyte) ((j<=rand()%5) ? 255 : 0);
      stripeImage[4*j+1] = (GLubyte) ((j>rand()%5) ? 255 : 0);
      stripeImage[4*j+2] = (GLubyte) 0;
      stripeImage[4*j+3] = (GLubyte) 255;
   }
}

/*  planes for texture coordinate generation  */
static GLfloat xequalzero[] = {1.0, 0.0, 0.0, 0.0};
static GLfloat slanted[] = {1.0, 1.0, 1.0, 0.0};
static GLfloat *currentCoeff;
static GLenum currentPlane;
static GLint currentGenMode;

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
#define MaxN 10000
point P[MaxN];
vector <Edge> E;
bool g[MaxN][MaxN];
            
int step1=0;
int step2=0;
int step3=0;
GLfloat mat_specular[]={1.0,0.0,0.0,1.0};
GLfloat mat_shininess[]={20.0};      
GLfloat light_position[]={10.0,10.0,0.0,0.0};
GLfloat white_light[]={1.0,0.0,0.0,1.0};

GLfloat position[]={1.0,1.0,1.0,0.0};
GLfloat light[]={1.0,1.0,1.0,0.9};

void init () {

glOrtho(4000.0, 10000.0, 1200.0, 5000.0, 0.0, 1000.0);
/*glClearColor(0.0,0.0,0.0,0.0);
glMaterialfv(GL_FRONT,GL_SPECULAR,mat_specular);
glMaterialfv(GL_FRONT,GL_SHININESS,mat_shininess);
glLightfv(GL_LIGHT0,GL_POSITION,light_position);
glLightfv(GL_LIGHT0,GL_DIFFUSE,white_light);
glLightfv(GL_LIGHT0,GL_SPECULAR,white_light);
glEnable(GL_LIGHTING);
glEnable(GL_LIGHT0);
  */
glLightfv(GL_LIGHT1,GL_POSITION,position);
glLightfv(GL_LIGHT1,GL_DIFFUSE,light);
glLightfv(GL_LIGHT1,GL_SPECULAR,light);
glEnable(GL_LIGHT1);
  

   glClearColor (0.0, 0.0, 0.0, 0.0);
   glEnable(GL_DEPTH_TEST);
   glShadeModel(GL_SMOOTH);

   makeStripeImage();
   glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

   glGenTextures(1, &texName);
   glBindTexture(GL_TEXTURE_1D, texName);
   glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_REPEAT);
   glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
   glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
   glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA, stripeImageWidth, 0, GL_RGBA, GL_UNSIGNED_BYTE, stripeImage);
   glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
   currentCoeff = xequalzero;
   currentGenMode = GL_OBJECT_LINEAR;
   currentPlane = GL_OBJECT_PLANE;
   glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, currentGenMode);
   glTexGenfv(GL_S, currentPlane, currentCoeff);

   glEnable(GL_TEXTURE_GEN_S);
   glEnable(GL_TEXTURE_1D);
   glEnable(GL_CULL_FACE);
   glEnable(GL_LIGHTING);
   glEnable(GL_LIGHT0);
   glEnable(GL_AUTO_NORMAL);
   glEnable(GL_NORMALIZE);
   glFrontFace(GL_CW);
   glCullFace(GL_BACK);
   glMaterialf (GL_FRONT, GL_SHININESS, 64.0);

}
        
GLfloat get () {
	return (rand() % 100) / 100.;
}

void display () {
glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

glLightfv(GL_LIGHT0,GL_POSITION,light_position);
	
glMatrixMode (GL_PROJECTION);
glLoadIdentity();
glPushMatrix();
glRotatef(step1,1.0,0.0,0.0);
glRotatef(step2,0.0,1.0,0.0);
glRotatef(step3,0.0,0.0,1.0);
glTranslatef(0.8, 0.0, 1.5);

	glBegin(GL_TRIANGLES); // of the color cube
    for (int i = 0; i < n; i++)
    	for (int j = i + 1; j < n; j++)
    		for (int k = j + 1; k < n; k++) {
    			if (g[i][j]==g[i][k] && g[i][k]==g[j][k] && g[i][j]) {
				    //glColor3f(get(), get(), get());                                          
				   glBindTexture(GL_TEXTURE_1D, texName);
    				GLfloat a1 = P[j].x - P[i].x, a2 = P[j].y - P[i].y, a3 = P[j].z - P[i].z;
    				GLfloat b1 = P[k].x - P[i].x, b2 = P[k].y - P[i].y, b3 = P[k].z - P[i].z;
    				GLfloat nx = a2*b3-a3*b2, ny = a3*b1-a1*b3, nz = a1*b2-a2*b1;
					if (ny<0)nx*=-1,ny*=-1,nz*=-1;    				
    				glNormal3f(nx, ny, nz);
    				glVertex3f(P[i].x, P[i].y, P[i].z);
    				glNormal3f(nx, ny, nz);
    				glVertex3f(P[j].x, P[j].y, P[j].z);
    				glNormal3f(nx, ny, nz);
    				glVertex3f(P[k].x, P[k].y, P[k].z);
    			}
    		}
    glPopMatrix();
	glEnd(); 
	glutSwapBuffers();
	glFlush();
	
}       
      
void reshape (int w, int h) {
	glViewport (0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode (GL_PROJECTION);
	glLoadIdentity();
	glFrustum (-1., 1., -1., 1., 1.5, 20.);
	glMatrixMode (GL_MODELVIEW);
}

void keyboard(unsigned char key,int x, int y)
{
switch(key)
{                 
case 'u':
step1=(step1+5)%360;
glutPostRedisplay();
break;

case 'd':
step1=(step1-5)%360;
glutPostRedisplay();
break;

case 'c':
step3=(step3+5)%360;
glutPostRedisplay();
break;

case 'f':
step3=(step3-5)%360;
glutPostRedisplay();
break;

case 'l':
step2=(step2+5)%360;
glutPostRedisplay();
break;       

case 'r':
step2=(step2-5)%360;
glutPostRedisplay();
break;

                                     
case '1':
light_position[0]=(light_position[0]+0.1);
glutPostRedisplay();
break;
case '2':
light_position[0]=(light_position[0]-0.1);
glutPostRedisplay();
break;
                                   
case '3':
light_position[1]=(light_position[1]+0.1);
glutPostRedisplay();
break;
case '4':
light_position[1]=(light_position[1]-0.1);
glutPostRedisplay();
break;

case '5':
light_position[2]=(light_position[2]+0.1);
glutPostRedisplay();
break;
case '6':
light_position[2]=(light_position[2]-0.1);
glutPostRedisplay();
break;

default:
break;
}
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
	return res;
}

int main (int argc, char**argv) {

	freopen ("input.txt", "r", stdin);
	freopen ("output.txt", "w", stdout);
	cin >> n;
	cout << "n = " << n << endl;
	map <int, vector <pair<int, int> > > T;
	for (int i = 0; i < n; i++) {
		double x, y, z;
		cin >> x >> y >> z;
		P[i] = point (x, y, z);
		T[(int)z].push_back(make_pair((int)x, (int)y));		
	}
	
	cout << T.size() << endl;
	n=0;
	for (map <int, vector <pair<int, int> > > :: iterator it = T.begin(); it != T.end(); it++) {
	    cout << it->first << " " << it->second.size() << endl;
	    int dd;
	    if (it->second.size() < 100)
	    	dd=1;
	    else
	    	dd=15;
		for (int j = 0; j < it->second.size(); j+=dd)
			P[n++] = point (it->second[j].first, it->second[j].second, it->first);
	}
	cout << n << endl;

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
	glutKeyboardFunc(keyboard);
	glutMainLoop();			
	return 0;
}
