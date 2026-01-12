#pragma once

/// <summary>
/// Values used in formulas (expressions) have general (variant) types and are ordered lists of scalar (simple) elements: numbers, OwlInstances or OwlClasses.
/// </summary>

//scalar (simple) type of a value element
enum class FormulaValueType : char { Undef, Number, String, Instance, Class, SetPropertyDerived, UnsetPropertyDerived };

//class carries scalar (single) value element
class __declspec(dllexport) FormulaValueElem
{
public:
	FormulaValueElem(double val) : m_type(FormulaValueType::Number), m_dVal(val) {}
	FormulaValueElem(int_t val) : m_type(FormulaValueType::Number), m_dVal((double)val) {}
	FormulaValueElem(const char* str);
	FormulaValueElem(bool val) : m_type(FormulaValueType::Number), m_dVal((double)val) {}
	FormulaValueElem(char val) : m_type(FormulaValueType::Number), m_dVal((double)val) {}
	FormulaValueElem(wchar_t val) : m_type(FormulaValueType::Number), m_dVal((double)val) {}

	FormulaValueElem(FormulaValueType type, RdfsResource val) : m_type(type), m_rdfsVal(val) {}

	FormulaValueElem(const FormulaValueElem& src) : m_type(FormulaValueType::Undef) { copy(src); }

	~FormulaValueElem();

	const FormulaValueElem& operator= (const FormulaValueElem& src) { copy(src); return *this; }

public:
	FormulaValueType type() const { return m_type; }

	operator double() const;

	RdfsResource get(FormulaValueType type) const;

	std::string ToString() const;
	int compare(const FormulaValueElem& v2) const;

private:
	void copy(const FormulaValueElem& src);
	void clear();
	void SetString(const char* str);

private:
	FormulaValueType m_type;

	union
	{
		double		 m_dVal;
		char*		 m_strVal;
		RdfsResource m_rdfsVal;
	};
};

/// <summary>
/// 
/// </summary>
struct FormulaValue
{
	FormulaValue() {}
	FormulaValue(double val) { lst.push_back(val); }

	std::vector<FormulaValueElem> lst;

	bool isNull() const { return lst.empty(); }

	__declspec(dllexport) std::string ToString() const;
	
	__declspec(dllexport) int compare(const FormulaValue& v2) const;

};

/// <summary>
/// 
/// </summary>
extern void TracePrint(FormulaValue const& value);
