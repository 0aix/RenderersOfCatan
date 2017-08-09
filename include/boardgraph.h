#pragma once
#include <vector>
#include "boardnode.h"

namespace Catan {
	namespace Generate {
		class Config;
		class BoardGraphForwardIterator;
		class BoardGraph {
		public:
			BoardGraph(Config *config);
			BoardGraphForwardIterator &ForwardIterator();
			// Generates a new board once, without checking any rules
			void Randomize();
			// Generates a new board by obeying rules 
			void RandomizeWithRules();
			void RandomizePorts();
			bool FollowsAllRules();
			int BoardHeight();
			int BoardWidth();
			int ColumnHeight(int index);
			std::vector<BoardNode*> GetIslands(bool (*test)(BoardNode*));

		private:
			std::vector<BoardNode*> firstNodes;
			Config *config;
      int boardHeight;

			BoardNode *GenerateRow(int size);
			void ColumnLink(int leftSize, BoardNode *firstLeft, int rightSize, BoardNode *firstRight);
			void UnMarkAll();
			void MarkAllIslandNodesFromSource(BoardNode *source, bool (*test)(BoardNode*));
			void ClearPorts();
			int ShoreLineSize(ShoreEdge *edge);
			ShoreEdge *RemoveShoreLineEdge(ShoreEdge *current);
			std::pair<ShoreEdge*, int> RemoveShoreLineEdge(ShoreEdge *edge, int rand, Port *port);
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