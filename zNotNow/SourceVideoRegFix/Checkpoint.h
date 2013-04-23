#ifndef __CHECKPOINT_H
#define __CHECKPOINT_H

struct CheckPoint
{
	int frame;
	int board, checkpointNumber;

	CheckPoint(int frameP, int boardP, int checkpointNumberP)
	{
		frame = frameP;
		board = boardP; 
		checkpointNumber = checkpointNumberP;
	}

	const bool operator<(const CheckPoint &other) const
	{
		if(frame < other.frame)
			return true;
		return false;
	}
};

#endif // __CHECKPOINT_H