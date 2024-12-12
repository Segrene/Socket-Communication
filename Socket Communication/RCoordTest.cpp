#include <stdio.h>
#include <conio.h>
#include <iostream>
#include <string>
#include "RCoord.h"

using namespace std;

int CoordTest() {
	RCOORD TestCoord(0, 0, 0, 0, 0, 0);
	for (int i = 1; i < 10; i++) {
		TestCoord.addPoint(i, i, i, i, i, i);
	}
	for (int i = 0; i < 10; i++) {
		cout << TestCoord.getPointString(i);
	}
	TestCoord.Clear();
	double DCoord[] = {0,0,0,0,0,0};
	TestCoord.setOrigin(DCoord);
	for (int i = 1; i < 10; i++) {
		double DACoord[] = {i,i,i,i,i,i};
		TestCoord.addPoint(DACoord);
	}
	for (int i = 0; i < 10; i++) {
		double DACoord[] = { 0,0,0,0,0,0 };
		TestCoord.getPoint(i, DACoord);
		for (int j = 0; j < 5; j++) {
			cout << DACoord[j] << ", ";
		}
		cout << DACoord[5];
		cout << endl;
	}
	TestCoord.Clear();
	TestCoord.setOrigin("0,0,0,0,0,0");
	TestCoord.addPoint("1,1,1,1,1,1");
	TestCoord.addPoint("2,2,2,2,2,2");
	TestCoord.addPoint("3,3,3,3,3,3");
	TestCoord.addPoint("4,4,4,4,4,4");
	TestCoord.addPoint("5,5,5,5,5,5");
	TestCoord.addPoint("6,6,6,6,6,6");
	TestCoord.addPoint("7,7,7,7,7,7");
	TestCoord.addPoint("8,8,8,8,8,8");
	TestCoord.addPoint("9,9,9,9,9,9");
	for (int i = 0; i < 10; i++) {
		cout << TestCoord.getPointString(i);
	}
	return 0;
}