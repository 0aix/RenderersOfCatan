#pragma once
#include "abstractrule.h"
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
			NoSpecialChitsTouchingRule(std::map<std::string, bool> &boolMap);

			bool IsFollowed();
		};
	}
}