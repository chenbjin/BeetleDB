//
// BlockHandle.h
//
// Created by Chenbjin on 2015/12/24.
// Copyright(C) 2015, Chenbjin. All rights reserved.
//
#pragma once
#ifndef _BLOCKHANDLE_H_
#define _BLOCKHANDLE_H

#include "BlockInfo.h"

class BlockHandle
{
public:
	BlockHandle(string p);
	~BlockHandle(void);

	int get_block_count();
	BlockInfo* GetUsableBlock();
	void FreeBlock(BlockInfo* block);

private:
	BlockInfo* first_block_;
	int block_size_;     //total block;
	int block_count_;    //usable block;
	string path_;
	BlockInfo* Add(BlockInfo* block);
};

#endif
