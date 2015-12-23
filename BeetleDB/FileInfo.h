//
// FileInfo.h
//
// Created by Chenbjin on 2015/12/24.
// Copyright(C) 2015, Chenbjin. All rights reserved.
//
#pragma once
#ifndef _FILEINFO_H_
#define _FILEINFO_H_

#include "BlockInfo.h"
#include <string>

using namespace std;

class BlockInfo;

class FileInfo
{
public:
	FileInfo(void);
	FileInfo(string db, int tp, string f, int rec_amount, int rec_len, BlockInfo* first, FileInfo* nxt);
	~FileInfo(void);
	string get_db_name();
	string get_file_name();
	int get_type();

	BlockInfo* GetFirstBlock();
	void SetFirstBlock(BlockInfo* bp);

	FileInfo* GetNext();
	void SetNext(FileInfo* fp);
	
	void IncreaseRecordAmount();
	void IncreaseRecordLength();

private:
	string db_name_;
	int type_;					// 0: data file, 1: index file
	string file_name_;			// the name of the file
	int record_amount_;			// the number of record in the file
	int record_length_;			// the length of the record in the file
	BlockInfo *first_block_;	// point to the first block within the file
	FileInfo *next_;			// the pointer points to the next file
};

#endif