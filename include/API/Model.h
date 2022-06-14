#pragma once

/**
* RDF LTD, Engine API, Copyright 2020, 2021-01-25-08-57-33
*/

#include "Factory.h"

/**
* C++ Model
*/
class Model : public _Model
{
public: 

	/**
	* ctor
	*/
	Model()
		: _Model()
	{
	}

	/**
	* ctor
	*/
	Model(int64_t iModel)
		: _Model(iModel)
	{
	}

	/**
	* Factory - Model is the owner
	*/
	_Thing* createByClassName(char* szClassName)
	{
		if ((szClassName == nullptr) || (strlen(szClassName) == 0))
		{
			assert(false);

			return nullptr;
		}

		std::map<std::string, createThing>::iterator itFactory = FACTORY.find(szClassName);
		if (itFactory == FACTORY.end())
		{
			assert(false);

			return nullptr;
		}

		int64_t iClass = GetClassByName(getInstance(), szClassName);
		assert(iClass != 0);

		int64_t iInstance = CreateInstance(iClass, NULL);

		return itFactory->second(this, iInstance);
	}

	/**
	* Factory - Model is the owner
	*/
	_Thing* createByClassInstance(int64_t iClassInstance)
	{
		if (iClassInstance == 0)
		{
			assert(false);

			return nullptr;
		}

		if (!IsClass(iClassInstance))
		{
			assert(false);

			return nullptr;
		}

		int64_t iInstance = CreateInstance(iClassInstance, NULL);

		char* szClassName = NULL;
		GetNameOfClass(iClassInstance, &szClassName);

		std::map<std::string, createThing>::iterator itFactory = FACTORY.find(szClassName);
		if (itFactory == FACTORY.end())
		{
			assert(false);

			return nullptr;
		}

		return itFactory->second(this, iInstance);
	}

	/**
	* Factory - Model is the owner
	*/
	void loadThings()
	{
		assert(getInstance() != 0);

		int64_t iInstance = GetInstancesByIterator(getInstance(), 0);
		while (iInstance != 0)
		{
			int64_t iClassInstance = GetInstanceClass(iInstance);
			assert(iClassInstance != 0);

			char* szClassName = NULL;
			GetNameOfClass(iClassInstance, &szClassName);

			std::map<std::string, createThing>::iterator itFactory = FACTORY.find(szClassName);
			if (itFactory == FACTORY.end())
			{
				// TEST!!!
				itFactory = FACTORY.find("GeometricItem");
				//assert(false);
			}

			itFactory->second(this, iInstance);

			iInstance = GetInstancesByIterator(getInstance(), iInstance);
		} // while (iInstance != 0)	
	}
};