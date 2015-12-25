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
	/* get the correct table to create index.*/
	string tb_name = st.get_tb_name();
	Table *tb = catalog_m_->GetDB(db_name_)->GetTable(tb_name);
	if (tb == NULL) throw TableNotExistException();
	if (tb->GetIndexNum() != 0) throw OneIndexEachTableException();

	/* index must be created on primary key. */
	Attribute *attr = tb->GetAttribute(st.get_column_name());
	if (attr->get_attr_type() != 1) throw IndexMustBeCreatedOnPrimaryKeyException();

	/* create a new index file */
	string file_name = catalog_m_->get_path() + db_name_ + "/" + st.get_index_name() + ".index";
	ofstream ofs(file_name.c_str(), ios::binary);
	ofs.close();

	/* new index object */
	Index idx(st.get_index_name(), st.get_column_name(), attr->get_data_type(), attr->get_length(),
		(4 * 1024 - 12)/ (4 + attr->get_length()) / 2 - 1);
	tb->AddIndex(idx);

	/* get all record and build b+ tree. */
	cout << "get all record and build b+ tree." << endl;
	BPlusTree tree(tb->GetIndex(0), buffer_m_, catalog_m_, db_name_);
	RecordManager *rm = new RecordManager(catalog_m_, buffer_m_, db_name_);
	
	int col_idx = tb->GetAttributeIndex(st.get_column_name());
	int block_num = tb->get_first_block_num();

	for (int i = 0; i < tb->get_block_count(); i++)
	{
		BlockInfo *bp = rm->GetBlockInfo(tb, block_num);
		for (int j = 0; j < bp->GetRecordCount(); j++)
		{
			//cout << "i&j: " << i << " " << j <<endl;
			vector<TKey> tkey_value = rm->GetRecord(tb, block_num, j);
			//cout << "after get record" << endl;
			tree.Add(tkey_value[col_idx], block_num, j);
			//cout << "tree.Add()" << endl;
		}
		block_num = bp->GetNextBlockNum();
	}
	delete rm;

	buffer_m_->WriteToDisk();
	catalog_m_->WriteArchiveFile();

	tree.Print();
}
