#pragma once
#include "abstractrule.h"
#include "boardgraph.h"
#include <map>
#include <string>

/*
Rule Spec:
{
	"name" : "NoPonds",
	"follow_strictly" : true|false (required)
}
*/

namespace Catan {
	namespace Generate {
		class NoPondsRule : public AbstractRule {
		public:
			NoPondsRule(std::map<std::string, bool> &boolMap) : AbstractRule(boolMap) {};

			bool IsFollowed(BoardGraph &graph);
		};
	}
}