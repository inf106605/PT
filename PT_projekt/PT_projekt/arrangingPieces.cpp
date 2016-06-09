#include "arrangingPieces.hpp"

#include <algorithm>
#include <iostream>
#include <map>
#include <deque>

#include "Indyvidual.hpp"


namespace {

	bool removeROCIfEmpty(const position_t &searchedPosition, int position_t::*const roc, Indyvidual &indyvidual)
	{
		const int searchedValue = searchedPosition.*roc;
		for (const ArrangedPiece &arrangedPiece : indyvidual.getArrangedPieces())
			if (arrangedPiece.position.*roc == searchedValue)
				return false;
		for (size_t i = 0; i != indyvidual.getArrangedPieces().size(); ++i)
		{
			position_t position = indyvidual.getPosition(i);
			if (position.*roc > searchedValue)
			{
				--(position.*roc);
				indyvidual.setPosition(i, position);
			}
		}
		return true;
	}

	inline void rotatePiece(Indyvidual &indyvidual, const size_t i)
	{
		indyvidual.setRotation(i, (Rotation)(rand() % 4));
	}

	void movePiece(Indyvidual &indyvidual, const size_t i)
	{
		size_t j = rand() % (indyvidual.getArrangedPieces().size() - 1);
		if (j >= i)
			++j;
		const ArrangedPiece &arrangedPiece1 = indyvidual.getArrangedPieces()[i];
		position_t position = arrangedPiece1.position;
		switch (rand() % 4)
		{
		case 0:
			--position.first;
			break;
		case 1:
			++position.first;
			break;
		case 2:
			--position.second;
			break;
		case 3:
			++position.second;
			break;
		}
		const Indyvidual::piecesInVectorMap_t &piecesInVectorMap = indyvidual.getPiecesInVectorMap();
		auto founded = piecesInVectorMap.find(position);
		if (founded == piecesInVectorMap.cend())
		{
			position_t lastPosition = arrangedPiece1.position;
			indyvidual.setPosition(i, position);
			//TODO is removing necesary?
			const bool columnRemoved = removeROCIfEmpty(lastPosition, &position_t::first, indyvidual);
			const bool rowRemoved = removeROCIfEmpty(lastPosition, &position_t::second, indyvidual);
		}
		else
		{
			j = founded->second;
			indyvidual.setPosition(j, indyvidual.getPosition(i));
			indyvidual.setPosition(i, position);
		}
	}

	void mutate(Indyvidual &indyvidual, unsigned mutationCount)
	{
		for (; mutationCount != 0; --mutationCount)
		{
			const size_t i = rand() % indyvidual.getArrangedPieces().size();
			rotatePiece(indyvidual, i);
			movePiece(indyvidual, i);
		}
	}

	typedef std::deque<Indyvidual> generation_t;

	generation_t createFirstGeneration(pieces_t &pieces)
	{
		const size_t FIRST_GENERATION_SIZE = 1;
		generation_t generation(FIRST_GENERATION_SIZE, pieces);
		return generation;
	}

	double nextGeneration(generation_t &generation, const unsigned mutationCount)
	{
		Indyvidual indyvidualCopy = generation.front();
		mutate(indyvidualCopy, mutationCount);
		double improvement = indyvidualCopy.getRating() - generation.front().getRating();
		if (improvement > 0)
			generation.front() = std::move(indyvidualCopy);
		return improvement;
	}

	Indyvidual doEvolution(pieces_t &pieces)
	{
		generation_t generation = createFirstGeneration(pieces);
		#ifdef _DEBUG
		unsigned maxFailedNumber = 2;
		#else
		unsigned maxFailedNumber = 128;
		#endif
		unsigned mutationCount = 128;
		unsigned failedNumber = 0;
		while (true)
		{
			const double improvement = nextGeneration(generation, mutationCount);
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
		Indyvidual result = std::move(generation.front());
		return result;
	}

}//

std::vector<ArrangedPiece> arrangePieces(pieces_t &pieces)
{
	std::cout << "Arranging pieces..." << std::endl;
	Indyvidual indyvidual = doEvolution(pieces);
	arrangedPieces_t result = std::move(indyvidual.getArrangedPieces());
	return result;
}
