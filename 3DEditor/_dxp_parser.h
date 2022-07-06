#pragma once


#include <algorithm> 
#include <functional> 
#include <cctype>
#include <locale>

#include <iostream>
#include <fstream>

#include <vector>
#include <string>

using namespace std;

namespace _dxf
{
	// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
	// _dxf_reader
	// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
	class _dxf_reader
	{
	private:
		ifstream& m_dxfFile;
		vector<string> m_vecBuffer;

	public:
		_dxf_reader(ifstream&);
		virtual ~_dxf_reader();
		bool read();
	};
	// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

	// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
	// _group_codes
	// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
	class _group_codes
	{
	public:	
		// 0 Text string indicating the entity type (fixed)
		static const string start;		
		// 2 Name (attribute tag, block name, and so on)
		static const string name;		
		static const string eof;
		static const string section;
		static const string end_section;
		static const string entities;
		static const string line;
	};
	// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

	// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
	// _group
	// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
	class _group
	{

	private: // Members
		string m_strName;

	public: // Methods
		_group(const string& strName);
		virtual ~_group();
		const string& name() const;
	};
	// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

	// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
	// _section
	// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
	class _section : public _group
	{
	private:

	public:
		_section(const string& strName);
		virtual ~_section();
	};
	// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

	// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
	// _entities_section
	// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
	class _entities_section : public _section
	{
	private:

	public:
		_entities_section();
		virtual ~_entities_section();
	};
	// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

	// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
	// _parser
	// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
	class _parser
	{
	private:
		int64_t m_iModel;

	public:
		_parser(int64_t iModel);
		virtual ~_parser();
		void load(const wchar_t* szFile);
	};
	// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
};

