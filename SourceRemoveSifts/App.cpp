#include <vector>
#include <string>
#include "Movement.h"

using namespace std;

string basic = "D:/Research/Project2011b/Data/Probability/";

int main(int argc, char *argv[])
{
	int _numFrames, _w, _h;
	int _numRemoves;
	int _numMovements;

	vector<Movement> _movements;
	vector<BBox> _removes;

	std::ifstream input(basic+"Info.txt");

	input >> _numFrames;
	input >> _w >> _h;
	input >> _numMovements;
	input >> _numRemoves;

	for(int i = 0; i < _numMovements; ++i)
	{
	}

	for(int i = 0; i < _numRemoves; ++i)
	{
		int minX, minY, maxX, maxY;
		input >> minX >> minY >> maxX >> maxY;
		_removes.push_back(BBox(RX::vec2(minX, minY), RX::vec2(minX, maxY), RX::vec2(maxX, maxY), RX::vec2(maxX, minY)));
	}

	for(int frame = 0; frame < _numFrames; ++frame)
	{
		cout << frame << endl;
		
		bool anyActive;
		if(_numRemoves == 0)
		{
			anyActive = false;
			for(int mov = 0; mov < _numMovements; ++mov) 
			{
				if(_movements[mov].active(frame)) {
					anyActive = true;
					break;
				}
			}
		}
		else
			anyActive = true;

		if(anyActive)
		{
			// read sifts
			std::string str = basic;
			char buf[20];
			sprintf(buf, "state_%d.txt", frame);
			str += buf;

			ifstream siftIn(str.c_str());
			vector<RX::vec2> x1, x2;

			int numSifts;
			siftIn >> numSifts;
			for(int i = 0; i < numSifts; ++i)
			{
				RX::vec2 point1, point2;
				siftIn >> point1.x >> point2.x;
				siftIn >> point1.y >> point2.y;
				
				// remove sift if it is inside any board
				bool isInside = false;
				for(int mov = 0; mov < _numMovements; ++mov) 
				{
					if(!(_movements[mov].active(frame))) continue;
					if(_movements[mov].bbox(frame).isInside(point1-RX::vec2(_w/2, _h/2))) {
						isInside = true;
						break;
					}
				}
				// remove sift if it is inside any bbox to be removed
				for(int rem = 0; rem < _numRemoves; ++rem) 
				{
					if(_removes[rem].isInside(point1-RX::vec2(_w/2, _h/2))) {
						isInside = true;
						break;
					}
				}
				if(!isInside)
				{
					x1.push_back(point1);
					x2.push_back(point2);
				}
			}		

			siftIn.close();
			ofstream siftOut(str.c_str());

			siftOut << x1.size() << endl;
			for(int i = 0; i < x1.size(); ++i)
			{
				siftOut << x1[i].x << " " << x2[i].x << endl;
				siftOut << x1[i].y << " " << x2[i].y << endl;
			}
		}
	}

}
