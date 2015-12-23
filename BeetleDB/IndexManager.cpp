//
// IndexManager.cpp
//
// Created by Chenbjin on 2015/12/18.
// Copyright(C) 2015, Chenbjin. All rights reserved.
//
#include "IndexManager.h"
#include "Exceptions.h"
#include "RecordManager.h"

#include <string>
#include <fstream>
using namespace std;

IndexManager::IndexManager(CatalogManager *cm, BufferManager *bm, string dbname):catalog_m_(cm), buffer_m_(bm), db_name_(dbname){}

IndexManager::~IndexManager(void){}

void IndexManager::CreateIndex(SQLCreateIndex& st)
{
	string tb_name = st.get_tb_name();
	Table *tb = catalog_m_->GetDB(db_name_)->GetTable(tb_name);
	if (tb == NULL) throw TableNotExistException();
	if (tb->GetIndexNum() != 0) throw OneIndexEachTableException();

	Attribute *attr = tb->GetAttribute(st.get_column_name());
	if (attr->get_attr_type() != 1) throw IndexMustBeCreatedOnPrimaryKeyException();

	string file_name = catalog_m_->get_path() + db_name_ + "/" + st.get_index_name() + ".index";
	ofstream ofs(file_name.c_str(),ios::binary);
	ofs.close();

	Index idx(st.get_index_name(),st.get_column_name(), attr->get_data_type(), attr->get_length(),
		(4 * 1024 - 12)/ (4 + attr->get_length()) /2 -1);
	tb->AddIndex(idx);

	BPlusTree tree(tb->GetIndex(0),buffer_m_,catalog_m_,db_name_);
	
	RecordManager *rm = new RecordManager(catalog_m_,buffer_m_,db_name_);
	int col_idx = tb->GetAttributeIndex(st.get_column_name());
	int block_num = tb->get_first_block_num();

	for (int i = 0; i < tb->get_block_count(); i++)
	{
		BlockInfo
	}

}
