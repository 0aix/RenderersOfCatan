#pragma once
#include <vector>
#include "boardnode.h"
#include "config.h"

namespace Catan {
	namespace Generate {
		class BoardGraphForwardIterator;
		class BoardGraph {
		public:
			BoardGraph(Config *config);
			BoardGraphForwardIterator &ForwardIterator();
			void Randomize();

		private:
			std::vector<BoardNode*> firstNodes;
			Config *config;

			BoardNode *GenerateRow(int size);
			void ColumnLink(int leftSize, BoardNode *firstLeft, int rightSize, BoardNode *firstRight);
		};

		// Used to traverse a BoardGraph, column by column
		class BoardGraphForwardIterator {
		public:
			BoardGraphForwardIterator(std::vector<BoardNode*> &firstNodes);
			bool HasNext();
			BoardNode *Next();
			
		private:
			bool first;
			int columnIndex;
			BoardNode *current;
			std::vector<BoardNode*> &firstNodes;
		};
	}
}