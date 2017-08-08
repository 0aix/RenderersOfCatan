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
			cout << "Took " << i << " tries" << endl;
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