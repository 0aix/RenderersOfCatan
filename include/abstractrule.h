#pragma once
#include "../rapidjson/Document.h"
#include <map>
#include <string>

namespace Catan {
	namespace Generate {
		class BoardGraph;
		class AbstractRule {
			bool followStrictly;

		protected:
			AbstractRule(std::map<std::string, bool> &boolMap);

		public:
			virtual bool IsFollowed(BoardGraph &graph) = 0;
		};

		class RuleParseException {};
	}
}