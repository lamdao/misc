#include <iostream>
#include <sstream>
#include "sqlite3pp.h"

int main()
{
	SQLite3PP db("test.db");
	if (!db) {
		std::cerr << db.error() << std::endl;
		return 1;
	}
	std::stringstream query;
	query
		<< "CREATE TABLE IF NOT EXISTS test ("
		<< "    id INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT, "
		<< "    name VARCHAR[20] NOT NULL, "
		<< "    age INTEGER"
		<< ")";
	if (!db.sql(query)) {
		std::cerr << db.error() << std::endl;
		return 1;
	}

	if (!db.sql("INSERT INTO test (name, age) VALUES (?, ?)", "Nikolas", 23)) {
		std::cerr << db.error() << std::endl;
		return 1;
	}

	if (!db.sql("INSERT INTO test (name) VALUES (?)", "Bob")) {
		std::cerr << db.error() << std::endl;
		return 1;
	}

	std::vector<SQLite3PP::Row> rows;
	if (!db.sql("SELECT name, age FROM test;", rows)) {
		std::cerr << db.error() << std::endl;
		return 1;
	}

	for (auto row = rows.begin(); row != rows.end(); row++) {
		for (auto column = row->begin(); column != row->end(); column++) {
			std::cout
				<< "[" << column->first << "]\t"
				<< (column->second ? *column->second : "N/A")
				<< std::endl;
		}
	}

	return 0;
}
