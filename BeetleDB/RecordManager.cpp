//
// RecordManager.cpp
//
// Created by Chenbjin on 2015/12/18.
// Copyright(C) 2015, Chenbjin. All rights reserved.
//
#include "RecordManager.h"


RecordManager::RecordManager(CatalogManager *cm, BufferManager *bm, string dbname):catalog_m_(cm), buffer_m_(bm), db_name_(dbname) {}

RecordManager::~RecordManager(void){}

void RecordManager::Insert(SQLInsert& st){}
void RecordManager::Select(SQLSelect& st){}
void RecordManager::Delete(SQLDelete& st){}
void RecordManager::Update(SQLUpdate& st){}