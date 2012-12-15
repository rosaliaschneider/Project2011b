/*************************************************************************
Copyright (c) 2008 The University of Western Ontario.  All rights reserved.

Contributors:
    * Islam Ali (Computer Science Department)

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are
met:

- Redistributions of source code must retain the above copyright
  notice, this list of conditions and the following disclaimer.

- Redistributions in binary form must reproduce the above copyright
  notice, this list of conditions and the following disclaimer listed
  in this license in the documentation and/or other materials
  provided with the distribution.

- Neither the name of the copyright holders nor the names of its
  contributors may be used to endorse or promote products derived from
  this software without specific prior written permission.
*************************************************************************/

#pragma once
//#define 

#include <RX/mat3d.h>
#include "define.h" 

class Ransac
{
public:
	Ransac();
	~Ransac();

	Ransac(matd&, matd&, int);
	void normalize();
	void fitHomography(RX::mat3d &H);
	void mainLoop(int mnop, double threshold, RX::mat3d &H);
	void denormalize(RX::mat3d &H);

	void getInliers(matd&, matd&, int, RX::mat3d&, double, vector<bool>&, int&, int&);

	// Gets
	int noi() { return _noi; }
	vector<bool> inliers() { return _inliers; }

private:
	double mean(vector<double>, int) const;
	void normalize(matd&, int, matd&, RX::mat3d&);
	void randgen(int, int, vector<int>&);
	bool isDegenerate(matd&, int);
	bool isCollinear(double*, double*, double*);
	void cross(double*, double*, double[]);
	double length(double[], int);

	void fitHomography(matd&, matd&, int, RX::mat3d&);
	void fitHomography(matd&, matd&, RX::mat3d&);

private:
	RX::mat3d _T1, _T2;
	matd _x1, _x2, _normx1, _normx2;
	matd _x1In, _x2In;
	int _nof, _noi;

	matd _H;
	vector<bool> _inliers;

	// for speed
	int _lwork;
	double * _work;
	double *_A, *_U, *_VT, *_S;
	static double epsilon;
};

void matrixMul(const RX::mat3d &m1, int row1, int col1, const matd& m2, int row2, int col2, matd& m3);
