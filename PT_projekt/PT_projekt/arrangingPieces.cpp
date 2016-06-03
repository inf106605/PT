#include "arrangingPieces.hpp"

#include <cstdint>
#include <algorithm>
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

	class Sampler
	{
	public:
		enum Side { UP = 0, LEFT = 1, DOWN = 2, RIGHT = 3 };
		Sampler(const cv::Mat &image, const Side side, const bool first);
		~Sampler() = default;
		unsigned getLength() const { return length; }
		const cv::Vec3b& get(double percent) const;
		static Side reverseSide(Side side) { return (Side)((side + 2) % 4); }
	private:
		static unsigned getLength(const cv::Mat &image,  Side side) { if (side == Side::UP || side == Side::DOWN) return image.cols; else return image.rows; }
		const cv::Mat &image;
		const unsigned length;
		cv::Point startPoint;
		cv::Point vector;
	public:
		Sampler(const Sampler &) = delete;
		Sampler& operator=(const Sampler &) = delete;
	};

	inline const cv::Vec3b& Sampler::get(double percent) const
	{
		return image.at<cv::Vec3b>(startPoint + (vector * (int)(length*percent)));
	}

	Sampler::Sampler(const cv::Mat &image, const Side side, const bool first) :
		image(image),
		length(getLength(image, side))
	{
		{
			const unsigned widthMinusTwo = getLength(image, (Side)((side + 1) % 4)) - 2;
			switch (side)
			{
			case Side::UP:
				startPoint.y = widthMinusTwo;
				break;
			case Side::LEFT:
				startPoint.x = 1;
				break;
			case Side::DOWN:
				startPoint.y = 1;
				break;
			case Side::RIGHT:
				startPoint.x = widthMinusTwo;
				break;
			}
		}
		{
			const unsigned lengthMinusOne = length - 1;
			if (side == Side::UP || side == Side::DOWN)
			{
				vector.y = 0;
				if ((side == Side::UP) == first)
				{
					vector.x = 1;
					startPoint.x = 0;
				}
				else
				{
					vector.x = -1;
					startPoint.x = lengthMinusOne;
				}
			}
			else
			{
				vector.x = 0;
				if ((side == Side::LEFT) == first)
				{
					vector.y = 1;
					startPoint.y = 0;
				}
				else
				{
					vector.y = -1;
					startPoint.y = lengthMinusOne;
				}
			}
		}
	}

	Sampler generateSampler(const ArrangedPiece &arrangedPiece1, const ArrangedPiece &arrangedPiece2, const bool first)
	{
		Sampler::Side side;
		if (arrangedPiece1.position.first != arrangedPiece2.position.first)
		{
			if (arrangedPiece1.position.first > arrangedPiece2.position.first)
				side = Sampler::Side::LEFT;
			else
				side = Sampler::Side::RIGHT;
		}
		else
		{
			if (arrangedPiece1.position.second > arrangedPiece2.position.second)
				side = Sampler::Side::DOWN;
			else
				side = Sampler::Side::UP;
		}
		if (!first)
			side = Sampler::reverseSide(side);
		const ArrangedPiece arrangedPiece = first ? arrangedPiece1 : arrangedPiece2;
		side = (Sampler::Side)((side + arrangedPiece.rotation) % 4);
		// The code below is marked as an error by the Visual Studio but it is correct.
		// The compiler will not use the deleted copy constructor due to the return value optimization.
		return Sampler(arrangedPiece.piece, side, first);
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
		const unsigned samplesNumber = (int)(minLength / (std::log((double)minLength)));
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
		const Sampler sampler1 = generateSampler(arrangedPiece1, arrangedPiece2, true);
		const Sampler sampler2 = generateSampler(arrangedPiece1, arrangedPiece2, false);
		if (!lenghtsAreSimilar(sampler1.getLength(), sampler2.getLength()))
			return -1.0;
		return compareColors(sampler1, sampler2);
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
		static const double LENGHT_PENALTY_WEIGHT = arrangedPieces.size() * 2;
		const double lengthPenalty = calculateLengthPenalty(piecesInVectorMap) * LENGHT_PENALTY_WEIGHT;
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

			arrangedPiece1.rotation = (Rotation)(rand() % 4);

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

	void doEvolution(arrangedPieces_t &arrangedPieces)
	{
		#ifdef _DEBUG
		unsigned maxFailedNumber = 2;
		#else
		unsigned maxFailedNumber = 128;
		#endif
		unsigned mutationCount = 512;
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
		std::cout << "Final rating: " << lastRating << std::endl;
	}

}//

std::vector<ArrangedPiece> arrangePieces(std::list<cv::Mat> &pieces)
{
	std::vector<ArrangedPiece> arrangedPieces = createFirstSpecimen(pieces);
	doEvolution(arrangedPieces);
	return arrangedPieces;
}
