// CouchDevel.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <memory>
#include <iostream>
#include "CouchDBHandler.h"

int main()
{
    CouchDBHandler couchdb("http://localhost:5984/", "admin", "password");

    std::string response = couchdb.deleteDatabase("my_database");
    std::cout << "Response: " << response << std::endl;

    response = couchdb.createDatabase("my_database");
    std::cout << "Response: " << response << std::endl;

    json jsonData;
    jsonData["name"] = "John";
    jsonData["age"] = 22;

    response = couchdb.createDocument("my_database", "1", jsonData);
    std::cout << "Response: " << response << std::endl;

    auto retn_1 = couchdb.getDocument("my_database", "1");
    std::cout << "Response: " << retn_1 << std::endl;

    retn_1["name"] = "Jane";

    response = couchdb.updateDocument("my_database", "1", retn_1);
    std::cout << "Response: " << response << std::endl;

    auto retn_2 = couchdb.getDocument("my_database", "1");
    std::cout << "Response: " << retn_2 << std::endl;

    response = couchdb.deleteDocument("my_database", "1");
    std::cout << "Response: " << response << std::endl;

    system("pause");
    return 0;
}
