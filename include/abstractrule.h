#pragma once
#include "../rapidjson/Document.h"
#include <map>
#include <string>

namespace Catan {
	namespace Generate {
		class AbstractRule {
			bool followStrictly;

		protected:
			AbstractRule(std::map<std::string, bool> boolMap);

		public:
			virtual bool IsFollowed(/* TODO */) = 0;
		};

		class RuleParseException {};
	}
}