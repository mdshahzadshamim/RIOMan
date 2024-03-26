#pragma once
#include <pqxx/pqxx>
#include <iostream>
using namespace std;
void clearInputBuffer();
float changePrecision(float, int);
class Functions
{
public:
	void inMain();
	void printFunctions();
	void printEditOptions();

	void status(pqxx::connection& conn);
	void variableQuery(pqxx::connection& conn);


	void ListIngredient(pqxx::connection& conn);
	void Purchase(pqxx::connection& conn);

	void ListDishes(pqxx::connection& conn);
	void ListAvailableDishes(pqxx::connection& conn);
	bool CheckDish(pqxx::connection& conn, const string& dish_name, int count = 1);
	void Sale(pqxx::connection& conn);


	void CreateIngredient(pqxx::connection& conn);
	void DeleteIngredient(pqxx::connection& conn);
	void CreateDish(pqxx::connection& conn);
	void DeleteDish(pqxx::connection& conn);
	void DishIngredientConnector(pqxx::connection& conn);
	void RemoveDishIngredientConnection(pqxx::connection& conn);
	void DishPriceChange(pqxx::connection& conn);
};