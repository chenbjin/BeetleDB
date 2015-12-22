//
// API.cpp
//
// Created by Chenbjin on 2015/12/20.
// Copyright(C) 2015, Chenbjin. All rights reserved.
//
#include "API.h"
#include "Exceptions.h"
#include "CatalogManager.h"
#include "RecordManager.h"
#include "IndexManager.h"

#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>

#include <boost/filesystem.hpp>

using namespace std;

API::API(string path):path_(path) 
{
	catalog_m_ = new CatalogManager(path);
}
API::~API(void) 
{
	delete buffer_m_;
	delete catalog_m_;
}

void API::Quit()
{
	delete buffer_m_;
	delete catalog_m_;
	cout << endl << "Bye" << endl;
}

void API::Help()
{
	cout << setiosflags(ios::left) << endl;
	cout << "BeetleDB 1.0.0 " << endl;
	cout << "List of all MySQL commands:" << endl;
	cout << "Note that all commands must be first on line and end with ';'" << endl;
	cout << "-------------------------------------------------------------" << endl;
	cout << setw(14) << "?"				<< setw(5) << "(\\?)"	<< "Synonym for 'help'." << endl;
	cout << setw(14) << "exit"			<< setw(5) << "(\\e)"	<< "Exit mysql. Same as quit." << endl;
	cout << setw(14) << "help"			<< setw(5) << "(\\h)"	<< "Display this help." << endl;
	cout << setw(14) << "quit"			<< setw(5) << "(\\q)"	<< "Exit mysql. Same as exit." << endl;
	cout << "-------------------------------------------------------------" << endl;
	//cout << "- - - - - - - - - - - - - - - - - - - - - - - - - - - - - -" << endl;
	cout << setw(16) << "show databases"	<< setw(2) << "|" << "Show the databases." << endl;
	cout << setw(16) << "show tables"		<< setw(2) << "|" << "Show the tables of current database." << endl;
	cout << setw(16) << "use"				<< setw(2) << "|" << "Use another database." << endl;
	cout << setw(16) << "create database"	<< setw(2) << "|" << "Create a new database." << endl;
	cout << setw(16) << "create table"		<< setw(2) << "|" << "Create a new table of current database." << endl;
	cout << setw(16) << "create index"		<< setw(2) << "|" << "Create index on table." << endl;
	cout << setw(16) << "drop database"		<< setw(2) << "|" << "Drop a database." << endl;
	cout << setw(16) << "drop table"		<< setw(2) << "|" << "Drop a table of current database." << endl;
	cout << setw(16) << "drop index"		<< setw(2) << "|" << "Drop index on table." << endl;
	cout << setw(16) << "select"			<< setw(2) << "|" << "Select record on table." << endl;
	cout << setw(16) << "insert"			<< setw(2) << "|" << "Insert new record to table." << endl;
	cout << setw(16) << "delete"			<< setw(2) << "|" << "Delete record from table." << endl;
	cout << setw(16) << "update"			<< setw(2) << "|" << "Update table set some records with new value." << endl;
	cout << setw(16) << "exec"				<< setw(2) << "|" << "Exec sql file." << endl;
	cout << "-------------------------------------------------------------" << endl;
}

void API::CreateDatabase(SQLCreateDatabase& statement)
{
}
void API::CreateTable(SQLCreateTable& statement){}
void API::CreateIndex(SQLCreateIndex& statement){}

void API::ShowDatabases()
{
	cout << "here" << endl;
	vector<Database> dbs = catalog_m_->GetDBs();
	cout << setiosflags(ios::left) << endl;
	cout << "+--------------------+" << endl;
	cout << "| " << setw(18) << "Database" << " |" << endl;
	cout << "+--------------------+" << endl;
	for (auto db = dbs.begin(); db != dbs.end(); db++)
		cout << "| " << setw(18) << (*db).get_db_name() << " |" << endl;
	cout << "+--------------------+" << endl;
}

void API::ShowTables()
{
	if (current_db_.size() == 0) throw NoDatabaseSelectedException();
	Database *db = catalog_m_->GetDB(current_db_);
	if (db == NULL) throw DatabaseNotExistException();
	cout << setiosflags(ios::left) << endl;
	cout << "+------------------------+" << endl;
	cout << "| " << setw(22) << "Tables_in_"+ current_db_ << " |" << endl;
	cout << "+------------------------+" << endl;
	for (auto tb = db->GetTables().begin(); tb != db->GetTables().end(); tb++)
		cout << "| " << setw(22) << (*tb).get_tb_name() << " |" << endl;
	cout << "+------------------------+" << endl;
}
void API::DropDatabase(SQLDropDatabase& statement){}
void API::DropTable(SQLDropTable& statement){}
void API::DropIndex(SQLDropIndex& statement){}
void API::Use(SQLUse& statement){}
void API::Insert(SQLInsert& statement){}
void API::Exec(SQLExec& statement){}
void API::Select(SQLSelect& statement){}
void API::Delete(SQLDelete& statement){}
void API::Update(SQLUpdate& statement){}
