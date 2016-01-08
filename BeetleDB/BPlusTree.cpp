//
// BPlusTree.cpp
//
// Created by Chenbjin on 2015/12/24.
// Copyright(C) 2015, Chenbjin. All rights reserved.
//
#include <iomanip>

#include "BPlusTree.h"
#include "Exceptions.h"
#include "StaticValue.h"

/* ------------------------- BPlusTree ------------------------------- */

BPlusTree::BPlusTree(Index* idx, BufferManager* bm, CatalogManager* cm, string dbname)
{
	buffer_m_ = bm;
	catalog_m_ = cm;
	idx_ = idx;
	degree_ = 2 * idx_->get_rank() + 1;
	db_name_ = dbname;
}

BPlusTree::~BPlusTree(void) {}

void BPlusTree::InitTree()
{
	BPlusTreeNode *root_node = new BPlusTreeNode(this, true, GetNewBlockNum(), true);
	idx_->set_root(0);
	idx_->set_leaf_head(idx_->get_root());
	idx_->set_key_count(0);
	idx_->set_node_count(1);
	idx_->set_level(1);
	root_node->SetNextLeaf(-1);
}

Index* BPlusTree::GetIndex() { return idx_; }
BufferManager* BPlusTree::GetBufferManager() { return buffer_m_; }
CatalogManager* BPlusTree::GetCatalogManager(){ return catalog_m_; }
int BPlusTree::get_degree(){ return degree_; }
string BPlusTree::get_db_name() { return db_name_; }

bool BPlusTree::Add(TKey& key, int block_num, int offset)
{
	int value=(block_num << 16) | offset; //?? why left shift.
	if (idx_->get_root() == -1) 
		InitTree();

	//cout << "before search" << endl;
	FindNodeParam fnp = Search(idx_->get_root(), key);
	//cout << "after search" << endl;
	if (!fnp.flag)
	{
		fnp.pnode->Add(key, value);
		//cout << "fnp.pnode->Add()" << endl;
		idx_->IncreaseKeyCount();
		if (fnp.pnode->GetCount() == degree_)
			return AdjustAfterAdd(fnp.pnode->get_block_num());
		return true;
	}
	return false;
}

bool BPlusTree::AdjustAfterAdd(int node)
{
	//cout << "AdjustAfterAdd" << endl;
	BPlusTreeNode *pnode = GetNode(node);
	TKey key(idx_->get_key_type(), idx_->get_key_len());
	/* ?? */
	BPlusTreeNode *newnode = pnode->Split(key);
	idx_->IncreaseNodeCount();
	int parent = pnode->GetParent();

	if (parent == -1) /* current node is root. */
	{
		BPlusTreeNode *newroot = new BPlusTreeNode(this,true, GetNewBlockNum());
		if (newroot == NULL) return false;

		idx_->IncreaseNodeCount();
		idx_->set_root(newroot->get_block_num());

		newroot->Add(key);
		newroot->SetValues(0, pnode->get_block_num());
		newroot->SetValues(1, newnode->get_block_num());

		pnode->SetParent(idx_->get_root());
		newnode->SetParent(idx_->get_root());
		newnode->SetNextLeaf(-1);
		idx_->IncreaseLevel();
		return true;
	}
	else
	{
		BPlusTreeNode *parentnode = GetNode(parent);
		int index = parentnode->Add(key);

		parentnode->SetValues(index,pnode->get_block_num());
		parentnode->SetValues(index+1,newnode->get_block_num());

		if(parentnode->GetCount() == degree_)
			return AdjustAfterAdd(parentnode->get_block_num());
		return true;
	}
}

bool BPlusTree::Remove(TKey key)
{
	if (idx_->get_root() == -1) return false;

	BPlusTreeNode *rootnode = GetNode(idx_->get_root());
	FindNodeParam fnp = Search(idx_->get_root(), key);

	if (fnp.flag)
	{
		if (idx_->get_root() == fnp.pnode->get_block_num())
		{
			rootnode->RemoveAt(fnp.index);
			idx_->DecreaseKeyCount();
			AdjustAfterRemove(fnp.pnode->get_block_num());
			return true;
		}

		if (fnp.index == fnp.pnode->GetCount()-1)
		{
			FindNodeParam fnpb = SearchBranch(idx_->get_root(), key);
			if (fnpb.flag)
				fnpb.pnode->SetKeys(fnpb.index, fnp.pnode->GetKeys(fnp.pnode->GetCount() - 2));
		}

		fnp.pnode->RemoveAt(fnp.index);
		idx_->DecreaseKeyCount();
		AdjustAfterRemove(fnp.pnode->get_block_num());
		return true;
	}
	return false;
}

bool BPlusTree::AdjustAfterRemove(int node)
{
	BPlusTreeNode* pnode = GetNode(node);
	if (pnode->GetCount() >= idx_->get_rank()) return true;

	if (pnode->IsRoot())
	{
		if (pnode->GetCount() == 0)
		{
			if (!pnode->GetIsLeaf())
			{
				idx_->set_root(pnode->GetValues(0));
				GetNode(pnode->GetValues(0))->SetParent(-1);
			}
			else
			{
				idx_->set_root(-1);
				idx_->set_leaf_head(-1);
			}
			delete pnode;
			idx_->DecreaseNodeCount();
			idx_->DecreaseLevel();
		}
		return true;
	}

	BPlusTreeNode *pbrother;
	BPlusTreeNode *pparent;
	int pos;

	pparent = GetNode(pnode->GetParent());
	pparent->Search(pnode->GetKeys(0), pos);

	if (pos == pparent->GetCount())
	{
		pbrother = GetNode(pparent->GetValues(pos-1));
		if(pbrother->GetCount() > idx_->get_rank())
		{
			if (pnode->GetIsLeaf())
			{
				for (int i = pnode->GetCount(); i > 0; i--)
				{
					pnode->SetKeys(i,pnode->GetKeys(i-1));
					pnode->SetValues(i,pnode->GetValues(i-1));
				}
				pnode->SetKeys(0,pbrother->GetKeys(pbrother->GetCount()-1));
				pnode->SetValues(0,pbrother->GetValues(pbrother->GetCount()-1));
				pnode->SetCount(pnode->GetCount()+1);
				
				pbrother->SetCount(pbrother->GetCount()-1);
				pparent->SetKeys(pos-1, pbrother->GetKeys(pbrother->GetCount()-1));
				return true;
			}
			else
			{
				for (int i = pnode->GetCount(); i > 0; i--)
					pnode->SetKeys(i,pnode->GetKeys(i-1));
				for(int i = pnode->GetCount()+1;i > 0 ; i--)
					pnode->SetValues(i,pnode->GetValues(i-1));

				pnode->SetKeys(0,pparent->GetKeys(pos-1));
				pparent->SetKeys(pos-1,pbrother->GetKeys(pbrother->GetCount()-1));

				pnode->SetValues(0,pbrother->GetValues(pbrother->GetCount()));
				pnode->SetCount(pnode->GetCount()+1);

				if(pbrother->GetValues(pbrother->GetCount()) >= 0)
				{
					GetNode(pbrother->GetValues(pbrother->GetCount()))->SetParent(pnode->get_block_num());
					pbrother->SetValues(pbrother->GetCount(),-1);
				}
				pbrother->SetCount(pbrother->GetCount()-1);
				return true;
			}
		}
		else
		{
			if(pnode->GetIsLeaf())
			{
				pparent->RemoveAt(pos-1);
				pparent->SetValues(pos-1,pbrother->get_block_num());
				
				for(int i = 0; i < pnode->GetCount(); i++)
				{
					pbrother->SetKeys(pbrother->GetCount()+i,pnode->GetKeys(i));
					pbrother->SetValues(pbrother->GetCount()+i,pnode->GetValues(i));
					pnode->SetValues(i,-1);
				}
				pbrother->SetCount(pbrother->GetCount()+pnode->GetCount());
				pbrother->SetNextLeaf(pnode->GetNextLeaf());
				
				delete pnode;
				idx_->DecreaseNodeCount();
				return AdjustAfterRemove(pparent->get_block_num());
			}
			else
			{
				pbrother->SetKeys(pbrother->GetCount(),pparent->GetKeys(pos-1));
				pbrother->SetCount(pbrother->GetCount()+1);
				pparent->RemoveAt(pos-1);
				pparent->SetValues(pos-1,pbrother->get_block_num());
				
				for(int i = 0; i < pnode->GetCount(); i++)
					pbrother->SetKeys(pbrother->GetCount()+i,pnode->GetKeys(i));

				for(int i = 0; i <= pnode->GetCount(); i++)
				{
					pbrother->SetValues(pbrother->GetCount()+i,pnode->GetValues(i));
					GetNode(pnode->GetValues(i))->SetParent(pbrother->get_block_num());
				}
				pbrother->SetCount( 2 * idx_->get_rank());
				
				delete pnode;
				idx_->DecreaseNodeCount();
				return AdjustAfterRemove(pparent->get_block_num());
			}
		}
	}
	else
	{
		pbrother = GetNode(pparent->GetValues(pos+1));
		if(pbrother->GetCount() > idx_->get_rank())
		{
			if(pnode->GetIsLeaf())
			{
				pparent->SetKeys(pos,pbrother->GetKeys(0));
				pnode->SetKeys(pnode->GetCount(),pbrother->GetKeys(0));
				pnode->SetValues(pnode->GetCount(),pbrother->GetValues(0));
				
				pbrother->SetValues(0,-1);
				pnode->SetCount(pnode->GetCount()+1);
				pbrother->RemoveAt(0);
				return true;
			}
			else
			{
				pnode->SetKeys(pnode->GetCount(),pparent->GetKeys(pos));
				pnode->SetValues(pnode->GetCount()+1,pbrother->GetValues(0));
				pnode->SetCount(pnode->GetCount()+1);
				pparent->SetKeys(pos,pbrother->GetKeys(0));
				GetNode(pbrother->GetValues(0))->SetParent(pnode->get_block_num());
				
				pbrother->RemoveAt(0);
				return true;
			}
		}
		else
		{
			if(pnode->GetIsLeaf())
			{
				for(int i=0 ; i <idx_->get_rank();i++) 
				{
					pnode->SetKeys(pnode->GetCount()+i,pbrother->GetKeys(i));
					pnode->SetValues(pnode->GetCount()+i,pbrother->GetValues(i));
					pbrother->SetValues(i,-1);
				}

				pnode->SetCount(pnode->GetCount()+idx_->get_rank());
				delete pbrother;
				idx_->DecreaseNodeCount();
				
				pparent->RemoveAt(pos);
				pparent->SetValues(pos,pnode->get_block_num());
				return AdjustAfterRemove(pparent->get_block_num());
			}
			else
			{
				pnode->SetKeys(pnode->GetCount(),pparent->GetKeys(pos));
				pparent->RemoveAt(pos);
				pparent->SetValues(pos,pnode->get_block_num());
				pnode->SetCount(pnode->GetCount()+1);

				for (int i = 0; i < idx_->get_rank(); i++)
					pnode->SetKeys(pnode->GetCount()+i,pbrother->GetKeys(i));

				for (int i = 0; i <= idx_->get_rank(); i++)
				{
					pnode->SetValues(pnode->GetCount()+i,pbrother->GetValues(i));
					GetNode(pbrother->GetValues(i))->SetParent(pnode->get_block_num());
				}

				pnode->SetCount(pnode->GetCount()+idx_->get_rank());
				delete pbrother;
				idx_->DecreaseNodeCount();
				return AdjustAfterRemove(pparent->get_block_num());
			}
		}
	}
}

FindNodeParam BPlusTree::Search(int node, TKey &key)
{
	FindNodeParam ans;
	int index = 0;
	BPlusTreeNode* pnode = GetNode(node);
	if (pnode->Search(key, index)) 
	{
		if (pnode->GetIsLeaf()) 
		{
			ans.flag = true;
			ans.index = index;
			ans.pnode = pnode;
        } 
		else 
		{
			pnode = GetNode(pnode->GetValues(index));
			while (!pnode->GetIsLeaf())
			{
				pnode = GetNode(pnode->GetValues(pnode->GetCount()));
			}
			ans.flag = true;
			ans.index = pnode->GetCount() - 1;
			ans.pnode = pnode;
		}
	}
	else 
	{
		if (pnode->GetIsLeaf()) 
		{
			ans.flag = false;
			ans.index = index;
			ans.pnode = pnode;
		}
		else 
		{
			return Search(pnode->GetValues(index), key);
		}
	}
	return ans;
}

FindNodeParam BPlusTree::SearchBranch(int node, TKey &key)
{
	FindNodeParam ans;
	int index = 0;
	BPlusTreeNode* pnode = GetNode(node);
	
	if (pnode->GetIsLeaf()) throw BPlusTreeException();
	if (pnode->Search(key, index))
	{
		ans.flag = true;
		ans.index = index;
		ans.pnode = pnode;
	}
	else
	{
		if (!GetNode(pnode->GetValues(index))->GetIsLeaf())
			ans = SearchBranch(pnode->GetValues(index),key);
		else
		{
			ans.flag = false;
			ans.index = index;
			ans.pnode = pnode;
		}
	}
	return ans;
}

BPlusTreeNode* BPlusTree::GetNode(int num)
{
	BPlusTreeNode* pnode = new BPlusTreeNode(this, false, num);
	return pnode;
}

int BPlusTree::GetVal(TKey key)
{
	int ans = -1;
	FindNodeParam fnp = Search(idx_->get_root(), key);
	if (fnp.flag)
		ans = fnp.pnode->GetValues(fnp.index);
	return ans;
}

int BPlusTree::GetNewBlockNum()
{
	return idx_->IncreaseMaxCount();
}

void BPlusTree::Print()
{
	printf("*----------------------------------------------*\n");
	printf("KeyCount: %d, NodeCount: %d, Level: %d, Root: %d \n", idx_->get_key_count(), idx_->get_node_count(), idx_->get_level(), idx_->get_root());
	
	if (idx_->get_root() != -1)
		PrintNode(idx_->get_root());
}

void BPlusTree::PrintNode(int num)
{
	BPlusTreeNode* pnode = GetNode(num);
	pnode->Print();
	if (!pnode->GetIsLeaf())
	{
		for (int i = 0; i <= pnode->GetCount(); ++i)
			PrintNode(pnode->GetValues(i));
	}
}

/* ------------------------- BPlusTreeNode ------------------------------- */
BPlusTreeNode::BPlusTreeNode(BPlusTree* tree, bool isnew, int blocknum,  bool newleaf)
{
	tree_ = tree;
	is_leaf_ = newleaf;
	rank_ = (tree_->get_degree() - 1) / 2;
	block_num_ = blocknum;
	GetBuffer();
	if (isnew)
	{
		SetParent(-1);
		SetNodeType(newleaf ? 1:0);
		SetCount(0);
	}
}

BPlusTreeNode::~BPlusTreeNode(){}

int BPlusTreeNode::get_block_num() { return block_num_; }

TKey BPlusTreeNode::GetKeys(int index)
{
	TKey k(tree_->GetIndex()->get_key_type(), tree_->GetIndex()->get_key_len());
	int base = 12;
	int lenr = 4 + tree_->GetIndex()->get_key_len();
	//cout << "before BPlusTreeNode::GetKeys():memcpy" << endl;
	memcpy(k.get_key(), &buffer_[base + index * lenr+4], tree_->GetIndex()->get_key_len());
	//cout << "after BPlusTreeNode::GetKeys():memcpy" << endl;
	return k;
}

int BPlusTreeNode::GetValues(int index)
{
	int base = 12;
	int lenR = 4 + tree_->GetIndex()->get_key_len();
	return *((int*)(&buffer_[base + index*lenR]));
}

int BPlusTreeNode::GetNextLeaf()
{
	int base = 12;
	int lenR = 4 + tree_->GetIndex()->get_key_len();
	return *((int*)(&buffer_[base + tree_->get_degree()*lenR]));
}

int BPlusTreeNode::GetParent(){ return *((int*)(&buffer_[8])); }
int BPlusTreeNode::GetNodeType(){ return *((int*)(&buffer_[0])); }
int BPlusTreeNode::GetCount(){ return *((int*)(&buffer_[4])); }
bool BPlusTreeNode::GetIsLeaf(){ return GetNodeType() == 1; }

void BPlusTreeNode::SetKeys(int index, TKey key)
{
	int base = 12;
	int lenr = 4 + tree_->GetIndex()->get_key_len();
	memcpy(&buffer_[base+index*lenr+4],key.get_key(),tree_->GetIndex()->get_key_len());
}

void BPlusTreeNode::SetValues(int index, int val) 
{
	int base = 12;
	int lenr = 4 + tree_->GetIndex()->get_key_len();
	*((int*)(&buffer_[base+index*lenr])) = val;
}

void BPlusTreeNode::SetNextLeaf(int val) 
{
	int base = 12;
	int len = 4 + tree_->GetIndex()->get_key_len();
	*((int*)(&buffer_[base + tree_->get_degree() * len])) = val;
}

void BPlusTreeNode::SetParent(int val) { *((int*)(&buffer_[8]))=val; }
void BPlusTreeNode::SetNodeType(int val) { *((int*)(&buffer_[0]))=val; }
void BPlusTreeNode::SetCount(int val) { *((int*)(&buffer_[4]))=val; }
void BPlusTreeNode::SetIsLeaf(bool val) { SetNodeType(val ? 1:0); }

void BPlusTreeNode::GetBuffer()
{
	BlockInfo *bp = tree_->GetBufferManager()->GetFileBlock(tree_->get_db_name(),tree_->GetIndex()->get_name(),FORMAT_INDEX, block_num_);
	buffer_ = bp->get_data();
	bp->set_dirty(true);
}

bool BPlusTreeNode::Search(TKey key, int &index)
{
	//cout << "BPlusTreeNode::Search" << endl;
	bool ans = false;
	if (GetCount() == 0) {  index = 0; return false; }
	if (GetKeys(0) > key) {  index = 0;  return false; }
	if (GetKeys(GetCount() - 1) < key) { index = GetCount(); return false; }
	//cout << "GetCount():" << GetCount() << endl;
	if (GetCount() > 20) /* binary search */
	{
		int mid, start, end;
		start = 0;
		end = GetCount() - 1;
		while (start < end)
		{
			mid = (start + end)/2;
			if (key == GetKeys(mid)) { index = mid; return true; }
			else if (key < GetKeys(mid)) end = mid;
			else start = mid;
			
			if (start == end - 1)
			{
				if (key == GetKeys(start)) { index = start; return true; }
				if (key == GetKeys(end)) { index = end; return true; }
				if (key < GetKeys(start)) { index = start; return false; }
				if (key < GetKeys(end)) { index = end; return false; }
				if (key > GetKeys(end)) { index = end+1; return false; }
			}
		}
		return false;
	}
	else /* sequential search */
	{
		for(int i=0; i < GetCount(); i++) 
		{
			if(key < GetKeys(i))
			{
				index = i;
				ans = false;
				break;
			}
			else if(key == GetKeys(i)) 
			{
				index = i;
				ans = true;
				break;
			}
		}
		return ans;
	}
}

int BPlusTreeNode::Add(TKey &key)
{
	int index = 0;
	if(GetCount() == 0) 
	{
		SetKeys(0,key);
		SetCount(1);
		return 0;
	}
	if(!Search(key,index)) 
	{  
		for(int i = GetCount(); i > index; i--) 
			SetKeys(i,GetKeys(i-1));

		for(int i = GetCount()+1; i > index; i--)
			SetValues(i,GetValues(i-1));
		
		SetKeys(index,key);
		SetValues(index,-1);
		SetCount(GetCount()+1);
	}
	return index;
}
int BPlusTreeNode::Add(TKey &key, int &val)
{
	int index=0;
	if(GetCount()==0) 
	{
		SetKeys(0,key);
		SetValues(0,val);
		SetCount(GetCount()+1);
		return 0;
	}
	if(!Search(key,index)) {
		for(int i = GetCount(); i > index;i--) 
		{
			SetKeys(i,GetKeys(i-1));
			SetValues(i,GetValues(i-1));
		}

		SetKeys(index,key);
		SetValues(index,val);
		SetCount(GetCount()+1);
	}
	return index;
}

BPlusTreeNode* BPlusTreeNode::Split(TKey &key)
{
	BPlusTreeNode* newnode = new BPlusTreeNode(tree_,true, tree_->GetNewBlockNum(), GetIsLeaf());
	if(newnode == NULL) 
	{
		throw BPlusTreeException();
		return NULL;
	}
	key = GetKeys(rank_);
	if(GetIsLeaf()) 
	{
		for(int i = rank_+1; i< tree_->get_degree(); i++) 
		{
			newnode->SetKeys(i-rank_-1,GetKeys(i));
			newnode->SetValues(i-rank_-1,GetValues(i));
		}

		newnode->SetCount(rank_);
		SetCount(rank_+1);
		newnode->SetNextLeaf(GetNextLeaf());
		SetNextLeaf(newnode->get_block_num());
		newnode->SetParent(GetParent());
	}
	else 
	{
		for(int i = rank_+1; i< tree_->get_degree(); i++)
			newnode->SetKeys(i-rank_-1,GetKeys(i));

		for(int i = rank_+1; i<= tree_->get_degree();i++)
			newnode->SetValues(i-rank_-1,GetValues(i));

		newnode->SetParent(GetParent());
		newnode->SetCount(rank_);
		
		int childnode_num;
		for(int i = 0;i <= newnode->GetCount();i++) 
		{
			childnode_num = newnode->GetValues(i);
			BPlusTreeNode* node=tree_->GetNode(childnode_num);
			if(node) node->SetParent(newnode->get_block_num());
		}
		SetCount(rank_);
	}
	return newnode;
}

bool BPlusTreeNode::IsRoot()
{
	if (GetParent() == -1) return true;
	return false;
}

bool BPlusTreeNode::RemoveAt(int index)
{
	if(index > GetCount()-1) return false;
	if(GetIsLeaf()) {
		for(int i = index; i < GetCount()-1; i++) 
		{
			SetKeys(i, GetKeys(i+1));
			SetValues(i, GetValues(i+1));
		}
	}
	else 
	{
		for(int i = index; i< GetCount()-1; i++)
			SetKeys(i, GetKeys(i+1));

		for(int i = index; i < GetCount(); i++)
			SetValues(i, GetValues(i+1));
	}
	SetCount(GetCount()-1);
	return true;
}

void BPlusTreeNode::Print()
{
	printf("*----------------------------------------------*\n");
	printf("BlockNum: %d Count: %d, Parent: %d  IsLeaf:%d\n",block_num_,GetCount(),GetParent(),GetIsLeaf());
	printf("Keys: { ");
	for (int i = 0; i < GetCount(); i++)
		cout << setw(9) << left << GetKeys(i);
	printf(" }\n");

	if(GetIsLeaf())
	{
		printf("Vals: { ");
		for (int i = 0; i < GetCount(); i++)
		{
			if (GetValues(i) == -1) printf("{NUL}");
			else //printf("%07d ",GetValues(i));
				cout << setw(9) << left << GetValues(i);
		}
		printf(" }\n");
		printf("NextLeaf: %5d\n", GetNextLeaf());
	}
	else
	{
		printf("Ptrs: {");
		for(int i = 0; i <= GetCount();i++)
			//printf("%07d ",GetValues(i));
			cout << setw(9) << left << GetValues(i);

		printf("}\n");
	}
}