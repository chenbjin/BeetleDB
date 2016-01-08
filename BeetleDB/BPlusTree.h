//
// BPlusTree.h
//
// Created by Chenbjin on 2015/12/24.
// Copyright(C) 2015, Chenbjin. All rights reserved.
//

#pragma once
#ifndef _BPLUSTREE_H_
#define _BPLUSTREE_H_

#include <string>
#include <vector>

#include "CatalogManager.h"
#include "BufferManager.h"

using namespace std;

class BPlusTree;
class BPlusTreeNode;

typedef struct
{
	BPlusTreeNode* pnode;
	int index;
	bool flag;
} FindNodeParam;

class BPlusTree
{
public:
	BPlusTree(Index* idx,BufferManager* bm, CatalogManager* cm,string dbname);
	~BPlusTree(void);
	
	Index* GetIndex();
	BufferManager* GetBufferManager();
	CatalogManager* GetCatalogManager();
	int get_degree();
	string get_db_name();

	bool Add(TKey& key, int block_num, int offset);
	bool AdjustAfterAdd(int node);

	bool Remove(TKey key);
	bool AdjustAfterRemove(int node);

	FindNodeParam Search(int node, TKey &key);
	FindNodeParam SearchBranch(int node, TKey &key);
	BPlusTreeNode* GetNode(int num);

	int GetVal(TKey key);
	int GetNewBlockNum();

	void Print();
	void PrintNode(int num);

private:
	Index *idx_;
	int degree_;
	BufferManager *buffer_m_;
	CatalogManager *catalog_m_;
	string db_name_;
	void InitTree();
};

class BPlusTreeNode
{
public:
	BPlusTreeNode(BPlusTree* tree,bool isnew,  int blocknum,  bool newleaf=false);
	~BPlusTreeNode();

	int get_block_num();

	TKey GetKeys(int index);
	int GetValues(int index);
	int GetNextLeaf();
	int GetParent();
	int GetNodeType();
	int GetCount();
	bool GetIsLeaf();

	void SetKeys(int index, TKey key);
	void SetValues(int index, int val);
	void SetNextLeaf(int val);
	void SetParent(int val);
	void SetNodeType(int val);
	void SetCount(int val);
	void SetIsLeaf(bool val);

	void GetBuffer();

	bool Search(TKey key, int &index);
	int Add(TKey &key);
	int Add(TKey &key, int &val);
	BPlusTreeNode* Split(TKey &key);

	bool IsRoot();
	bool RemoveAt(int index);
	void Print();

private:
	BPlusTree* tree_;
	int block_num_;
	int rank_;
	char* buffer_;
	bool is_leaf_;
	bool is_new_node_;
};

#endif