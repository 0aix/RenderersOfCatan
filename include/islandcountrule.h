#pragma once
#include "abstractrule.h"
#include "boardgraph.h"
#include <map>
#include <string>

/*
 * Rule spec:
{
  "name" : "IslandCount",
  "min" : <positive int>, (optional)
  "max" : <positive int>, (optional)
  "follow_strictly" : true|false (required)
}
*/

namespace Catan {
	namespace Generate {
		class IslandCountRule : public AbstractRule {
		private:
			int min = 1;
      int max = 2147483647;
      int minSize = 1;

      int IslandSize(BoardNode *node);
		public:
			IslandCountRule(std::map<std::string, int> &intMap, std::map<std::string, bool> &boolMap);

			bool IsFollowed(BoardGraph &graph);
		};
	}
}