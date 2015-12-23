//
// FileHandle.h
//
// Created by Chenbjin on 2015/12/24.
// Copyright(C) 2015, Chenbjin. All rights reserved.
//
#pragma once
#ifndef _FILEHANDLE_H_
#define _FILEHANDLE_H_

#include <string>
#include "FileInfo.h"
#include "BlockInfo.h"

using namespace std;

class FileHandle
{
public:
	FileHandle(string p);
	~FileHandle(void);
	FileInfo* GetFileInfo(string db_name, string tb_name, int file_type);
	BlockInfo* GetBlockInfo(FileInfo* file, int block_pos);
	BlockInfo* RecycleBlock();
	void AddBlockInfo(BlockInfo* block);
	void AddFileInfo(FileInfo* file);
	void IncreaseAge();
	void WriteToDisk();

private:
	FileInfo* first_file_;
	string path_;
};

#endif