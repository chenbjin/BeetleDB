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
void API::CreateDatabase(){}
void API::CreateTable(){}
void API::CreateIndex(){}
void API::ShowDatabases(){}
void API::ShowTables(){}
void API::DropDatabase(){}
void API::DropTable(){}
void API::DropIndex(){}
void API::Use(){}
void API::Insert(){}
void API::Exec(){}
void API::Select(){}
void API::Delete(){}
void API::Update(){}
