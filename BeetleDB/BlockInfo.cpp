//
// BlockInfo.cpp
//
// Created by Chenbjin on 2015/12/24.
// Copyright(C) 2015, Chenbjin. All rights reserved.
//
#include "BlockInfo.h"
#include "StaticValue.h"
#include <fstream>

using namespace std;

BlockInfo::BlockInfo(int num):dirty_(false), next_(NULL), file_(NULL), age_(0), block_num_(num)
{
	data_ = new char[4*1024];
}
BlockInfo::~BlockInfo(void){ delete []data_; }
	
FileInfo* BlockInfo::GetFile(){ return file_; }
void BlockInfo::SetFile(FileInfo *f){ file_ = f; }

int BlockInfo::get_block_num(){ return block_num_; }
void BlockInfo::set_block_num(int num){ block_num_ = num; }

char* BlockInfo::get_data(){ return data_; }

bool BlockInfo::get_dirty(){ return dirty_; }
void BlockInfo::set_dirty(bool dt){ dirty_ = dt; }

long BlockInfo::get_age(){ return age_; }

BlockInfo* BlockInfo::GetNext(){ return next_; }
void BlockInfo::SetNext(BlockInfo* block){ next_ = block; }

void BlockInfo::IncreaseAge(){ ++age_; }
void BlockInfo::ResetAge(){ age_ = 0; }

int  BlockInfo::GetPrevBlockNum(){ return *(int*)(data_); }
void BlockInfo::SetPrevBlockNum(int num){ *(int*)(data_) = num; }

int  BlockInfo::GetNextBlockNum(){ return *(int*)(data_+4); }
void BlockInfo::SetNextBlockNum(int num){ *(int*)(data_+4) = num; }

int  BlockInfo::GetRecordCount(){ return *(int*)(data_+8); }
void BlockInfo::SetRecordCount(int count){ *(int*)(data_+8) = count; }
void BlockInfo::DecreaseRecordCount(){ *(int*)(data_+8) = *(int*)(data_+8) - 1; }

char* BlockInfo::GetContentAdress(){ return data_ + 12; }

void BlockInfo::ReadInfo(string path)
{
	path += file_->get_db_name() + "/" + file_->get_file_name();
	if (file_->get_type() == FORMAT_INDEX) path += ".index";
	else path += ".records";

	ifstream ifs(path, ios::binary);
	ifs.seekg(block_num_*4*1024); //?
	ifs.read(data_,4*1024);
	ifs.close();
}

void BlockInfo::WriteInfo(string path)
{
	path += file_->get_db_name() + "/" + file_->get_file_name();
	if (file_->get_type() == FORMAT_INDEX) path += ".index";
	else path += ".records";

	ofstream ofs(path, ios::binary);
	ofs.seekp(block_num_*4*1024);
	ofs.write(data_,4*1024);
	ofs.close();
}
