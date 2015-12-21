//
// Interpreter.h
//
// Created by Chenbjin on 2015/12/18.
// Copyright(C) 2015, Chenbjin. All rights reserved.
//
#pragma once
#ifndef _INTERPRETER_H_
#define _INTERPRETER_H_

#include <string>
#include <vector>
#include "API.h"
#include "SQLStatement.h"

using namespace std;

class Interpreter
{
public:
	Interpreter(void);
	~Interpreter(void);
	void ExecSQL(string statement);
private:
	API* api;
	string sql_statement_;
	vector<string> sql_vector_;
	int sql_type_;
	vector<string> SplitSQL(string statement, string seg);
	void GeneralizeSQL();
	void GetSQLType();
	void ParseSQL();
};

#endif
