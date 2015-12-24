//
// BufferManager.h
//
// Created by Chenbjin on 2015/12/18.
// Copyright(C) 2015, Chenbjin. All rights reserved.
//
#pragma once
#ifndef _BUFFERMANAGER_H_
#define _BUFFERMANAGER_H_

#include <string>
#include "BlockHandle.h"
#include "FileHandle.h"

using namespace std;

class BufferManager
{
public:
	BufferManager(string path);
	~BufferManager(void);
	BlockInfo* GetFileBlock(std::string db_name, std::string tb_name, int file_type, int block_num);
	void WriteBlock(BlockInfo* block);
	void WriteToDisk();

private:
	BlockHandle* bhandle_;
	FileHandle* fhandle_;
	string path_;
	BlockInfo* GetUsableBlock();
};

#endif