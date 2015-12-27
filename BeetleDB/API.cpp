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

/* How Managers mainly work?

CatalogManager: Create Database/Table, Drop, Use.

RecordManager: Insert, Select, Delete, Update.

IndexManager: Create Index.
*/

void API::CreateDatabase(SQLCreateDatabase& statement)
{
	cout << "Creating database: " << statement.get_db_name() << endl;
	string folder_name(path_ + statement.get_db_name());
	boost::filesystem::path folder_path(folder_name); /* create directory for database. */

	if (catalog_m_->GetDB(statement.get_db_name()) != NULL) throw DatabaseAlreadyExistsException();
	if (boost::filesystem::exists(folder_path))
	{
		boost::filesystem::remove_all(folder_path);
		cout << "Database folder exists and deleted!" << endl;
	}

	boost::filesystem::create_directories(folder_path);
	cout << "Database folder created!" << endl;
	
	catalog_m_->CreateDatabase(statement.get_db_name());
	cout << "Catalog written!" << endl;
	catalog_m_->WriteArchiveFile();
}

void API::CreateTable(SQLCreateTable& statement)
{
	cout << "Creating table: " << statement.get_tb_name() << endl;
	if (current_db_.length() == 0) throw NoDatabaseSelectedException();

	Database *db = catalog_m_->GetDB(current_db_);
	if (db == NULL) throw DatabaseNotExistException();
	if (db->GetTable(statement.get_tb_name()) != NULL) throw TableAlreadyExistsException();

	string file_name(path_ + current_db_ + "/" + statement.get_tb_name() + ".records");
	boost::filesystem::path folder_path(file_name);

	if (boost::filesystem::exists(folder_path))
	{
		boost::filesystem::remove_all(folder_path);
		cout << "Table file exists and deleted!" << endl;
	}

	ofstream ofs(file_name);
	ofs.close();

	cout << "Table file created!" << endl;
	db->CreateTable(statement);

	cout << "Catalog written!" << endl;
	catalog_m_->WriteArchiveFile();
}

void API::CreateIndex(SQLCreateIndex& statement)
{
	cout << "Creating Index: " << statement.get_index_name() << endl;
	if (current_db_.length() == 0) throw NoDatabaseSelectedException();
	
	Database *db = catalog_m_->GetDB(current_db_);
	if (db->GetTable(statement.get_tb_name()) == NULL) throw TableNotExistException();
	if (db->CheckIfIndexExists(statement.get_index_name()) ) throw IndexAlreadyExistsException();

	IndexManager *im = new IndexManager(catalog_m_, buffer_m_, current_db_);
	im->CreateIndex(statement);
	delete im;
}

void API::ShowDatabases()
{
	//cout << "here" << endl;
	vector<Database> dbs = catalog_m_->GetDBs();
	if (dbs.size() == 0)
	{
		cout << "No databases exist now." << endl;
		cout << "Use 'create database' command to create a new database." << endl;
		return;
	}
	cout << setiosflags(ios::left) << endl;
	cout << "+--------------------+" << endl;
	cout << "| " << setw(18) << "Database" << " |" << endl;
	cout << "+--------------------+" << endl;
	for (auto db = dbs.begin(); db != dbs.end(); db++)
		cout << "| " << setw(18) << (*db).get_db_name() << " |" << endl;
	cout << "+--------------------+" << endl;
	cout << dbs.size() << " row in set ";
}

void API::ShowTables()
{
	if (current_db_.size() == 0) throw NoDatabaseSelectedException();
	Database *db = catalog_m_->GetDB(current_db_);
	if (db == NULL) throw DatabaseNotExistException();
	if (db->GetTables().size() == 0)
	{
		cout << "No table exist now." << endl;
		cout << "Use 'create table' command to create a new table." << endl;
		return;
	}
	cout << setiosflags(ios::left) << endl;
	cout << "+------------------------+" << endl;
	cout << "| " << setw(22) << "Tables_in_"+ current_db_ << " |" << endl;
	cout << "+------------------------+" << endl;
	for (auto tb = db->GetTables().begin(); tb != db->GetTables().end(); tb++)
		cout << "| " << setw(22) << (*tb).get_tb_name() << " |" << endl;
	cout << "+------------------------+" << endl;
}

void API::DropDatabase(SQLDropDatabase& statement)
{
	cout << "Droping database: " << statement.get_db_name() << endl;
	bool found = false;

	vector<Database> dbs = catalog_m_->GetDBs();
	for (auto db = dbs.begin(); db != dbs.end(); db++)
	{
		if (db->get_db_name() == statement.get_db_name())
		{
			found = true;
			//break;
		}
	}
	if (found == false) throw DatabaseNotExistException();
	
	string folder_name(path_ + statement.get_db_name());
	boost::filesystem::path folder_path(folder_name);
	
	if (!boost::filesystem::exists(folder_path)) cout << "Database folder doesn't exists!" << endl;
	else
	{
		boost::filesystem::remove_all(folder_path);
		cout << "Database folder deleted!" << endl;
	}

	catalog_m_->DeleteDatabase(statement.get_db_name());
	cout << "Database removed from catalog!" << endl;
	catalog_m_->WriteArchiveFile();

	if (statement.get_db_name() == current_db_)
	{
		current_db_ = "";
		delete buffer_m_;
	}
}

void API::DropTable(SQLDropTable& statement)
{
	cout << "Droping table: " << statement.get_tb_name() << endl;
	if (current_db_.length() == 0) throw NoDatabaseSelectedException();

	Database *db = catalog_m_->GetDB(current_db_);
	if (db == NULL) throw DatabaseNotExistException();

	Table *tb = db->GetTable(statement.get_tb_name());
	if (tb == NULL) throw TableNotExistException();
	
	string file_name(path_ + current_db_ + "/" + statement.get_tb_name() + ".records");

	if (!boost::filesystem::exists(file_name)) cout << "Table file doesn't exists!" << endl;
	else
	{
		boost::filesystem::remove(file_name);
		cout << "Table file deleted!" << endl;
	}

	cout << "Removing Index files!" << endl;
	for (unsigned int i = 0; i < tb->GetIndexNum(); i++)
	{
		string file_name(path_ + current_db_ + "/" + tb->GetIndex(i)->get_name() + ".index");
		if (!boost::filesystem::exists(file_name)) cout << "Index file doesn't exist!" << endl;
		else
		{
			boost::filesystem::remove(file_name);
			cout << "Index file removed!" << endl;
		}
	}

	db->DropTable(statement);
	cout << "Catalog written!" << endl;
	catalog_m_->WriteArchiveFile();
}

void API::DropIndex(SQLDropIndex& statement)
{
	if (current_db_.length() == 0) throw NoDatabaseSelectedException();

	Database *db = catalog_m_->GetDB(current_db_);
	if (db == NULL) throw DatabaseNotExistException();
	if (!db->CheckIfIndexExists(statement.get_index_name())) throw IndexNotExistException();

	string file_name(path_ + current_db_ + "/" + statement.get_index_name() + ".index");
	if (!boost::filesystem::exists(file_name))
	{
		cout << "Index file doesn't exist!" << endl;
		return;
	}
	boost::filesystem::remove(file_name);
	cout << "Index file removed!" << endl;

	db->DropIndex(statement);
	cout << "Catalog written!" << endl;
	catalog_m_->WriteArchiveFile();
}

void API::Use(SQLUse& statement)
{
	cout <<"Reading table information for completion of table and column name" << endl;
	Database *db = catalog_m_->GetDB(statement.get_db_name());
	if (db == NULL) throw DatabaseNotExistException();

	if (current_db_.length() != 0)
	{
		cout << "Closing the old database: " << current_db_ << endl;
		catalog_m_->WriteArchiveFile();
		delete buffer_m_;
	}
	current_db_ = statement.get_db_name();
	buffer_m_ = new BufferManager(path_);
	cout << endl << "Database changed" << endl;
}

void API::Insert(SQLInsert& statement)
{
	if (current_db_.length() == 0) throw NoDatabaseSelectedException();
	Database *db = catalog_m_->GetDB(current_db_);
	if (db == NULL) throw DatabaseNotExistException();
	
	RecordManager *rm = new RecordManager(catalog_m_, buffer_m_, current_db_);
	rm->Insert(statement);
	delete rm;
}

void API::Select(SQLSelect& statement)
{
	if (current_db_.length() == 0) throw NoDatabaseSelectedException();
	Table *tb = catalog_m_->GetDB(current_db_)->GetTable(statement.get_tb_name());
	if (tb == NULL) throw TableNotExistException();
	
	RecordManager *rm = new RecordManager(catalog_m_, buffer_m_, current_db_);
	rm->Select(statement);
	delete rm;
}

void API::Delete(SQLDelete& statement)
{
	if (current_db_.length() == 0) throw NoDatabaseSelectedException();
	Table *tb = catalog_m_->GetDB(current_db_)->GetTable(statement.get_tb_name());
	if (tb == NULL) throw TableNotExistException();
	
	RecordManager *rm = new RecordManager(catalog_m_, buffer_m_, current_db_);
	rm->Delete(statement);
	delete rm;
}

void API::Update(SQLUpdate& statement)
{
	if (current_db_.length() == 0) throw NoDatabaseSelectedException();
	Database *db = catalog_m_->GetDB(current_db_);
	if (db == NULL) throw DatabaseNotExistException();

	Table *tb = db->GetTable(statement.get_tb_name());
	if (tb == NULL) throw TableNotExistException();
	
	RecordManager *rm = new RecordManager(catalog_m_, buffer_m_, current_db_);
	rm->Update(statement);
	delete rm;
}
