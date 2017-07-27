#include "include/abstractrule.h"
#include "rapidjson/Document.h"

using namespace rapidjson;

namespace Catan {
	namespace Generate {
		AbstractRule::AbstractRule(Document &document) {
			// look for "follow_strictly"
			required = document["follow_strictly"].GetBool();
		}
	}
}