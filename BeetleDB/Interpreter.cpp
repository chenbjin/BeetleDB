//
// Interpreter.cpp
//
// Created by Chenbjin on 2015/12/18.
// Copyright(C) 2015, Chenbjin. All rights reserved.
//
#include "Interpreter.h"
#include <iostream>
#include <fstream>
#include <vector>

#include <boost/regex.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>

#include "API.h"
#include "Exceptions.h"

using namespace std;

Interpreter::Interpreter(void) : sql_type_(-1)
{
	//string path = string(getenv("DBHOME")) + "/BeetleDBData/";
	string path = "/BeetleDBData/";
	api = new API(path);
}

Interpreter::~Interpreter(void) { delete api; }

vector<string> Interpreter::SplitSQL(string statement)
{
	char* cstr = const_cast<char*>(statement.c_str());
	char* current;
	vector<string> arr;
	current = strtok(cstr," ");
	while (current != NULL)
	{
		arr.push_back(current);
		current = strtok(NULL," ");
	}
	return arr;
}

void Interpreter::GeneralizeSQL()
{
	// remove newlines, tabs
	boost::regex reg("[\r\n\t]");
	//string newstr(" ");
	sql_statement_ = boost::regex_replace(sql_statement_, reg, " ");
	// remove ; and chars after ;
	reg = ";.*$";
	//string newstr(" ");
	sql_statement_ = boost::regex_replace(sql_statement_, reg, "");
	
	// remove leading spaces and trailing spaces
	reg = "(^ +)|( +$)";
	sql_statement_ = boost::regex_replace(sql_statement_, reg, "");
	
	// remove duplicate spaces
	reg = " +";
	sql_statement_ = boost::regex_replace(sql_statement_, reg, " ");
	
	// insert space before or after ( ) , = <> < >
	reg = " ?(\\(|\\)|,|=|(<>)|<|>) ?";
	sql_statement_ = boost::regex_replace(sql_statement_, reg, " $1 ");
	reg = "< *>";
	sql_statement_ = boost::regex_replace(sql_statement_, reg, "<>");
	reg = "< *=";
	sql_statement_ = boost::regex_replace(sql_statement_, reg, "<=");
	reg = "> *=";
	sql_statement_ = boost::regex_replace(sql_statement_, reg, ">=");
	
	// split sql_statement_
	sql_vector_ = SplitSQL(sql_statement_);
}

void Interpreter::GetSQLType()
{
	if (sql_vector_.size() == 0)
	{
		sql_type_ = -1;
		cout << "logging: SQL TYPE: empty" << endl;
		return;
	}
	boost::algorithm::to_lower(sql_vector_[0]);
	if (sql_vector_[0] == "quit") { sql_type_ = 10;} /* QUIT */
	else if (sql_vector_[0] == "help" || sql_vector_[0] == "\\?") { sql_type_ = 20; }  /* HELP */
	else if (sql_vector_[0] == "create")
	{
		boost::algorithm::to_lower(sql_vector_[1]);
		if (sql_vector_[1] == "database") { sql_type_ = 30; } /* CREATE DATABASE */
		else if (sql_vector_[1] == "table") { sql_type_ = 31; } /* CREATE TABLE */
		else if (sql_vector_[1] == "index") { sql_type_ = 32; } /* CREATE INDEX */
		else { sql_type_ = -1; }
	}
	else if (sql_vector_[0] == "show")
	{
		boost::algorithm::to_lower(sql_vector_[1]);
		if (sql_vector_[1] == "database") { sql_type_ = 40; } /* SHOW DATABASES */
		else if (sql_vector_[1] == "table") { sql_type_ = 41; } /* SHOW TABLES */
		else { sql_type_ = -1; }	
	}
	else if (sql_vector_[0] == "drop")
	{
		boost::algorithm::to_lower(sql_vector_[1]);
		if (sql_vector_[1] == "database") { sql_type_ = 50; } /* DROP DATABASE */
		else if (sql_vector_[1] == "table") { sql_type_ = 51; } /* DROP TABLE */
		else if (sql_vector_[1] == "index") { sql_type_ = 52; } /* DROP INDEX */
		else { sql_type_ = -1; }
	}
	else if (sql_vector_[0] == "use") { sql_type_ = 60; } /* USE */
	else if (sql_vector_[0] == "insert") { sql_type_ = 70; } /* INSERT */
	else if (sql_vector_[0] == "exec") { sql_type_ = 80; } /* EXEC */
	else if (sql_vector_[0] == "select") { sql_type_ = 90; } /* SELECT */
	else if (sql_vector_[0] == "delete") { sql_type_ = 100; } /* DELETE */
	else if (sql_vector_[0] == "update") { sql_type_ = 110; } /* UPDATE */
	else  /* UNKNOW */
	{ 
		sql_type_ = -1; 
		cout << "logging: Unknow SQL type." << endl;
	}
}

void Interpreter::ParseSQL()
{
	try
	{
		switch (sql_type_)
		{
		case 10:
			api->Quit();
			exit(0);
			break;
		case 20:
			api->Help();
			break;
		case 30:
			api->CreateDatabase();
			break;
		case 31:
			api->CreateTable();
			break;
		case 32:
			api->CreateIndex();
			break;
		case 40:
			api->ShowDatabases();
			break;
		case 41:
			api->ShowTables();
			break;
		case 50:
			api->DropDatabase();
			break;
		case 51:
			api->DropTable();
			break;
		case 52:
			api->DropIndex();
			break;
		case 60:
			api->Use();
			break;
		case 70:
			api->Insert();
			break;
		case 80:
			api->Exec();
			break;
		case 90:
			api->Select();
			break;
		case 100:
			api->Delete();
			break;
		case 110:
			api->Update();
			break;
		default:
			break;
		}
	}
	catch (SyntaxErrorException& e) { cerr << "Syntax Error!" << endl; }
	catch (NoDatabaseSelectedException& e) { cerr << "No database selected!" << endl; }
	catch (DatabaseNotExistException& e) { cerr << "Database doesn't exist!" << endl; }
	catch (DatabaseAlreadyExistsException& e) { cerr << "Database already exists!" << endl; }
	catch (TableNotExistException& e) { cerr << "Table doesn't exist!" << endl; }
	catch (TableAlreadyExistsException& e) { cerr << "Table already exists!" << endl; }
	catch (IndexAlreadyExistsException& e) { cerr << "Index already exists!" << endl; }
	catch (IndexNotExistException& e) { cerr << "Index doesn't exist!" << endl; }
	catch (OneIndexEachTableException& e) { cerr << "Each table could only have one index!" << endl; }
	catch (BPlusTreeException& e) { cerr << "BPlusTree exception!" << endl; }
	catch (IndexMustBeCreatedOnPrimaryKeyException& e) { cerr << "Index must be created on primary key!" << endl; }
	catch (PrimaryKeyConflictException& e) { cerr << "Primary key conflicts!" << endl; }
}

void Interpreter::ExecSQL(string statement)
{
	sql_statement_ = statement;
	GeneralizeSQL();
	GetSQLType();
	cout << "logging: SQL statement:" << sql_statement_ << endl;
	ParseSQL();
}