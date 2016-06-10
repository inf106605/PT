#include "Generation.hpp"

#include <functional>
#include <queue>


Generation::Generation(pieces_t &pieces)
{
	createInitialIndyviduals(pieces);
	rating = calculateRating();
}

double Generation::switchToNext(const unsigned mutationCount, pieces_t pieces)
{
	while (indyviduals.size() > 1)
		indyviduals.erase(indyviduals.begin());
	Indyvidual indyvidualCopy = *indyviduals.front();
	indyvidualCopy.mutate(mutationCount);
	double improvement = indyvidualCopy.getRating() - indyviduals.front()->getRating();
	if (improvement > 0)
		indyviduals.front() = indyvidual_prt_t(new Indyvidual(std::move(indyvidualCopy)));
	return improvement;
}

double Generation::calculateRating() const
{
	double currentRating = -std::numeric_limits<double>::infinity();
	for (const auto &indyvidual : indyviduals)
		if (currentRating < indyvidual->getRating())
			currentRating = indyvidual->getRating();
	return currentRating;
}

void Generation::createInitialIndyviduals(pieces_t &pieces)
{
	for (size_t i = 0; i != INITIAL_SIZE; ++i)
		indyviduals.push_back(indyvidual_prt_t(new Indyvidual(pieces)));
}
