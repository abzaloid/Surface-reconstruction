#include <iostream>
#include <fstream>
#include <cstdio>
#include <string>
#include <vector>

using namespace std;

int main () {
	
	ifstream fin ("done.txt");
		
	string s;	
	for(int i = 0; i < 5456; i++) {
		getline (fin, s);
		int h = 0;
		for (h = 0; s[h] >= '0' && s[h] <= '9' ; h++);
		int cnt = 0;
		while (s[h] < '0' || s[h] > '9')
			h++;
		vector <int> r;
		int cur = 0;
		for (int i = h; i < s.size(); i++) {
			if (s[i] == ' ') {
				r.push_back(cur);
				cur = 0;
				continue;
			}
			cur = cur * 10 + (s[i] - '0');						
		}
		cout << r.size() << " ";
		for (int i = 0; i < r.size(); i++)
			cout << r[i] << " ";
		cout << endl;	   
	}


	fin.close();

	return 0;
}
