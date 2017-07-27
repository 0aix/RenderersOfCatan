#pragma once
#include "../rapidjson/Document.h"

namespace Catan {
	namespace Generate {
		class AbstractRule {
			bool required;

		protected:
			AbstractRule(rapidjson::Document &document);

		public:
			virtual bool IsFollowed(/* TODO */) = 0;
		};
	}
}