#pragma once
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <array>

using namespace std;

class RCOORD {
private:
	int Point = 0;
	vector<array<double, 6>> Coord = { {0,0,0,0,0,0} };
public:
	RCOORD() {}
	RCOORD(double x, double y, double z, double rx, double ry, double rz) { Coord[0] = {x,y,z,rx,ry,rz}; }
	RCOORD(double coord[]) {
		for (int i = 0; i < 6; i++) {
			Coord[0][i] = coord[i];
		}
	}

	void setCoord(double x, double y, double z, double rx, double ry, double rz) { Coord[0] = {x,y,z,rx,ry,rz}; }
	void addCoord(double x, double y, double z, double rx, double ry, double rz) { Coord.push_back({ x,y,z,rx,ry,rz }); Point++; }
	void setCoord(double coord[]) { 
		for (int i = 0; i < 6; i++) {
			Coord[0][i] = coord[i];
		}
	}
	void addCoord(double coord[]) {
		array<double, 6> addCoord = {0};
		for (int i = 0; i < 6; i++) {
			addCoord[i] = coord[i];
		}
		Coord.push_back(addCoord);
		Point++;
	}
	void setCoord(string coord) {
		istringstream ss(coord);
		array<string, 6> coordString;
		int i = 0;
		string buffer;
		while (getline(ss, buffer, ',')) {
			coordString[i] = buffer;
			i++;
		}
		for (int j = 0; j < 6; j++) {
			Coord[0][j] = stod(coordString[j]);
		}
	}
	void addCoord(string coord) {
		array<double, 6> addCoord = { 0 };
		istringstream ss(coord);
		array<string, 6> coordString;
		int i = 0;
		string buffer;
		while (getline(ss, buffer, ',')) {
			coordString[i] = buffer;
			i++;
		}
		for (int j = 0; j < 6; j++) {
			addCoord[j] = stod(coordString[j]);
		}
		Coord.push_back(addCoord);
		Point++;
	}
	string getCoordString(int point) {
		if (point > Point) { throw out_of_range("vector 범위 초과"); }
		std::string coordString;
		for (int i = 0; i < 5; i++) {
			coordString += to_string(Coord[point][i]);
			coordString += ", ";
		}
		coordString += to_string(Coord[point][5]);
		coordString += "\n";
		return coordString;
	}
	void getCoord(int point, double* coord) {
		if (point > Point) { throw out_of_range("vector 범위 초과"); }
		for (int i = 0; i < 6; i++) {
			coord[i] = Coord[point][i];
		}
	}
	int getPoint() {
		return Point;
	}
	void Clear() {
		Coord.clear();
		Coord.push_back({0,0,0,0,0,0});
	}
};