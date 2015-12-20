//
// CatalogManager.h
//
// Created by Chenbjin on 2015/12/20.
// Copyright(C) 2015, Chenbjin. All rights reserved.
//
#pragma once
#ifndef _CATALOGMANAGER_H_
#define _CATALOGMANAGER_H_

#include "SQLStatement.h"
#include <vector>
#include <string>

using namespace std;

class Database;
class Table;
class Attribute;
class Index;

class CatalogManager
{
public:
	CatalogManager(void);
	~CatalogManager(void);
};

class Attribute
{
public:
	Attribute();
	~Attribute();
	
	string GetAttributeName();
	void SetAttributeName(string name);
	
	int GetAttributeType();
	void SetAttibuterType(int type);

	int GetDataType();
	void SetDataType(int type);

	int GetLength();
	void SetLength(int length);

private:
	string attr_name_;
	int data_type_;
	int length_;
	int attr_type_;
};

#endif