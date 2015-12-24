#include "BlockHandle.h"


BlockHandle::BlockHandle(string path)
{
	first_block_ = new BlockInfo(0);
	block_size_ = 300; //
	block_count_ = 0;
	path_ = path;
	Add(first_block_);
}

BlockHandle::~BlockHandle(void)
{
	BlockInfo* b = first_block_;
	while (block_count_ > 0)
	{
		BlockInfo* bn = b->GetNext();
		delete b;
		b = bn;
		block_count_ --;
	}
}

int BlockHandle::get_block_count() { return block_count_; }

/* return first_block ->next(); */
BlockInfo* BlockHandle::GetUsableBlock()
{
	if (block_count_ == 0) return NULL;  //?block_count means valuable block or dirty block?
	
	BlockInfo* p = first_block_->GetNext();
	first_block_->SetNext(first_block_->GetNext()->GetNext());
	block_count_ --;

	p->ResetAge();
	p->SetNext(NULL);
	return p;
}

void BlockHandle::FreeBlock(BlockInfo* block)
{
	if (block_count_ == 0)
	{
		first_block_ = block;
		block->SetNext(block);
	}
	else
	{
		block->SetNext(first_block_->GetNext());
		first_block_->SetNext(block);
	}
	block_count_ ++;
}

BlockInfo* BlockHandle::Add(BlockInfo* block) 
{
	BlockInfo* adder = new BlockInfo(0);
	adder->SetNext(block->GetNext());
	block->SetNext(adder);
	block_count_++;
	if (block_count_ == block_size_) return adder;
    else return Add(adder);
}