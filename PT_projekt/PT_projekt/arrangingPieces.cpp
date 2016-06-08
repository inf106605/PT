#include "arrangingPieces.hpp"

#include <cstdint>
#include <algorithm>
#include <iostream>
#include <map>
#include <deque>

#include "Sampler.hpp"


namespace {

	typedef std::map<position_t, size_t> piecesInVectorMap_t;

	struct Indyvidual
	{
		std::vector<ArrangedPiece> arrangedPieces;
		piecesInVectorMap_t piecesInVectorMap;
		double rating;

		Indyvidual() = default;
		~Indyvidual() = default;
		Indyvidual(const Indyvidual &) = default;
		Indyvidual& operator=(const Indyvidual &) = default;
		Indyvidual(Indyvidual &&indyvidual) : arrangedPieces(std::move(indyvidual.arrangedPieces)), piecesInVectorMap(std::move(indyvidual.piecesInVectorMap)), rating(indyvidual.rating) {}
		Indyvidual& operator=(Indyvidual &&indyvidual) { arrangedPieces = std::move(indyvidual.arrangedPieces); piecesInVectorMap = std::move(indyvidual.piecesInVectorMap); rating = indyvidual.rating; return *this; }
	};

	piecesInVectorMap_t createPiecesInVectorMap(const arrangedPieces_t &arrangedPieces)
	{
		piecesInVectorMap_t result;
		for (size_t i = 0; i != arrangedPieces.size(); ++i)
			result.emplace(arrangedPieces[i].position, i);
		return result;
	}

	double calculateLengthPenalty(const piecesInVectorMap_t &piecesInVectorMap)
	{
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
		const double lengthPenalty = std::log((double)maxLength);
		return lengthPenalty;
	}

	bool lenghtsAreSimilar(unsigned length1, unsigned length2)
	{
		if (length1 > length2)
			std::swap(length1, length2);
		return length1 * 10 / 9 >= length2;
	}

	double compareColors(const Sampler &sampler1, const Sampler &sampler2)
	{
		const unsigned minLength = std::min(sampler1.getLength(), sampler2.getLength());
		const unsigned samplesNumber = (int)(minLength / std::log((double)minLength)) / 2;
		const double stepSize = 1.0 / samplesNumber;
		double result = 0.0;
		double currentPosition = stepSize * 0.5;
		for (unsigned i = 0; i != samplesNumber; ++i, currentPosition += stepSize)
		{
			const cv::Vec3b& color1 = sampler1.get(currentPosition);
			const cv::Vec3b& color2 = sampler2.get(currentPosition);
			const std::int_fast16_t colorDiff = std::max({ std::abs((std::int_fast16_t)color1[0] - (std::int_fast16_t)color2[0]),
					std::abs((std::int_fast16_t)color1[1] - (std::int_fast16_t)color2[1]),
					std::abs((std::int_fast16_t)color1[2] - (std::int_fast16_t)color2[2]) });
			const double colorRating = ((255 / 2) - colorDiff) / (double)(255 / 2);
			result += colorRating;
		}
		if (result <= 0.0)
			result = 0.0;
		else
		{
			result *= stepSize;
			if (result > 0.7)
				result = 2.0;
		}
		return result;
	}

	double calculateMatchRating(const ArrangedPiece &arrangedPiece1, const ArrangedPiece &arrangedPiece2)
	{
		// The code below is marked as an error by the Visual Studio but it is correct.
		// The compiler will not use the deleted copy constructor due to the copy elision optimization.
		const Sampler sampler1 = Sampler::generate(arrangedPiece1, arrangedPiece2, true);
		const Sampler sampler2 = Sampler::generate(arrangedPiece1, arrangedPiece2, false);
		if (!lenghtsAreSimilar(sampler1.getLength(), sampler2.getLength()))
			return -1.0;
		return compareColors(sampler1, sampler2);
	}

	double calculateMatchRating(const Indyvidual &indyvidual)
	{
		const std::vector<ArrangedPiece> &arrangedPieces = indyvidual.arrangedPieces;
		const piecesInVectorMap_t &piecesInVectorMap = indyvidual.piecesInVectorMap;

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

	void calculateRating(Indyvidual &indyvidual)
	{
		static const double LENGHT_PENALTY_WEIGHT = indyvidual.arrangedPieces.size() * 2;
		const double lengthPenalty = calculateLengthPenalty(indyvidual.piecesInVectorMap) * LENGHT_PENALTY_WEIGHT;
		const double mathRating = calculateMatchRating(indyvidual);
		indyvidual.rating = mathRating - lengthPenalty;
	}

	Indyvidual createRandomIndyvidual(pieces_t &pieces)
	{
		int maxValuePlusOne = (size_t)(std::sqrt(pieces.size())) + 2;
		int minValue = -maxValuePlusOne / 2;
		maxValuePlusOne += minValue;
		std::deque<position_t> remainingPositions;
		for (int i = minValue; i != maxValuePlusOne; ++i)
		for (int j = minValue; j != maxValuePlusOne; ++j)
			remainingPositions.push_back(position_t(i, j));
		Indyvidual indyvidual;
		indyvidual.arrangedPieces.reserve(pieces.size());
		for (cv::Mat &piece : pieces)
		{
			const size_t i = rand() % remainingPositions.size();
			const auto iterator = remainingPositions.begin() + i;
			indyvidual.arrangedPieces.push_back(ArrangedPiece{ piece, (Rotation)(rand() % 4), *iterator });
			remainingPositions.erase(iterator);
		}
		indyvidual.piecesInVectorMap = createPiecesInVectorMap(indyvidual.arrangedPieces);
		calculateRating(indyvidual);
		return indyvidual;
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

	inline void rotatePiece(ArrangedPiece &arrangedPiece)
	{
		arrangedPiece.rotation = (Rotation)(rand() % 4);
	}

	void movePiece(Indyvidual &indyvidual, ArrangedPiece &arrangedPiece1, size_t i)
	{
		size_t j = rand() % (indyvidual.arrangedPieces.size() - 1);
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
		auto founded = indyvidual.piecesInVectorMap.find(position);
		if (founded == indyvidual.piecesInVectorMap.cend())
		{
			position_t lastPosition = arrangedPiece1.position;
			indyvidual.piecesInVectorMap.erase(indyvidual.piecesInVectorMap.find(lastPosition));
			arrangedPiece1.position = position;
			indyvidual.piecesInVectorMap.emplace(position, i);
			const bool columnRemoved = removeROCIfEmpty(lastPosition, &position_t::first, indyvidual.arrangedPieces);
			const bool rowRemoved = removeROCIfEmpty(lastPosition, &position_t::second, indyvidual.arrangedPieces);
			if (columnRemoved || rowRemoved)
				indyvidual.piecesInVectorMap = createPiecesInVectorMap(indyvidual.arrangedPieces);
		}
		else
		{
			j = founded->second;
			ArrangedPiece &arrangedPiece2 = indyvidual.arrangedPieces[j];
			indyvidual.piecesInVectorMap.erase(founded);
			indyvidual.piecesInVectorMap.erase(indyvidual.piecesInVectorMap.find(arrangedPiece1.position));
			arrangedPiece2.position = arrangedPiece1.position;
			arrangedPiece1.position = position;
			indyvidual.piecesInVectorMap.emplace(arrangedPiece1.position, i);
			indyvidual.piecesInVectorMap.emplace(arrangedPiece2.position, j);
		}
	}

	void mutate(Indyvidual &indyvidual, unsigned mutationCount)
	{
		for (; mutationCount != 0; --mutationCount)
		{
			const size_t i = rand() % indyvidual.arrangedPieces.size();
			ArrangedPiece &arrangedPiece = indyvidual.arrangedPieces[i];

			rotatePiece(arrangedPiece);

			movePiece(indyvidual, arrangedPiece, i);
		}
	}

	typedef std::deque<Indyvidual> generation_t;

	generation_t createFirstGeneration(pieces_t &pieces)
	{
		const size_t FIRST_GENERATION_SIZE = 1;
		generation_t generation;
		for (size_t i = 0; i != FIRST_GENERATION_SIZE; ++i)
			generation.push_back(createRandomIndyvidual(pieces));
		return generation;
	}

	double nextGeneration(generation_t &generation, const unsigned mutationCount)
	{
		Indyvidual indyvidualCopy = generation.front();
		mutate(indyvidualCopy, mutationCount);
		calculateRating(indyvidualCopy);
		double improvement = indyvidualCopy.rating - generation.front().rating;
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
	//std::vector<ArrangedPiece> arrangedPieces = createRandomSpecimen(pieces);
	Indyvidual indyvidual = doEvolution(pieces);
	return indyvidual.arrangedPieces;
}
