#pragma once
#include "abstractrule.h"
#include "boardgraph.h"
#include <string>
#include <map>

/*
Rule Spec:
{
	"name" : "NoSpecialChitsTouching",
	"follow_strictly" : true|false (required)
}
*/

namespace Catan {
	namespace Generate {
		class NoSpecialChitsTouchingRule : public AbstractRule {
		public:
			NoSpecialChitsTouchingRule(std::map<std::string, bool> &boolMap) : AbstractRule(boolMap){}

			bool IsFollowed(BoardGraph &graph);
		};
	}
}