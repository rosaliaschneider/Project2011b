#include "Homography.h"

//string basic = "D:/Research/Project2011b/Data/Differentials/";
//string basic = "D:/Research/Project2011b/Data/Recitation13/";
string basic = "D:/Research/Project2011b/Data/Probability/";
//string basic = "D:/Research/Project2011b/Data/Separable/";
//string basic = "D:/Research/Project2011b/Data/Poisson/";
//string basic = "D:/Research/Project2011b/Data/Reasoning/";

int main(int argc, char *argv[])
{
	Homography _homography;
	RX::mat3 _hom;
	
	_homography.loadInfo(basic+"Info.txt");
	
    ofstream out(basic+"Homs.txt");
	out << _homography.numFrames() << endl;

	_hom.setIdentity();
	_homography.setSiftFolder(basic+"Sifts/");
	_homography.setInliersFolder(basic+"Inliers/");

	for(int i = 0; i < 3; ++i) 
	{
		for(int j = 0; j < 3; ++j)
			out << _hom.at(i, j) << " ";
		out << endl;
	}

	for(int frame = 1; frame < _homography.numFrames(); ++frame)
	{
		// ESTIMATE MOVEMENT FROM POINTS
		_homography.loadPoints(frame-1, true);
		_homography.all();

		_hom.setIdentity();
		if(_homography.numPoints() >= 30)
		{
			_homography.estimate();
			if(_homography.numInliers() >= 20)
				_hom = _homography.hom();
		}

		for(int i = 0; i < 3; ++i) 
		{
			for(int j = 0; j < 3; ++j)
				out << _hom.at(i, j) << " ";
			out << endl;
		}

		cout << frame << " ";
	}
}