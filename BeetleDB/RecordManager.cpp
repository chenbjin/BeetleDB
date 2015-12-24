//
// RecordManager.cpp
//
// Created by Chenbjin on 2015/12/18.
// Copyright(C) 2015, Chenbjin. All rights reserved.
//
#include "RecordManager.h"


RecordManager::RecordManager(CatalogManager *cm, BufferManager *bm, string dbname):catalog_m_(cm), buffer_m_(bm), db_name_(dbname) {}

RecordManager::~RecordManager(void){}

void RecordManager::Insert(SQLInsert& st){}
void RecordManager::Select(SQLSelect& st){}
void RecordManager::Delete(SQLDelete& st){}
void RecordManager::Update(SQLUpdate& st){}

BlockInfo* RecordManager::GetBlockInfo(Table* tbl, int block_num)
{
	if (block_num == -1) return NULL;
	BlockInfo* block = buffer_m_->GetFileBlock(db_name_, tbl->get_tb_name(), 0, block_num);
	return block;
}

vector<TKey> RecordManager::GetRecord(Table* tbl, int block_num, int offset)
{
	vector<TKey> keys;
	BlockInfo *bp = GetBlockInfo(tbl, block_num);
	char *content = bp->get_data() + offset * tbl->get_record_length() + 12;
	
	for(int i = 0; i < tbl->GetAttributeNum(); ++i)
	{
		int value_type = tbl->GetAttributes()[i].get_data_type();
		int length = tbl->GetAttributes()[i].get_length();
		
		TKey tmp(value_type, length);
		memcpy(tmp.get_key(), content, length);
		
		keys.push_back(tmp);
		content += length;
	}
	return keys;
}

void RecordManager::DeleteRecord(Table* tbl, int block_num, int offset){}
void RecordManager::UpdateRecord(Table* tbl, int block_num, int offset, vector<int>& indices, vector<TKey>& values){}
bool RecordManager::SatisfyWhere(Table* tbl, vector<TKey> keys, SQLWhere where){}