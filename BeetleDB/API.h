//
// API.h
//
// Created by Chenbjin on 2015/12/20.
// Copyright(C) 2015, Chenbjin. All rights reserved.
//
#pragma once
#ifndef _API_H_
#define _API_H_

#include <string>
#include "BufferManager.h"
#include "CatalogManager.h"
#include "SQLStatement.h"

using namespace std;

class API
{
public:
	API(string path);
	~API(void);
	void Quit();
	void Help();
	void CreateDatabase(SQLCreateDatabase& statement);
	void CreateTable(SQLCreateTable& statement);
	void CreateIndex(SQLCreateIndex& statement);
	void ShowDatabases();
	void ShowTables();
	void DropDatabase(SQLDropDatabase& statement);
	void DropTable(SQLDropTable& statement);
	void DropIndex(SQLDropIndex& statement);
	void Use(SQLUse& statement);
	void Insert(SQLInsert& statement);
	void Select(SQLSelect& statement);
	void Delete(SQLDelete& statement);
	void Update(SQLUpdate& statement);

private:
	string path_;
	string current_db_;
	CatalogManager* catalog_m_;
	BufferManager*	buffer_m_;
};

#endif
