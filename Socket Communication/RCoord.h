#pragma once
#include <iostream>
#include <string>
#include <array>

using namespace std;

class RCOORD {
private:
	array<double, 6> Coord1;
	array<double, 6> Coord2;
	double length;
public:
	RCOORD() {
		Coord1 = { 0,0,0,0,0,0 }; Coord2 = { 0,0,0,0,0,0 }; length = 0;
	}
	RCOORD(double x, double y, double z, double rx, double ry, double rz) { Coord1 = {x,y,z,rx,ry,rz}; }

	void setCoord1(double x, double y, double z, double rx, double ry, double rz) { Coord1 = { x,y,z,rx,ry,rz }; }
	void setCoord2(double x, double y, double z, double rx, double ry, double rz) { Coord2 = { x,y,z,rx,ry,rz }; }
	void setCoord1(double coord[]) { 
		for (int i = 0; i < 6; i++) {
			Coord1[i] = coord[i];
		}
	}
	void setCoord2(double coord[]) {
		for (int i = 0; i < 6; i++) {
			Coord2[i] = coord[i];
		}
	}
	string getCoord1String() {
		std::string coordString;
		for (int i = 0; i < 5; i++) {
			coordString += to_string(Coord1[i]);
			coordString += ", ";
		}
		coordString += to_string(Coord1[5]);
		coordString += "\n";
		return coordString;
	}
	string getCoord2String() {
		std::string coordString;
		for (int i = 0; i < 5; i++) {
			coordString += to_string(Coord2[i]);
			coordString += ", ";
		}
		coordString += to_string(Coord2[5]);
		coordString += "\n";
		return coordString;
	}
	void getCoord1(double* coord) {
		for (int i = 0; i < 6; i++) {
			coord[i] = Coord1[i];
		}
	}
	void getCoord2(double* coord) {
		for (int i = 0; i < 6; i++) {
			coord[i] = Coord2[i];
		}
	}
};