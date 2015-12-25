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

int SQL::get_sql_type() { return sql_type_; }

void SQL::set_sql_type(int sql_type) { sql_type_ = sql_type; }

int SQL::ParseDataType(vector<string> sql_vector, Attribute &attr, unsigned int pos)
{
	boost::algorithm::to_lower(sql_vector[pos]);
	if (sql_vector[pos] == "int")
	{
		//cout << "SQL::ParseDataType().logging: Type: int" << endl;
		attr.set_data_type(T_INT);
		attr.set_length(4);
		pos ++;
		if (sql_vector[pos] == ",") pos ++;
	}
	else if (sql_vector[pos] == "float")
	{
		//cout << "SQL::ParseDataType().logging: Type: float" << endl;
		attr.set_data_type(T_FLOAT);
		attr.set_length(4);
		pos ++;
		if (sql_vector[pos] == ",") pos ++;
	}
	else if (sql_vector[pos] == "char" || sql_vector[pos] == "varchar")
	{
		//cout << "SQL::ParseDataType().logging: Type: char" << endl;
		attr.set_data_type(T_CHAR);
		pos ++;
		if (sql_vector[pos] == "(") pos ++;
		attr.set_length(atoi(sql_vector[pos].c_str()));
		pos ++;
		if (sql_vector[pos] == ")") pos ++;
		if (sql_vector[pos] == ",") pos ++;
	}
	else throw SyntaxErrorException();
	
	return pos;
}

/* -------------- SQLCreateDatabase ----------------- */
SQLCreateDatabase::SQLCreateDatabase(vector<string> sql_vector) { Parse(sql_vector); }

string SQLCreateDatabase::get_db_name() { return db_name_; }

void SQLCreateDatabase::set_db_name(string dbname) { db_name_ = dbname; }

void SQLCreateDatabase::Parse(vector<string> sql_vector)
{
	sql_type_ = 30;
	if (sql_vector.size() <= 2) throw SyntaxErrorException();
	else
	{
		//cout << "logging: DB Name: " << sql_vector[2] << endl;
		db_name_ = sql_vector[2];
	}
}

/* -------------- SQLCreateTable ----------------- */
SQLCreateTable::SQLCreateTable(vector<string> sql_vector) { Parse(sql_vector); }

string SQLCreateTable::get_tb_name() { return tb_name_; }

void SQLCreateTable::set_tb_name(string dbname) { tb_name_ = dbname; }

vector<Attribute> SQLCreateTable::GetAttributes() { return attrs_; }

void SQLCreateTable::SetAttributes(vector<Attribute> attr) { attrs_ = attr; }

void SQLCreateTable::Parse(vector<string> sql_vector)
{
	sql_type_ = 31;
	unsigned int pos = 2;
	if (sql_vector.size() <= pos) throw SyntaxErrorException();
	
	//cout << "SQLCreateTable::Parse().logging: Table Name: " << sql_vector[pos] << endl;
	tb_name_ = sql_vector[pos];
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
			//cout << "SQLCreateTable::Parse().logging: Column: " << sql_vector[pos] << endl;
			Attribute attr;
			attr.set_attr_name(sql_vector[pos]);
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
				if ((*att).get_attr_name() == sql_vector[pos])
				{
					(*att).set_attr_type(1);
					//cout << "SQLCreateTable::Parse().logging: Primary Key:" << sql_vector[pos] << endl;
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

string SQLCreateIndex::get_tb_name() { return tb_name_; }

string SQLCreateIndex::get_index_name() { return index_name_; }

string SQLCreateIndex::get_column_name() { return col_name_; }

void SQLCreateIndex::Parse(vector<string> sql_vector)
{
	sql_type_ = 32;
	unsigned int pos = 2;
	if (sql_vector.size() <= pos) throw SyntaxErrorException();
	
	//cout << "SQLCreateIndex::Parse().logging: Index Name: " << sql_vector[pos] << endl;
	index_name_ = sql_vector[pos];
	pos ++;
	
	if (boost::algorithm::to_lower_copy(sql_vector[pos]) != "on") throw SyntaxErrorException();
	pos ++;

	//cout << "SQLCreateIndex::Parse().logging: Table Name: " << sql_vector[pos] << endl;
	tb_name_ = sql_vector[pos];
	pos ++;

	if (boost::algorithm::to_lower_copy(sql_vector[pos]) != "(") throw SyntaxErrorException();
	pos ++;

	//cout << "SQLCreateIndex::Parse().logging:: Column Name: " << sql_vector[pos] << endl;
	col_name_ = sql_vector[pos];
	pos ++;

	if (boost::algorithm::to_lower_copy(sql_vector[pos]) != ")") throw SyntaxErrorException();
	pos ++;
}

/* -------------- SQLDropDatabase ----------------- */
SQLDropDatabase::SQLDropDatabase(vector<string> sql_vector) { Parse(sql_vector); }

string SQLDropDatabase::get_db_name() { return db_name_; }

void SQLDropDatabase::set_db_name(string dbname) { db_name_ = dbname; }

void SQLDropDatabase::Parse(vector<string> sql_vector)
{
	sql_type_ = 50;
	if (sql_vector.size() <= 2) throw SyntaxErrorException();
	else
	{
		//cout << "SQLDropDatabase::Parse().logging: DB Name: " << sql_vector[2] << endl;
		db_name_ = sql_vector[2];
	}
}

/* -------------- SQLDropTable -------------------- */
SQLDropTable::SQLDropTable(vector<string> sql_vector) { Parse(sql_vector); }

string SQLDropTable::get_tb_name() { return tb_name_; }

void SQLDropTable::set_tb_name(string tbname) { tb_name_ = tbname; }

void SQLDropTable::Parse(vector<string> sql_vector)
{
	sql_type_ = 51;
	if (sql_vector.size() <= 2) throw SyntaxErrorException();
	else tb_name_ = sql_vector[2];
}

/* -------------- SQLDropIndex ------------------- */
SQLDropIndex::SQLDropIndex(vector<string> sql_vector) { Parse(sql_vector); }

string SQLDropIndex::get_index_name() { return index_name_; }

void SQLDropIndex::set_index_name(string idxname) { index_name_ = idxname; }

void SQLDropIndex::Parse(vector<string> sql_vector)
{
	sql_type_ = 52;
	if (sql_vector.size() <= 2) throw SyntaxErrorException();
	else index_name_ = sql_vector[2];
}

/* -------------- SQLUse ----------------- */
SQLUse::SQLUse(vector<string> sql_vector) { Parse(sql_vector); }

string SQLUse::get_db_name() { return db_name_; }

void SQLUse::set_db_name(string dbname) { db_name_ = dbname; }

void SQLUse::Parse(vector<string> sql_vector)
{
	sql_type_ = 60;
	if (sql_vector.size() <= 1) throw SyntaxErrorException();
	else
	{
		//cout << "SQLUse::Parse().logging: DB Name: " << sql_vector[1] << endl;
		db_name_ = sql_vector[1];
	}
}

/* ----------------- SQLExec ----------------*/
SQLExec::SQLExec(vector<string> sql_vector) { Parse(sql_vector); }

string SQLExec::get_file_name() { return file_name_; }

void SQLExec::Parse(vector<string> sql_vector)
{
	sql_type_ = 80;
	if (sql_vector.size() <= 1) throw SyntaxErrorException();
	else file_name_ = sql_vector[1];
}

/* ----------------- SQLInsert ------------------ */
SQLInsert::SQLInsert(vector<string> sql_vector) { Parse(sql_vector); }

string SQLInsert::get_tb_name() { return tb_name_; }

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
			sql_value.data_type = 2;
		}
		else
		{
			if (val.find('.') == string::npos) sql_value.data_type = 0;
			else sql_value.data_type = 1;
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

string SQLSelect::get_tb_name() { return tb_name_; }

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

string SQLDelete::get_tb_name() { return tb_name_; }

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

string SQLUpdate::get_tb_name() { return tb_name_; }

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

/* ----------------- TKey -------------------- */
TKey::TKey(int keytype, int length)
{
	key_type_ = keytype;
	if (keytype == 2) length_ = length; //char(100)
	else length_ = 4;
	key_ = new char[length_];

}

TKey::TKey(const TKey& t1)
{
	key_type_ = t1.key_type_;
	length_ = t1.length_;
	key_ = new char[length_];
	memcpy(key_, t1.key_, length_);
}

TKey::~TKey()
{
	if (key_ != NULL)
		delete []key_;
}

void TKey::ReadValue(const char *content)
{
	switch (key_type_)
	{
	case 0:
	{
		int a = atoi(content);
		memcpy(key_, &a, length_);
	}
		break;
	case 1:
	{
		float b = atof(content);
		memcpy(key_, &b, length_);
	}
		break;
	case 2:
		memcpy(key_, content, length_);
		break;
	}
}

void TKey::ReadValue(string content)
{
	switch (key_type_)
	{
	case 0:
	{
		int a = atoi(content.c_str());
		memcpy(key_, &a, length_);
	}
		break;
	case 1:
	{
		float b = atof(content.c_str());
		memcpy(key_, &b, length_);
	}
		break;
	case 2:
		memcpy(key_, content.c_str(), length_);
		break;
	}
}

int TKey::get_key_type(){ return key_type_; }
char* TKey::get_key(){ return key_; }
int TKey::get_length(){ return length_; }

std::ostream & operator<<(std::ostream& out, const TKey& object)
{
	switch (object.key_type_)
	{
	case 0:
	{
		int a;
		memcpy(&a, object.key_, object.length_);
		cout << setw(9) << left <<  a;
	}
		break;
	case 1:
	{
		float a;
		memcpy(&a, object.key_, object.length_);
		cout << setw(9) << left <<  a;
	}
		break;
	case 2:
	{
		cout << setw(9) << left << object.key_;
	}
		break;
	}
	return out;
}

bool TKey::operator<(const TKey t1)
{
	switch (t1.key_type_)
	{
	case 0:
		return *(int*)key_ < *(int*)t1.key_;
	case 1:
		return *(float*)key_ < *(float*)t1.key_;
	case 2:
		return (strncmp(key_, t1.key_, length_) < 0);
	default:
		return false;
	}
}

bool TKey::operator>(const TKey t1)
{
	switch (t1.key_type_)
	{
	case 0:
		return *(int*)key_ > *(int*)t1.key_;
	case 1:
		return *(float*)key_ > *(float*)t1.key_;
	case 2:
		return (strncmp(key_, t1.key_, length_) > 0);
	default:
		return false;
	}
}

bool TKey::operator==(const TKey t1) 
{
	switch (t1.key_type_)
	{
	case 0:
		return *(int*)key_ == *(int*)t1.key_;
	case 1:
		return *(float*)key_ == *(float*)t1.key_;
	case 2:
		return (strncmp(key_, t1.key_, length_) == 0);
	default:
		return false;
	}
}

bool TKey::operator<=(const TKey t1) { return !(operator>(t1)); }
bool TKey::operator>=(const TKey t1) { return !(operator<(t1)); }
bool TKey::operator!=(const TKey t1) { return !(operator==(t1)); }