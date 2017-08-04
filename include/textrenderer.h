#pragma once

#include "boardgraph.h"
#include "boardnode.h"
#include <iostream>
#include <string>

using namespace std;
using namespace Catan::Generate;

namespace Catan {
	namespace Draw {
		const int TEXT_HEX_WIDTH = 10;
		const int TEXT_HEX_HEIGHT = 6;

		void PopulateLineString(char **buffer, int rowIndex, int colIndex, string str) {
			for (int i = 0; str.size(); i++) {
				char c = str.at(i);
				buffer[rowIndex][colIndex++] = c;
			}
		}

		void PopulateWithHex(char **buffer, int rowIndex, int colIndex, BoardNode *node) {
			PopulateLineString(buffer, rowIndex, colIndex, "   _____  ");
		}

		void RenderAsText(BoardGraph &graph) {
			// Get Height and width requirements
			const int BUFFER_HEIGHT = TEXT_HEX_HEIGHT * graph.BoardHeight();
			const int BUFFER_WIDTH = TEXT_HEX_WIDTH * graph.BoardHeight();

			char **buffer = new char*[BUFFER_HEIGHT];
			for (int i = 0; i < BUFFER_HEIGHT; i++) {
				buffer[i] = new char[BUFFER_WIDTH];
			}

			BoardGraphForwardIterator it = graph.ForwardIterator();

			for (int col = 0; col < BUFFER_WIDTH; col += TEXT_HEX_WIDTH) {
				int rowLower = (BUFFER_HEIGHT - graph.ColumnHeight(col) * TEXT_HEX_HEIGHT) / 2;
				int rowUpper = rowLower + graph.ColumnHeight(col) * TEXT_HEX_HEIGHT;
				
				for (; rowLower < rowUpper; rowLower += TEXT_HEX_HEIGHT) {
					PopulateWithHex(buffer, rowLower, col, it.Next());
				}
			}

			for (int i = 0; i < BUFFER_HEIGHT; i++) {
				delete [] buffer[i];
			}
			delete [] buffer;
		}
	}
}