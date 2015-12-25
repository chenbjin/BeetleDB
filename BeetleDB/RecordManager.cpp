//
// RecordManager.cpp
//
// Created by Chenbjin on 2015/12/18.
// Copyright(C) 2015, Chenbjin. All rights reserved.
//
#include "RecordManager.h"
#include "BPlusTree.h"
#include "StaticValue.h"

#include <iomanip>

RecordManager::RecordManager(CatalogManager *cm, BufferManager *bm, string dbname):catalog_m_(cm), buffer_m_(bm), db_name_(dbname) {}

RecordManager::~RecordManager(void){}

void RecordManager::Insert(SQLInsert& st)
{
	string tb_name = st.get_tb_name();
	auto values_size = st.GetValues().size();
	/* get table */
	Table *tb = catalog_m_->GetDB(db_name_)->GetTable(tb_name);
	if (tb == NULL) throw TableNotExistException();

	/* each block 4K, block head 12bits */
	int max_count = (4096-12)/(tb->get_record_length());
	
	vector<TKey> tkey_values;
	int primary_key_index = -1;

	/* get every value, change to Tkey object */
	for (auto i = 0; i < values_size; i++)
	{
		int value_type = st.GetValues()[i].data_type;
		string value = st.GetValues()[i].value;
		int length = tb->GetAttributes()[i].get_length();

		TKey tmp(value_type, length);
		tmp.ReadValue(value.c_str());
		
		tkey_values.push_back(tmp);
		if (tb->GetAttributes()[i].get_attr_type() == 1) 
			primary_key_index = i;
	}
	
	if (primary_key_index != -1)
	{
		/* check index */
		if (tb->GetIndexNum() != 0)
		{
			BPlusTree tree(tb->GetIndex(0), buffer_m_, catalog_m_, db_name_);
			if (tree.GetVal(tkey_values[primary_key_index]) != -1)
				throw PrimaryKeyConflictException();
		}
		else
		{
			/* check if record exist */
			int block_num = tb->get_first_block_num();
			for (int i = 0; i < tb->get_block_count(); i++)
			{
				BlockInfo *bp = GetBlockInfo(tb, block_num);
				for (int j = 0; j < bp->GetRecordCount(); j++)
				{
					vector<TKey> tuple = GetRecord(tb, block_num,j);
					if (tuple[primary_key_index] == tkey_values[primary_key_index])
						throw PrimaryKeyConflictException();
				}
				block_num = bp->GetNextBlockNum();
			}
		}
	}
	char *content;
	int use_block = tb->get_first_block_num();
	int first_rubbish_block = tb->get_first_rubbish_num();
	int last_use_block;
	int blocknum, offset;
	//find the useful block
	while (use_block != -1)
	{
		last_use_block = use_block;
		BlockInfo *bp = GetBlockInfo(tb, use_block);
		
		if (bp->GetRecordCount() == max_count) /* this block is full */
		{
			use_block = bp->GetNextBlockNum();
			continue;
		}
		content = bp->GetContentAdress() + bp->GetRecordCount() * tb->get_record_length(); /* get content */
		for (auto iter = tkey_values.begin(); iter  != tkey_values.end(); iter ++)
		{
			memcpy(content, iter->get_key(), iter->get_length());
			content += iter->get_length(); //?
		}
		bp->SetRecordCount( 1 + bp->GetRecordCount());
		
		blocknum = use_block;
		offset = bp->GetRecordCount()-1;
		buffer_m_->WriteBlock(bp);

		/* add record to index */
		if (tb->GetIndexNum() != 0)
		{
			BPlusTree tree(tb->GetIndex(0), buffer_m_, catalog_m_, db_name_);
			for (auto i = 0; i < tb->GetAttributes().size(); i++)
			{
				if (tb->GetIndex(0)->get_attr_name() == tb->GetIndex(i)->get_attr_name())
				{
					tree.Add(tkey_values[i], blocknum, offset);
					break;
				}
			}
		}
		buffer_m_->WriteToDisk();
		catalog_m_->WriteArchiveFile();
		cout << "Insert successfully." << endl;
		return;
	}

	if (first_rubbish_block != -1)
	{
		BlockInfo *bp = GetBlockInfo(tb, first_rubbish_block);
		content = bp->GetContentAdress(); /* get content */
		for (auto iter = tkey_values.begin(); iter  != tkey_values.end(); iter ++)
		{
			memcpy(content, iter->get_key(), iter->get_length());
			content += iter->get_length(); //?
		}
		bp->SetRecordCount(1);
		BlockInfo *last_use_block_p = GetBlockInfo(tb, last_use_block);
		last_use_block_p->SetNextBlockNum(first_rubbish_block);

		tb->set_first_rubbish_num(bp->GetNextBlockNum());
		bp->SetPrevBlockNum(last_use_block);
		bp->SetNextBlockNum(-1);

		blocknum = first_rubbish_block;
		offset = 0;

		buffer_m_->WriteBlock(bp);
		buffer_m_->WriteBlock(last_use_block_p);
	}
	else
	{
		/*initial or add a block*/
		int next_block = tb->get_first_block_num();
		if (next_block != -1)
		{
			BlockInfo *up = GetBlockInfo(tb, tb->get_first_block_num());
			up->SetPrevBlockNum(tb->get_block_count());
			buffer_m_->WriteBlock(up);
		}
		tb->set_first_block_num(tb->get_block_count());
		BlockInfo *bp = GetBlockInfo(tb, tb->get_first_block_num());
		
		bp->SetPrevBlockNum(-1);
		bp->SetNextBlockNum(next_block);
		bp->SetRecordCount(1);

		content = bp->GetContentAdress();
		for (auto iter = tkey_values.begin(); iter  != tkey_values.end(); iter ++)
		{
			memcpy(content, iter->get_key(), iter->get_length());
			content += iter->get_length(); 
		}
		blocknum = tb->get_block_count();
		offset = 0;
		buffer_m_->WriteBlock(bp);
		tb->IncreaseBlockCount();
	}
	/* add record to index */
	if (tb->GetIndexNum() != 0)
	{
		BPlusTree tree(tb->GetIndex(0), buffer_m_, catalog_m_, db_name_);
		for (auto i = 0; i < tb->GetAttributes().size(); i++)
		{
			if (tb->GetIndex(0)->get_attr_name() == tb->GetIndex(i)->get_attr_name())
			{
				tree.Add(tkey_values[i], blocknum, offset);
				break;
			}
		}
	}
	buffer_m_->WriteToDisk();
	catalog_m_->WriteArchiveFile();
	cout << "Insert successfully." << endl;
}

void RecordManager::Select(SQLSelect& st)
{
	Table *tb = catalog_m_->GetDB(db_name_)->GetTable(st.get_tb_name());

	vector<vector<TKey> > tuples; //all record satisfy the where condition.
	bool has_index = false;
	int index_idx;
	int where_idx;

	/* check index */
	if (tb->GetIndexNum() != 0)
	{
		for (auto i = 0; i < tb->GetIndexNum(); i++)
		{
			Index *idx = tb->GetIndex(i);
			for (auto j = 0; j < st.GetWheres().size(); j++)
			{
				if (idx->get_attr_name() == st.GetWheres()[j].key && st.GetWheres()[j].op_type == SIGN_EQ)
				{
					has_index = true;
					index_idx = i;
					where_idx = j;
				}
			}
		}
	}

	if (!has_index)
	{
		/* get record from all blocks */
		int block_num = tb->get_first_block_num();
		for (int i = 0; i < tb->get_block_count(); i++)
		{
			BlockInfo *bp = GetBlockInfo(tb, block_num);
			for (int j = 0; j < bp->GetRecordCount(); j++)
			{
				vector<TKey> tuple = GetRecord(tb, block_num, j);
				/* check if satisfy the where condition*/
				bool sats = true;
				for (auto k = 0; k < st.GetWheres().size(); k++)
				{
					SQLWhere where = st.GetWheres()[k];
					if (!SatisfyWhere(tb, tuple, where)) sats = false;
				}
				if (sats) tuples.push_back(tuple);
			}
			block_num = bp->GetNextBlockNum();
		}
	}
	else /* has index */
	{
		BPlusTree tree(tb->GetIndex(index_idx), buffer_m_, catalog_m_, db_name_);

		/* build tkey for search */
		int type = tb->GetIndex(index_idx)->get_key_type();
		int length = tb->GetIndex(index_idx)->get_key_len();
		string value = st.GetWheres()[where_idx].value;
		TKey dest_key(type, length);
		dest_key.ReadValue(value);

		int blocknum = tree.GetVal(dest_key);

		if (blocknum != -1)
		{
			/* shift offset */
			int blockoffset = blocknum;
			blocknum = blocknum >> 16;
			blocknum = blocknum && 0xffff;
			blockoffset = blockoffset & 0xffff;

			vector<TKey> tuple = GetRecord(tb, blocknum, blockoffset);
			bool sats = true;
			for (auto k = 0; k < st.GetWheres().size(); k++)
			{
				SQLWhere where = st.GetWheres()[k];
				if (!SatisfyWhere(tb, tuple, where)) sats = false;
			}
			if (sats) tuples.push_back(tuple);
		}
	}
	if (tuples.size() == 0)
	{
		cout << "Empty set ";
		return;
	}
	/* print attributes name */
	string sline = "";
	for (int i = 0; i < tb->GetAttributes().size(); i++)
	{
		cout <<"+----------";
		sline += "+----------";
	}
	cout <<"+"<<endl;
	sline += "+";

	for (auto attr = tb->GetAttributes().begin(); attr != tb->GetAttributes().end(); attr++)
	{
		cout << "| "<< setw(9) << left << attr->get_attr_name();
	}
	cout << "|" << endl;
	cout << sline << endl;
	/* show select result */
	for (auto tuple = tuples.begin(); tuple != tuples.end(); tuple++)
	{
		for (auto val = tuple->begin(); val != tuple->end(); val++)
		{
			cout << "| "<< setw(10) << (*val);
		}
		cout << "|" << endl;
		cout << sline << endl;
	}

	/* Print index tree. */
	if (tb->GetIndexNum() != 0)
	{
		BPlusTree tree(tb->GetIndex(0), buffer_m_, catalog_m_, db_name_);
		tree.Print();
	}
}

void RecordManager::Delete(SQLDelete& st)
{
	Table *tb = catalog_m_->GetDB(db_name_)->GetTable(st.get_tb_name());
	bool has_index = false;
	int index_idx;
	int where_idx;
	
	/* check index */
	if (tb->GetIndexNum() != 0)
	{
		for (auto i = 0; i < tb->GetIndexNum(); i++)
		{
			Index *idx = tb->GetIndex(i);
			for (auto j = 0; j < st.GetWheres().size(); j++)
			{
				if (idx->get_attr_name() == st.GetWheres()[j].key && st.GetWheres()[j].op_type == SIGN_EQ)
				{
					has_index = true;
					index_idx = i;
					where_idx = j;
				}
			}
		}
	}
	if (!has_index)
	{
		/* get record from all blocks */
		int block_num = tb->get_first_block_num();
		for (int i = 0; i < tb->get_block_count(); i++)
		{
			BlockInfo *bp = GetBlockInfo(tb, block_num);
			for (int j = 0; j < bp->GetRecordCount(); j++)
			{
				vector<TKey> tuple = GetRecord(tb, block_num, j);
				/* check if satisfy the where condition*/
				bool sats = true;
				for (int k = 0; k < st.GetWheres().size(); k++)
				{
					SQLWhere where = st.GetWheres()[k];
					if (!SatisfyWhere(tb, tuple, where)) sats = false;
				}
				if (sats)
				{
					DeleteRecord(tb, block_num, j);
					if (tb->GetIndexNum() != 0)
					{
						BPlusTree tree(tb->GetIndex(index_idx), buffer_m_, catalog_m_, db_name_);
						int idx = -1;
						for (int i = 0; i < tb->GetAttributeNum(); i++)
						{
							if (tb->GetAttributes()[i].get_attr_name() == tb->GetIndex(index_idx)->get_attr_name())
								idx = i;
						}
						tree.Remove(tuple[idx]);
					}
				}
			}
			block_num = bp->GetNextBlockNum();
		}
	}
	else
	{
		BPlusTree tree(tb->GetIndex(index_idx), buffer_m_, catalog_m_, db_name_);

		/* build tkey for search */
		int type = tb->GetIndex(index_idx)->get_key_type();
		int length = tb->GetIndex(index_idx)->get_key_len();
		string value = st.GetWheres()[where_idx].value;
		TKey dest_key(type, length);
		dest_key.ReadValue(value);

		int blocknum = tree.GetVal(dest_key);

		if (blocknum != -1)
		{
			/* shift offset */
			int blockoffset = blocknum;
			blocknum = blocknum >> 16;
			blocknum = blocknum && 0xffff;
			blockoffset = blockoffset & 0xffff;

			vector<TKey> tuple = GetRecord(tb, blocknum, blockoffset);
			bool sats = true;
			for (int k = 0; k < st.GetWheres().size(); k++)
			{
				SQLWhere where = st.GetWheres()[k];
				if (!SatisfyWhere(tb, tuple, where)) sats = false;
			}
			if (sats)
			{
				DeleteRecord(tb, blocknum, blockoffset);
				tree.Remove(dest_key);
			}
		}
	}
	buffer_m_->WriteToDisk();
	cout << "Delete successfully." << endl; 
}

void RecordManager::Update(SQLUpdate& st)
{
	Table *tb = catalog_m_->GetDB(db_name_)->GetTable(st.get_tb_name());

	vector<int> indices;
	vector<TKey> tuple;
	int primary_key_index = -1;
	int affect_index = -1;

	/* find primarykey */
	for(int i = 0; i < tb->GetAttributes().size(); ++i)
	{
		if (tb->GetAttributes()[i].get_attr_type() == 1)
			primary_key_index = i;
	}

	/* find the record index to update */
	for (int i = 0; i < st.GetKeyValues().size(); i++)
	{
		int index = tb->GetAttributeIndex(st.GetKeyValues()[i].key);
		indices.push_back(index);
		TKey value(tb->GetAttributes()[index].get_data_type(), tb->GetAttributes()[index].get_length());
		value.ReadValue(st.GetKeyValues()[i].value);
		tuple.push_back(value);
		
		if (index == primary_key_index) affect_index = i;
	}

	if (affect_index != -1)
	{
		if (tb->GetIndexNum() != 0)
		{
			BPlusTree tree(tb->GetIndex(0), buffer_m_, catalog_m_, db_name_);
			if (tree.GetVal(tuple[affect_index]) != -1)
				throw PrimaryKeyConflictException();
		}
		else
		{
			int block_num = tb->get_first_block_num();
			for (int i = 0; i < tb->get_block_count(); i++)
			{
				BlockInfo *bp = GetBlockInfo(tb, block_num);

				for (int j = 0; j < bp->GetRecordCount(); j++)
				{
					vector<TKey> tp = GetRecord(tb, block_num, j);
					if (tp[primary_key_index] == tuple[affect_index])
						throw PrimaryKeyConflictException();
				}
				block_num = bp->GetNextBlockNum();
			}
		}
	}
	int block_num = tb->get_first_block_num(); 
	for (int i = 0; i < tb->get_block_count(); i++)
	{
		BlockInfo *bp = GetBlockInfo(tb, block_num);

		for (int j = 0; j < bp->GetRecordCount(); j++)
		{
			vector<TKey> tp = GetRecord(tb, block_num,j);
			bool sats = true;
			for (int k = 0; k < st.GetWheres().size(); k++)
			{
				SQLWhere where = st.GetWheres()[k];
				if (!SatisfyWhere(tb, tuple, where)) sats = false;
			}
			if (sats)
			{
				/* remove from index. */
				if (tb->GetIndexNum() != 0)
				{
					BPlusTree tree(tb->GetIndex(0), buffer_m_, catalog_m_, db_name_);
					int idx = -1;
					for (int i = 0; i < tb->GetAttributeNum(); i++)
					{
						if (tb->GetAttributes()[i].get_attr_name() == tb->GetIndex(0)->get_attr_name())
							idx = i;
					}
					tree.Remove(tuple[idx]);
				}
				UpdateRecord(tb, block_num, j, indices, tuple);
				
				tp = GetRecord(tb, block_num, j);
				/* add index for new key. */
				if (tb->GetIndexNum() != 0)
				{
					BPlusTree tree(tb->GetIndex(0), buffer_m_, catalog_m_, db_name_);
					int idx = -1;
					for (int i = 0; i < tb->GetAttributeNum(); i++)
					{
						if (tb->GetAttributes()[i].get_attr_name() == tb->GetIndex(0)->get_attr_name())
							idx = i;
					}
					tree.Add(tuple[idx], block_num, j);
				}
			}
		}
		block_num = bp->GetNextBlockNum();
	}
	buffer_m_->WriteToDisk();
	cout << "Update successfully." << endl;
}

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
		//cout << "RecordManager::GetRecord::memcpy :" << content << " to " << tmp.get_key() << endl;
		keys.push_back(tmp);
		content += length;
	}
	return keys;
}

void RecordManager::DeleteRecord(Table* tbl, int block_num, int offset)
{
	BlockInfo *bp = GetBlockInfo(tbl, block_num);
	char *content = bp->get_data() + offset * tbl->get_record_length() + 12;
	char *replace = bp->get_data() + (bp->GetRecordCount() - 1) * tbl->get_record_length() + 12;
	memcpy(content, replace, tbl->get_record_length());

	bp->DecreaseRecordCount();

	if (bp->GetRecordCount() == 0) /* add the block to rubbish block chain */
	{
		int prevnum = bp->GetPrevBlockNum();
		int nextnum = bp->GetNextBlockNum();
		if (prevnum != -1) 
		{
			BlockInfo *pbp = GetBlockInfo(tbl, prevnum);
			pbp->SetNextBlockNum(nextnum);
			buffer_m_->WriteBlock(pbp);
		}
		if (nextnum != -1) 
		{
			BlockInfo *nbp = GetBlockInfo(tbl, nextnum);
			nbp->SetNextBlockNum(prevnum);
			buffer_m_->WriteBlock(nbp);
		}

		BlockInfo *firstrubbish = GetBlockInfo(tbl,tbl->get_first_block_num());
		bp->SetNextBlockNum(-1);
		bp->SetPrevBlockNum(-1);
		if (firstrubbish != NULL)
		{
			firstrubbish->SetPrevBlockNum(block_num);
			bp->SetNextBlockNum(firstrubbish->get_block_num());
		}
		tbl->set_first_rubbish_num(block_num);
	}
	buffer_m_->WriteBlock(bp);
}

void RecordManager::UpdateRecord(Table* tbl, int block_num, int offset, vector<int>& indices, vector<TKey>& values)
{
	BlockInfo *bp = GetBlockInfo(tbl, block_num);
	char *content = bp->get_data() + offset * tbl->get_record_length() + 12;
	
	for (int i = 0; i < tbl->GetAttributeNum(); i++)
	{
		auto iter = find(indices.begin(), indices.end(), i);
		if (iter != indices.end())
			memcpy(content, values[iter - indices.begin()].get_key(), values[iter - indices.begin()].get_length());
		content += tbl->GetAttributes()[i].get_length();
	}
	buffer_m_->WriteBlock(bp);
}

bool RecordManager::SatisfyWhere(Table* tbl, vector<TKey> keys, SQLWhere where)
{
	int idx = -1;
	for (int i = 0; i < tbl->GetAttributeNum(); ++i)
	{
		if (tbl->GetAttributes()[i].get_attr_name() == where.key)
			idx = i;
	}

	TKey tmp(tbl->GetAttributes()[idx].get_data_type(), tbl->GetAttributes()[idx].get_length());
	tmp.ReadValue(where.value.c_str());
	
	switch (where.op_type)
	{
	case SIGN_EQ:
		return keys[idx] == tmp;
		break;
	case SIGN_NE:
		return keys[idx] != tmp;
		break;
	case SIGN_LT:
		return keys[idx] < tmp;
		break;
	case SIGN_GT:
		return keys[idx] > tmp;
		break;
	case SIGN_LE:
		return keys[idx] <= tmp;
		break;
	case SIGN_GE:
		return keys[idx] >= tmp;
		break;
	default:
		return false;
		break;
	}
}