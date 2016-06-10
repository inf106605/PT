#include "arrangingPieces.hpp"

#include <algorithm>
#include <iostream>
#include <map>
#include <deque>

#include "Indyvidual.hpp"
#include "Generation.hpp"


arrangedPieces_t arrangePieces(pieces_t &pieces)
{
	std::cout << "Arranging pieces..." << std::endl;
	Generation generation(pieces);
	#ifdef _DEBUG
	unsigned maxFailedNumber = 1;
	#else
	unsigned maxFailedNumber = 32;
	#endif
	unsigned mutationCount = 8;
	unsigned failedNumber = 0;
	while (true)
	{
		const double improvement = generation.switchToNext(mutationCount, pieces);
		if (improvement > 0.0)
		{
			failedNumber = 0;
		}
		else
		{
			if (++failedNumber == maxFailedNumber)
			{
				if ((mutationCount /= 2) == 0)
					break;
				if (mutationCount > 2)
					maxFailedNumber = maxFailedNumber * 2;
				else if (mutationCount == 2)
					maxFailedNumber = maxFailedNumber * 4;
				else
					maxFailedNumber = maxFailedNumber * 10;
				failedNumber = 0;
			}
		}
	}
	return generation.getBest().getArrangedPieces();
}
