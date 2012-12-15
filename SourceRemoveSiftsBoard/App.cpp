#include <vector>
#include <string>
#include "Movement.h"

using namespace std;

string basic = "D:/Research/Project2011b/Data/Diffmov/";

int main(int argc, char *argv[])
{
	int _numFrames, _w, _h;
	int _numMovements;
	vector<Movement> _movements;

	std::ifstream input(basic+"_MoveInfo.txt");

	input >> _numFrames;
	input >> _w >> _h;

	std::ifstream input2(basic+"_LocalBoards.txt");
	input2 >> _numMovements;
	
	for(int i = 0; i < _numMovements; ++i)
	{
		_movements.push_back(Movement());

		int start, end, board;
		input2 >> start >> end >> board;;

		_movements[_movements.size()-1].start = start;
		_movements[_movements.size()-1].end = end;
		_movements[_movements.size()-1].board = board;

		for(int j = _movements[_movements.size()-1].start; j <= _movements[_movements.size()-1].end; ++j)
		{
			RX::vec2 p1, p2, p3, p4;
			input2 >> p1 >> p2 >> p3 >> p4;
			_movements[_movements.size()-1].addBoard(p1, p2, p3, p4);
		}
	}


	for(int frame = 0; frame < _numFrames; ++frame)
	{
		cout << frame << endl;
		
		bool anyActive = false;
		for(int mov = 0; mov < _numMovements; ++mov) 
		{
			if(_movements[mov].active(frame)) {
				anyActive = true;
				break;
			}
		}

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
