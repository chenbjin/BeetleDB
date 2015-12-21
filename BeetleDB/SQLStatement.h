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

/* Base class: SQL (sql statement) */
class SQL
{
public:
	SQL();
	SQL(int sql_type);
	virtual void Parse(vector<string> sql_vector) = 0;
	int GetSQLType();
	void SetSQLType(int sql_type);
	int ParseDataType(vector<string> sql_vector, Attribute &attr, unsigned int pos);
protected:
	int sql_type_;
};

/* class: SQLCreateDatabase 
	eg.: create database dbname;
*/
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

/* class: SQLCreateTable
	eg.: create table tbname (attr1, char(100), attr2, char(20), primary key(attr1));
*/
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

class SQLDropTable: public SQL
{
public:
	SQLDropTable(vector<string> sql_vector);
	string GetTableName();
	void SetTableName(string tbname);
	void Parse(vector<string> sql_vector);
private:
	string tb_name_;
};

class SQLDropIndex: public SQL
{
public:
	SQLDropIndex(vector<string> sql_vector);
	string GetIndexName();
	void SetIndexName(string  idxname);
	void Parse(vector<string> sql_vector);
private:
	string index_name_;
};

/* class: SQLUse
	eg.: use dbname;
*/
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

/* Execute sql file */
class SQLExec: public SQL
{
public:
	SQLExec(vector<string> sql_vector);
	string GetFileName();
	void Parse(vector<string> sql_vector);
private:
	string file_name_;
};

typedef	struct
{
	int data_type_;
	string value;
}SQLValue;

/* eg.: insert into tbname values(val1, val2); */
class SQLInsert: public SQL
{
public:
	SQLInsert(vector<string> sql_vector);
	string GetTableName();
	vector<SQLValue>& GetValues();  //??reference
	void Parse(vector<string> sql_vector);
private:
	string tb_name_;
	vector<SQLValue> values_;
};

typedef struct
{
	int op_type;
	string key;
	string value;
}SQLWhere;

class SQLSelect: public SQL
{
public:
	SQLSelect(vector<string> sql_vector);
	string GetTableName();
	vector<SQLWhere>& GetWheres();
	void Parse(vector<string> sql_vector);
private:
	string tb_name_;
	vector<SQLWhere> wheres_;
};

class SQLDelete: public SQL
{
public:
	SQLDelete(vector<string> sql_vector);
	string GetTableName();
	vector<SQLWhere>& GetWheres();
	void Parse(vector<string> sql_vector);
private:
	string tb_name_;
	vector<SQLWhere> wheres_;
};

typedef struct
{
	string key;
	string value;
}SQLKeyValue;

class SQLUpdate: public SQL
{
public:
	SQLUpdate(vector<string> sql_vector);
	string GetTableName();
	vector<SQLWhere>& GetWheres();
	vector<SQLKeyValue>& GetKeyValues();
	void Parse(vector<string> sql_vector);
private:
	string tb_name_;
	vector<SQLWhere> wheres_;
	vector<SQLKeyValue> keyvalues_;
};

#endif