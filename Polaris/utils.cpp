#include "utils.h"

namespace polaris
{
	void Utils::OpenTwitter()
	{
		std::string twitterUrl = "https://twitter.com/ProjectPolaris_";
		std::cin >> twitterUrl;

		system(std::string("start " + twitterUrl).c_str());
	}
}