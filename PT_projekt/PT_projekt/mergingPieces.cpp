#include "mergingPieces.hpp"

void rotate_90n(cv::Mat &piece, Rotation angle)
{
	switch (angle)
	{
		case Rotation::R0:
			break;
		case Rotation::R90:
		{
			cv::transpose(piece, piece);
			cv::flip(piece, piece, 0);
			break;
		}
	
		case Rotation::R180:
		{
			cv::flip(piece, piece, -1);
			break;
		}

		case Rotation::R270:
		{
			cv::transpose(piece, piece);
			cv::flip(piece, piece, 1);
			break;
		}

	}
}

void rotateArrangedPieces(arrangedPieces_t &arrangedPieces)
{
	for (size_t i = 0; i < arrangedPieces.size(); i++)
	{
		rotate_90n(arrangedPieces[i].piece, arrangedPieces[i].rotation);
	}
}

int searchMinX(arrangedPieces_t &arrangedPieces)
{
	int minX = std::numeric_limits<int>::max();

	for (size_t i = 0; i < arrangedPieces.size(); i++)
	{
		if (arrangedPieces[i].position.first < minX)
			minX = arrangedPieces[i].position.first;
	}

	return minX;
}

int searchMinY(arrangedPieces_t &arrangedPieces)
{

	int minY = std::numeric_limits<int>::max();

	for (size_t i = 0; i < arrangedPieces.size(); i++)
	{
		if (arrangedPieces[i].position.second < minY)
			minY = arrangedPieces[i].position.second;
	}

	return minY;
}

int CountCols(arrangedPieces_t &arrangedPieces)
{
	int maxX = std::numeric_limits<int>::min();

	for (size_t i = 0; i < arrangedPieces.size(); i++)
	{

		if (arrangedPieces[i].position.first > maxX)
			maxX = arrangedPieces[i].position.first;
	}

	return maxX + 1;
}

int CountRows(arrangedPieces_t &arrangedPieces)
{
	int maxY = std::numeric_limits<int>::min();

	for (size_t i = 0; i < arrangedPieces.size(); i++)
	{
		if (arrangedPieces[i].position.second > maxY)
			maxY = arrangedPieces[i].position.second;
	}
	return maxY + 1;
}

void substractionMinXAndMinY(arrangedPieces_t &arrangedPieces, int minX, int minY)
{
	for (size_t i = 0; i < arrangedPieces.size(); i++)
	{
		arrangedPieces[i].position.first -= minX;
		arrangedPieces[i].position.second -= minY;
	}
}

cv::Mat mergePieces(arrangedPieces_t &arrangedPieces)
{
	// obracanie obrazka o zadany k¹t
	rotateArrangedPieces(arrangedPieces);

	// szukanie min X i min Y
	int minX = searchMinX(arrangedPieces);
	int minY = searchMinY(arrangedPieces);

	// odejmowanie wartoœci wspó³rzêdnych arrangedPices z minX oraz minY
	substractionMinXAndMinY(arrangedPieces,minX,minY);

	// obliczyæ wartoœci ka¿dej kolumny i ka¿dego wiersza

	std::vector<int> sizeEveryCols;
	std::vector<int> sizeEveryRows;

	// maksymalne wartoœci kolumn oraz wierszy

	const int ColsCount = CountCols(arrangedPieces);
	const int RowsCount = CountRows(arrangedPieces);

	sizeEveryCols.resize(ColsCount, 0);
	sizeEveryRows.resize(RowsCount, 0);

	for (const ArrangedPiece &i : arrangedPieces)
	{
		const int columnNumber = i.position.first; // kolumna 
		const int width = i.piece.cols;

		const int rowsNumber = i.position.second;
		const int height = i.piece.rows;

		sizeEveryCols[columnNumber] = std::max(width, sizeEveryCols[columnNumber]);
		sizeEveryRows[rowsNumber] = std::max(height, sizeEveryRows[rowsNumber]);
	}

	std::cout << "maxX: " << ColsCount << "\tmaxY: " << RowsCount << std::endl;
	
	sizeEveryCols.insert(sizeEveryCols.begin(), 0);
	for (size_t i = 1; i < sizeEveryCols.size(); i++)
		sizeEveryCols[i] += sizeEveryCols[i - 1];

	sizeEveryRows.insert(sizeEveryRows.begin(), 0);
	for (size_t i = 1; i < sizeEveryRows.size(); i++)
		sizeEveryRows[i] += sizeEveryRows[i - 1];

	cv::Mat matrixOutput(sizeEveryRows.back(), sizeEveryCols.back(), arrangedPieces[0].piece.type(), cvScalar(255, 255, 255));
	
	for (size_t i = 0; i < arrangedPieces.size(); i++)
	{
		
		const cv::Rect rectangle(sizeEveryCols[arrangedPieces[i].position.first], sizeEveryRows[arrangedPieces[i].position.second], arrangedPieces[i].piece.cols, arrangedPieces[i].piece.rows);
		arrangedPieces[i].piece.copyTo(matrixOutput(rectangle));
	}

	return matrixOutput;
}
