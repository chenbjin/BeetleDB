//
// RecordManager.h
//
// Created by Chenbjin on 2015/12/18.
// Copyright(C) 2015, Chenbjin. All rights reserved.
//
#pragma once
#ifndef _RECORDMANAGER_H_
#define _RECORDMANAGER_H_

#include <string>
#include <vector>

#include "SQLStatement.h"
#include "BufferManager.h"
#include "CatalogManager.h"
#include "Exceptions.h"

using namespace std;

class RecordManager
{
public:
	RecordManager(CatalogManager *cm, BufferManager *bm, string dbname);
	~RecordManager(void);
	
	void Insert(SQLInsert& st);
	void Select(SQLSelect& st);
	void Delete(SQLDelete& st);
	void Update(SQLUpdate& st);
private:
	CatalogManager* catalog_m_;
	BufferManager* buffer_m_;
	string db_name_;
};

#endif