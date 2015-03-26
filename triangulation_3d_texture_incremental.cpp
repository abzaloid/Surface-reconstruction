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

const double oo = 1e5;

#define stripeImageWidth 32
GLubyte stripeImage[4*stripeImageWidth];

inline double get () { return (rand()%100)/100.; }      

static GLuint texName;

void makeStripeImage(void)
{
   int j;
    
   for (j = 0; j < stripeImageWidth; j++) {
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
	int id;
	point () {}                                                 
	point (double X, double Y, double Z) {x = X, y = Y, z = Z; }
	point (double X, double Y, double Z, int ID) {x = X, y = Y, z = Z, id = ID; }
	bool operator < (const point& other) const {
		if (x < other.x)
			return true;               
		if (x == other.x && y < other.y)
			return true;
		if (x == other.x && y == other.y && z < other.z)
			return true;
		return false;	
	}
	bool operator == (const point& other) const {
		return x == other.x && y == other.y && z == other.z;
	}
};
struct _triangle {
	point h, m, l;
	double area;
	_triangle () {}
	_triangle (point A, point B, point C) {
		h = A, m = B, l = C;
		area = 0.5 * abs (A.x * B.y + B.x * C.y + C.x * A.y - A.y * B.x - B.y * C.x - C.y * A.x);
	}
	bool operator < (const _triangle& other) const {
		if (area < other.area)
			return true;
		if (area == other.area) {
			if (h < other.h)
				return true;
			if (h == other.h && m < other.m)
				return true;
			if (h == other.h && m == other.m && l < other.l)
				return true;
			return false;
		}
		return false;
	}
};

int n;
#define MaxN 6000
point D[MaxN], P[MaxN];
vector <int> g[MaxN];
bool C[MaxN][MaxN];            
set <point> ConvexHull;
set <_triangle> T;
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

glOrtho(4000., 10000.0, 1200., 5000.0, 0.0, 1000.0);

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

void display () {
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glLightfv(GL_LIGHT0,GL_POSITION,light_position);
		
	glMatrixMode (GL_PROJECTION);
	glLoadIdentity();
	glPushMatrix();
	glRotatef(step1,1.0,0.0,0.0);
	glRotatef(step2,0.0,1.0,0.0);
	glRotatef(step3,0.0,0.0,1.0);
	glTranslatef(0.2, 0.0, 2.);

	glBegin(GL_TRIANGLES); 
		for (set <_triangle> :: iterator it = T.begin(); it != T.end(); it++) {
				    point i = it->h, j = it->m, k = it->l;
				    glBindTexture(GL_TEXTURE_1D, texName);
    				GLfloat a1 = j.x - i.x, a2 = j.y - i.y, a3 = j.z - i.z;
    				GLfloat b1 = k.x - i.x, b2 = k.y - i.y, b3 = k.z - i.z;
    				GLfloat nx = a2*b3-a3*b2, ny = a3*b1-a1*b3, nz = a1*b2-a2*b1;
					if (ny<0)nx*=-1,ny*=-1,nz*=-1;    				
    				glNormal3f(nx, ny, nz);
    				glVertex3f(i.x, i.y, i.z);
    				glNormal3f(nx, ny, nz);
    				glVertex3f(j.x, j.y, j.z);
    				glNormal3f(nx, ny, nz);
    				glVertex3f(k.x, k.y, k.z);
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

void dfs (int v, int from);

int main (int argc, char**argv) {
    freopen ("large_input.txt", "r", stdin);
	freopen ("res.txt", "w", stdout);
	cin >> n;
	cout << n << endl;
	for (int i = 0; i < n; i++) {
		double x, y, z;
		cin >> x >> y >> z;                             
		P[i] = point (x, y, z, i);
		D[i] = point (x, y, z, i);
	}
	sort (P, P + n);
	bool convok = false;
	ConvexHull.insert (P[0]);
	int j=1,cnt=0;
	for (int i = 1; i < n; i++) {
		j = i;
		if (P[i].x != P[0].x)
			break;
		C[P[i].id][P[i - 1].id]=C[P[i - 1].id][P[i].id]=1;
		g[P[i].id].push_back(P[i - 1].id);
		g[P[i - 1].id].push_back(P[i].id);		
		ConvexHull.insert(P[i]);
	}
	for (int i = j; i < n; i++) {

	printf ("now the point id is: %d\n", P[i].id + 1);
	printf ("inside Convex Hull these ids: ");
	for (set <point> :: iterator it = ConvexHull.begin(); it != ConvexHull.end(); it++)
		cout << it->id + 1 << " ";
	puts ("");
		


		double l = oo, r = -oo;
		double lx, ly, lz, rx, ry, rz;
		int l_id, r_id;
		for (set <point> :: iterator it = ConvexHull.begin(); it != ConvexHull.end(); it++) {
			point cur = *it;
			double x = cur.x - P[i].x, y = cur.y - P[i].y;
			double dot = y, len = sqrt(x * x + y * y);
			if (acos(dot/len) <= l)
				l = acos(dot / len), lx = cur.x, ly = cur.y, lz = cur.z, l_id = cur.id;
			if (acos(dot/len) >= r)
				r = acos(dot / len), rx = cur.x, ry = cur.y, rz = cur.z, r_id = cur.id;
		}
		vector <point> toErase;
		for (set <point> :: iterator it = ConvexHull.begin(); it != ConvexHull.end(); it++) {
			point cur = *it;
			if ((ly * cur.x > cur.y * lx && ry * cur.x < rx * cur.y) || !convok) {
			    if (cur.id != l_id && cur.id != r_id)
					toErase.push_back (cur);
			}
		}                    
		g[P[i].id].push_back(l_id);
		g[l_id].push_back(P[i].id);
		C[P[i].id][l_id] = C[l_id][P[i].id] = 1;
	    cnt++;
	    if (l_id != r_id) g[P[i].id].push_back(r_id), g[r_id].push_back(P[i].id), C[P[i].id][r_id] = C[r_id][P[i].id] = 1, cnt++;                       
	      
		for (int k = 0; k < toErase.size(); k++) {      
			g[toErase[k].id].push_back(P[i].id);				
			g[P[i].id].push_back(toErase[k].id);
			C[toErase[k].id][P[i].id] = C[P[i].id][toErase[k].id] = 1;
			ConvexHull.erase(toErase[k]);				
			cnt++;
		}

		cout << "l_id = " << l_id + 1 << " r_id = " << r_id + 1 << endl;

		printf ("became the following: ");
		ConvexHull.insert(P[i]);
		if (P[i].x != P[0].x)
			convok = true;
		for (set <point> :: iterator it = ConvexHull.begin(); it != ConvexHull.end(); it++)
			cout << it->id + 1 << " ";
		puts ("");	
	}

	cout << "convex hull size = " << ConvexHull.size() << endl;
	ConvexHull.clear();
	dfs(0, 0);
	cout << "T.size = " << T.size() << endl;		
	cout << "connections = " << cnt << endl;
	
	for (int i = 0; i < n; i++) {
		printf ("%dth connected with: ", i + 1);
		for (int j = 0; j < g[i].size(); j++) {
			printf ("%d, ", g[i][j] + 1);
		}
		puts ("");
	}

	printf ("the following triangles are created:\n");
	for (set<_triangle>::iterator it=T.begin();it!=T.end();it++) {
		cout << it->h.id + 1 << " " << it->m.id + 1 << " " << it->l.id + 1 << endl;
	}

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGBA);
	glutInitWindowSize(500, 500);
	glutInitWindowPosition(100, 100);
	glutCreateWindow ("hello world! ");
	init ();
	glutDisplayFunc (display);
	glutKeyboardFunc (keyboard);
	glutMainLoop();			
	return 0;
}

bool used[MaxN];

void dfs (int v, int from) {
	used[v] = true;
	for (int i = 0; i < g[v].size(); i++)
		if (!used[g[v][i]])
			dfs(g[v][i], v);
	if (v == from)
		return;
	for (int i = 0; i < g[v].size(); i++)
		if (C[g[v][i]][from])
			T.insert (_triangle (D[v], D[g[v][i]], D[from]));		
}
