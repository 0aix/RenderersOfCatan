#include "include/islandcountrule.h"
#include "include/boardgraph.h"
#include <map>
#include <string>
#include <vector>

using namespace std;

namespace Catan {
	namespace Generate {
		IslandCountRule::IslandCountRule(map<string, int> &intMap, map<string, bool> &boolMap) : AbstractRule(boolMap) {
			// Populate min & max
			auto it = intMap.find("min");
			if (it != intMap.end()) {
				min = it->second;
			}

			it = intMap.find("max");

			if (it != intMap.end()) {
				max = it->second;
			}

			it = intMap.find("minSize");

			if (it != intMap.end()) {
				minSize = it->second;
			}
		}

		int IslandCountRule::IslandSize(BoardNode *node) {
			node->marked = true;
			if (node->type == WATER) {
				return 0;
			}

			int sum = 1;
			for (BoardNode *neighbour : node->NonNullNeighbours()) {
				if (!neighbour->marked) {
					sum += IslandSize(neighbour);
				}
			}

			return sum;
		}

		bool IslandCountRule::IsFollowed(BoardGraph &graph) {
			auto NotWater = [](BoardNode *node) {
				return node->type != WATER; 
			};

			vector<BoardNode*> islandHeads = graph.GetIslands(NotWater);	
			if (islandHeads.size() < min || islandHeads.size() > max) {
				return false;
			}		

			for (BoardNode *node : islandHeads) {
				if (IslandSize(node) < minSize) {
					return false;
				}
			}

			graph.UnMarkAll();
			return true;
		}
	}
}