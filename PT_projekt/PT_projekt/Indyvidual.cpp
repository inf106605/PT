#include "Indyvidual.hpp"

#include <deque>

#include "rating.hpp"


Indyvidual::Indyvidual(pieces_t &pieces) :
	arrangedPieces(),
	piecesInVectorMap(),
	outdatedPiecesInVectorMap(),
	rating(0.0),
	ratingUpdated(false)
{
	createRandomRotationsAndPositions(pieces);
	createPiecesInVectorMap();
}

void Indyvidual::setPosition(const size_t n, const position_t position)
{
	ArrangedPiece &arrangedPiece = arrangedPieces[n];
	outdatedPiecesInVectorMap.emplace(n, arrangedPiece.position);
	arrangedPiece.position = position;
	ratingUpdated = false;
}

void Indyvidual::createRandomRotationsAndPositions(pieces_t &pieces)
{
	int maxValuePlusOne = (size_t)(std::sqrt(pieces.size())) + 2;
	int minValue = -maxValuePlusOne / 2;
	maxValuePlusOne += minValue;
	std::deque<position_t> remainingPositions;
	for (int i = minValue; i != maxValuePlusOne; ++i)
		for (int j = minValue; j != maxValuePlusOne; ++j)
			remainingPositions.push_back(position_t(i, j));
	arrangedPieces.reserve(pieces.size());
	for (cv::Mat &piece : pieces)
	{
		const size_t i = rand() % remainingPositions.size();
		const auto iterator = remainingPositions.begin() + i;
		arrangedPieces.push_back(ArrangedPiece{ piece, (Rotation)(rand() % 4), *iterator });
		remainingPositions.erase(iterator);
	}
}

void Indyvidual::createPiecesInVectorMap()
{
	for (size_t i = 0; i != arrangedPieces.size(); ++i)
		piecesInVectorMap.emplace(arrangedPieces[i].position, i);
}

void Indyvidual::updatePiecesInVectorMap() const
{
	for (const auto &outdatedPiece : outdatedPiecesInVectorMap)
		piecesInVectorMap.erase(outdatedPiece.second);
	for (const auto &outdatedPiece : outdatedPiecesInVectorMap)
		piecesInVectorMap.emplace(arrangedPieces[outdatedPiece.first].position, outdatedPiece.first);
	outdatedPiecesInVectorMap.clear();
}
