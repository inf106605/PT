#include "rating.hpp"

#include <cstdint>

#include "Indyvidual.hpp"
#include "Sampler.hpp"


namespace {

	double calculateLengthPenalty(const Indyvidual &indyvidual)
	{
		int minX = std::numeric_limits<int>::max();
		int maxX = std::numeric_limits<int>::min();
		int minY = std::numeric_limits<int>::max();
		int maxY = std::numeric_limits<int>::min();
		for (const auto &element : indyvidual.getPiecesInVectorMap())
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
		const arrangedPieces_t &arrangedPieces = indyvidual.getArrangedPieces();
		const Indyvidual::piecesInVectorMap_t &piecesInVectorMap = indyvidual.getPiecesInVectorMap();

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

}

double calculateRating(const Indyvidual &indyvidual)
{
	static const double LENGHT_PENALTY_WEIGHT = indyvidual.getArrangedPieces().size() * 2;
	const double lengthPenalty = calculateLengthPenalty(indyvidual) * LENGHT_PENALTY_WEIGHT;
	const double mathRating = calculateMatchRating(indyvidual);
	double rating = mathRating - lengthPenalty;
	return rating;
}
