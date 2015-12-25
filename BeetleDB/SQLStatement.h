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
	int get_sql_type();
	void set_sql_type(int sql_type);
	int ParseDataType(vector<string> sql_vector, Attribute &attr, unsigned int pos);
protected:
	int sql_type_;
};

/* class: SQLCreateDatabase */
class SQLCreateDatabase: public SQL
{
public:
	SQLCreateDatabase(vector<string> sql_vector);
	string get_db_name();
	void set_db_name(string dbname);
	void Parse(vector<string> sql_vector);
private:
	string db_name_;
};

/* class: SQLCreateTable: create table tbname (attr1, char(100), attr2, char(20), primary key(attr1));*/
class SQLCreateTable: public SQL 
{
public:
	SQLCreateTable(vector<string> sql_vector);
	string get_tb_name();
	void set_tb_name(string dbname);
	vector<Attribute> GetAttributes();
	void SetAttributes(vector<Attribute> attr);
	void Parse(vector<string> sql_vector);
private:
	string tb_name_;
	vector<Attribute> attrs_;
};

class SQLCreateIndex: public SQL
{
public:
	SQLCreateIndex(vector<string> sql_vector);
	string get_tb_name();
	string get_index_name();
	string get_column_name();
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
	string get_db_name();
	void set_db_name(string dbname);
	void Parse(vector<string> sql_vector);
private:
	string db_name_;
};

class SQLDropTable: public SQL
{
public:
	SQLDropTable(vector<string> sql_vector);
	string get_tb_name();
	void set_tb_name(string tbname);
	void Parse(vector<string> sql_vector);
private:
	string tb_name_;
};

class SQLDropIndex: public SQL
{
public:
	SQLDropIndex(vector<string> sql_vector);
	string get_index_name();
	void set_index_name(string  idxname);
	void Parse(vector<string> sql_vector);
private:
	string index_name_;
};

/* class: SQLUse: use dbname; */
class SQLUse: public SQL
{
public:
	SQLUse(vector<string> sql_vector);
	string get_db_name();
	void set_db_name(string dbname);
	void Parse(vector<string> sql_vector);
private:
	string db_name_;
};

/* Execute sql file */
class SQLExec: public SQL
{
public:
	SQLExec(vector<string> sql_vector);
	string get_file_name();
	void Parse(vector<string> sql_vector);
private:
	string file_name_;
};

typedef	struct
{
	int data_type;
	string value;
}SQLValue;

/* eg.: insert into tbname values(val1, val2); */
class SQLInsert: public SQL
{
public:
	SQLInsert(vector<string> sql_vector);
	string get_tb_name();
	vector<SQLValue>& GetValues();  //reference
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
	string get_tb_name();
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
	string get_tb_name();
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
	string get_tb_name();
	vector<SQLWhere>& GetWheres();
	vector<SQLKeyValue>& GetKeyValues();
	void Parse(vector<string> sql_vector);
private:
	string tb_name_;
	vector<SQLWhere> wheres_;
	vector<SQLKeyValue> keyvalues_;
};

class TKey
{
public:
	TKey(int keytype, int length);
	TKey(const TKey& t1);
	~TKey();
	void ReadValue(const char *content);
	void ReadValue(string content);

	int get_key_type();
	char* get_key();
	int get_length();

	friend std::ostream & operator<<(std::ostream& out, const TKey& object);
	bool operator<(const TKey t1);
	bool operator>(const TKey t1);
	bool operator<=(const TKey t1);
	bool operator>=(const TKey t1);
	bool operator==(const TKey t1);
	bool operator!=(const TKey t1);
private:
	int key_type_;
	char *key_;
	int length_;
};

#endif