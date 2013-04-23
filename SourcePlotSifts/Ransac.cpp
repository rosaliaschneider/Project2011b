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

#include <cmath>
#include <ctime> 
#include <cstdlib> 
#include <sstream> 
#include "Define.h"
#include "Ransac.h"
using namespace RX;

#define MATRIX_IDX(n, i, j) j*n + i
#define MATRIX_ELEMENT(A, m, n, i, j) A[ MATRIX_IDX(m, i, j) ]

extern "C" 
{	
void dgesvd_(const char* jobu, const char* jobvt, const int* M, const int* N,
 			 double* A, const int* lda, double* S, double* U, const int* ldu,
			 double* VT, const int* ldvt, double* work,const int* lwork, const int* info);
}

double Ransac::epsilon = 2.220446049250313e-16;

void init_matrix(double* A, int m, int n)
{
   double element = 0.0;
   for (int j = 0; j < n; j++)
   {
      for (int i = 0; i < m; i++)
      {
         MATRIX_ELEMENT(A, m, n, i, j) = 1.0 / (i + j + 1);
         element += 0.25;
      }
   }
}

void print_matrix(const double* A, int m, int n)
{
	/*
	logVT << "[" << std::endl;
	for (int i = 0; i < m; i++)
	{
		for (int j = 0; j < n; j++)
		{
			logVT << MATRIX_ELEMENT(A, m, n, i, j) << " ";
		}
		logVT << ";" << std::endl;
	}
	logVT << "]" << endl;
	*/
}

Ransac::Ransac(void)
{
}

// Constructor
Ransac::Ransac(matd& x1, matd& x2, int nof)
{
	srand(12345); 
	_nof = nof;

	createMatrix(_x1, 3, _nof);
	createMatrix(_x2, 3, _nof);
	createMatrix(_normx1, 3, _nof);
	createMatrix(_normx2, 3, _nof);
	createMatrix(_x1In, 3, _nof);
	createMatrix(_x2In, 3, _nof);

	for(int i = 0; i < _nof; ++i)
		_inliers.push_back(false);

	// create your own copy of the data
	for(int i=0; i <3; i++) {
		for(int j = 0; j < _nof; j++) {
			_x1[i][j] = x1[i][j];
			_x2[i][j] = x2[i][j];
		}
	}

	// preallocate svd memory
	_A = new double[72];
	_U = new double[64];
	_VT = new double[81];
	_S = new double[8];
	_work = new double[10];

	_lwork = -1;
	char a = 'A';
	int m = 8, n = 9, info;

	// query for right lwork
	dgesvd_(&a, &a, &m, &n, _A, &m, _S, _U, &m, _VT, &n, _work, &_lwork, &info);
	_lwork = _work[0];
	delete[] _work;
	_work = new double[_lwork];

}

// Destructor
Ransac::~Ransac(void)
{
	delete[] _A;
	delete[] _U;
	delete[] _VT;
	delete[] _S;
	delete[] _work;
}

// Normalize
 void Ransac::normalize()
{
	normalize(_x1, _nof, _normx1, _T1);
	_T2 = _T1;
	normalize(_x2, _nof, _normx2, _T2, false);
}

// Normalize
void Ransac::normalize(matd& x, int noe, matd& norm_x, mat3 &T, bool recalculate)
{
	if(recalculate)
	{
		// in case the scale is not 1
		for(int i=0; i<noe; i++)
		{
			x[0][i]/=x[2][i];
			x[1][i]/=x[2][i];
			x[2][i]=1;
		}
	
		// find the means (center)
		double mean_x = mean(x[0], noe);
		double mean_y = mean(x[1], noe);

		// shift to center
		vector<double> new_x, new_y, dist;

		for(int i = 0; i < noe; i++)
		{
			new_x.push_back(x[0][i]-mean_x);
			new_y.push_back(x[1][i]-mean_y);
			dist.push_back(sqrt(new_x[i]*new_x[i] + new_y[i]*new_y[i]));
		}

		double mean_dist = mean(dist, noe);
		double scale = sqrt(2.0f)/mean_dist;

		T.set(0,0, scale);	T.set(0,1, 0.0);	T.set(0,2, -scale*mean_x);
		T.set(1,0, 0.0);	T.set(1,1, scale);	T.set(1,2, -scale*mean_y);
		T.set(2,0, 0.0);	T.set(2,1, 0.0);	T.set(2,2, 1);
	}

	matrixMul(T, 3, 3, x, 3, noe, norm_x);
}

// Mean
double Ransac::mean(vector<double> arr, int noe) const
{
	double mean = 0;
	for(int i = 0; i < noe; i++)
		mean+= arr[i];
	return (double)mean/noe;
	
}

// MainLoop
void Ransac::mainLoop(int mnop, double threshold, mat3 &H)
{
	double p = 0.99f;    // Probability of choosing at least one data set without outliers
	double maxNOT = 1000.0;	//Maximum Number of Trials.
	double maxDataTrials = 100;	//Max number of attempts to select a non Colinear Points

	double tcount = 0;	//Trial Counter
	int noi_tmp =  0;	//Temporary Number of Inliers
	int noo_tmp =  0;	//Temporary Number of Outliers
	double N = 1000;	// Initial Number of Trials


	bool degenerate;
	int count;

	vector<int> rand4; // indexes of the 4 random numbers
	for(int i = 0; i < mnop; ++i)
		rand4.push_back(0);
	
	matd rand1; createMatrix(rand1, 3, mnop);	//the 4 random samples from the frst image
	matd rand2; createMatrix(rand2, 3, mnop); //the 4 random samples from the scnd image
	mat3 currentHomography;
	currentHomography.setIdentity();

	vector<bool> currentInliers;
	for(int i = 0; i < _nof; ++i)
		currentInliers.push_back(false);

	_noi = 0;

	while(N > tcount && tcount <= maxNOT)
	//while(tcount <= maxNOT)
	{
		degenerate = true;
		count = 1;
		while(degenerate && count <= maxDataTrials)
		{
			// Generate mnop random indicies in the range 1..npts
			randgen(_nof, mnop+1, rand4);

			for(int i=0; i<3; i++) {
				for(int j=0; j<mnop; j++) {
					rand1[i][j] = _normx1[i][rand4[j]];
					rand2[i][j] = _normx2[i][rand4[j]];
				}
			}

			degenerate = (isDegenerate(rand1, mnop) || isDegenerate(rand2, mnop));
			if(!degenerate)
				fitHomography(rand1, rand2, currentHomography);

			++count;
		}

		if(degenerate)
			break;

		// Find the inliers to the calculated homography
		getInliers(_normx1, _normx2, _nof, currentHomography, threshold, currentInliers, noi_tmp, noo_tmp);

		if(noi_tmp > _noi)
		{
			_noi = noi_tmp;

			// update the inliers
			for(int i = 0; i < _nof; ++i)
				_inliers[i] = currentInliers[i];

			// update the homography
			H = currentHomography;

			N = log(1-p)/log(1 -  pow((double)_noi/(double)_nof,(double)mnop));
		}

		++tcount;  
	}

	int j=0;
	for(int i=0; i<_nof; ++i)
	{
		if(_inliers[i] == 1)
		{
			_x1In[0][j] = _normx1[0][i];
			_x1In[1][j] = _normx1[1][i];
			_x1In[2][j] = _normx1[2][i];

			_x2In[0][j] = _normx2[0][i];
			_x2In[1][j] = _normx2[1][i];
			_x2In[2][j] = _normx2[2][i];

			++j;
		}
	}

}

void Ransac::denormalize(mat3 &H)
{
	H = _T2.inverse() * (H *_T1);

	matd m1, m2;
	createMatrix(m1, 3, _noi);
	createMatrix(m2, 3, _noi);
	matrixMul(_T1.inverse(), 3, 3, _x1In, 3, _noi, m1);
	matrixMul(_T2.inverse(), 3, 3, _x2In, 3, _noi, m2);		

	for(int i = 0; i < _noi; ++i) {
		double a, b, c, d;
		a = _x1In[0][i] = m1[0][i];
		b = _x1In[1][i] = m1[1][i];
		c = _x2In[0][i] = m2[0][i];
		d = _x2In[1][i] = m2[1][i];
	}
}

// randgen
void Ransac::randgen(int noe, int nPlus1, vector<int> &randn)
{
    int lowest=0, highest=noe-1; 
    int range=(highest-lowest)+1;
	int randnum;
    for(int index=0; index<nPlus1; index++)
	{ 
		//Ignore the first random value
		randnum = lowest+int(range*rand()/(RAND_MAX + 1.0));
		if (index !=0)
			randn[index-1] = randnum; 
    } 
}

// isDegenerate
bool Ransac::isDegenerate(matd &x, int noe)
{
	double a[3];
	double b[3];
	double c[3];

	for(int i=0; i<noe-2; i++)
	{
		a[0] = x[0][i];
		a[1] = x[1][i];
		a[2] = x[2][i];

		for(int j=i+1; j<noe-1; j++)
		{
			b[0] = x[0][j];
			b[1] = x[1][j];
			b[2] = x[2][j];

			for(int k=j+1; k<noe; k++)
			{
				c[0] = x[0][k];
				c[1] = x[1][k];
				c[2] = x[2][k];

				if(isCollinear(a, b, c))
					return true;
			}
		}
	}

	return false;
}

// isCollinear
bool Ransac::isCollinear(double* a, double*b, double*c)
{
	double v1[] = {b[0]-a[0], b[1]-a[1], b[2]-a[2]};
	double v2[] = {c[0]-a[0], c[1]-a[1], c[2]-a[2]};
	double v[3];
	cross(v1, v2, v);
	return (length(v,3)<epsilon)?true:false;
}

// Cross
void Ransac::cross(double* v1, double* v2, double v[])
{
	v[0] = v1[1]*v2[2] - v1[2]*v2[1];
	v[1] = v1[2]*v2[0] - v1[0]*v2[2];
	v[2] = v1[0]*v2[1] - v1[1]*v2[0];
}

// Norm of a vector
double Ransac::length(double v[], int d)
{
	double n=0.0;
	for(int i=0; i<d; ++i)
		n+=v[i]*v[i];

	return sqrt(n);
}

// Fit Homography
void Ransac::fitHomography(mat3 &H)
{
	fitHomography(_x1In, _x2In, _noi, H);
}

// Fit Homography
void Ransac::fitHomography(matd &x1, matd &x2, int noe, mat3 &H)
{
	int m = 2*noe;
	int n = 9;

	double *A = new double[m*n];

	for(int i = 0; i < noe; ++i)
	{
		double _x1 = x1[0][i], _y1 = x1[1][i];
		double _X1 = x2[0][i], _Y1 = x2[1][i];

		A[(2*i)]  = _x1;			A[(2*i)+m] = _y1;			A[(2*i)+2*m] = 1;
		A[(2*i)+3*m] = 0;			A[(2*i)+4*m] = 0;			A[(2*i)+5*m] = 0;
		A[(2*i)+6*m] = -_x1*_X1;	A[(2*i)+7*m] = -_y1*_X1;	A[(2*i)+8*m] = -_X1;

		A[(2*i+1)]	 = 0;			A[(2*i+1)+m] = 0;			A[(2*i+1)+2*m] = 0;
		A[(2*i+1)+3*m] = _x1;		A[(2*i+1)+4*m] = _y1;		A[(2*i+1)+5*m] = 1;
		A[(2*i+1)+6*m] = -_x1*_Y1;	A[(2*i+1)+7*m] = -_y1*_Y1;	A[(2*i+1)+8*m] = -_Y1;

	}

	int p = (m < n ? m : n);

	//double *A = new double[m * n];
	double *U = new double[m * m];
	double *VT = new double[n * n];
	double *S = new double[p * 1];

	//   init_matrix(A, m, n);

	//printf("Matrix A (%d x %d) is:\n", m, n);
	//print_matrix(A, m, n);

	int info;
	char a = 'A';
	double *work = new double[10];
	int lwork = -1;
	dgesvd_(&a, &a, &m, &n, A, &m, S, U, &m, VT, &n, work, &lwork, &info);

	lwork = work[0];
	delete[] work;
	work = new double[lwork];
	dgesvd_(&a, &a, &m, &n, A, &m, S, U, &m, VT, &n, work, &lwork, &info);
	delete[] work;

	if(info != 0) {
		fprintf(stderr, "Warning: dgesvd returned with a non-zero status (info = %d)\n", info);
	}

	for(int i=0; i<3; i++) {
		for(int j=0; j<3; j++) {
			int b = 3*i+j;
			H.set(i,j, MATRIX_ELEMENT(VT, 9, 9, 8, b));
		}
	}

	for(int i=0; i<3; i++)
		for(int j=0; j<3; j++)
			H.set(i,j, H.at(i, j)/H.at(2,2));

	delete[] A;
	delete[] U;
	delete[] VT;
	delete[] S;
}

void Ransac::fitHomography(matd &x1, matd &x2, mat3 &H)
{
	for(int i = 0; i < 4; ++i)
	{
		double _x1 = x1[0][i], _y1 = x1[1][i];
		double _X1 = x2[0][i], _Y1 = x2[1][i];

		_A[(2*i)]  = _x1;			_A[(2*i)+8] = _y1;			_A[(2*i)+2*8] = 1;
		_A[(2*i)+3*8] = 0;			_A[(2*i)+4*8] = 0;			_A[(2*i)+5*8] = 0;
		_A[(2*i)+6*8] = -_x1*_X1;	_A[(2*i)+7*8] = -_y1*_X1;	_A[(2*i)+8*8] = -_X1;

		_A[(2*i+1)]	 = 0;			_A[(2*i+1)+8] = 0;			_A[(2*i+1)+2*8] = 0;
		_A[(2*i+1)+3*8] = _x1;		_A[(2*i+1)+4*8] = _y1;		_A[(2*i+1)+5*8] = 1;
		_A[(2*i+1)+6*8] = -_x1*_Y1;	_A[(2*i+1)+7*8] = -_y1*_Y1;	_A[(2*i+1)+8*8] = -_Y1;

	}

	int info, m = 8, n = 9;
	char a = 'A';
	dgesvd_(&a, &a, &m, &n, _A, &m, _S, _U, &m, _VT, &n, _work, &_lwork, &info);

	if (info != 0)
	{
		fprintf(stderr, "Warning: dgesvd returned with a non-zero status (info = %d)\n", info);
	}

	for(int i=0; i<3; i++) {
		for(int j=0; j<3; j++) {
			int b = 3*i+j;
			H.set(i,j, MATRIX_ELEMENT(_VT, 9, 9, 8, b));
		}
	}

	for(int i=0; i<3; i++)
		for(int j=0; j<3; j++)
			H.set(i,j, H.at(i, j)/H.at(2,2));
}

// Get inliers
void Ransac::getInliers(matd &x1, matd &x2, int noe, mat3 &H, double t, vector<bool> &inliers, int& noi, int& noo)
{
	matd Hx1; createMatrix(Hx1, 3, noe);
	matd invHx2; createMatrix(invHx2, 3, noe);
	mat3 invH = H.inverse();
	
    // Calculate, in both directions, the transferred points    
	matrixMul(H, 3, 3, x1, 3, noe, Hx1);
	matrixMul(invH, 3, 3, x2, 3, noe, invHx2);
	
	//for(int i=0; i<noe; i++)
	//{
	//	x1[0][i] /= x1[2][i];
	//	x1[1][i] /= x1[2][i];
	//	x1[2][i]		= 1.0;
	//
	//	x2[0][i] /= x2[2][i];
	//	x2[1][i] /= x2[2][i];
	//	x2[2][i]		= 1.0;
	//
	//	Hx1[0][i] /= Hx1[2][i];
	//	Hx1[1][i] /= Hx1[2][i];
	//	Hx1[2][i]		= 1.0;
	//
	//	invHx2[0][i] /= invHx2[2][i];
	//	invHx2[1][i] /= invHx2[2][i];
	//	invHx2[2][i]	= 1.0;
	//}

	double d2;
	noi = 0;
	noo = 0;
	for(int i=0; i<noe; i++)
	{
		double ax = x1[0][i], ay = x1[1][i], az = x1[2][i];
		double bx = x2[0][i], by = x2[1][i], bz = x2[2][i];
		double cx = Hx1[0][i], cy = Hx1[1][i], cz = Hx1[2][i];
		double dx = invHx2[0][i], dy = invHx2[1][i], dz = invHx2[2][i];

		d2 = 0;
		d2 += sqrt((ax-dx) * (ax-dx) + (ay-dy) * (ay-dy) + (az-dz) * (az-dz));
		d2 += sqrt((cx-bx) * (cx-bx) + (cy-by) * (cy-by) + (cz-bz) * (cz-bz));
		
		if(d2 <= t)	{	
			++noi;	inliers[i]	= 1;
		}
		else {	
			++noo;	
			inliers[i]	= 0;
		}
	}
}

void Ransac::saveOutliers(std::string filename)
{
	ofstream output(filename.c_str());

	output << _nof - _noi << endl;
	for(int i = 0; i < _inliers.size(); ++i) {
		if(!_inliers[i])
			output << _x2[0][i] << " " << _x2[1][i] << endl;
	}

}


// MatrixMul 
 void matrixMul(const mat3 &m1, int row1, int col1, const matd& m2, int row2, int col2, matd& m3)
{
	// if col1 != row2 then ERROR
	for(int i=0; i<row1; i++)
	{
		for(int j=0; j<col2; j++)
		{
			m3[i][j] = 0.0;
			for(int k=0; k< col1; k++)
				m3[i][j] += m1.at(i,k) * m2[k][j];
		}
	}
}
