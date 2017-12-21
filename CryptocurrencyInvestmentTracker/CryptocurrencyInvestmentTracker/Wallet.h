#pragma once

typedef struct {
	Currency currency;
	double amountOwned;
} CurrencyWallet;

/*!
* @brief Wallet object that contains information about the each cryptocurrency the user owns
*/
class Wallet {
public:
	/*!
	* @param[in] query PriceQuery object for querying the information for each cryptocurrency
	* @param[in] file Path to the wallet file
	*/
	Wallet(PriceQuery &query, std::string file);
	~Wallet();

	/*!
	* @brief Loads the wallet file from the disk, queries the current value for each cryptocurrencies and calculates the current value of the wallet
	*/
	bool loadWallet();

	/*!
	* @brief Setup for the user to create a wallet containing the cryptocurrencies he owns and how much he invested in them
	*/
	bool Wallet::createWallet();
	friend std::ostream& operator<<(std::ostream& os, const Wallet& wallet);
private:
	void clearWallet();
	void calculateWalletValue();
	std::vector<CurrencyWallet*> currenciesOwned;
	double walletValue;
	double walletValueInBTC;
	double walletBaseValue;
	double percent;
	std::string walletFile;
	PriceQuery query;
	BaseCurrency baseCurrency;
	char baseCurrencySign;
};
