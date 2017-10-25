#include "stdafx.h"

/*!
* @brief Initializes the Wallet object with PriceQuery
* @param[in] query PriceQuery object for querying information about each cryptocurrency that the user owns
* @param[in] file Path to the Wallet file
*/
Wallet::Wallet(PriceQuery &query, std::string file) {
	this->query = query;
	this->walletFile = file;
}

/*!
* @brief Cleans up the Wallet objects allocated memory
*/
Wallet::~Wallet() {
	clearWallet();
}

/*!
* @brief Cleans up the allocated CurrencyWallet objects
*/
void Wallet::clearWallet() {
	for (auto currency : this->currenciesOwned) {
		delete currency;
	}
	this->currenciesOwned.clear();
}

/*!
* @brief Calculates the total value of the users investments and updates them inside the Wallet object
*/
void Wallet::calculateWalletValue() {
	this->walletValue = 0;
	for (auto currency : this->currenciesOwned) {
		this->walletValue += currency->amountOwned * currency->currency.price[this->baseCurrency];
	}
	this->percent = ((this->walletValue - this->walletBaseValue) / this->walletBaseValue) * 100;
}

/*!
* @brief Loads the users investment information from the wallet file
* @param[in] file Path to the wallet file
*/
bool Wallet::loadWallet() {
	clearWallet();
	std::string line;
	std::ifstream myfile(this->walletFile);
	if (myfile.is_open())
	{
		if (!getline(myfile, line) || line.size() < 2) {
			myfile.close();
			return false;
		}
		char currencySign = line[line.size() - 1];
		if (currencySign == '$')
			baseCurrency = USD;
		else if (currencySign == 'e')
			baseCurrency = EUR;
		else if (currencySign == 'B')
			baseCurrency = BTC;
		else {
			std::cout << "Unknow base currency: " << currencySign << std::endl;
			myfile.close();
			return false;
		}
		this->walletBaseValue = std::stod(line.substr(0, line.length() - 1));
		while (getline(myfile, line))
		{
			if (line.empty())
				continue;
			int index = line.find(" ");
			if (index == -1 || index == 0 || index == line.length() - 1) {
				myfile.close();
				return false;
			}
			CurrencyWallet *currency = new CurrencyWallet;
			std::string currencyName = line.substr(0, index);
			double amountOwned = std::stod(line.substr(index + 1));
			currency->amountOwned = amountOwned;
			if (!query.queryCurrency(currency->currency, currencyName)) {
				std::cout << "Failed to resolve currency: " << currencyName << std::endl;
				myfile.close();
				return false;
			}
			currenciesOwned.push_back(currency);
		}
		myfile.close();
		calculateWalletValue();
		return true;
	}
	else
		return false;
}

/*!
* @brief Setup for the user to create a wallet containing the cryptocurrencies he owns and how much he invested in them
*/
bool Wallet::createWallet() {
	std::cout << "Input wallet file name (empty for wallet.txt): ";
	std::string file, baseValueStr;
	char baseCurrency;
	double baseValue;
	std::getline(std::cin, file);
	if (file.empty())
		file = "wallet.txt";
	std::cout << "Input the value originally invested in cryptocurrencies(e = EURO, $ = dollar, B = Bitcoin: ";
	try {
		std::getline(std::cin, baseValueStr);
		if (baseValueStr.length() < 2)
			return false;
		baseCurrency = baseValueStr[baseValueStr.length() - 1];
		if (baseCurrency != 'e' && baseCurrency != '$' && baseCurrency != 'B')
			return false;
		baseValueStr = baseValueStr.substr(0, baseValueStr.length() - 1);
		std::replace(baseValueStr.begin(), baseValueStr.end(), ',', '.');
		baseValue = std::stod(baseValueStr);
	}
	catch (const std::invalid_argument& ia) {
		std::cerr << "Invalid value: " << ia.what() << '\n';
		return false;
	}
	std::cout << "Input currency and the amount you own. (e.g. Bitcoin 0.555) Empty line will stop: ";
	std::string line;
	std::vector<std::string> currencies;
	while (true) {
		std::getline(std::cin, line);
		if (line.empty())
			break;
		std::replace(baseValueStr.begin(), baseValueStr.end(), ',', '.');
		int index = line.find(" ");
		if (index == -1 || index == 0 || index == line.length() - 1) {
			std::cout << "Invalid input! (input format: AMOUNT CURRENCY)" << std::endl;
			continue;
		}
		std::string currency = line.substr(0, index);
		try {
			std::string amountStr = line.substr(index + 1);
			double amount = std::stod(amountStr);
			currencies.push_back(line);
			std::cout << "Added " << currency << ":" << amount << std::endl;
		}
		catch (const std::invalid_argument& ia) {
			std::cerr << "Invalid value: " << ia.what() << '\n';
		}
	}
	// Write out the currencies
	std::ofstream output(file);
	if (output.is_open())
	{
		output << baseValue << baseCurrency << "\n";
		for (std::string currency : currencies) {
			output << currency << "\n";
		}
		output.close();
	}
	else {
		std::cout << "Failed to open file for writing: \"" << file << "\"" << std::endl;
		return false;
	}
	return true;
}

/*!
* @brief Formats the wallet object for printing
*/
std::ostream& operator<<(std::ostream& os, const Wallet& wallet)
{
	if (wallet.currenciesOwned.size() == 0) {
		os << "Wallet is empty";
		return os;
	}
	for (auto currencyWallet : wallet.currenciesOwned) {
		os << "Name: " << currencyWallet->currency.name << "\nBTC price: " << currencyWallet->currency.price[BTC]
			<< "\nUSD price: " << currencyWallet->currency.price[USD] << "\nEUR price: " << currencyWallet->currency.price[EUR]
			<< "\nChange in 24h: " << currencyWallet->currency.changePercentInDay << "%\n------------\n";
	}
	os << "Total base value: " << wallet.walletBaseValue << "\nTotal current value: " << wallet.walletValue << "\nTotal change: " << wallet.walletValue - wallet.walletBaseValue << "\nTotal change percent: " << wallet.percent << "%\n";
	return os;
}