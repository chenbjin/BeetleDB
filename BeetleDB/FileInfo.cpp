//
// FileInfo.cpp
//
// Created by Chenbjin on 2015/12/24.
// Copyright(C) 2015, Chenbjin. All rights reserved.
//

#include "FileInfo.h"
#include "StaticValue.h"

FileInfo::FileInfo(void)
{
	db_name_= "";
	type_ = FORMAT_RECORD;
	file_name_ = "";
	record_amount_ = 0;
	record_length_ = 0;
	first_block_ = 0;
	next_ = 0;
}
FileInfo::FileInfo(string db, int tp, string f, int rec_amount, int rec_len, BlockInfo* first, FileInfo* nxt)
{
	db_name_ = db;
	type_ = tp;
	file_name_ = f;
	record_amount_ = rec_amount;
	record_length_ = rec_len;
	first_block_ = first;
	next_ = nxt;
}
FileInfo::~FileInfo(void){}

string FileInfo::get_db_name(){ return db_name_; }
string FileInfo::get_file_name(){ return file_name_; }
int FileInfo::get_type(){ return type_; }

BlockInfo* FileInfo::GetFirstBlock(){ return first_block_; }
void FileInfo::SetFirstBlock(BlockInfo* bp) { first_block_ = bp; }

FileInfo* FileInfo::GetNext(){ return next_; }
void FileInfo::SetNext(FileInfo* fp){ next_ = fp; }
	
void FileInfo::IncreaseRecordAmount(){ record_amount_ ++; }
void FileInfo::IncreaseRecordLength(){ record_length_ += 4096; }
