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

void init () {
	glClearColor (0.0, 0.0, 0.0, 0.0);
	glMatrixMode (GL_PROJECTION);
	glLoadIdentity();
 glOrtho(-1.0, 9400.0, -1.0, 3500.0, -1.0, 1000.0);
}
        
GLfloat get () {
	return (rand() % 100) / 100.;
}

void display () {
	glClear (GL_COLOR_BUFFER_BIT);
	glBegin(GL_TRIANGLES); 
	for (set <_triangle> :: iterator it = T.begin(); it != T.end(); it++) {
		point i = it->h, j = it->m, k = it->l;
        glColor3f(get(), get(), get());             
   		glVertex3f(i.x, i.y, 0.);
    	glVertex3f(j.x, j.y, 0.);
    	glVertex3f(k.x, k.y, 0.);				
	}    				
	glEnd();
	glFlush();
}       
      
void reshape (int w, int h) {
	glViewport (0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode (GL_PROJECTION);
	glLoadIdentity();
	glFrustum (-1., 1., -1., 1., 1.5, 20.);
	glMatrixMode (GL_MODELVIEW);
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
	glutInitDisplayMode(GLUT_SINGLE|GLUT_RGBA);
	glutInitWindowSize(500, 500);
	glutInitWindowPosition(100, 100);
	glutCreateWindow ("hello world! ");
	init ();
	glutDisplayFunc (display);
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
