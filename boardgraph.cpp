#include "include/boardgraph.h"
#include "include/config.h"
#include "include/boardnode.h"
#include "include/abstractrule.h"
#include <algorithm>
#include <cstdlib>
#include <vector>
#include <iostream>
#include <assert.h>
#include <cstdlib>
#include <utility>

using namespace std;

namespace Catan {
	namespace Generate {
		BoardGraph::BoardGraph(Config *config) : config(config) {
			assert(config != NULL);

			vector<int> &boardSpec = config->boardColumns;

			// Link nodes in individual columns together, and add them to the first node list
			for (int i : boardSpec) {
				firstNodes.push_back(GenerateRow(i));
			}

			// Link columns together
			for (int i = 1; i < firstNodes.size(); i++) {
				ColumnLink(boardSpec[i - 1], firstNodes[i - 1], boardSpec[i], firstNodes[i]);
			}

      boardHeight = 0;
      for (int col : config->boardColumns) {
        boardHeight = max(boardHeight, col);
      }
		}

		BoardNode *BoardGraph::GenerateRow(int size) {
			assert(size >= 1);
			BoardNode *first = new BoardNode();
			BoardNode *current = first;

			for (int i = 1; i < size; i++) {
				BoardNode *next = new BoardNode();
				current->neighbours[S_INDEX] = next;
				next->neighbours[N_INDEX] = current;
				current = next;
			}

			return first;
		}

		void BoardGraph::ColumnLink(int leftSize, BoardNode *firstLeft, int rightSize, BoardNode *firstRight) {
			// Get the position in the two rows where they each meet
    		// Each pair of rows should have a length difference that is odd (Enforced by ConfigFile)
			int positionLeft = max(0, (leftSize - rightSize) / 2);
			int positionRight = max(0, (rightSize - leftSize) / 2);

			BoardNode *leftCurrent = firstLeft;
			BoardNode *rightCurrent = firstRight;

			for (; positionLeft > 0; positionLeft--) {
				leftCurrent = leftCurrent->neighbours[S_INDEX];
			}

			for (; positionRight > 0; positionRight--) {
				rightCurrent = rightCurrent->neighbours[S_INDEX];
			}

			// Start stitching the nodes together
			while (leftCurrent != NULL && rightCurrent != NULL) {
				if (leftSize < rightSize) {
					leftCurrent->neighbours[NE_INDEX] = rightCurrent;
					rightCurrent->neighbours[SW_INDEX] = leftCurrent;
					leftCurrent->neighbours[SE_INDEX] = rightCurrent->neighbours[S_INDEX];
					rightCurrent->neighbours[S_INDEX]->neighbours[NW_INDEX] = leftCurrent;
				} else {
					rightCurrent->neighbours[NW_INDEX] = leftCurrent;
					leftCurrent->neighbours[SE_INDEX] = rightCurrent;
					rightCurrent->neighbours[SW_INDEX] = leftCurrent->neighbours[S_INDEX];
					leftCurrent->neighbours[S_INDEX]->neighbours[NE_INDEX] = rightCurrent;
				}
				leftCurrent = leftCurrent->neighbours[S_INDEX];
				rightCurrent = rightCurrent->neighbours[S_INDEX];
			}
		}

		BoardGraphForwardIterator &BoardGraph::ForwardIterator() {
			return *(new BoardGraphForwardIterator(firstNodes));
		}

		bool BoardGraph::FollowsAllRules() {
			for (AbstractRule *rule : config->rules) {
				if (rule->FollowStrictly() && !rule->IsFollowed(*this)) {
					return false;
				}
			}
			return true;
		}

		void BoardGraph::RandomizeWithRules() {
			int i = 0;
			do {
				i++;
				Randomize();
			} while (!FollowsAllRules());
			ClearPorts();
			RandomizePorts();
		}

		int BoardGraph::ShoreLineSize(ShoreEdge *edge) {
			ShoreEdge *first = edge;
			ShoreEdge *current = first->next;
			int size = 1;

			while (current != first) {
				size++;
				assert(current != NULL);
				current = current->next;
			}

			return size;
		}

		ShoreEdge *BoardGraph::RemoveShoreLineEdge(ShoreEdge *current) {
			if (current == current->next) {
				current->prev = NULL;
				current->next = NULL;
				return NULL;
			}

			current->prev->next = current->next;
			current->prev->originalRight = false;
			current->next->prev = current->prev;
			current->next->originalLeft = false;

			ShoreEdge *nextHead = current->next;
			
			return nextHead;
		}

		pair<ShoreEdge*, int> BoardGraph::RemoveShoreLineEdge(ShoreEdge *edge, int rand, Port *port) {
			ShoreEdge *current = edge;

			for (int i = 0; i < rand; i++) {
				current = current->next;
			}

			int removed = 1;

			// Need to remove current			
			BoardNode *chosenNode = current->attachedNode;
			chosenNode->AddPort(current->index, port);

			ShoreEdge *nextHead = RemoveShoreLineEdge(current);

			if (current->originalLeft) {
				// Remove left
				nextHead = RemoveShoreLineEdge(current->prev);
				removed++;
			}

			if (current->originalRight) {
				nextHead = RemoveShoreLineEdge(current->next);
				removed++;
			}

			return make_pair(nextHead, removed);
		}

		void BoardGraph::RandomizePorts() {
			vector<Port*> ports = config->ports;

			auto RandomIterator = [](int i) {
          return rand() % i;
      };

      auto NotWater = [](BoardNode *node) {
      	return node->type != WATER;
      };

			random_shuffle(ports.begin(), ports.end(), RandomIterator);

			// Generate shorelist
			vector<BoardNode*> islandHeaders = GetIslands(NotWater);
			vector<ShoreEdge*> lines;
			int *weightArray = new int[islandHeaders.size()];
			int prev = 0;
			int total = 0;

			for (int i = 0; i < islandHeaders.size(); i++) {
				ShoreEdge *edge = islandHeaders[i]->GenerateShoreLine();
				lines.push_back(edge);
				int size = ShoreLineSize(edge);
				weightArray[i] = prev + size;
				total += size;
				prev = weightArray[i];
			}

			// Place the ports somewhere
			int coastLinesLeft = islandHeaders.size();
			int portPosition = 0;

			while (coastLinesLeft > 0 && portPosition < ports.size()) {
				int weight = rand() % total + 1;
				int shoreLineIndex = -1;
				for (int i = 0; i < islandHeaders.size(); i++) {
					if (weight <= weightArray[i]) {
						shoreLineIndex = i;
						break;
					}
				}

				assert(shoreLineIndex != -1);
				ShoreEdge *chosenLine = lines[shoreLineIndex];
				pair<ShoreEdge*, int> result = RemoveShoreLineEdge(chosenLine, weightArray[shoreLineIndex] - weight, ports[portPosition++]);

				lines[shoreLineIndex] = result.first;
				if (result.first == NULL) {
					coastLinesLeft--;
				}

				weightArray[shoreLineIndex] -= result.second;
				total -= result.second;
			}
			
			delete [] weightArray;
		}

		void BoardGraph::ClearPorts() {
			auto it = BoardGraphForwardIterator(firstNodes);

			while (it.HasNext()) {
				it.Next()->ClearPorts();
			}
		}

		void BoardGraph::Randomize() {
			BoardGraphForwardIterator it = BoardGraphForwardIterator(firstNodes);
			int tilePos = 0;
			int chitPos = 0;

			// vector to hold chits of each individual tiles and chits
			vector<TileType> typesList;
			vector<int> chitList;


			for (int type = FIELD; type != NONE; type++) {
				TileType t = static_cast<TileType>(type);
				typesList.insert(typesList.end(), config->TileCountFromType(t), t);
			}

			for (int i = 0; i < config->chits.size(); i++) {
				if (config->chits[i] > 0) {
					chitList.insert(chitList.end(), config->chits[i], i + 1);
				}
			}

      auto RandomIterator = [](int i) {
          return rand() % i;
      };

			random_shuffle(typesList.begin(), typesList.end(), RandomIterator);
			random_shuffle(chitList.begin(), chitList.end(), RandomIterator);

			while (it.HasNext()) {
				BoardNode *node = it.Next();
				node->type = typesList[tilePos++];
				
				if (node->CanPlaceChit()) {
					node->chit = chitList[chitPos++];
				}
			}
		}

		vector<BoardNode*> BoardGraph::GetIslands(bool (*test)(BoardNode*)) {
			vector<BoardNode*> islandHeaders;

			auto it = ForwardIterator();
			while (it.HasNext()) {
				BoardNode *node = it.Next();

				if (!node->marked) {
					node->marked = true;
					if ((*test)(node)) {
						islandHeaders.push_back(node);
						MarkAllIslandNodesFromSource(node, test);
					}
				}
			}

			UnMarkAll();
			return islandHeaders;
		}

		// Assumes source has already been marked
		// DFS
		void BoardGraph::MarkAllIslandNodesFromSource(BoardNode *source, bool (*test)(BoardNode*)) {
			for (BoardNode *neighbour : source->NonNullNeighbours()) {
				if (!neighbour->marked) {
					neighbour->marked = true;
					if ((*test)(neighbour)) {
						MarkAllIslandNodesFromSource(neighbour, test);
					}
				}
			}
		}

		int BoardGraph::BoardHeight() {
			return boardHeight;
		}

		int BoardGraph::BoardWidth() {
			return firstNodes.size();
		}

		int BoardGraph::ColumnHeight(int index) {
			return config->boardColumns[index];
		}

		void BoardGraph::UnMarkAll() {
			BoardGraphForwardIterator it = BoardGraphForwardIterator(firstNodes);
			while (it.HasNext()) {
				it.Next()->marked = false;
			}
		}

		BoardGraphForwardIterator::BoardGraphForwardIterator(vector<BoardNode*> &firstNodes) : firstNodes(firstNodes) {
			columnIndex = 0;
			current = firstNodes[0];
			first = true;
		}

		bool BoardGraphForwardIterator::HasNext() {
			return current != NULL && (current->neighbours[S_INDEX] != NULL || columnIndex < firstNodes.size() - 1);
		}

		BoardNode *BoardGraphForwardIterator::Next() {
			assert(current != NULL);
			if (first) {
				first = false;
				return current;
			}

			current = current->neighbours[S_INDEX];

			if (current == NULL) {
				// Goto the next column
				columnIndex++;
				if (columnIndex < firstNodes.size()) {
					current = firstNodes[columnIndex];
				}
			}

			return current;
		}
	}
}