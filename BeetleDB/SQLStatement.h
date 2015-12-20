//
// SQLStatement.h
//
// Created by Chenbjin on 2015/12/18.
// Copyright(C) 2015, Chenbjin. All rights reserved.
//
#pragma once
#ifndef _SQLSTATEMENT_H_
#define _SQLSTATEMENT_H_

#include <string>
#include <vector>

#include "CatalogManager.h"

using namespace std;

class CatalogManager;
class Database;
class Table;
class Attribute;
class Index;

class SQL
{
public:
	SQL();
	SQL(int sql_type);
	virtual ~SQL();
	virtual void Parse(vector<string> sql_vector) = 0;
	int GetSQLType();
	void SetSQLType(int sql_type);
	int ParseDataType(vector<string> sql_vector, Attribute &attr, unsigned int pos);
protected:
	int sql_type_;
};

class SQLCreateDatabase: public SQL
{
public:
	SQLCreateDatabase(vector<string> sql_vector);
	string GetDBName();
	void SetDBName(string dbname);
	void Parse(vector<string> sql_vector);

private:
	string db_name_;
};

class SQLCreateTable: public SQL 
{
public:
	SQLCreateTable(vector<string> sql_vector);
	string GetDBName();
	void SetDBName(string dbname);
	vector<Attribute> GetAttributes();
	void SetAttributes(vector<Attribute> attr);
	void Parse(vector<string> sql_vector);
private:
	string db_name_;
	vector<Attribute> attrs_;
};

class SQLCreateIndex: public SQL
{
public:
	SQLCreateIndex(vector<string> sql_vector);
	string GetTableName();
	string GetIndexName();
	string GetColumnName();
	void Parse(vector<string> sql_vector);
private:
	string index_name_;
	string tb_name_;
	string col_name_;
};

class SQLDropDatabase: public SQL 
{
public:
	SQLDropDatabase(vector<string> sql_vector);
	string GetDBName();
	void SetDBName(string dbname);
	void Parse(vector<string> sql_vector);
private:
	string db_name_;
};

class SQLUse: public SQL
{
public:
	SQLUse(vector<string> sql_vector);
	string GetDBName();
	void SetDBName(string dbname);
	void Parse(vector<string> sql_vector);
private:
	string db_name_;
};

#endif