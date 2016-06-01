#include "arrangingPieces.hpp"

#include <iostream>
#include <map>


namespace {

	arrangedPieces_t createFirstSpecimen(std::list<cv::Mat> &pieces)
	{
		arrangedPieces_t arrangedPieces;
		arrangedPieces.reserve(pieces.size());
		int x = 0;
		for (cv::Mat &piece : pieces)
			arrangedPieces.push_back(ArrangedPiece{ piece, Rotation::R0, position_t(++x, 0) });
		return arrangedPieces;
	}

	typedef std::map<position_t, size_t> piecesInVectorMap_t;

	piecesInVectorMap_t createPiecesInVectorMap(const arrangedPieces_t &arrangedPieces)
	{
		piecesInVectorMap_t result;
		for (size_t i = 0; i != arrangedPieces.size(); ++i)
			result.emplace(arrangedPieces[i].position, i);
		return result;
	}

	double calculateLengthPenalty(const piecesInVectorMap_t &piecesInVectorMap)
	{
		const double MULTIPLER = 100.0;
		int minX = std::numeric_limits<int>::max();
		int maxX = std::numeric_limits<int>::min();
		int minY = std::numeric_limits<int>::max();
		int maxY = std::numeric_limits<int>::min();
		for (const auto &element : piecesInVectorMap)
		{
			const position_t &position = element.first;
			if (minX > position.first)
				minX = position.first;
			if (maxX < position.first)
				maxX = position.first;
			if (minY > position.second)
				minY = position.second;
			if (maxY < position.second)
				maxY = position.second;
		}
		const int maxLength = std::max(maxX - minX, maxY - minY);
		const double lengthPenalty = std::log((double)maxLength) * MULTIPLER;
		return lengthPenalty;
	}

	double calculateMatchRating(const ArrangedPiece &arrangedPiece1, const ArrangedPiece &arrangedPiece2)
	{
		//TODO
		return 0.0;
	}

	double calculateMatchRating(const arrangedPieces_t &arrangedPieces, const piecesInVectorMap_t &piecesInVectorMap)
	{
		const double NO_NEIGHBOR_PENALTY = 0.1;
		double matchRating = 0.0;
		for (const ArrangedPiece &arrangedPiece1 : arrangedPieces)
		{
			position_t position = arrangedPiece1.position;
			++position.first;
			auto founded = piecesInVectorMap.find(position);
			if (founded != piecesInVectorMap.cend())
				matchRating += calculateMatchRating(arrangedPiece1, arrangedPieces[founded->second]);
			else
				matchRating -= NO_NEIGHBOR_PENALTY;
			--position.first;
			++position.second;
			founded = piecesInVectorMap.find(position);
			if (founded != piecesInVectorMap.cend())
				matchRating += calculateMatchRating(arrangedPiece1, arrangedPieces[founded->second]);
			else
				matchRating -= NO_NEIGHBOR_PENALTY;
			--position.first;
			--position.second;
			founded = piecesInVectorMap.find(position);
			if (founded != piecesInVectorMap.cend())
				matchRating += calculateMatchRating(arrangedPiece1, arrangedPieces[founded->second]);
			else
				matchRating -= NO_NEIGHBOR_PENALTY;
			++position.first;
			--position.second;
			founded = piecesInVectorMap.find(position);
			if (founded != piecesInVectorMap.cend())
				matchRating += calculateMatchRating(arrangedPiece1, arrangedPieces[founded->second]);
			else
				matchRating -= NO_NEIGHBOR_PENALTY;
		}
		return matchRating;
	}

	double calculateRating(const arrangedPieces_t &arrangedPieces, const piecesInVectorMap_t &piecesInVectorMap)
	{
		const double lengthPenalty = calculateLengthPenalty(piecesInVectorMap);
		const double mathRating = calculateMatchRating(arrangedPieces, piecesInVectorMap);
		const double rating = mathRating - lengthPenalty;
		return rating;
	}

	bool removeROCIfEmpty(const position_t &searchedPosition, int position_t::*const roc, arrangedPieces_t &arrangedPieces)
	{
		const int searchedValue = searchedPosition.*roc;
		for (const ArrangedPiece &arrangedPiece : arrangedPieces)
			if (arrangedPiece.position.*roc == searchedValue)
				return false;
		for (ArrangedPiece &arrangedPiece : arrangedPieces)
			if (arrangedPiece.position.*roc > searchedValue)
				--(arrangedPiece.position.*roc);
		return true;
	}

	void mutate(arrangedPieces_t &arrangedPieces, piecesInVectorMap_t &piecesInVectorMap, unsigned mutationCount)
	{
		for (; mutationCount != 0; --mutationCount)
		{
			const size_t i = rand() % arrangedPieces.size();
			ArrangedPiece &arrangedPiece1 = arrangedPieces[i];
			if (rand() % 2 == 0)
			{
				arrangedPiece1.rotation = (Rotation)(rand() % 4);
			}
			else
			{
				size_t j = rand() % (arrangedPieces.size() - 1);
				if (j >= i)
					++j;
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
				auto founded = piecesInVectorMap.find(position);
				if (founded == piecesInVectorMap.cend())
				{
					position_t lastPosition = arrangedPiece1.position;
					piecesInVectorMap.erase(piecesInVectorMap.find(lastPosition));
					arrangedPiece1.position = position;
					piecesInVectorMap.emplace(position, i);
					const bool columnRemoved = removeROCIfEmpty(lastPosition, &position_t::first, arrangedPieces);
					const bool rowRemoved = removeROCIfEmpty(lastPosition, &position_t::second, arrangedPieces);
					if (columnRemoved || rowRemoved)
						piecesInVectorMap = createPiecesInVectorMap(arrangedPieces);
				}
				else
				{
					j = founded->second;
					ArrangedPiece &arrangedPiece2 = arrangedPieces[j];
					piecesInVectorMap.erase(founded);
					piecesInVectorMap.erase(piecesInVectorMap.find(arrangedPiece1.position));
					arrangedPiece2.position = arrangedPiece1.position;
					arrangedPiece1.position = position;
					piecesInVectorMap.emplace(arrangedPiece1.position, i);
					piecesInVectorMap.emplace(arrangedPiece2.position, j);
				}
			}
		}
	}

	void doEvolution(arrangedPieces_t &arrangedPieces)
	{
		#ifdef _DEBUG
		unsigned maxFailedNumber = 2;
		#else
		unsigned maxFailedNumber = 64;
		#endif
		unsigned mutationCount = 4096;
		unsigned failedNumber = 0;
		piecesInVectorMap_t piecesInVectorMap = createPiecesInVectorMap(arrangedPieces);
		double lastRating = calculateRating(arrangedPieces, piecesInVectorMap);
		std::cout << "Initial rating: " << lastRating << std::endl;
		while (true)
		{
			arrangedPieces_t arrangedPiecesCopy = arrangedPieces;
			piecesInVectorMap_t piecesInVectorMapCopy = piecesInVectorMap;
			mutate(arrangedPiecesCopy, piecesInVectorMapCopy, mutationCount);
			double rating = calculateRating(arrangedPiecesCopy, piecesInVectorMapCopy);
			if (lastRating < rating)
			{
				lastRating = rating;
				arrangedPieces = std::move(arrangedPiecesCopy);
				piecesInVectorMap = std::move(piecesInVectorMapCopy);
				failedNumber = 0;
			}
			else
			{
				if (++failedNumber == maxFailedNumber)
				{
					maxFailedNumber = maxFailedNumber * 17 / 10;
					if ((mutationCount /= 2) == 0)
						break;
					failedNumber = 0;
				}
			}
		}
		std::cout << "Final rating: " << lastRating << std::endl;
	}

}//

std::vector<ArrangedPiece> arrangePieces(std::list<cv::Mat> &pieces)
{
	std::vector<ArrangedPiece> arrangedPieces = createFirstSpecimen(pieces);
	doEvolution(arrangedPieces);
	return arrangedPieces;
}
