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

/* -------------- SQLDropTable -------------------- */
SQLDropTable::SQLDropTable(vector<string> sql_vector) { Parse(sql_vector); }

string SQLDropTable::GetTableName() { return tb_name_; }

void SQLDropTable::SetTableName(string tbname) { tb_name_ = tbname; }

void SQLDropTable::Parse(vector<string> sql_vector)
{
	sql_type_ = 51;
	if (sql_vector.size() <= 2) throw SyntaxErrorException();
	else tb_name_ = sql_vector[2];
}

/* -------------- SQLDropIndex ------------------- */
SQLDropIndex::SQLDropIndex(vector<string> sql_vector) { Parse(sql_vector); }

string SQLDropIndex::GetIndexName() { return index_name_; }

void SQLDropIndex::SetIndexName(string idxname) { index_name_ = idxname; }

void SQLDropIndex::Parse(vector<string> sql_vector)
{
	sql_type_ = 52;
	if (sql_vector.size() <= 2) throw SyntaxErrorException();
	else index_name_ = sql_vector[2];
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

/* ----------------- SQLExec ----------------*/
SQLExec::SQLExec(vector<string> sql_vector) { Parse(sql_vector); }

string SQLExec::GetFileName() { return file_name_; }

void SQLExec::Parse(vector<string> sql_vector)
{
	sql_type_ = 80;
	if (sql_vector.size() <= 1) throw SyntaxErrorException();
	else file_name_ = sql_vector[1];
}

/* ----------------- SQLInsert ------------------ */
SQLInsert::SQLInsert(vector<string> sql_vector) { Parse(sql_vector); }

string SQLInsert::GetTableName() { return tb_name_; }

vector<SQLValue>& SQLInsert::GetValues() { return values_; }

void SQLInsert::Parse(vector<string> sql_vector)
{
	sql_type_ = 70;
	unsigned int pos = 1;
	bool is_attr = true;

	if (boost::algorithm::to_lower_copy(sql_vector[pos]) != "into") throw SyntaxErrorException();
	pos ++;

	tb_name_ = sql_vector[pos];
	pos ++;

	if (boost::algorithm::to_lower_copy(sql_vector[pos]) != "values") throw SyntaxErrorException();
	pos ++;

	if (sql_vector[pos] != "(") throw SyntaxErrorException();
	pos ++;

	while (is_attr)
	{
		is_attr = false;
		SQLValue sql_value;
		string val = sql_vector[pos];
		if (val[0] == '\'' || val[0] == '\"')
		{
			val.assign(val, 1, val.length()-2);
			sql_value.data_type_ = 2;
		}
		else
		{
			if (val.find('.') == string::npos) sql_value.data_type_ = 0;
			else sql_value.data_type_ = 1;
		}
		sql_value.value = val;
		values_.push_back(sql_value);
		pos ++;
		if (sql_vector[pos] != ")") is_attr = true;
		pos ++;
	}
}

/* ---------------- SQLSelect ----------------- */
SQLSelect::SQLSelect(vector<string> sql_vector) { Parse(sql_vector); }

string SQLSelect::GetTableName() { return tb_name_; }

vector<SQLWhere>& SQLSelect::GetWheres() {  return wheres_; }

void SQLSelect::Parse(vector<string> sql_vector) /* only support "select * ". */
{
	sql_type_ = 90;
	unsigned int pos = 1;
	if (sql_vector.size() <= 1) throw SyntaxErrorException();
	if (sql_vector[pos] != "*") throw SyntaxErrorException(); 
	pos ++;
	
	if (boost::algorithm::to_lower_copy(sql_vector[pos]) != "from") throw SyntaxErrorException();
	pos ++;
	
	tb_name_ = sql_vector[pos];
	pos ++;
	
	if (sql_vector.size() == pos) return; /* sql statement like: "select * from tb;". */
	if (boost::algorithm::to_lower_copy(sql_vector[pos]) != "where") throw SyntaxErrorException();
	pos ++;
	
	while (true)
	{
		SQLWhere sql_where;
		sql_where.key = sql_vector[pos];
		pos ++;

		if (sql_vector[pos] == "<") sql_where.op_type = SIGN_LT;
		else if (sql_vector[pos] == "=") sql_where.op_type = SIGN_EQ;
		else if (sql_vector[pos] == ">") sql_where.op_type = SIGN_GT;
		else if (sql_vector[pos] == "<=") sql_where.op_type = SIGN_LE;
		else if (sql_vector[pos] == ">=") sql_where.op_type = SIGN_GE;
		else if (sql_vector[pos] == "<>") sql_where.op_type = SIGN_NE;
		pos ++;

		sql_where.value = sql_vector[pos];
		if (sql_where.value[0] == '\'' || sql_where.value[0] == '\"') sql_where.value.assign(sql_where.value, 1, sql_where.value.length()-2);
		wheres_.push_back(sql_where);
		pos ++;

		if (sql_vector.size() == pos) break; /* one where condition, break. */
		if (boost::algorithm::to_lower_copy(sql_vector[pos]) != "and") throw SyntaxErrorException();
		pos ++;
	}

}

/* ---------------- SQLDelete ---------------- */
SQLDelete::SQLDelete(vector<string> sql_vector) { Parse(sql_vector); }

string SQLDelete::GetTableName() { return tb_name_; }

vector<SQLWhere>& SQLDelete::GetWheres() {  return wheres_; }

void SQLDelete::Parse(vector<string> sql_vector) /* only support "select * ". */
{
	sql_type_ = 100;
	unsigned int pos = 1;
	if (sql_vector.size() <= 1) throw SyntaxErrorException();
	if (boost::algorithm::to_lower_copy(sql_vector[pos]) != "from") throw SyntaxErrorException();
	pos ++;
	
	tb_name_ = sql_vector[pos];
	pos ++;
	
	if (sql_vector.size() == pos) return; /* sql statement like: "delete from tb;". */
	if (boost::algorithm::to_lower_copy(sql_vector[pos]) != "where") throw SyntaxErrorException();
	pos ++;
	
	while (true)
	{
		SQLWhere sql_where;
		sql_where.key = sql_vector[pos];
		pos ++;

		if (sql_vector[pos] == "<") sql_where.op_type = SIGN_LT;
		else if (sql_vector[pos] == "=") sql_where.op_type = SIGN_EQ;
		else if (sql_vector[pos] == ">") sql_where.op_type = SIGN_GT;
		else if (sql_vector[pos] == "<=") sql_where.op_type = SIGN_LE;
		else if (sql_vector[pos] == ">=") sql_where.op_type = SIGN_GE;
		else if (sql_vector[pos] == "<>") sql_where.op_type = SIGN_NE;
		pos ++;

		sql_where.value = sql_vector[pos];
		if (sql_where.value[0] == '\'' || sql_where.value[0] == '\"') sql_where.value.assign(sql_where.value, 1, sql_where.value.length()-2);
		wheres_.push_back(sql_where);
		pos ++;

		if (sql_vector.size() == pos) break; /* one where condition, break. */
		if (boost::algorithm::to_lower_copy(sql_vector[pos]) != "and") throw SyntaxErrorException();
		pos ++;
	}

}

/* ---------------  SQLUpdate ----------------*/
SQLUpdate::SQLUpdate(vector<string> sql_vector) { Parse(sql_vector); }

string SQLUpdate::GetTableName() { return tb_name_; }

vector<SQLWhere>& SQLUpdate::GetWheres() { return wheres_; }

vector<SQLKeyValue>& SQLUpdate::GetKeyValues() { return keyvalues_; }

void SQLUpdate::Parse(vector<string> sql_vector)
{
	sql_type_ = 110;
	unsigned int pos = 1;
	if (sql_vector.size() <= 1) throw SyntaxErrorException();
	
	tb_name_ = sql_vector[pos];
	pos ++;

	if (boost::algorithm::to_lower_copy(sql_vector[pos]) != "set") throw SyntaxErrorException();
	pos ++;

	while (true)
	{
		SQLKeyValue sql_kv;
		sql_kv.key = sql_vector[pos];
		pos ++;
		
		if (sql_vector[pos] != "=") throw SyntaxErrorException();
		pos ++;

		sql_kv.value = sql_vector[pos];
		if (sql_kv.value[0] == '\'' || sql_kv.value[0] == '\"') sql_kv.value.assign(sql_kv.value, 1, sql_kv.value.length()-2);
		keyvalues_.push_back(sql_kv);
		pos ++;

		if (sql_vector[pos] == ",") pos ++;
		else if (boost::algorithm::to_lower_copy(sql_vector[pos]) == "where") break;
		else throw SyntaxErrorException();
	}
	if (sql_vector[pos] != "where") throw SyntaxErrorException();
	pos ++;

	while (true)
	{
		SQLWhere sql_where;
		sql_where.key = sql_vector[pos];
		pos ++;

		if (sql_vector[pos] == "<") sql_where.op_type = SIGN_LT;
		else if (sql_vector[pos] == "=") sql_where.op_type = SIGN_EQ;
		else if (sql_vector[pos] == ">") sql_where.op_type = SIGN_GT;
		else if (sql_vector[pos] == "<=") sql_where.op_type = SIGN_LE;
		else if (sql_vector[pos] == ">=") sql_where.op_type = SIGN_GE;
		else if (sql_vector[pos] == "<>") sql_where.op_type = SIGN_NE;
		pos ++;

		sql_where.value = sql_vector[pos];
		if (sql_where.value[0] == '\'' || sql_where.value[0] == '\"') sql_where.value.assign(sql_where.value, 1, sql_where.value.length()-2);
		wheres_.push_back(sql_where);
		pos ++;

		if (sql_vector.size() == pos) break; /* one where condition, break. */
		if (boost::algorithm::to_lower_copy(sql_vector[pos]) != "and") throw SyntaxErrorException();
		pos ++;
	}
}

