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

using namespace std;

class BufferManager
{
public:
	BufferManager(string path);
	~BufferManager(void);
private:
	string path_;
};

#endif