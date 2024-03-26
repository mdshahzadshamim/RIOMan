#include <string>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <pqxx/pqxx>
#include "Functions.h"

int main()
{
    try
    {
        pqxx::connection conn("dbname=rioman user=rioman password=rioman port=5432");

        Functions f;

        int input = 0, option = 0;
        char repeat = 'y', again = 'y';

        while (repeat != 'n')
        {
            system("cls");

            std::cout << endl;
            f.inMain();
            f.printFunctions();

            cin >> input;
            clearInputBuffer();
            
            switch (input)
            {
            case 1:
                f.status(conn);
                break;
            case 2:
                f.variableQuery(conn);
                break;
            case 3:
                f.Purchase(conn);
                break;
            case 4:
                f.Sale(conn);
                break;
            case 5:
                again = 'y';
                while (again != 'n')
                {
                    f.printEditOptions();

                    cin >> option;
                    clearInputBuffer();

                    switch (option)
                    {
                    case 1:
                        f.CreateIngredient(conn);
                        break;
                    case 2:
                        f.DeleteIngredient(conn);
                        break;
                    case 3:
                        f.CreateDish(conn);
                        break;
                    case 4:
                        f.DeleteDish(conn);
                        break;
                    case 5:
                        f.DishIngredientConnector(conn);
                        break;
                    case 6:
                        f.RemoveDishIngredientConnection(conn);
                        break;
                    case 7:
                        f.DishPriceChange(conn);
                        break;

                    default:
                        break;
                    }

                    std::cout << "\nContinue Modify(n)   ";
                    again = std::cin.get();
                }
                break;
            
            default:
                break;
            }
            
            std::cout << "\nRepeat RIOMan(n)   ";
            repeat = std::cin.get();
        }
    }
    catch (const exception& e)
    {
        cerr << "Error: " << e.what() << endl;
        return 1;
    }

    return 0;
}