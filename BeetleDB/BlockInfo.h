//
// BlockInfo.h
//
// Created by Chenbjin on 2015/12/24.
// Copyright(C) 2015, Chenbjin. All rights reserved.
//
#pragma once
#ifndef _BLOCKINFO_H_
#define _BLOCKINFO_H_

#include "FileInfo.h"

#include <string>
#include <sys/types.h>

using namespace std;

class FileInfo;

class BlockInfo
{
public:
	BlockInfo(int num);
	~BlockInfo(void);
	
	FileInfo* GetFile();
	void SetFile(FileInfo *f);

	int get_block_num();
	void set_block_num(int num);

	char* get_data();

	bool get_dirty();
	void set_dirty(bool dt);

	long get_age();

	BlockInfo* GetNext();
	void SetNext(BlockInfo* block);

	void IncreaseAge();
	void ResetAge();

	int GetPrevBlockNum();
	void SetPrevBlockNum(int num);

	int GetNextBlockNum();
	void SetNextBlockNum(int num);

	int GetRecordCount();
	void SetRecordCount(int count);
	void DecreaseRecordCount();

	char* GetContentAdress();

	void ReadInfo(string path);
	void WriteInfo(string path);

private:
	FileInfo *file_;
	int block_num_;
	char *data_;
	bool dirty_;
	long age_;
	BlockInfo *next_;
};

#endif