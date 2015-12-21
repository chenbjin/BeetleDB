//
// API.cpp
//
// Created by Chenbjin on 2015/12/20.
// Copyright(C) 2015, Chenbjin. All rights reserved.
//
#include "API.h"


API::API(string path):path_(path)
{
}


API::~API(void)
{
}

void API::Quit(){}
void API::Help(){}
void API::CreateDatabase(SQLCreateDatabase& statement){}
void API::CreateTable(SQLCreateTable& statement){}
void API::CreateIndex(SQLCreateIndex& statement){}
void API::ShowDatabases(){}
void API::ShowTables(){}
void API::DropDatabase(SQLDropDatabase& statement){}
void API::DropTable(SQLDropTable& statement){}
void API::DropIndex(SQLDropIndex& statement){}
void API::Use(SQLUse& statement){}
void API::Insert(SQLInsert& statement){}
void API::Exec(SQLExec& statement){}
void API::Select(SQLSelect& statement){}
void API::Delete(SQLDelete& statement){}
void API::Update(SQLUpdate& statement){}
