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

void makeStripeImage(void)
{
	  for (int j = 0; j < stripeImageWidth; j++) {
      stripeImage[4*j] = (GLubyte) ((j<=rand()%5) ? 255 : 0);
      stripeImage[4*j+1] = (GLubyte) ((j>rand()%5) ? 255 : 0);
      stripeImage[4*j+2] = (GLubyte) 0;
      stripeImage[4*j+3] = (GLubyte) 255;
   }
}

static GLfloat xequalzero[] = {1.0, 0.0, 0.0, 0.0};
static GLfloat slanted[] = {1.0, 1.0, 1.0, 0.0};
static GLfloat *currentCoeff;
static GLenum currentPlane;
static GLint currentGenMode;

struct Edge {
	double dist;
    int u, v;
    Edge () {}
    Edge (double D, int U, int V) {dist = D, u = U, v = V; }		
	
	bool operator < (const Edge& other) const {
		return dist < other.dist;
	}
};
    
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
vector <Edge> E;
        
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
   glOrtho(-1.0, 8400.0, -1.0, 2800.0, -1.0, 700.0);
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
	switch(key) {                 
		case 'u':
			step1 = (step1 + 5) % 360;
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

void dfs (int v, int from);

int main (int argc, char**argv) {

	freopen ("input.txt", "r", stdin);
	freopen ("output.txt", "w", stdout);
	
	cin >> n;
	map <int, vector <pair<double, double> > > TP;
	map <int, vector <int> > ID;
	cout <<	"n = " << n << endl;
	for (int i = 0; i < n; i++) {
		double x, y, z;
		cin >> x >> y >> z;
		P[i] = point (x, y, z, i);
		D[i] = point (x, y, z, i);
		TP[(int)z].push_back(make_pair(x, y));		
		ID[(int)z].push_back(i);
	}
	int cnt = 0;	
	cout << TP.size() << endl;
	map<int, vector <int> > :: iterator jt = ID.begin();
	for (map <int, vector <pair<double, double> > > :: iterator it = TP.begin(); it != TP.end(); jt++, it++) {
		n = 0;
		cout << ++cnt << endl;
		for (int j = 0; j < it->second.size(); j++)
			P[n++] = point (it->second[j].first, it->second[j].second, it->first, jt->second[j]);
		map <int, vector <pair<double, double> > > :: iterator _it = it;
		map<int, vector<int> > :: iterator _jt = jt;
		_it++; _jt++;
		cout<<"rr\n";
		if (_it == TP.end())
			break;
		for (int j = 0; j < _it->second.size(); j++)
			P[n++] = point (_it->second[j].first, _it->second[j].second, _it->first, _jt->second[j]);
		cout << "currently n = " << n << endl;
		E.clear();
		for (int i = 0; i < n; i++)
			for (int j = i + 1; j < n; j++)
				E.push_back(Edge(calcDist (i, j), P[i].id, P[j].id));
		vector <int> res;	
		sort (E.begin(), E.end());
		for (int i = 0; i < E.size(); i++) {
			bool ok = true;
				for (int j = 0; j < res.size(); j++)
					if (isIntersect(i, res[j])) {
						ok = false;
						break;				
					}
			if (ok) {
				res.push_back (i);          
				g[E[i].u].push_back(E[i].v);
				g[E[i].v].push_back(E[i].u);
				C[E[i].u][E[i].v] = C[E[i].v][E[i].u] = 1;
			}
		}
	}
	dfs(0, 0);

	for (int i = 0; i < MaxN; i++) {
		cout << i + 1 << "th connected with " << " : " ; 
		for (int j = 0; j < g[i].size(); j++) {
			cout << g[i][j] + 1 << " ";						
		}
		puts ("");
	}	
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
