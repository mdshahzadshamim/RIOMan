#include <string>
#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <iomanip>
#include <pqxx/pqxx>
#include <limits>
#include "Functions.h"
using namespace std;

void clearInputBuffer()
{
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

float changePrecision(float input, int size)
{
    stringstream ss;
    ss << fixed << setprecision(size) << input;
    string formatted_value = ss.str();
    input = stof(formatted_value);
    return input;
}

void Functions::inMain()
{
    system("cls");
    cout << "\n\tR\tI\tO\tM\tA\tN\n\n";
}

void Functions::printFunctions()
{
    cout << "\n\t1 - Status\n";
    cout << "\t2 - Random Query\n";
    cout << "\t3 - Purchase\n";
    cout << "\t4 - Sale\n";
    cout << "\t5 - Modify\n";
    
    cout << "\n\n\tFunction: ";
}
void Functions::printEditOptions()
{
    system("cls");
    cout << "\n\tM\tO\tD\tI\tF\tY\n\n";

    cout << "\n\t1 - Create Ingredient\n";
    cout << "\t2 - Delete Ingredient\n";
    cout << "\t3 - Create Dish\n";
    cout << "\t4 - Delete Dish\n";
    cout << "\t5 - Connect Dish-Ingredient\n";
    cout << "\t6 - Remove Dish-Ingredient Connection\n";
    cout << "\t7 - Change Dish Price\n";
    
    cout << "\n\n\tEdit Option: ";
}

void Functions::status(pqxx::connection& conn)
{
    try
    {
        system("cls");

        pqxx::work txn(conn);
        pqxx::result result = txn.exec("SELECT ingredient, available_units, unit FROM inventory ORDER BY ingredient");

        cout << "\n\tI   N   V   E   N   T   O   R   Y\n\n\n"; 
        cout << "\t" << setw(30) << left << "Ingredient" << "Available Units\n\n";
        for (const auto& row : result)
        {
            cout << "\t" << setw(30) << left << row[0].as<string>() << " ";
            cout << setw(10) << left << row[1].as<float>();
            cout << row[2].as<string>();
            cout << endl;
        }

        txn.commit();

        cout << "\nExiting Status\n";
    }
    catch (const exception& e)
    {
        cerr << "Error: " << e.what() << endl;
        return;
    }
}

void Functions::variableQuery(pqxx::connection& conn)
{
    try
    {
        system("cls");

        cout << endl << "  Enter the query:  ";
        string query;
        getline(cin, query);

        cout << endl;

        pqxx::work txn(conn);

        pqxx::result result = txn.exec(query);

        txn.commit();

        for (const auto& row : result)
        {
            cout << "  ";
            for (const auto& col : row)
            {
                cout << setw(40) << left << col.c_str();
            }
            cout << "\n\n";
        }

        cout << "Exiting Random Query\n";
    }
    catch (const exception& e)
    {
        cerr << "Error: " << e.what() << endl;
        return;
    }
}

void Functions::ListIngredient(pqxx::connection& conn)
{
    try
    {
        system("cls");
        cout << endl;

        pqxx::work txn(conn);
        pqxx::result result = txn.exec("SELECT id, ingredient, available_units, unit FROM inventory ORDER BY ingredient");
        txn.commit();

        cout << "\t" << setw(5) << left << "Id" << setw(30) << left << "Ingredient" << "Available Units\n\n";
        for (const auto& row : result)
        {
            cout << "\t" << setw(5) << left << row[0].as<int>();
            cout << setw(30) << left << row[1].as<string>() << " ";
            cout << setw(10) << row[2].as<float>();
            cout << row[3].as<string>();
            cout << endl;
        }
        cout << endl;
    }
    catch (const exception& e)
    {
        cerr << "Error: " << e.what() << endl;
        return;
    }
}

void Functions::Purchase(pqxx::connection& conn)
{
    try
    {
        string time_stamp, uuid;
        float sum = 0;

        pqxx::work txnuuid(conn);
        pqxx::result resultuuid = txnuuid.exec("SELECT uuid_generate_v4()");
        txnuuid.commit();

        if (resultuuid.empty()) return;
        else if (!resultuuid.empty()) uuid = resultuuid[0][0].as<string>();

        pqxx::work txn(conn);
        pqxx::result result = txn.exec("SELECT CURRENT_TIMESTAMP");
        txn.commit();

        if (!result.empty())
        {
            time_stamp = result[0][0].as<string>();

            pqxx::work txn1(conn);
            txn1.exec_params("INSERT INTO purchase (id, amount, time_stamp) "
                "VALUES ($1, 0, $2)", uuid, time_stamp);
            txn1.commit();

            char repeat = 'y';
            while (repeat != 'n')
            {
                ListIngredient(conn);

                int ingredient_id = 0;
                float available_units = 0, units_needed = 0, units_to_be_added = 0;
                float updated_units = 0, price = 0, total = 0;
                string ingredient_name, unit_name;

                cout << "\nIngredient Id: ";
                cin >> ingredient_id;
                clearInputBuffer();

                pqxx::work txn2(conn);
                pqxx::result result1 = txn2.exec_params("SELECT ingredient, unit, available_units FROM inventory WHERE id = $1", ingredient_id);
                txn2.commit();

                if (!result1.empty())
                {
                    ingredient_name = result1[0][0].as<string>();
                    unit_name = result1[0][1].as<string>();
                    available_units = result1[0][2].as<float>();

                    cout << "\nAmount in " << unit_name << ": ";
                    cin >> units_to_be_added;
                    clearInputBuffer();

                    units_to_be_added = changePrecision(units_to_be_added, 3);

                    cout << "\nPrice: ";
                    cin >> price;
                    clearInputBuffer();

                    price = changePrecision(price, 2);

                    updated_units = available_units + units_to_be_added;

                    total = units_to_be_added * price;

                    total = changePrecision(total, 2);

                    if (total <= 0)
                        break;
                    
                    pqxx::work txn5(conn);
                    txn5.exec_params("UPDATE inventory SET available_units = $1 WHERE ingredient = $2", updated_units, ingredient_name);
                    txn5.commit();

                    pqxx::work txn6(conn);
                    txn6.exec_params("INSERT INTO purchase_ingredient_junction (purchase_id, ingredient, price, units, total, time_stamp) "
                        "VALUES ($1, $2, $3, $4, $5, $6)", uuid, ingredient_name, price, units_to_be_added, total, time_stamp);
                    txn6.commit();

                    sum += total;
                }
                cout << "\nStock more in the Inventory(n) - ";
                repeat = cin.get();
            }
            
            if (sum > 0)
            {
                pqxx::work txn7(conn);
                txn7.exec_params("UPDATE purchase SET amount = $1 WHERE id = $2", sum, uuid);
                txn7.commit();
            }
        }
        pqxx::work txn9(conn);
        txn9.exec("DELETE FROM purchase_ingredient_junction WHERE total <= 0");
        txn9.commit();
        pqxx::work txn8(conn);
        txn8.exec("DELETE FROM purchase WHERE amount <= 0");
        txn8.commit();
    }
    catch (const exception& e)
    {
        cerr << "Error: " << e.what() << endl;
        return;
    }
}

void Functions::ListDishes(pqxx::connection& conn)
{
    try
    {
        system("cls");
        cout << endl;

        pqxx::work txn(conn);
        pqxx::result result = txn.exec("SELECT id, dish, price FROM menu ORDER BY dish");
        txn.commit();

        cout << "\t" << setw(5) << left << "Id" << setw(20) << left << "Dish" << "Price\n\n";
        for (const auto& row : result)
        {
            cout << "\t" << setw(5) << left << row[0].as<int>() << setw(20) << left << row[1].as<string>() << row[2].as<float>() << endl;
        }
        cout << endl;
    }
    catch (const exception& e)
    {
        cerr << "Error: " << e.what() << endl;
        return;
    }
}

void Functions::ListAvailableDishes(pqxx::connection& conn)
{
    try
    {
        system("cls");
        cout << endl;

        pqxx::work txn(conn);
        pqxx::result result = txn.exec("SELECT id, dish, price FROM menu ORDER BY dish");
        txn.commit();

        cout << "\n\tM\tE\tN\tU\n\n"; 
        cout << "\t" << setw(5) << left << "Id" << setw(20) << left << "Available Dishes" << "Price\n\n";
        for (const auto& row : result)
        {
            string dish_name = row[1].as<string>();
            if (CheckDish(conn, dish_name) == true)
                cout << "\t" << setw(5) << left << row[0].as<int>() << setw(20) << left << row[1].as<string>() << row[2].as<float>() << endl;
        }
    }
    catch (const exception& e)
    {
        cerr << "Error: " << e.what() << endl;
        return;
    }
}

bool Functions::CheckDish(pqxx::connection& conn, const string& dish_name, int count)
{
    try
    {
        pqxx::work txn(conn);

        pqxx::result result = txn.exec("SELECT dij.units_needed, inv.available_units "
            "FROM dish_ingredient_junction AS dij "
            "JOIN inventory AS inv ON dij.ingredient = inv.ingredient "
            "WHERE dish = " + txn.quote(dish_name));

        txn.commit();

        bool value = false;

        for (const auto& row : result)
        {
            float need = row[0].as<float>();
            float avlbl = row[1].as<float>();

            // Check: cout << endl << dish_name << "\tNeed: " << need * count << "\tAvlbl: " << avlbl << endl;

            if (avlbl >= count * need)
                value = true;
            else
                return false;
        }
        return value;
    }
    catch (const exception& e)
    {
        cerr << "Error: " << e.what() << endl;
        return false;
    }
}

void Functions::Sale(pqxx::connection& conn)
{
    try
    {
        string time_stamp, uuid;
        float sum = 0;

        pqxx::work txnuuid(conn);
        pqxx::result resultuuid = txnuuid.exec("SELECT uuid_generate_v4()");
        txnuuid.commit();

        if (resultuuid.empty()) return;
        else if (!resultuuid.empty()) uuid = resultuuid[0][0].as<string>();

        pqxx::work txn(conn);
        pqxx::result result = txn.exec("SELECT CURRENT_TIMESTAMP");
        txn.commit();

        if (!result.empty())
        {
            time_stamp = result[0][0].as<string>();

            pqxx::work txn1(conn);
            txn1.exec_params("INSERT INTO sale (id, amount, time_stamp) "
                "VALUES ($1, 0, $2)", uuid, time_stamp);
            txn1.commit();

            char repeat = 'y';
            while (repeat != 'n')
            {
                ListAvailableDishes(conn);

                int dish_id = 0, count = 0;
                float available_units = 0, units_needed = 0, updated_units = 0, price = 0, total = 0;
                string dish_name, ingredient_name;
                cout << "\n\nDish Id: ";
                cin >> dish_id;
                clearInputBuffer();

                pqxx::work txn2(conn);
                pqxx::result result1 = txn2.exec_params("SELECT dish, price FROM menu WHERE id = $1", dish_id);
                txn2.commit();

                if (!result1.empty())
                {
                    dish_name = result1[0][0].as<string>();
                    price = result1[0][1].as<float>();

                    cout << "\nCount: ";
                    cin >> count;
                    clearInputBuffer();

                    if (CheckDish(conn, dish_name, count) == true)
                    {
                        pqxx::work txn3(conn);
                        pqxx::result result2 = txn3.exec_params("SELECT ingredient, units_needed FROM dish_ingredient_junction WHERE dish = $1", dish_name);
                        txn3.commit();

                        if (!result2.empty())
                        {
                            for (const auto& row : result2)
                            {
                                ingredient_name = row[0].as<string>();
                                units_needed = row[1].as<float>();

                                pqxx::work txn4(conn);
                                pqxx::result result3 = txn4.exec_params("SELECT available_units FROM inventory WHERE ingredient = $1", ingredient_name);
                                txn4.commit();

                                if (!result3.empty())
                                {
                                    available_units = result3[0][0].as<float>();
                                    updated_units = available_units - (units_needed * count);

                                    pqxx::work txn5(conn);
                                    txn5.exec_params("UPDATE inventory SET available_units = $1 WHERE ingredient = $2", updated_units, ingredient_name);
                                    txn5.commit();
                                }
                            }
                            total = count * price;

                            pqxx::work txn6(conn);
                            txn6.exec_params("INSERT INTO sale_dish_junction (sale_id, dish, price, count, total, time_stamp) "
                                "VALUES ($1, $2, $3, $4, $5, $6)", uuid, dish_name, price, count, total, time_stamp);
                            txn6.commit();

                            sum += total;
                        }
                    }
                }

                cout << "\nAdd More to this Order(n) - ";
                repeat = cin.get();
            }

            if (sum > 0)
            {
                pqxx::work txn7(conn);
                txn7.exec_params("UPDATE sale SET amount = $1 WHERE id = $2", sum, uuid);
                txn7.commit();
            }
        }
        pqxx::work txn9(conn);
        txn9.exec("DELETE FROM sale_dish_junction WHERE total = 0");
        txn9.commit();

        pqxx::work txn8(conn);
        txn8.exec("DELETE FROM sale WHERE amount = 0");
        txn8.commit();
    }
    catch (const exception& e)
    {
        cerr << "Error: " << e.what() << endl;
        return;
    }
}

void Functions::CreateIngredient(pqxx::connection& conn)
{
    try
    {
        system("cls");

        string ingredient_name, unit;
        //float amount_in_units;
        int input = 0;

        char repeat = 'y';

        while (repeat != 'n')
        {
            system("cls");
            cout << endl << "Ingredient Name: ";
            getline(cin, ingredient_name);

            cout << endl << "Unit kg-1,L-2: ";
            while (true)
            {
                cin >> input;
                clearInputBuffer();

                if (input == 1)
                {
                    unit = "kg";
                    break;
                }
                else if (input == 2)
                {
                    unit = "L";
                    break;
                }
            }

            // cout << endl << "Amount in " << unit << ": ";
            // cin >> amount_in_units;
            // clearInputBuffer();
            // amount_in_units = 0;
            // amount_in_units = changePrecision(amount_in_units, 3);

            pqxx::work txn(conn); 
            txn.exec_params("INSERT INTO inventory (ingredient, unit, available_units) "
                "VALUES ($1, $2, 0)", ingredient_name, unit);
            txn.commit();

            cout << "\nAdd more Ingredients(n) - ";
            repeat = cin.get();
        }

        cout << "\nExiting Ingredient Creator\n\n";
    }
    catch (const exception& e)
    {
        cerr << "Error: " << e.what() << endl;
        return;
    }
}

void Functions::DeleteIngredient(pqxx::connection& conn)
{
    try
    {
        system("cls");

        string ingredient_name;
        int input = 0;

        char repeat = 'y';

        while (repeat != 'n')
        {
            ListIngredient(conn);

            cout << endl << "Delete Ingredient - Id: ";
            cin >> input;
            clearInputBuffer();

            pqxx::work txn(conn);
            pqxx::result result = txn.exec_params("SELECT ingredient FROM inventory WHERE id = $1", input);
            txn.commit();

            if (!result.empty())
            {
                ingredient_name = result[0][0].as<string>();

                pqxx::work txn2(conn);
                txn2.exec_params("DELETE FROM inventory WHERE ingredient = $1", ingredient_name);
                txn2.commit();
            }
            cout << "\nDelete More Ingredients(n) - ";
            repeat = cin.get();
        }

        cout << "\nExiting Ingredient Deleter\n";
    }
    catch (const exception& e)
    {
        cerr << "Error: " << e.what() << endl;
        return;
    }
}

void Functions::CreateDish(pqxx::connection& conn)
{
    try
    {
        system("cls");

        char repeat = 'y';

        while (repeat != 'n')
        {
            string dish_name;
            float price = 0;

            system("cls");
            cout << endl << "Dish Name: ";
            getline(cin, dish_name);

            cout << endl << "Price: ";
            cin >> price;
            clearInputBuffer();

            pqxx::work txn(conn); 
            txn.exec_params("INSERT INTO menu (dish, price) "
                "VALUES ($1, $2)", dish_name, price);
            txn.commit();

            cout << "\nAdd more Dishes to the Menu(n) - ";
            repeat = cin.get();
        }

        cout << "\nExiting Dish Creator\n";
    }
    catch (const exception& e)
    {
        cerr << "Error: " << e.what() << endl;
        return;
    }
}

void Functions::DeleteDish(pqxx::connection& conn)
{
    try
    {
        system("cls");

        char repeat = 'y';

        while (repeat != 'n')
        {
            string dish_name;
            int input = 0;

            ListDishes(conn);

            cout << endl << "Delete Dish - Id: ";
            cin >> input;
            clearInputBuffer();

            pqxx::work txn(conn); 
            pqxx::result result = txn.exec_params("SELECT dish FROM menu WHERE id = $1", input);
            txn.commit();

            if (!result.empty())
            {
                dish_name = result[0][0].as<string>();
                
                pqxx::work txn1(conn);
                txn1.exec_params("DELETE FROM menu WHERE dish = $1", dish_name);
                txn1.commit();
            }

            cout << "\nDelete more Dishes from the Menu(n) - ";
            repeat = cin.get();
        }
        cout << "\nExiting Dish Deleter\n";
    }
    catch (const exception& e)
    {
        cerr << "Error: " << e.what() << endl;
        return;
    }
}

void Functions::DishIngredientConnector(pqxx::connection& conn)
{
    try
    {
        system("cls");

        string dish_name, ingredient_name, unit;
        float amount_in_units;
        int input_1 = 0, input_2 = 0;

        ListDishes(conn);
        cout << endl << "Dish Id: ";
        cin >> input_1;
        clearInputBuffer();

        pqxx::work txn(conn);
        pqxx::result result1 = txn.exec_params("SELECT dish FROM menu WHERE id = $1", input_1);
        txn.commit();

        if (!result1.empty())
        {
            dish_name = result1[0][0].as<string>();

            char repeat = 'y';

            while (repeat != 'n')
            {
                ListIngredient(conn);

                cout << "\nFor " + dish_name << "\n\nIngredient Id: ";
                cin >> input_2;
                clearInputBuffer();

                pqxx::work txn(conn);
                pqxx::result result2 = txn.exec_params("SELECT ingredient, unit FROM inventory WHERE id = $1", input_2);
                txn.commit();
                if (!result2.empty())
                {
                    ingredient_name = result2[0][0].as<string>();
                    unit = result2[0][1].as<string>();

                    cout << endl << "Amount in " << unit << ": ";
                    cin >> amount_in_units;
                    clearInputBuffer();

                    pqxx::work txn(conn);
                    txn.exec_params("INSERT INTO dish_ingredient_junction (dish, ingredient, units_needed) "
                        "VALUES ($1, $2, $3)", dish_name, ingredient_name, amount_in_units);
                    txn.commit();
                }
                cout << "\nConnect more Ingredients to this Dish(n) - ";
                repeat = cin.get();
            }
        }

        cout << "\nExiting Dish-Ingredient Connector\n";
    }
    catch (const exception& e)
    {
        cerr << "Error: " << e.what() << endl;
        return;
    }
}

void Functions::RemoveDishIngredientConnection(pqxx::connection& conn)
{
    try
    {
        system("cls");

        string dish_name, ingredient_name;
        int input_1 = 0, input_2 = 0;

        ListDishes(conn);

        cout << endl << "Dish Id: ";
        cin >> input_1;
        clearInputBuffer();

        pqxx::work txn(conn);
        pqxx::result result1 = txn.exec_params("SELECT dish FROM menu WHERE id = $1", input_1);
        txn.commit();

        if (!result1.empty())
        {
            dish_name = result1[0][0].as<string>();


            char repeat = 'y';

            while (repeat != 'n')
            {
                system("cls");

                pqxx::work txn(conn);
                pqxx::result result2 = txn.exec_params("SELECT id, dish, ingredient from dish_ingredient_junction where dish = $1 ORDER BY ingredient", dish_name);
                txn.commit();

                if (!result2.empty())
                {
                    cout << "  " << setw(5) << left << "Id" << setw(20) << left << "Dish" << "Ingredient\n\n";
                    for (const auto& row2 : result2)
                    {
                        cout << "  " << setw(5) << left << row2[0].as<int>();
                        cout << setw(20) << left << row2[1].as<string>();
                        cout << row2[2].as<string>() << endl;
                    }

                    cout << endl << "Connection Id: ";
                    cin >> input_2;
                    clearInputBuffer();

                    pqxx::work txn(conn);
                    pqxx::result result3 = txn.exec_params("SELECT ingredient FROM dish_ingredient_junction WHERE id = $1 ORDER BY ingredient", input_2);
                    txn.commit();

                    if (!result3.empty())
                    {
                        ingredient_name = result3[0][0].as<string>();

                        pqxx::work txn(conn);
                        txn.exec_params("DELETE FROM dish_ingredient_junction WHERE "
                            "dish = $1 AND ingredient = $2", dish_name, ingredient_name);
                        txn.commit();
                    }
                }
                cout << "\nRemove More Ingredients from this Dish(n) - ";
                repeat = cin.get();
            }
        }

        cout << "\nExiting Dish-Ingredient Connection Remover\n";
    }
    catch (const exception& e)
    {
        cerr << "Error: " << e.what() << endl;
        return;
    }
}

void Functions :: DishPriceChange(pqxx::connection& conn)
{
    try
    {
        int way = 0, operation = 0;
        float value = 0;
        char repeat = 'y';
        system("cls");

        cout << "\n\t1 - Modify All Prices\n";
        cout << "\t2 - Modify Individual Price\n";

        cout << "\n\n\tAction: ";
        cin >> way;
        clearInputBuffer();

        switch (way)
        {
        case 1 :
            cout << "\n\n\t1 - Add\n";
            cout << "\t2 - Subtract\n";
            cout << "\t3 - Multiply\n";
            cout << "\t4 - Divide\n";

            cout << "\n\n\tOperation: ";
            cin >> operation;
            clearInputBuffer();

            cout << "\n\n\tValue: ";
            cin >> value;
            clearInputBuffer();

            if (operation > 0 && operation < 5)
            {
                pqxx::work txn(conn);
                pqxx::result result = txn.exec_params("SELECT price, dish FROM menu");
                txn.commit();

                if (!result.empty())
                {
                    for (const auto& row : result)
                    {
                        float old_price = row[0].as<float>();
                        string dish_name = row[1].as<string>();
                        float new_price = 0;

                        switch (operation)
                        {
                        case 1:
                            new_price = old_price + value;
                            break;
                        case 2:
                            new_price = old_price - value;
                            break;
                        case 3:
                            new_price = old_price * value;
                            break;
                        case 4:
                            new_price = old_price / value;
                            break;
                        default:
                            break;
                        }
                        new_price = changePrecision(new_price, 2);

                        pqxx::work txn2(conn);
                        txn2.exec_params("UPDATE menu SET price = $1 WHERE dish = $2",new_price, dish_name);
                        txn2.commit();

                        pqxx::work txn3(conn);
                        txn3.exec_params("INSERT INTO dish_price_change (id, dish, old_price, updated_price, time_stamp) "
                            "VALUES (uuid_generate_v4(), $1, $2, $3, current_timestamp)", dish_name, old_price, new_price);
                        txn3.commit();
                    }
                }
            }
            break;
        case 2:
            while (repeat != 'n')
            {
                int dish_id = 0;
                float old_price = 0, new_price = 0;
                string dish_name;
                ListDishes(conn);

                cout << "Change Price - Dish Id: ";
                cin >> dish_id;
                clearInputBuffer();
                
                pqxx::work txn(conn);
                pqxx::result result = txn.exec_params("SELECT price, dish FROM menu WHERE id = $1", dish_id);
                txn.commit();
                if (!result.empty())
                {
                    old_price = result[0][0].as<float>();
                    dish_name = result[0][1].as<string>();
                    cout << "Old Price for " << dish_name << ": " << old_price << endl;

                    cout << "New Price for " << dish_name << ": ";
                    cin >> new_price;
                    clearInputBuffer();

                    pqxx::work txn1(conn);
                    txn1.exec_params("UPDATE menu SET price = $1 WHERE dish = $2",new_price, dish_name);
                    txn1.commit();

                    pqxx::work txn2(conn);
                    txn2.exec_params("INSERT INTO dish_price_change (id, dish, old_price, updated_price, time_stamp) "
                        "VALUES (uuid_generate_v4(), $1, $2, $3, current_timestamp)", dish_name ,old_price, new_price);
                    txn2.commit();
                }

                cout << "\nModify more Dish Prices(n) - ";
                repeat = cin.get();
            }
            break;
        default:
            break;
        }
    }
    catch (const exception& e)
    {
        cerr << "Error: " << e.what() << endl;
        return;
    }
}

// cout << row[0].c_str() << "\t";

