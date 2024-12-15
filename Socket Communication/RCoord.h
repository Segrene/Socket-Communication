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

	void setOrigin(double x, double y, double z, double rx, double ry, double rz) { Coord[0] = {x,y,z,rx,ry,rz}; }
	void addPoint(double x, double y, double z, double rx, double ry, double rz) { Coord.push_back({ x,y,z,rx,ry,rz }); Point++; }
	void setOrigin(double coord[]) { 
		for (int i = 0; i < 6; i++) {
			Coord[0][i] = coord[i];
		}
	}
	void addPoint(double coord[]) {
		array<double, 6> addPoint = {0};
		for (int i = 0; i < 6; i++) {
			addPoint[i] = coord[i];
		}
		Coord.push_back(addPoint);
		Point++;
	}
	void setOrigin(string coord) {
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
	void addPoint(string coord) {
		array<double, 6> addPoint = { 0 };
		istringstream ss(coord);
		array<string, 6> coordString;
		int i = 0;
		string buffer;
		while (getline(ss, buffer, ',')) {
			coordString[i] = buffer;
			i++;
		}
		for (int j = 0; j < 6; j++) {
			addPoint[j] = stod(coordString[j]);
		}
		Coord.push_back(addPoint);
		Point++;
	}
	void setOrigin(array<double, 6> coord) {
		Coord[0] = coord;
	}
	void addPoint(array<double, 6> coord) {
		Coord.push_back(coord);
	}
	void editPoint(int pointNum, int coordNum, double coord) {
		if (pointNum > Point) { throw out_of_range("vector 범위 초과"); }
		Coord[pointNum][coordNum] = coord;
	}
	void makeLifting() {
		if (Point < 1) { cout << "No available point" << endl; }
		array<double, 6> lift1 = Coord[Point];
		array<double, 6> lift2 = Coord[0];
		lift1[2] += 20;
		lift2[2] += 20;
		this->addPoint(lift1);
		this->addPoint(lift2);
	}
	string getPointString(int point) {
		if (point > Point) { throw out_of_range("vector 범위 초과"); }
		string coordString;
		for (int i = 0; i < 5; i++) {
			coordString += to_string(Coord[point][i]);
			coordString += ", ";
		}
		coordString += to_string(Coord[point][5]);
		coordString += "\n";
		return coordString;
	}
	void getPoint(int point, double* coord) {
		if (point > Point) { throw out_of_range("vector 범위 초과"); }
		for (int i = 0; i < 6; i++) {
			coord[i] = Coord[point][i];
		}
	}
	int getPointCount() {
		return Point;
	}
	void Clear() {
		Coord.clear();
		Coord.push_back({0,0,0,0,0,0});
		Point = 0;
	}
};