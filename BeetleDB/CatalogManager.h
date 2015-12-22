//
// CatalogManager.h
//
// Created by Chenbjin on 2015/12/20.
// Copyright(C) 2015, Chenbjin. All rights reserved.
//
#pragma once
#ifndef _CATALOGMANAGER_H_
#define _CATALOGMANAGER_H_

#include "SQLStatement.h"
#include <vector>
#include <string>

#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/utility.hpp>
#include <boost/serialization/vector.hpp>

using namespace std;

class Database;
class Table;
class Attribute;
class Index;
class SQLCreateTable;
class SQLDropTable;
class SQLDropIndex;

class CatalogManager
{
public:
	CatalogManager(string path);
	~CatalogManager(void);
	string get_path();
	vector<Database>& GetDBs();
	Database* GetDB(string db_name);
	void ReadArchiveFile();
	void WriteArchiveFile();
	void CreateDatabase(string dbname);
	void DeleteDatabase(string dbname);
private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version)
	{
		ar & dbs_;
	}
	string path_;
	vector<Database> dbs_;
};

class Database {
public:
	Database();
	Database(string dbname);
	~Database();
	string get_db_name();
	vector<Table>& GetTables();
	Table* GetTable(string tb_name);
	void CreateTable(SQLCreateTable& st);
	void DropTable(SQLDropTable& st);
	void DropIndex(SQLDropIndex& st);
	bool CheckIfIndexExists(string index_name);
private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version)
	{
		ar & db_name_;
		ar & tbs_;
	}
	string db_name_;
	vector<Table> tbs_;
};

class Table {
public:
	Table();
	~Table();
	string get_tb_name();
	void set_tb_name(string tbname);
	int get_record_length();
	void set_record_length(int len);
	
	vector<Attribute>& GetAttributes();
	Attribute* GetAttribute(string name);
	int GetAttributeIndex(string name);

	int get_first_block_num();
	void set_first_block_num(int num);
	int get_first_rubbish_num();
	void set_first_rubbish_num(int num);
	int get_block_count();

	unsigned long GetAttributeNum();
	void AddAttribute(Attribute& attr);
	void IncreaseBlockCount();
	
	std::vector<Index>& get_ids();
	Index* GetIndex(int num);
	unsigned long GetIndexNum();
	void AddIndex(Index& idx);
private:
    friend class boost::serialization::access;
    
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
        ar & tb_name_;
        ar & record_length_;
        ar & first_block_num_;
        ar & first_rubbish_num_;
        ar & block_count_;
        ar & ats_;
        ar & ids_;
    } 
    string tb_name_;
    int record_length_;
    int first_block_num_;
    int first_rubbish_num_;
    int block_count_;
    
    std::vector<Attribute> ats_;
    std::vector<Index> ids_;
};

class Attribute
{
public:
	Attribute();
	~Attribute();
	
	string get_attr_name();
	void set_attr_name(string name);
	
	int get_attr_type();
	void set_attr_type(int type);

	int get_data_type();
	void set_data_type(int type);

	int get_length();
	void set_length(int length);

private:
	friend class boost::serialization::access;

	template<class Archive>
	void serialize(Archive & ar, const unsigned int version)
	{
		ar & attr_name_;
		ar & data_type_;
		ar & length_;
		ar & attr_type_;
	}

	string attr_name_;
	int data_type_;
	int length_;
	int attr_type_;
};

class Index {
public:
	Index();
    Index(std::string name, std::string attr_name, int keytype, int keylen,int rank);
    
    // accessors and mutators
	string get_attr_name();
	string get_name();
	int get_key_len();
	int get_key_type();
	int get_rank();
	int get_root();
	void set_root(int root);
	int get_leaf_head();
	void set_leaf_head(int leaf_head);
	int get_key_count();
	void set_key_count(int key_count);
	int get_level();
	void set_level(int level);
	int get_node_count();
	void set_node_count(int node_count);
	
	int IncreaseMaxCount();
	int IncreaseKeyCount();
	int IncreaseNodeCount();
	int IncreaseLevel();
	int DecreaseKeyCount();
	int DecreaseNodeCount();
	int DecreaseLevel();
private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version)
	{
		ar & max_count_;
		ar & attr_name_;
		ar & name_;
		ar & key_len_;
		ar & key_type_;
		ar & rank_;
		ar & rubbish_;
		ar & root_;
		ar & leaf_head_;
		ar & key_count_;
		ar & level_;
		ar & node_count_;
	}
	int max_count_;
	int key_len_;
	int key_type_;
	int rank_;
	int rubbish_;
	int root_;
	int leaf_head_;
	int key_count_;
	int level_;
	int node_count_;
	string attr_name_;
	string name_;
};

#endif