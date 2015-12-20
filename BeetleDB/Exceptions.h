//
// Exceptions.h
//
// Created by Chenbjin on 2015/12/20.
// Copyright(C) 2015, Chenbjin. All rights reserved.
//
#pragma once
#ifndef _EXCEPTIONS_H_
#define _EXCEPTIONS_H_

#include <exception>

class SyntaxErrorException : public std::exception {

};

class NoDatabaseSelectedException : public std::exception {

};

class DatabaseNotExistException : public std::exception {

};

class DatabaseAlreadyExistsException : public std::exception {

};

class TableNotExistException : public std::exception {

};

class TableAlreadyExistsException : public std::exception {

};

class IndexAlreadyExistsException : public std::exception {

};

class IndexNotExistException : public std::exception {

};

class OneIndexEachTableException : public std::exception {

};

class BPlusTreeException : public std::exception {

};

class IndexMustBeCreatedOnPrimaryKeyException : public std::exception {

};

class PrimaryKeyConflictException : public std::exception {

};

#endif 

