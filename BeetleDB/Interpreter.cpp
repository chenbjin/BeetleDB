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

Interpreter::Interpreter() : sql_type_(-1)
{
	string path;
	if (getenv("DBHOME"))
		path = string(getenv("DBHOME")) + "/BeetleDBData/";
	else
		path = boost::filesystem::initial_path<boost::filesystem::path>().string() + "/BeetleDBData/"; /* get current dir path.*/
	
	if (!boost::filesystem::exists(path)) boost::filesystem::create_directory(path);

	api = new API(path);
}

Interpreter::~Interpreter(void) { delete api; }

vector<string> Interpreter::SplitSQL(string statement, string seg)
{
	char* cstr = const_cast<char*>(statement.c_str());
	char* current;
	vector<string> arr;
	current = strtok(cstr,seg.c_str());
	while (current != NULL)
	{
		arr.push_back(current);
		current = strtok(NULL, seg.c_str());
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
	sql_vector_ = SplitSQL(sql_statement_, " ");
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
	if (sql_vector_[0] == "quit" || sql_vector_[0] == "\\q" || sql_vector_[0] == "\\e") { sql_type_ = 10;} /* QUIT */
	else if (sql_vector_[0] == "help"|| sql_vector_[0] == "\\h" || sql_vector_[0] == "\\?" || sql_vector_[0] == "?") { sql_type_ = 20; }  /* HELP */
	else if (sql_vector_[0] == "create")
	{
		if(sql_vector_.size() <= 1)
		{
			sql_type_ = -1;
			cout << "SyntaxError: use 'help;' to get correct command." << endl;
			return;
		}
		boost::algorithm::to_lower(sql_vector_[1]);
		if (sql_vector_[1] == "database") { sql_type_ = 30; } /* CREATE DATABASE */
		else if (sql_vector_[1] == "table") { sql_type_ = 31; } /* CREATE TABLE */
		else if (sql_vector_[1] == "index") { sql_type_ = 32; } /* CREATE INDEX */
		else { sql_type_ = -1; }
	}
	else if (sql_vector_[0] == "show")
	{
		if(sql_vector_.size() <= 1)
		{
			sql_type_ = -1;
			cout << "SyntaxError: use 'help;' to get correct command." << endl;
			return;
		}
		boost::algorithm::to_lower(sql_vector_[1]);
		if (sql_vector_[1] == "databases") { sql_type_ = 40; } /* SHOW DATABASES */
		else if (sql_vector_[1] == "tables") { sql_type_ = 41; } /* SHOW TABLES */
		else 
		{ 
			sql_type_ = -1;
			cout << "SyntaxError: use 'help;' to get correct command." << endl;
			return;
		}	
	}
	else if (sql_vector_[0] == "drop")
	{
		if(sql_vector_.size() <= 1)
		{
			sql_type_ = -1;
			cout << "SyntaxError: use 'help;' to get correct command." << endl;
			return;
		}
		boost::algorithm::to_lower(sql_vector_[1]);
		if (sql_vector_[1] == "database") { sql_type_ = 50; } /* DROP DATABASE */
		else if (sql_vector_[1] == "table") { sql_type_ = 51; } /* DROP TABLE */
		else if (sql_vector_[1] == "index") { sql_type_ = 52; } /* DROP INDEX */
		else 
		{
			sql_type_ = -1;
			cout << "SyntaxError: use 'help;' to get correct command." << endl;
			return;
		}
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
		{
			SQLCreateDatabase *st = new SQLCreateDatabase(sql_vector_);
			api->CreateDatabase(*st);
			delete st;
		}
			break;
		case 31:
		{
			SQLCreateTable *st = new SQLCreateTable(sql_vector_);
			api->CreateTable(*st);
			delete st;
		}
			break;
		case 32:
		{
			SQLCreateIndex *st = new SQLCreateIndex(sql_vector_);
			api->CreateIndex(*st);
			delete st;
		}
			break;
		case 40:
		{
			api->ShowDatabases();
		}
			break;
		case 41:
		{
			api->ShowTables();
		}
			break;
		case 50:
		{
			SQLDropDatabase *st = new SQLDropDatabase(sql_vector_);
			api->DropDatabase(*st);
			delete st;
		}
			break;
		case 51:
		{
			SQLDropTable *st = new SQLDropTable(sql_vector_);
			api->DropTable(*st);
			delete st;
		}
			break;
		case 52:
		{
			SQLDropIndex *st = new SQLDropIndex(sql_vector_);
			api->DropIndex(*st);
			delete st;
		}
			break;
		case 60:
		{
			SQLUse *st = new SQLUse(sql_vector_);
			api->Use(*st);
			delete st;
		}
			break;
		case 70:
		{
			SQLInsert *st = new SQLInsert(sql_vector_);
			api->Insert(*st);
			delete st;
		}
			break;
		case 80:
		{
			SQLExec *st  = new SQLExec(sql_vector_);
			string sql_content;
			ifstream in(st->get_file_name(), ios::in | ios::binary);
			in.seekg(0, ios::end);
			sql_content.resize(in.tellg());

			in.seekg(0, ios::beg);
			in.read(&sql_content[0], sql_content.size());
			in.close();

			vector<string> sqls = SplitSQL(sql_content, ";");
			for (auto sql = sqls.begin(); sql != sqls.end(); sql++)
				ExecSQL((*sql));
			delete st;
		}
			break;
		case 90:
		{
			SQLSelect *st = new SQLSelect(sql_vector_);
			api->Select(*st);
			delete st;
		}
			break;
		case 100:
		{
			SQLDelete *st = new SQLDelete(sql_vector_);
			api->Delete(*st);
			delete st;
		}
			break;
		case 110:
		{
			SQLUpdate *st = new SQLUpdate(sql_vector_);
			api->Update(*st);
			delete st;
		}
			break;
		default:
			break;
		}
	}
	catch (SyntaxErrorException& e) { cerr << "Error: Syntax Error!" << endl; }
	catch (NoDatabaseSelectedException& e) { cerr << "Error: No database selected." << endl; }
	catch (DatabaseNotExistException& e) { cerr << "Error: Database doesn't exist!" << endl; }
	catch (DatabaseAlreadyExistsException& e) { cerr << "Error: Database already exists!" << endl; }
	catch (TableNotExistException& e) { cerr << "Error: Table doesn't exist!" << endl; }
	catch (TableAlreadyExistsException& e) { cerr << "Error: Table already exists!" << endl; }
	catch (IndexAlreadyExistsException& e) { cerr << "Error: Index already exists!" << endl; }
	catch (IndexNotExistException& e) { cerr << "Error: Index doesn't exist!" << endl; }
	catch (OneIndexEachTableException& e) { cerr << "Error: Each table could only have one index!" << endl; }
	catch (BPlusTreeException& e) { cerr << "Error: BPlusTree exception!" << endl; }
	catch (IndexMustBeCreatedOnPrimaryKeyException& e) { cerr << "Error: Index must be created on primary key!" << endl; }
	catch (PrimaryKeyConflictException& e) { cerr << "Error: Primary key conflicts!" << endl; }
}

void Interpreter::ExecSQL(string statement)
{
	sql_statement_ = statement;  /* get SQL statement */
	GeneralizeSQL();	/* format SQL statement */
	GetSQLType();		/* get sql type */
	ParseSQL();			/* construct SQL object and pass to API */
}