#pragma once
#include "json.hpp"

enum BaseCurrency {
	USD,
	EUR,
	BTC
};

struct Currency {
	std::string name;
	double price[3];
	double changePercentInDay;
};
typedef struct Currency Currency;

/*!
* @brief PriceQuery object for querying information for each cryptocurrency
*/
class PriceQuery {
public:
	/*!
	* @brief Queries information for a single cryptocurrency
	* @param[out] currency The information for the queried currency
	* @param[in] name Name of the cryptocurrency
	*/
	bool queryCurrency(Currency &currency, std::string name);
private:

};