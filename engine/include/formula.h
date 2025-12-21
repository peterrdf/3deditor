#pragma once

/*
VOCABULARY

"Formula" mainly refers to text serialization of an "expression", while "expression" refers to a parsed structure of the same.
They are operations and functions over "constants" and "variables".

"Number" numeric constant value.

"Variable" is an RdfProperty.

"Formula context" maps some ("known") variables to its datatype or instance values.

"Assignment" is a formula of kind <variable>:=<right-formula> or <variable>~<right-formula>, where the variable does not appear in the right-formula
~ is called default value assignment

"Equation"(constrain, restriction) is a formula of kind <left-formula>=<right-formula>

"Rule" is an assignment or equation.

"Formula Context" provides values for variables, if a variable has value it is named "known" variable.

When all variables in a formula are known, the formula may be "evaluated" to "formula value".

Assignments with all known variables in its right formula provides the value to its left variable. 

Equation with only one variable unknown can be resolved to assignment.

Equation with all variables known is an "assertion". It is evaluated to 1(true) or 0(false).
    Remark: maybe we should use == (boolean operations) to separate assertions from rules? Equation cannot be evaluated in with this approach.

"Instructions" is an well-ordered list of expressions, where each assignment enriches input context
with calculated value that can be used to evaluate the next expressions.

*/

#include "formulaValue.h"

////////////////////////////////////////////////////////////////////////
// 

/// <summary>
/// Type for atomic formulas is constant, variable, type or function.
/// </summary>
enum class FormulaType : unsigned char
{
    Undefined,

    //unary type
    Inversion,

    //binary type
    Add,
    Subtract,
    Multiply,
    Divide,
    Pow, //exponent
    Reference, //reference to property value <ObjectProperty>::<Property>

    List, //a,b,c

    //functions
    Sin,
    Cos,
    Tan,
    Asin,
    Acos,
    Atan,
    Atan2,
    Sqr,
    Sqrt,
    Tranc,
    FindRoot, //find root of rule

    //terminal constant production
    Number,   //Numeric constant
    String,   //Textual constant 
    Class,    //owlClass constant
    Property, //rdfProperty (variable)

    //rules
    Equality,
    Assignment,
    DefaultValue, //aka soft assignment

    //Parsing time symbols (only for temporary/internal use, not used in final expressions)
    OpenParenthis,
    CloseParenthis,
    OpenCurlyBrace,
    CloseCurlyBrace,
    Sentinel
};

/// <summary>
/// Expression is parsed formula structure
/// </summary>
struct Expression;

typedef std::shared_ptr<Expression> ExpressionPtr;

typedef std::vector<ExpressionPtr> ExpressionList;

struct Expression
{
    FormulaType type;

    //union
    ExpressionList  operands;
    RdfProperty		rdfProperty = 0;
    OwlClass		owlClass = 0;
    double			dValue = 0;
    std::string     strValue;

    std::weak_ptr<ParaModel>  model;

    FormulaValue    value;  //can bring formula value in ResolverNode

public:
    _declspec(dllexport) Expression(std::weak_ptr<ParaModel> model, FormulaType type);
    _declspec(dllexport) ~Expression();

    void ClearError()                       { m_error.clear(); }
    void AddError(const char* error);
    const char* GetError()                  { return m_error.empty() ? NULL : m_error.c_str(); }

    //expression can have external m_trackingData which survives in formula transformation
    //it is used to track source rule while build instructions
    void SetTrackingData(void* data);
    void* GetTrackingData() { return m_trackingData; }
private:
    std::string   m_error;  //can bring error information in ResolverNode
    void*         m_trackingData = NULL;
};

/// <summary>
/// It is caller decision how to provide values for variables, and caller must implement this interface
/// </summary>
struct IFormulaContext
{
    virtual bool GetValue(RdfProperty prop, FormulaValue* value = NULL) = NULL;
    virtual void TracePrint() = NULL;
};

/// <summary>
/// Mode of printing expression to a formula text
/// </summary>
enum class FormulaFormat
{
    Serialization, //serialization format, it can be parsed back
    Display      //better human readable but can not be parsed
};


/// <summary>
/// Creates expression of given type
/// </summary>
static inline ExpressionPtr formula_Create(std::weak_ptr<ParaModel> model, FormulaType type) { return std::make_shared<Expression>(model, type); }


/// <summary>
/// Parse text string of the formula formatted with prefixes defined in specified file
/// In case of error returns NULL, paralib_GetLastError will provide the error details
/// </summary>
ExpressionPtr       DECL STDC  formula_Parse
                                    (const char*    formula, //formula in serialized format 
                                     RdfFile        file,    //path NULL for URI-formatting
                                     ParaModelPtr   model    //can be NULL if file is not NULL
                                    );


/// <summary>
///Saves expression to string, formatted with prefixes defined in specified file
///In case of error it returns NULL, and paralib_GetLastError can provide the error details
/// </summary>
std::string     DECL STDC formula_Format
                                    (ExpressionPtr  expr,
                                     RdfFile        file, //path NULL for URI-formatting (FormulaFormat::Serialization) or use default file (FormuaFormat::Display)
                                     FormulaFormat  format
                                    );

OwlClass  DECL STDC formula_ParseClassTree
                                    (const char*    formula, //formula in serialized format 
                                     RdfFile        file,    //path NULL for URI-formatting
                                     OwlModel       model    //can be NULL if file is not NULL
                                    );

std::string     DECL STDC formula_FormatClassTree
                                    (OwlClass       ruleNode,
                                     RdfFile        file, //path NULL for URI-formatting (FormulaFormat::Serialization) or use default file (FormuaFormat::Display)
                                     OwlModel       model, //can be NULL if file is not NULL
                                     FormulaFormat  format
                                    );

/// <summary>
/// Calculates value for expression
/// Formula context must provide values for all variables, except (in case of assignment) left side of assignment.
/// It returns false if the expression can not be evaluated, paralib_GetLastError will provide the error details
/// For an assignment the resulting value is the right formula value.
/// For an assertion the resulting value is 1 (accepted) or 0 (failed).
/// </summary>
bool            DECL STDC  formula_Evaluate(
                            ExpressionPtr    expr,
                            IFormulaContext* input, 
                            FormulaValue&    value
                            );



/// <summary>
/// resolve input set of rule to a sequence of assignments and assertions
/// returns false if not possible, paralib_GetLastError will provide the error details
/// </summary>
bool            DECL STDC  formula_BuildInstructions(
                                    ExpressionList&     expressions,     //IN-OUT
                                    IFormulaContext*    input,
                                    ExpressionList*     remains = NULL   //remaining, if you allow partial solution
                                    );


/// <summary>
/// Returns set of properties and classes used in expression
/// If classes = NULL if will recursively add to props all properties from rules of dependency classes
/// returns false if fails, paralib_GetLastError will provide the error details
/// </summary>
bool            DECL STDC   formula_GetReferences(
                         ExpressionPtr           expr,
                         std::set<RdfProperty>*  props,
                         std::set<OwlClass>*     classes
                    );


/// <summary>
/// Returns true if the expression is an assignment, and can return information about the assignment
/// returns false if fails, paralib_GetLastError will provide the error details
/// </summary>
bool            DECL STDC   formula_IsAssignment(ExpressionPtr expr, RdfProperty* pLeft = NULL, ExpressionPtr* ppRight = NULL, bool* isDefaultValue = NULL);


/// <summary>
/// Gets property value from given resource as formula value
/// returns false if fails, paralib_GetLastError will provide the error details
/// </summary>
bool            DECL STDC   formula_GetPropertyValue(RdfsResource propHolder, RdfProperty prop, FormulaValue& value);


/// <summary>
/// Sets property value to given resource from formula value
/// returns false if fails, paralib_GetLastError will provide the error details
/// </summary>
bool            DECL STDC   formula_SetPropertyValue(RdfsResource propHolder, RdfProperty prop, FormulaValue const& value);


/// <summary>
/// 
/// </summary>
FormulaValue    DECL * STDC   formula_NewValue();
void            DECL STDC   formula_DeleteValue(FormulaValue* value);
