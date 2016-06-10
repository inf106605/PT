#pragma once


#include <deque>

#include "Indyvidual.hpp"


class Generation
{
public:
	Generation(pieces_t &pieces) : indyviduals(INITIAL_GENERATION_SIZE, pieces) {}
	~Generation() = default;

	double switchToNext(const unsigned mutationCount);

	const Indyvidual& operator[](const size_t n) const { return indyviduals[n]; }

private:
	typedef std::deque<Indyvidual> indyviduals_t;

	const size_t INITIAL_GENERATION_SIZE = 1;

	indyviduals_t indyviduals;

public:
	Generation(const Generation &) = delete;
	Generation& operator=(const Generation &) = delete;
};
