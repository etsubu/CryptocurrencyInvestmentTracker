#include "stdafx.h"

using json = nlohmann::json;

/*!
* @brief Downloads a whole html page to memory buffer
* @param[in] url URL of the html page to download
* @param[in] webpage string that will contain the downloaded webpage
*/
bool DownloadFile(std::string url, std::string &webpage) {
	char buf[4096];
	DWORD read;
	HINTERNET hRequest = InternetOpenA(NULL, INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
	HINTERNET hDownload = InternetOpenUrlA(hRequest, url.c_str(), NULL, 0, INTERNET_FLAG_NO_UI | INTERNET_FLAG_IGNORE_REDIRECT_TO_HTTPS | INTERNET_FLAG_NO_CACHE_WRITE, NULL);
	if (hDownload) {
		while (InternetReadFile(hDownload, buf, sizeof(buf), &read) && read > 0) {
			webpage.append(buf, read);
		}
	}
	else
		return false;
	InternetCloseHandle(hDownload);
	InternetCloseHandle(hRequest);
	return true;
}

/*!
* @brief Queries information for a single cryptocurrency
* @param[out] currency The information for the queried currency
* @param[in] name Name of the cryptocurrency
*/
bool PriceQuery::queryCurrency(Currency &currency, std::string name) {
	std::string url = "https://api.coinmarketcap.com/v1/ticker/" + name + "/?convert=EUR";
	std::string webpage;
	int pageSize = 0;
	if (!DownloadFile(url, webpage)) {
		return false;
	}
	int index = webpage.find("{");
	if (index != -1) {
		webpage = webpage.substr(webpage.find("{"));
		webpage = webpage.substr(0, webpage.find("}") + 1);
	}
	auto j3 = json::parse(webpage);
	if (j3.count("error")) {
		std::cout << "Unknown currency " << name << std::endl;
		return false;
	}
	currency.name = j3.at("name").get<std::string>();
	currency.changePercentInDay = std::stod(j3.at("percent_change_24h").get<std::string>());
	currency.price[BTC] = std::stod(j3.at("price_btc").get<std::string>());//j3.at("price_btc").get<double>();
	currency.price[EUR] = std::stod(j3.at("price_eur").get<std::string>());//j3.at("price_eur").get<double>();
	currency.price[USD] = std::stod(j3.at("price_usd").get<std::string>());//j3.at("price_usd").get<double>();
	return true;
}