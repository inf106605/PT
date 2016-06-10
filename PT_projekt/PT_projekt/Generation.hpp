#pragma once


#include <deque>
#include <memory>

#include "Indyvidual.hpp"


class Generation
{
public:
	Generation(pieces_t &pieces);
	~Generation() = default;

	double switchToNext(const unsigned mutationCount, pieces_t pieces);

	const Indyvidual& getBest() const { return *indyviduals.front(); }
	double getRating() const { return rating; }

private:
	typedef std::unique_ptr<Indyvidual> indyvidual_prt_t;
	typedef std::deque<indyvidual_prt_t> indyviduals_t;

	static const size_t INITIAL_SIZE = 10;

	double calculateRating() const;

	void createInitialIndyviduals(pieces_t &pieces);

	indyviduals_t indyviduals;
	double rating;

public:
	Generation(const Generation &) = delete;
	Generation& operator=(const Generation &) = delete;
};
