#include "Generation.hpp"

#include <functional>
#include <queue>


Generation::Generation(pieces_t &pieces)
{
	createInitialIndividuals(pieces);
	rating = calculateRating();
}

double Generation::switchToNext(const unsigned mutationCount, pieces_t &pieces)
{
	createNewIndividuals(mutationCount, pieces);
	doSelection();
	const double newRating = calculateRating();
	const double ratingImprovement = newRating - rating;
	rating = newRating;
	return ratingImprovement;
}

double Generation::calculateRating() const
{
	double currentRating = -std::numeric_limits<double>::infinity();
	for (const auto &individual : individuals)
		if (currentRating < individual->getRating())
			currentRating = individual->getRating();
	return currentRating;
}

void Generation::createInitialIndividuals(pieces_t &pieces)
{
	for (size_t i = 0; i != INITIAL_SIZE; ++i)
		individuals.push_back(individual_prt_t(new Individual(pieces)));
}

void Generation::createNewIndividuals(const unsigned mutationCount, pieces_t &pieces)
{
	const size_t initialSize = individuals.size();
	for (size_t i = 0; i != CROSSES_NUMBER; ++i)
	{
		const size_t x = rand() % initialSize;
		size_t y = rand() % (initialSize - 1);
		if (y >= x)
			++y;
		Individual individualCopy1 = *individuals[x];
		Individual individualCopy2 = *individuals[y];
		individualCopy1.mutate(mutationCount);
		individualCopy2.mutate(mutationCount);
		Individual newIndividual(individualCopy1, individualCopy2);
		individuals.push_back(individual_prt_t(new Individual(std::move(newIndividual))));
	}
	for (size_t i = 0; i != MUTATION_NUMBER; ++i)
	{
		const size_t x = rand() % initialSize;
		Individual newIndividual = *individuals[x];
		newIndividual.mutate(mutationCount);
		individuals.push_back(individual_prt_t(new Individual(std::move(newIndividual))));
	}
	for (size_t i = 0; i != RANDOM_NUMBER; ++i)
		individuals.push_back(individual_prt_t(new Individual(pieces)));
}

void Generation::doSelection()
{
	std::sort(individuals.begin(), individuals.end(), [](const individual_prt_t &i1, const individual_prt_t &i2) -> bool { return i1->getRating() < i2->getRating(); });
	for (size_t i = 0; i != individuals.size() - SELECTION_SIZE;)
		if (rand() % 100 < SELECTION_LEAVE_PERCENT)
			++i;
		else
			individuals.erase(individuals.begin() + i);
}
