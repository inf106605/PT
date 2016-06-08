#pragma once


#include <list>
#include <utility>
#include <vector>

#include <opencv2/core/core.hpp>

#include "findingPieces.hpp"


enum Rotation
{
	R0 = 0,
	R90 = 1,
	R180 = 2,
	R270 = 3
};

typedef std::pair<int, int> position_t;

struct ArrangedPiece
{
	piece_t &piece;
	Rotation rotation;
	position_t position;

	ArrangedPiece(const ArrangedPiece &) = default;
	ArrangedPiece& operator=(const ArrangedPiece &) = default;
};

typedef std::vector<ArrangedPiece> arrangedPieces_t;


arrangedPieces_t arrangePieces(pieces_t &pieces);
