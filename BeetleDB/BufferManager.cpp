//
// BufferManager.cpp
//
// Created by Chenbjin on 2015/12/18.
// Copyright(C) 2015, Chenbjin. All rights reserved.
//
#include "BufferManager.h"
#include "StaticValue.h"

#include <string>
#include <fstream>

BufferManager::BufferManager(string path):path_(path)
{
	bhandle_ = new BlockHandle(path);
	fhandle_ = new FileHandle(path);
}

BufferManager::~BufferManager(void)
{
	delete bhandle_;
	delete fhandle_;
}

BlockInfo* BufferManager::GetUsableBlock()
{
	if (bhandle_->get_block_count() > 0)
		return bhandle_->GetUsableBlock();
	else
		return fhandle_->RecycleBlock();
}

BlockInfo* BufferManager::GetFileBlock(string db_name, string tb_name, int file_type, int block_num)
{
	fhandle_->IncreaseAge();
	FileInfo *file = fhandle_->GetFileInfo(db_name, tb_name, file_type);

	if (file)
	{
		BlockInfo *blo = fhandle_->GetBlockInfo(file, block_num);
		if (blo)
			return blo;
		else /* new block */
		{
			BlockInfo *bp = GetUsableBlock();
			bp->set_block_num(block_num);
			bp->SetFile(file);
			bp->ReadInfo(path_);
			fhandle_->AddBlockInfo(bp);
			return bp;
		}
	}
	else
	{
		BlockInfo *bp = GetUsableBlock();
		bp->set_block_num(block_num);
		FileInfo *fp = new FileInfo(db_name,file_type,tb_name, 0 , 0, NULL, NULL);
		fhandle_->AddFileInfo(fp);
		bp->SetFile(fp);
		bp->ReadInfo(path_);
		fhandle_->AddBlockInfo(bp);
		return bp;
	}
	return 0;
}

void BufferManager::WriteBlock(BlockInfo* block)
{
	block->set_dirty(true);
}
	
void BufferManager::WriteToDisk()
{
	fhandle_->WriteToDisk();
}