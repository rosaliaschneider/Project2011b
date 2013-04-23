#include "Homography.h"

string basic = "D:/Research/Project2011b/Data/Reasoning/";

int main(int argc, char *argv[])
{
	Homography _homography;
	RX::mat3 _hom;
	
	_homography.loadInfo(basic+"Info.txt");
	
    ofstream out(basic+"Homs.txt");
	out << _homography.numFrames() << endl;

	for(int frame = 0; frame < _homography.numFrames(); ++frame)
	{
		// ESTIMATE MOVEMENT FROM POINTS
		_hom.setIdentity();
		for(int i = 0; i < 3; ++i) 
		{
			for(int j = 0; j < 3; ++j)
				out << _hom.at(i, j) << " ";
			out << endl;
		}
		cout << frame << " ";
	}
}
