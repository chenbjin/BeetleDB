//
// IndexManager.cpp
//
// Created by Chenbjin on 2015/12/18.
// Copyright(C) 2015, Chenbjin. All rights reserved.
//
#include "IndexManager.h"


IndexManager::IndexManager(CatalogManager *cm, BufferManager *bm, string dbname):catalog_m_(cm), buffer_m_(bm), db_name_(dbname){}

IndexManager::~IndexManager(void){}

void IndexManager::CreateIndex(SQLCreateIndex& st){}
