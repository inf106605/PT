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
	std::cout << "Merging pieces into a single output image..." << std::endl;

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

	std::vector<int> widthEveryCols;
	std::vector<int> heightEveryRows;

	// maksymalne wartoœci kolumn oraz wierszy

	const int ColsCount = CountCols(arrangedPieces);
	const int RowsCount = CountRows(arrangedPieces);

	sizeEveryCols.resize(ColsCount, 0);
	sizeEveryRows.resize(RowsCount, 0);

	for (const ArrangedPiece &i : arrangedPieces)
	{
		const int columnNumber = i.position.first; // kolumna 
		const int width = i.piece.cols; // wiersz

		const int rowsNumber = i.position.second;
		const int height = i.piece.rows;

		sizeEveryCols[columnNumber] = std::max(width, sizeEveryCols[columnNumber]);
		sizeEveryRows[rowsNumber] = std::max(height, sizeEveryRows[rowsNumber]);
	}
	


	sizeEveryCols.insert(sizeEveryCols.begin(), 0);
	for (size_t i = 1; i < sizeEveryCols.size(); i++)
	{
		widthEveryCols.push_back(sizeEveryCols[i]);
		sizeEveryCols[i] += sizeEveryCols[i - 1];
	}
	sizeEveryRows.insert(sizeEveryRows.begin(), 0);
	for (size_t i = 1; i < sizeEveryRows.size(); i++)
	{
		heightEveryRows.push_back(sizeEveryRows[i]);
		sizeEveryRows[i] += sizeEveryRows[i - 1];
	}

	cv::Mat matrixOutput(sizeEveryRows.back(), sizeEveryCols.back(), arrangedPieces[0].piece.type(), cvScalar(255, 255, 255));
	
	for (size_t i = 0; i < arrangedPieces.size(); i++)
	{
		const int columnNumber = arrangedPieces[i].position.first;
		const int rowsNumber = arrangedPieces[i].position.second;

		const int columnWidth = widthEveryCols[columnNumber];
		const int columnHeight = heightEveryRows[rowsNumber];

		const cv::Rect rectangle(sizeEveryCols[columnNumber], sizeEveryRows[rowsNumber], columnWidth, columnHeight);
		
		cv::Size size(columnWidth, columnHeight);
		cv::Mat resizedPiece(columnWidth, columnHeight, arrangedPieces[0].piece.type(), cvScalar(255, 255, 255));
		cv::resize(arrangedPieces[i].piece, resizedPiece, size);
		/*
		std::cout << "Rectangle-> Height: " << rectangle.height << " Width: " << rectangle.width << " x=" << rectangle.x << " y=" << rectangle.y << std::endl;
		std::cout << "Matrix-> Height: " << matrixOutput.rows << " Width: " << matrixOutput.cols << std::endl;
		std::cout << "ResizedPiece-> Height: " << resizedPiece.rows << " Width: " << resizedPiece.cols << std::endl;
		*/
		resizedPiece.copyTo(matrixOutput(rectangle));
	}

	return matrixOutput;
}
