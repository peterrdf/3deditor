#pragma once

#include "formula.h"

////////////////////////////////////////////////////////////////////////
//
// Rules are meta-properties associated with an OwlClass
// They are expressed by formulas
//
// There are following kinds of rules :
//   - equation constraints
//   - fixed values assignments
//   - default values assignments
//
// Inheritance :
//    1.    Assignments for a derived class will override assignment to the same property from parent classes.
//          If property assignment is not defined for a derived class it will use first parent class found. 
//          Multi-inheritance order is undetermined.
//    2.    Equation are collected from a class and all its parents.

    struct RdfRule_s : public RepoResource_s {};
    typedef RdfRule_s* RdfRule;

    //
    RdfRule     DECL STDC       rules_AddRule(OwlClass cls, RdfFile file, ExpressionPtr expr);

    //Iterates rules
    RdfRule     DECL STDC       rules_GetRuleByIterator(OwlClass cls, RdfRule prev);

    //
    ExpressionPtr DECL STDC     rules_GetRuleFormula(RdfRule rule);

    //file were rule is defined
    RdfFile     DECL STDC       rules_GetRuleFile(RdfRule rule);
    
    //
    OwlClass    DECL STDC       rules_GetRuleClass(RdfRule rule);

    //
    bool         DECL STDC      rules_SetRuleFormula(RdfRule rule, ExpressionPtr expr);

    //
    static inline bool          rules_RemoveRule(RdfRule rule) { return rules_SetRuleFormula(rule, NULL); }

    // collect from this and parent classes
    void          DECL STDC     rules_Collect(OwlClass cls, ExpressionList& expressions);

    //-----------------------------------------------------------------------------------
    //insertion adapter is used only to set parameters of a parametric concept
    //and not intended to be shown in UI

    //if so, it will return inserting (parent) class 
    OwlClass         DECL STDC   rules_IsInsertionAdapter(OwlClass cls);

    OwlClass         DECL STDC   rules_MakeInsertionAdapter(OwlClass adopted, RdfFile file);

