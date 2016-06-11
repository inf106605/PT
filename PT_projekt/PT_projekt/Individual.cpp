#include "Individual.hpp"

#include <deque>
#include <unordered_set>

#include "rating.hpp"


Individual::Individual(pieces_t &pieces) :
	arrangedPieces(),
	piecesInVectorMap(),
	outdatedPiecesInVectorMap(),
	rating(0.0),
	ratingUpdated(false)
{
	createRandomRotationsAndPositions(pieces);
	createPiecesInVectorMap();
}

Individual::Individual(const Individual &individual1, const Individual &individual2) :
	arrangedPieces(),
	piecesInVectorMap(),
	outdatedPiecesInVectorMap(),
	rating(0.0),
	ratingUpdated(false)
{
	crossRotationsAndPositions(individual1.getArrangedPieces(), individual2.getArrangedPieces());
	createPiecesInVectorMap();
}

void Individual::setPosition(const size_t n, const position_t position)
{
	ArrangedPiece &arrangedPiece = arrangedPieces[n];
	outdatedPiecesInVectorMap.emplace(n, arrangedPiece.position);
	arrangedPiece.position = position;
	ratingUpdated = false;
}

void Individual::mutate(unsigned mutationCount)
{
	for (; mutationCount != 0; --mutationCount)
	{
		const size_t i = rand() % getArrangedPieces().size();
		rotatePiece(i);
		movePiece(i);
	}
}

void Individual::createRandomRotationsAndPositions(pieces_t &pieces)
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

void Individual::crossRotationsAndPositions(const arrangedPieces_t &arrangedPieces1, const arrangedPieces_t &arrangedPieces2)
{
	int minX = std::numeric_limits<int>::max();
	int maxX = std::numeric_limits<int>::min();
	int minY = std::numeric_limits<int>::max();
	int maxY = std::numeric_limits<int>::min();
	for (const ArrangedPiece &arrangedPiece : arrangedPieces1)
	{
		minX = std::min(minX, arrangedPiece.position.first);
		maxX = std::max(maxX, arrangedPiece.position.first);
		minY = std::min(minY, arrangedPiece.position.second);
		maxY = std::max(maxY, arrangedPiece.position.second);
	}
	for (const ArrangedPiece &arrangedPiece : arrangedPieces2)
	{
		minX = std::min(minX, arrangedPiece.position.first);
		maxX = std::max(maxX, arrangedPiece.position.first);
		minY = std::min(minY, arrangedPiece.position.second);
		maxY = std::max(maxY, arrangedPiece.position.second);
	}

	std::unordered_set<position_t> unoccupiedPositions;
	for (int i = minX; i <= maxX; ++i)
		for (int j = minY; j <= maxY; ++j)
			unoccupiedPositions.emplace(i, j);

	for (size_t i = 0; i != arrangedPieces1.size(); ++i)
	{
		const arrangedPieces_t &choosenArrangedPieces = (rand() % 2 == 0) ? arrangedPieces1 : arrangedPieces2;
		ArrangedPiece newArrangedPiece = choosenArrangedPieces[i];
		auto it = unoccupiedPositions.find(newArrangedPiece.position);
		if (it == unoccupiedPositions.cend())
		{
			it = unoccupiedPositions.begin();
			std::advance(it, rand() % unoccupiedPositions.size());
			newArrangedPiece.position = *it;
		}
		unoccupiedPositions.erase(it);
		arrangedPieces.push_back(newArrangedPiece);
	}
}

void Individual::createPiecesInVectorMap()
{
	for (size_t i = 0; i != arrangedPieces.size(); ++i)
		piecesInVectorMap.emplace(arrangedPieces[i].position, i);
}

bool Individual::removeROCIfEmpty(const position_t &searchedPosition, int position_t::*const roc)
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

void Individual::rotatePiece(const size_t i)
{
	setRotation(i, (Rotation)(rand() % 4));
}

void Individual::movePiece(const size_t i)
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
	const Individual::piecesInVectorMap_t &piecesInVectorMap = getPiecesInVectorMap();
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

void Individual::updatePiecesInVectorMap() const
{
	for (const auto &outdatedPiece : outdatedPiecesInVectorMap)
		piecesInVectorMap.erase(outdatedPiece.second);
	for (const auto &outdatedPiece : outdatedPiecesInVectorMap)
		piecesInVectorMap.emplace(arrangedPieces[outdatedPiece.first].position, outdatedPiece.first);
	outdatedPiecesInVectorMap.clear();
}
