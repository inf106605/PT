#pragma once


#include "findingPieces.hpp"
#include "arrangingPieces.hpp"


class Sampler
{
public:
	enum Side { UP = 0, LEFT = 1, DOWN = 2, RIGHT = 3 };

	static Sampler generate(const ArrangedPiece &arrangedPiece1, const ArrangedPiece &arrangedPiece2, const bool first);

	Sampler(const piece_t &piece, const Side side, const bool first);
	~Sampler() = default;

	unsigned getLength() const { return length; }
	const cv::Vec3b& get(double percent) const { return piece.at<cv::Vec3b>(startPoint + (vector * (int)(length*percent))); }
	static Side reverseSide(Side side) { return (Side)((side + 2) % 4); }

private:
	static unsigned getLength(const piece_t &piece, Side side) { if (side == Side::UP || side == Side::DOWN) return piece.cols; else return piece.rows; }

	const piece_t &piece;
	const unsigned length;
	cv::Point startPoint;
	cv::Point vector;

public:
	Sampler(const Sampler &) = delete;
	Sampler& operator=(const Sampler &) = delete;
};
