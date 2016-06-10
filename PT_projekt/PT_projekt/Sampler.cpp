#include "Sampler.hpp"


Sampler::Sampler(const piece_t &piece, const Side side, const bool first) :
	piece(piece),
	length(getLength(piece, side))
{
	{
		const unsigned widthMinusTwo = getLength(piece, (Side)((side + 1) % 4)) - 2;
		switch (side)
		{
		case Side::UP:
			startPoint.y = widthMinusTwo;
			break;
		case Side::LEFT:
			startPoint.x = 1;
			break;
		case Side::DOWN:
			startPoint.y = 1;
			break;
		case Side::RIGHT:
			startPoint.x = widthMinusTwo;
			break;
		}
	}
	{
	const unsigned lengthMinusOne = length - 1;
	if (side == Side::UP || side == Side::DOWN)
	{
		vector.y = 0;
		if ((side == Side::UP) == first)
		{
			vector.x = 1;
			startPoint.x = 0;
		}
		else
		{
			vector.x = -1;
			startPoint.x = lengthMinusOne;
		}
	}
	else
	{
		vector.x = 0;
		if ((side == Side::LEFT) == first)
		{
			vector.y = 1;
			startPoint.y = 0;
		}
		else
		{
			vector.y = -1;
			startPoint.y = lengthMinusOne;
		}
	}
}
}

Sampler Sampler::generate(const ArrangedPiece &arrangedPiece1, const ArrangedPiece &arrangedPiece2, const bool first)
{
	Sampler::Side side;
	if (arrangedPiece1.position.first != arrangedPiece2.position.first)
	{
		if (arrangedPiece1.position.first > arrangedPiece2.position.first)
			side = Sampler::Side::LEFT;
		else
			side = Sampler::Side::RIGHT;
	}
	else
	{
		if (arrangedPiece1.position.second > arrangedPiece2.position.second)
			side = Sampler::Side::DOWN;
		else
			side = Sampler::Side::UP;
	}
	if (!first)
		side = Sampler::reverseSide(side);
	const ArrangedPiece arrangedPiece = first ? arrangedPiece1 : arrangedPiece2;
	side = (Sampler::Side)((side + arrangedPiece.rotation) % 4);
	// The code below is marked as an error by the Visual Studio but it is correct.
	// The compiler will not use the deleted copy constructor due to the return value optimization.
	return Sampler(arrangedPiece.piece, side, first);
}
