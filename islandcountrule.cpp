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
		}

		bool IslandCountRule::IsFollowed(BoardGraph &graph) {
			auto NotWater = [](BoardNode *node) {
				return node->type != WATER; 
			};

			vector<BoardNode*> islandHeads = graph.GetIslands(NotWater);			
			return islandHeads.size() >= min && islandHeads.size() <= max;
		}
	}
}