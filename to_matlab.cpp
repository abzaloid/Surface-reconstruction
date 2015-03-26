#include <iostream>
#include <fstream>
#include <cstdio>
#include <string>
#include <vector>

using namespace std;

int main () {
	
	freopen ("input.txt", "r", stdin);

	double x, y, z;
	int n;
	cin >> n;
	vector <double> X,Y,Z;
	
	double mx,my,mz,px=100000,py=100000,pz=100000;
	
	for (int i = 0; i < n; i++) {
		cin >> x >> y >> z;
		X.push_back(x);
		Y.push_back(y);
		Z.push_back(z);
		mx=max(mx,x);
		px=min(px,x);
		my=max(my,y);
		py=min(py,y);
		mz=max(mz,z);
		pz=min(pz,z);		
	}
	cout << n << endl;
	for (int i = 0; i < n; i++)
		cout << X[i] - px << " " << Y[i] - py << " " << Z[i] - pz << endl;
	
	cout << mx - px << " " << my - py << " " << mz - pz << endl;	
	
	/*cout << "X = [ " << endl;
	for (int i = 0; i < X.size(); i++) {
		cout << X[i] << " ";
	}
	puts ("]; ");

	cout << "Y = [ " << endl;
	for (int i = 0; i < Y.size(); i++) {
		cout << Y[i] << " ";
	}
	puts ("]; ");

	cout << "Z = [ " << endl;
	for (int i = 0; i < Z.size(); i++) {
		cout << Z[i] << " ";
	}
	puts ("]; ");
    */
	return 0;
}
