//
// SQLStatement.cpp
//
// Created by Chenbjin on 2015/12/20.
// Copyright(C) 2015, Chenbjin. All rights reserved.
//
#include "SQLStatement.h"
#include "Exceptions.h"
#include "StaticValue.h"

#include <iomanip>
#include <iostream>
#include <boost/algorithm/string.hpp>

using namespace std;

/* -------------------- SQL -------------------------- */
SQL::SQL():sql_type_(-1){}

SQL::SQL(int sql_type): sql_type_(sql_type){}

int SQL::GetSQLType() { return sql_type_; }

int SQL::ParseDataType(vector<string> sql_vector, Attribute &attr, unsigned int pos)
{
	boost::algorithm::to_lower(sql_vector[pos]);
	if (sql_vector[pos] == "int")
	{
		cout << "logging: Type: int" << endl;
		attr.SetDataType(T_INT);
		attr.SetLength(4);
		pos ++;
		if (sql_vector[pos] == ",") pos ++;
	}
	else if (sql_vector[pos] == "float")
	{
		cout << "logging: Type: float" << endl;
		attr.SetDataType(T_FLOAT);
		attr.SetLength(4);
		pos ++;
		if (sql_vector[pos] == ",") pos ++;
	}
	else if (sql_vector[pos] == "char" || sql_vector[pos] == "varchar")
	{
		cout << "logging: Type: char" << endl;
		attr.SetDataType(T_CHAR);
		pos ++;
		if (sql_vector[pos] == ")") pos ++;
		if (sql_vector[pos] == ",") pos ++;
	}
	else throw SyntaxErrorException();
	
	return pos;
}

/* -------------- SQLCreateDatabase ----------------- */
SQLCreateDatabase::SQLCreateDatabase(vector<string> sql_vector) { Parse(sql_vector); }

string SQLCreateDatabase::GetDBName() { return db_name_; }

void SQLCreateDatabase::SetDBName(string dbname) { db_name_ = dbname; }

void SQLCreateDatabase::Parse(vector<string> sql_vector)
{
	sql_type_ = 30;
	if (sql_vector.size() <= 2) throw SyntaxErrorException();
	else
	{
		cout << "logging: DB Name: " << sql_vector[2] << endl;
		db_name_ = sql_vector[2];
	}
}

/* -------------- SQLCreateTable ----------------- */
SQLCreateTable::SQLCreateTable(vector<string> sql_vector) { Parse(sql_vector); }

string SQLCreateTable::GetDBName() { return db_name_; }

void SQLCreateTable::SetDBName(string dbname) { db_name_ = dbname; }

vector<Attribute> SQLCreateTable::GetAttributes() { return attrs_; }

void SQLCreateTable::SetAttributes(vector<Attribute> attr) { attrs_ = attr; }

void SQLCreateTable::Parse(vector<string> sql_vector)
{
	sql_type_ = 31;
	unsigned int pos = 2;
	if (sql_vector.size() <= pos) throw SyntaxErrorException();
	
	cout << "logging: Table Name: " << sql_vector[pos] << endl;
	db_name_ = sql_vector[pos];
	pos ++;

	if (sql_vector[pos] != "(") throw SyntaxErrorException();
	pos ++;

	bool is_attr = true;
	bool has_primary_key = false;

	while (is_attr)
	{
		is_attr = false;
		if (sql_vector[pos] != "primary")
		{
			cout << "logging: Column: " << sql_vector[pos] << endl;
			Attribute attr;
			attr.SetAttributeName(sql_vector[pos]);
			pos++;
			pos = ParseDataType(sql_vector, attr, pos);
			attrs_.push_back(attr);
			if (sql_vector[pos] != ")") is_attr = true;
		}
		else  /* primary key */
		{
			if (has_primary_key) throw SyntaxErrorException();
			pos ++;
			if (sql_vector[pos] != "key") throw SyntaxErrorException();
			pos ++;
			if (sql_vector[pos] != "(") throw SyntaxErrorException();
			pos ++;
			for (auto att = attrs_.begin(); att != attrs_.end() ; att++)
			{
				if ((*att).GetAttributeName() == sql_vector[pos])
				{
					(*att).SetAttibuterType(1);
					cout << "logging: Primary Key:" << sql_vector[pos] << endl;
				}
			}
			pos ++;
			if (sql_vector[pos] != ")") throw SyntaxErrorException();
			has_primary_key = true;
		}
	}
}

/* -------------- SQLCreateIndex ----------------- */
SQLCreateIndex::SQLCreateIndex(vector<string> sql_vector) { Parse(sql_vector); }

string SQLCreateIndex::GetTableName() { return tb_name_; }

string SQLCreateIndex::GetIndexName() { return index_name_; }

string SQLCreateIndex::GetColumnName() { return col_name_; }

void SQLCreateIndex::Parse(vector<string> sql_vector)
{
	sql_type_ = 32;
	unsigned int pos = 2;
	if (sql_vector.size() <= pos) throw SyntaxErrorException();
	
	cout << "logging: Index Name: " << sql_vector[pos] << endl;
	index_name_ = sql_vector[pos];
	pos ++;
	
	if (boost::algorithm::to_lower_copy(sql_vector[pos]) != "on") throw SyntaxErrorException();
	pos ++;

	cout << "logging: Table Name: " << sql_vector[pos] << endl;
	tb_name_ = sql_vector[pos];
	pos ++;

	if (boost::algorithm::to_lower_copy(sql_vector[pos]) != "(") throw SyntaxErrorException();
	pos ++;

	cout << "logging:: Column Name: " << sql_vector[pos] << endl;
	col_name_ = sql_vector[pos];
	pos ++;

	if (boost::algorithm::to_lower_copy(sql_vector[pos]) != ")") throw SyntaxErrorException();
	pos ++;
}

/* -------------- SQLDropDatabase ----------------- */
SQLDropDatabase::SQLDropDatabase(vector<string> sql_vector) { Parse(sql_vector); }

string SQLDropDatabase::GetDBName() { return db_name_; }

void SQLDropDatabase::SetDBName(string dbname) { db_name_ = dbname; }

void SQLDropDatabase::Parse(vector<string> sql_vector)
{
	sql_type_ = 50;
	if (sql_vector.size() <= 2) throw SyntaxErrorException();
	else
	{
		cout << "logging: DB Name: " << sql_vector[2] << endl;
		db_name_ = sql_vector[2];
	}
}

/* -------------- SQLUse ----------------- */
SQLUse::SQLUse(vector<string> sql_vector) { Parse(sql_vector); }

string SQLUse::GetDBName() { return db_name_; }

void SQLUse::SetDBName(string dbname) { db_name_ = dbname; }

void SQLUse::Parse(vector<string> sql_vector)
{
	sql_type_ = 60;
	if (sql_vector.size() <= 1) throw SyntaxErrorException();
	else
	{
		cout << "logging: DB Name: " << sql_vector[1] << endl;
		db_name_ = sql_vector[1];
	}
}