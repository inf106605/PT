#pragma once


#include <deque>
#include <memory>
#include <mutex>

#include "Individual.hpp"
#include "ThreadPool.hpp"


class Generation
{
public:
	Generation(pieces_t &pieces);
	~Generation() = default;

	double switchToNext(const unsigned mutationCount, pieces_t &pieces);

	const Individual& getBest() const { return *individuals.back(); }
	double getRating() const { return rating; }

private:
	typedef std::unique_ptr<Individual> individual_prt_t;
	typedef std::deque<individual_prt_t> individuals_t;

	static const size_t INITIAL_SIZE = 10;
	static const size_t SELECTION_SIZE = 10;
	static const unsigned SELECTION_LEAVE_PERCENT = 20;
	static const size_t CROSSES_NUMBER = 25;
	static const size_t MUTATION_NUMBER = 10;
	static const size_t RANDOM_NUMBER = 5;

	double calculateRating() const;

	void createInitialIndividuals(pieces_t &pieces);
	void createNewIndividuals(const unsigned mutationCount, pieces_t &pieces);
	void doSelection();

	individuals_t individuals;
	double rating;
	ThreadPool threadPool;
	std::mutex mutex;

public:
	Generation(const Generation &) = delete;
	Generation& operator=(const Generation &) = delete;
};
