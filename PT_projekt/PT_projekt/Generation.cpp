#include "Generation.hpp"


double Generation::switchToNext(const unsigned mutationCount)
{
	Indyvidual indyvidualCopy = indyviduals.front();
	indyvidualCopy.mutate(mutationCount);
	double improvement = indyvidualCopy.getRating() - indyviduals.front().getRating();
	if (improvement > 0)
		indyviduals.front() = std::move(indyvidualCopy);
	return improvement;
}
