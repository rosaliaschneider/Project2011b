#include "Define.h"
#include "Homography.h"

#define MATRIX_IDX(n, i, j) j*n + i
#define MATRIX_ELEMENT(A, m, n, i, j) A[ MATRIX_IDX(m, i, j) ]

extern "C" 
{	
void dgesvd_(const char* jobu, const char* jobvt, const int* M, const int* N,
 			 double* A, const int* lda, double* S, double* U, const int* ldu,
			 double* VT, const int* ldvt, double* work,const int* lwork, const int* info);
}


Homography::Homography()
: _nused(0)
{
	_hom.setIdentity();
}

Homography::~Homography()
{
}

void Homography::setPoints1(RX::vec2 p1, RX::vec2 p2, RX::vec2 p3, RX::vec2 p4)
{
	createMatrix(_ux1, 3, 0);
	
	_ux1[0].push_back(p1.x);
	_ux1[1].push_back(p1.y);
	_ux1[2].push_back(1);

	_ux1[0].push_back(p2.x);
	_ux1[1].push_back(p2.y);
	_ux1[2].push_back(1);

	_ux1[0].push_back(p3.x);
	_ux1[1].push_back(p3.y);
	_ux1[2].push_back(1);

	_ux1[0].push_back(p4.x);
	_ux1[1].push_back(p4.y);
	_ux1[2].push_back(1);
	
}

void Homography::setPoints2(RX::vec2 p1, RX::vec2 p2, RX::vec2 p3, RX::vec2 p4)
{  
	createMatrix(_ux2, 3, 0);

	_ux2[0].push_back(p1.x);
	_ux2[1].push_back(p1.y);
	_ux2[2].push_back(1);

	_ux2[0].push_back(p2.x);
	_ux2[1].push_back(p2.y);
	_ux2[2].push_back(1);

	_ux2[0].push_back(p3.x);
	_ux2[1].push_back(p3.y);
	_ux2[2].push_back(1);

	_ux2[0].push_back(p4.x);
	_ux2[1].push_back(p4.y);
	_ux2[2].push_back(1);
}

void Homography::estimate()
{
	fit(_ux1, _ux2, _hom);
}

void Homography::fit(matd &x1, matd &x2, RX::mat3 &H)
{
	int _lwork;
	double * _work;
	double *_A, *_U, *_VT, *_S;

	// preallocate svd memory
	_A = new double[72];
	_U = new double[64];
	_VT = new double[81];
	_S = new double[8];
	_work = new double[10];

	_lwork = -1;

	int info, m = 8, n = 9;
	char a = 'A';

	// query for right lwork
	dgesvd_(&a, &a, &m, &n, _A, &m, _S, _U, &m, _VT, &n, _work, &_lwork, &info);
	_lwork = _work[0];
	delete[] _work;
	_work = new double[_lwork];
	
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

	delete[] _A;
	delete[] _U;
	delete[] _VT;
	delete[] _S;
	delete[] _work;
}

