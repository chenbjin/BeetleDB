#pragma once
#ifndef _API_H_
#define _API_H_

#include <string>

using namespace std;

class API
{
public:
	API(string path);
	~API(void);
	void Quit();
	void Help();
	void CreateDatabase();
	void CreateTable();
	void CreateIndex();
	void ShowDatabases();
	void ShowTables();
	void DropDatabase();
	void DropTable();
	void DropIndex();
	void Use();
	void Insert();
	void Exec();
	void Select();
	void Delete();
	void Update();

private:
	string path_;
	string current_db_;
};

#endif
