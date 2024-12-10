#include <stdio.h>
#include <conio.h>
#include <iostream>
#include <string>
#include "RCoord.h"

using namespace std;

int CoordTest() {
	RCOORD TestCoord(0, 0, 0, 0, 0, 0);
	for (int i = 1; i < 10; i++) {
		TestCoord.addCoord(i, i, i, i, i, i);
	}
	for (int i = 0; i < 10; i++) {
		cout << TestCoord.getCoordString(i);
	}
	TestCoord.Clear();
	double DCoord[] = {0,0,0,0,0,0};
	TestCoord.setCoord(DCoord);
	for (int i = 1; i < 10; i++) {
		double DACoord[] = {i,i,i,i,i,i};
		TestCoord.addCoord(DACoord);
	}
	for (int i = 0; i < 10; i++) {
		double DACoord[] = { 0,0,0,0,0,0 };
		TestCoord.getCoord(i, DACoord);
		for (int j = 0; j < 5; j++) {
			cout << DACoord[j] << ", ";
		}
		cout << DACoord[5];
		cout << endl;
	}
	TestCoord.Clear();
	TestCoord.setCoord("0,0,0,0,0,0");
	TestCoord.addCoord("1,1,1,1,1,1");
	TestCoord.addCoord("2,2,2,2,2,2");
	TestCoord.addCoord("3,3,3,3,3,3");
	TestCoord.addCoord("4,4,4,4,4,4");
	TestCoord.addCoord("5,5,5,5,5,5");
	TestCoord.addCoord("6,6,6,6,6,6");
	TestCoord.addCoord("7,7,7,7,7,7");
	TestCoord.addCoord("8,8,8,8,8,8");
	TestCoord.addCoord("9,9,9,9,9,9");
	for (int i = 0; i < 10; i++) {
		cout << TestCoord.getCoordString(i);
	}
	return 0;
}