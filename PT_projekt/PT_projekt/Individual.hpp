#pragma once


#include <unordered_map>

#include "findingPieces.hpp"
#include "arrangingPieces.hpp"
#include "rating.hpp"


class Individual
{
public:
	typedef std::unordered_map<position_t, size_t> piecesInVectorMap_t;

	Individual(pieces_t &pieces);
	Individual(const Individual &individual1, const Individual &individual2);
	Individual(const Individual &) = default;
	Individual(Individual &&individual) : arrangedPieces(std::move(individual.arrangedPieces)), piecesInVectorMap(std::move(individual.piecesInVectorMap)), outdatedPiecesInVectorMap(std::move(individual.outdatedPiecesInVectorMap)), rating(individual.rating), ratingUpdated(individual.ratingUpdated) {}
	~Individual() = default;

	Individual& operator=(const Individual &) = default;
	Individual& operator=(Individual &&individual) { arrangedPieces = std::move(individual.arrangedPieces); piecesInVectorMap = std::move(individual.piecesInVectorMap); outdatedPiecesInVectorMap = std::move(individual.outdatedPiecesInVectorMap); rating = individual.rating; ratingUpdated = individual.ratingUpdated; return *this; }

	const arrangedPieces_t& getArrangedPieces() const { return arrangedPieces; }
	Rotation getRotation(const size_t n) const { return arrangedPieces[n].rotation; }
	void setRotation(const size_t n, const Rotation rotation) { arrangedPieces[n].rotation = rotation; ratingUpdated = false; };
	const position_t& getPosition(const size_t n) const { return arrangedPieces[n].position; }
	void setPosition(const size_t n, const position_t position);

	void mutate(unsigned mutationCount);

	const piecesInVectorMap_t& getPiecesInVectorMap() const { if (!isPiecesInVectorMapUpdated()) updatePiecesInVectorMap(); return piecesInVectorMap; }
	double getRating() const { if (!ratingUpdated) updateRating();  return rating; }

private:
	void createPiecesInVectorMap();
	void createRandomRotationsAndPositions(pieces_t &pieces);
	void crossRotationsAndPositions(const arrangedPieces_t &arrangedPieces1, const arrangedPieces_t &arrangedPieces2);

	bool removeROCIfEmpty(const position_t &searchedPosition, int position_t::*const roc);
	void rotatePiece(const size_t i);
	void movePiece(const size_t i);

	bool isPiecesInVectorMapUpdated() const { return outdatedPiecesInVectorMap.empty(); }
	void updatePiecesInVectorMap() const;
	void updateRating() const { rating = calculateRating(*this); ratingUpdated = true; }

	arrangedPieces_t arrangedPieces;
	mutable piecesInVectorMap_t piecesInVectorMap;
	mutable std::unordered_map<size_t, position_t> outdatedPiecesInVectorMap;
	mutable double rating;
	mutable bool ratingUpdated;
};
