//
// CatalogManager.cpp
//
// Created by Chenbjin on 2015/12/18.
// Copyright(C) 2015, Chenbjin. All rights reserved.
//
#include "CatalogManager.h"
#include <fstream>
#include <boost/filesystem.hpp>

/* ---------------- CatalogManager ------------ */
CatalogManager::CatalogManager(string path):path_(path) { ReadArchiveFile(); }

CatalogManager::~CatalogManager(void) { WriteArchiveFile();}

string CatalogManager::get_path(){ return path_; }

vector<Database>& CatalogManager::GetDBs(){ return dbs_; }

Database* CatalogManager::GetDB(string db_name)
{
	for (auto db = dbs_.begin(); db != dbs_.end(); db++)
		if (db->get_db_name() == db_name) 
			return &(*db);    //some problem: exception?
	return NULL;
}
/* Read archive file(binary) to current catalog_m_*/
void CatalogManager::ReadArchiveFile()
{
	string file_name = path_ + "catalog";
	boost::filesystem::path file_path(file_name);

	//file_path.imbue(locale("en.US.UTF-8"));  /* encode */
	if (boost::filesystem::exists(file_path))
	{
		ifstream ifs;
		ifs.open(file_name.c_str(), ios::binary); /* open catalog with binary code */
		boost::archive::binary_iarchive iar( ifs ); /* iarchive file (unzip) */
		iar >> (*this);    /* asign to current catalog_m_; */
		ifs.close();
	}
}

/* Write catalog to binary file. */
void CatalogManager::WriteArchiveFile()
{
	string file_name = path_ + "catalog";

	ofstream ofs;
	ofs.open(file_name.c_str(), ios::binary);
	boost::archive::binary_oarchive oar( ofs );
	oar << (*this);
	ofs.close();
}

void CatalogManager::CreateDatabase(string dbname)
{
	dbs_.push_back(Database(dbname));
}

void CatalogManager::DeleteDatabase(string dbname)
{
	for (auto i = 0; i < dbs_.size(); i++)
		if (dbs_[i].get_db_name() == dbname)
			dbs_.erase(dbs_.begin() + i);
	/*
	if (dbs_.size() == 0) return;
	for (auto db = dbs_.begin(); db != dbs_.end() ; db++) // error happened! should validate! 
	{
		if (db->get_db_name() == dbname)
			dbs_.erase(db);  
	}*/
}

/* ---------------- Database ------------ */
Database::Database(){}
Database::Database(string dbname):db_name_(dbname){}
Database::~Database(){}

string Database::get_db_name() { return db_name_; }

vector<Table>& Database::GetTables() { return tbs_; }

Table* Database::GetTable(string tb_name)
{
	for (auto tb = tbs_.begin(); tb != tbs_.end(); tb++)
	{
		if (tb->get_tb_name() == tb_name)
			return &(*tb);
	}
	return NULL;
}

void Database::CreateTable(SQLCreateTable& st) 
{
	int record_length = 0;
	Table tb;
	vector<Attribute> attrs = st.GetAttributes();
	for (auto attr = attrs.begin(); attr != attrs.end(); attr++)
	{
		tb.AddAttribute((*attr));
		record_length += (*attr).get_length();
	}
	tb.set_tb_name(st.get_tb_name());
	tb.set_record_length(record_length);
	tbs_.push_back(tb);
}

void Database::DropTable(SQLDropTable& st)
{
	for (auto tb = tbs_.begin(); tb != tbs_.end(); tb++)
	{
		if (tb->get_tb_name() == st.get_tb_name())
		{
			tbs_.erase(tb);
			return;
		}
	}
}

void Database::DropIndex(SQLDropIndex& st)
{
	for (auto tb = tbs_.begin(); tb != tbs_.end(); tb++)
	{
		for (auto idx = tb->get_ids().begin(); idx != tb->get_ids().end(); idx++)
		{
			if (idx->get_name() == st.get_index_name())
			{
				tb->get_ids().erase(idx);
				return ;
			}
		}
	}
}

bool Database::CheckIfIndexExists(string index_name)
{
	bool exists = false;
	for (auto tb = tbs_.begin(); tb != tbs_.end(); tb++)
	{
		for (auto idx = tb->get_ids().begin(); idx != tb->get_ids().end(); idx++)
		{
			if (idx->get_name() == index_name)
			{
				exists = true;
				return exists;
			}
		}
	}
	return exists;
}

/* ----------------- Table ---------------------- */
Table::Table():tb_name_(""),record_length_(-1),first_block_num_(-1),first_rubbish_num_(-1),block_count_(0) {}
Table::~Table(){}

string Table::get_tb_name() { return tb_name_; }
void Table::set_tb_name(string tbname) { tb_name_ = tbname; }
int Table::get_record_length() { return record_length_; }
void Table::set_record_length(int len) { record_length_ = len; } 

vector<Attribute>& Table::GetAttributes() { return ats_; }

Attribute* Table::GetAttribute(string name)
{
	for (auto attr = ats_.begin(); attr != ats_.end(); attr++)
	{
		if (attr->get_attr_name() == name)
			return &(*attr);
	}
	return NULL;
}

int Table::GetAttributeIndex(string name)
{
	for (unsigned int i = 0; i < ats_.size(); i++)
	{
		if (ats_[i].get_attr_name() == name)
			return i;
	}
	return -1;
}

int Table::get_first_block_num() { return first_block_num_; }
void Table::set_first_block_num(int num) { first_block_num_ = num; }

int Table::get_first_rubbish_num() { return first_rubbish_num_; }
void Table::set_first_rubbish_num(int num) { first_rubbish_num_ = num; }

int Table::get_block_count() { return block_count_; }

unsigned long Table::GetAttributeNum() { return ats_.size(); }
void Table::AddAttribute(Attribute& attr) { ats_.push_back(attr); }
void Table::IncreaseBlockCount() { block_count_++; }

vector<Index>& Table::get_ids() { return ids_; }
Index* Table::GetIndex(int num) { return &(ids_[num]); }
unsigned long Table::GetIndexNum() { return ids_.size(); }
void Table::AddIndex(Index& idx) { ids_.push_back(idx); }

/* --------------- Attribute ----------------- */
Attribute::Attribute():attr_name_(""),data_type_(-1),length_(-1),attr_type_(0){}
Attribute::~Attribute(){}

string Attribute::get_attr_name(){ return attr_name_; }
void Attribute::set_attr_name(string name) { attr_name_ = name; }
	
int Attribute::get_attr_type() { return attr_type_; }
void Attribute::set_attr_type(int type) { attr_type_ = type; }

int Attribute::get_data_type() { return data_type_; }
void Attribute::set_data_type(int type) { data_type_ = type; }

int Attribute::get_length() { return length_; }
void Attribute::set_length(int length) { length_ = length; }

/* --------------- Index ------------------------ */
Index::Index() {}
Index::Index(std::string name, std::string attr_name, int keytype, int keylen,int rank)
{
	attr_name_ = attr_name;
	name_ = name;
	key_count_ = 0;
	level_ = -1;
	root_ = -1;
	leaf_head_ = -1;
	key_type_ = keytype;
	key_len_ = keylen;
	rank_ = rank;
	rubbish_ = -1;
	max_count_ = 0;
}

string Index::get_attr_name() { return attr_name_; }
string Index::get_name() { return name_; }

int Index::get_key_len() { return key_len_; }
int Index::get_key_type() { return key_type_; }
int Index::get_rank() { return rank_; }

int Index::get_root() { return root_; }
void Index::set_root(int root) { root_ = root; }

int Index::get_leaf_head() { return leaf_head_; }
void Index::set_leaf_head(int leaf_head) { leaf_head_ = leaf_head; }

int Index::get_key_count() { return key_count_; }
void Index::set_key_count(int key_count) { key_count_ = key_count; } 

int Index::get_level() { return level_; }
void Index::set_level(int level) { level_ = level; }

int Index::get_node_count() { return node_count_; }
void Index::set_node_count(int node_count) { node_count_ = node_count; }

int Index::IncreaseMaxCount() { return max_count_++; }
int Index::IncreaseKeyCount() { return key_count_++; }
int Index::IncreaseNodeCount() { return node_count_++; }
int Index::IncreaseLevel() { return level_++; }
int Index::DecreaseKeyCount() { return key_count_--; }
int Index::DecreaseNodeCount() { return node_count_--; }
int Index::DecreaseLevel() { return level_--; }