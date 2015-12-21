//
// CatalogManager.cpp
//
// Created by Chenbjin on 2015/12/18.
// Copyright(C) 2015, Chenbjin. All rights reserved.
//
#include "CatalogManager.h"

/* ---------------- CatalogManager ------------ */
CatalogManager::CatalogManager(void)
{
}

CatalogManager::~CatalogManager(void)
{
}

/* --------------- Attribute ----------------- */
Attribute::Attribute():attr_name_(""),data_type_(-1),length_(-1),attr_type_(0){}
Attribute::~Attribute(){}

string Attribute::GetAttributeName(){ return attr_name_; }
void Attribute::SetAttributeName(string name) { attr_name_ = name; }
	
int Attribute::GetAttributeType() { return attr_type_; }
void Attribute::SetAttibuterType(int type) { attr_type_ = type; }

int Attribute::GetDataType() { return data_type_; }
void Attribute::SetDataType(int type) { data_type_ = type; }

int Attribute::GetLength() { return length_; }
void Attribute::SetLength(int length) { length_ = length; }