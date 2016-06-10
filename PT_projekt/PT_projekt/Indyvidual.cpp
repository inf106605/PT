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

void Indyvidual::mutate(unsigned mutationCount)
{
	for (; mutationCount != 0; --mutationCount)
	{
		const size_t i = rand() % getArrangedPieces().size();
		rotatePiece(i);
		movePiece(i);
	}
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

Indyvidual Indyvidual::operator*(const Indyvidual &indyvidual) const
{
	//TODO
	return *this;
}

void Indyvidual::createPiecesInVectorMap()
{
	for (size_t i = 0; i != arrangedPieces.size(); ++i)
		piecesInVectorMap.emplace(arrangedPieces[i].position, i);
}

bool Indyvidual::removeROCIfEmpty(const position_t &searchedPosition, int position_t::*const roc)
{
	const int searchedValue = searchedPosition.*roc;
	for (const ArrangedPiece &arrangedPiece : getArrangedPieces())
	if (arrangedPiece.position.*roc == searchedValue)
		return false;
	for (size_t i = 0; i != getArrangedPieces().size(); ++i)
	{
		position_t position = getPosition(i);
		if (position.*roc > searchedValue)
		{
			--(position.*roc);
			setPosition(i, position);
		}
	}
	return true;
}

void Indyvidual::rotatePiece(const size_t i)
{
	setRotation(i, (Rotation)(rand() % 4));
}

void Indyvidual::movePiece(const size_t i)
{
	size_t j = rand() % (getArrangedPieces().size() - 1);
	if (j >= i)
		++j;
	const ArrangedPiece &arrangedPiece1 = getArrangedPieces()[i];
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
	const Indyvidual::piecesInVectorMap_t &piecesInVectorMap = getPiecesInVectorMap();
	auto founded = piecesInVectorMap.find(position);
	if (founded == piecesInVectorMap.cend())
	{
		position_t lastPosition = arrangedPiece1.position;
		setPosition(i, position);
		//TODO is removing necesary?
		removeROCIfEmpty(lastPosition, &position_t::first);
		removeROCIfEmpty(lastPosition, &position_t::second);
	}
	else
	{
		j = founded->second;
		setPosition(j, getPosition(i));
		setPosition(i, position);
	}
}

void Indyvidual::updatePiecesInVectorMap() const
{
	for (const auto &outdatedPiece : outdatedPiecesInVectorMap)
		piecesInVectorMap.erase(outdatedPiece.second);
	for (const auto &outdatedPiece : outdatedPiecesInVectorMap)
		piecesInVectorMap.emplace(arrangedPieces[outdatedPiece.first].position, outdatedPiece.first);
	outdatedPiecesInVectorMap.clear();
}
