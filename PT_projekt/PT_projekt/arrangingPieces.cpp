#include "arrangingPieces.hpp"

#include <algorithm>
#include <iostream>
#include <map>
#include <deque>
#include <chrono>

#include "Indyvidual.hpp"
#include "Generation.hpp"


namespace {

	class Timer
	{
	public:
		Timer() : startTime(std::chrono::system_clock::now()) {}
		void reset() { startTime = std::chrono::system_clock::now(); }
		long long getSeconds() const { return std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now() - startTime).count(); }

	private:
		std::chrono::time_point<std::chrono::system_clock> startTime;
	};

}//

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
	unsigned long long timeLimit = 1;
	Timer timer;
	while (true)
	{
		const double improvement = generation.switchToNext(mutationCount, pieces);
		if (improvement > 0.0)
		{
			failedNumber = 0;
		}
		else
		{
			if (++failedNumber == maxFailedNumber || timer.getSeconds() > 1)
			{
				if ((mutationCount /= 2) == 0)
					break;
				timeLimit *= 2;
				if (mutationCount > 2)
					maxFailedNumber *= 2;
				else if (mutationCount == 2)
					maxFailedNumber *= 4;
				else
					maxFailedNumber *= 10;
				failedNumber = 0;
				timer.reset();
			}
		}
	}
	return generation.getBest().getArrangedPieces();
}
