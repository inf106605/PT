#include "arrangingPieces.hpp"


namespace {

	std::vector<ArrangedPiece> createFirstSpecimen(const std::list<cv::Mat> &pieces)
	{
		const int size = (int)std::sqrt(pieces.size()) + 1;
		std::vector<ArrangedPiece> arrangedPieces;
		arrangedPieces.reserve(pieces.size());
		int x = 0, y = 0;
		for (const cv::Mat &piece : pieces)
		{
			arrangedPieces.push_back(ArrangedPiece{ &piece, Rotation::R0, cv::Point(x, y) });
			if (++x == size)
			{
				x = 0;
				++y;
			}
		}
		return arrangedPieces;
	}

	double calculateRating(const std::vector<ArrangedPiece> &arrangedPieces)
	{
		//TODO
		return 0.0;
	}

	void mutate(std::vector<ArrangedPiece> &arrangedPieces, unsigned mutationCount)
	{
		//TODO
	}

}//

std::vector<ArrangedPiece> arrangePieces(const std::list<cv::Mat> &pieces)
{
	static const unsigned MAX_FAILED_NUMBER = 10;
	std::vector<ArrangedPiece> arrangedPieces = createFirstSpecimen(pieces);
	double lastRating = calculateRating(arrangedPieces);
	unsigned mutationCount = 128;
	unsigned failedNumber = 0;
	while (true)
	{
		std::vector<ArrangedPiece> arrangedPiecesCopy = arrangedPieces;
		mutate(arrangedPiecesCopy, mutationCount);
		double rating = calculateRating(arrangedPieces);
		if (rating > lastRating)
		{
			arrangedPieces = std::move(arrangedPiecesCopy);
			failedNumber = 0;
		}
		else
		{
			if (++failedNumber == MAX_FAILED_NUMBER)
			{
				if ((mutationCount /= 2) == 0)
					break;
				failedNumber = 0;
			}
		}
	}
	return arrangedPieces;
}
