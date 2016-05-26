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

}//

std::vector<ArrangedPiece> arrangePieces(const std::list<cv::Mat> &pieces)
{
	std::vector<ArrangedPiece> arrangedPieces = createFirstSpecimen(pieces);
	//TODO
	return arrangedPieces;
}
