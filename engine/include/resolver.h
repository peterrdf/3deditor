#pragma once

#include "formula.h"
#include "rules.h"

////////////////////////////////////////////////////////////////////////
// 

/// <summary>
/// ResolverNodes maps OwlClass to construction tree of OwlInstances.
/// It has root OwlInstance created for OwlClass and next has children according to rules.
/// The tree can be updated when a rule is changed rather then complete construction.
/// </summary>
struct                                      ResolverNode;
typedef std::shared_ptr<ResolverNode>       ResolverNodePtr;
typedef std::list<ResolverNodePtr>          ResolverNodeList;

struct ResolverNode
{
    typedef std::map<RdfProperty, FormulaValue> PropBag;

    ResolverNode();      //use resolver_CreateNode to create
    ~ResolverNode();

    // parameters (input properties) - was set before call to resolver_UpdateNode
    PropBag                                 parameters;

    //
    ParaModelPtr                            model;

    //
    // this node of geometry construction
    //

    OwlClass                                cls = NULL;
    OwlInstance                             instance = NULL;

    //
    // tree structure
    //

    struct PropertyChildren
    {
        ExpressionPtr              expr;  //rule which created this children nodes
        ResolverNodeList           nodes; //list of children node
        bool                       inuse = false; //always true when update procedure is finished
    };

    struct PropertyValueRange
    {
        RdfProperty                                      prop;
        double                                           minValue;
        double                                           maxValue;  
    };

    typedef std::map<OwlObjectProperty, PropertyChildren> Children;

    //
    Children                                children;
    std::weak_ptr<ResolverNode>             parent;

    //
    // track creation, updating, tracing and error information
    //
    std::map<RdfProperty, FormulaValue>     populatedFromContainers; //properties known from container

    //rules processing information
    ExpressionList                          instructions;            //solved rules (script to create/update children and properties)

    //build instructions tracking
    int64_t                                 rulesTimeMark = 0;       //class modification mark when instructions were built
    PropBag                                 rulesInput;              //properties used in rules and their known values
    ExpressionList                          unsolvedRules;           //list of rules can not be resolved

    //evaluate instructions tracking
    std::set<RdfProperty>                   undefindedVariables;     //these properties needs values 
    std::set<RdfProperty>                   fixedProps;              //properties with fixed assignments :=
    std::list<RdfProperty>                  violateType;             //rules violate type for these properties
    std::list<RdfProperty>                  violateCardinality;      //rules violate cardinality for these properties

    unsigned long                           updateCounter = 0;        //updated every time the instance changed

    PropBag                                 populatedProps;           //all calculated properties after last update
};

/// <summary>
/// Position of a node in construction tree
/// </summary>
struct ResolverChildNodeLocator
{
    OwlObjectProperty prop;
    int_t             index;
};

typedef std::list<ResolverChildNodeLocator> ResolverNodePath;

typedef std::list<std::pair<ResolverNodePtr, std::string>> ResolverNodeErrors;

/// <summary>
/// Data to track changes in construction tree
/// </summary>
struct ResolverChange
{
    enum Type { Undef, BuildInstructions, AddNestedNode, RemoveNestedNodes, CreateInstance, ReplaceInstance, UpdateProperty };

    Type                type = Undef;
    ResolverNodePtr     node;
    ResolverNodePath    path;
    std::string         info;

    ResolverChange(Type type_, ResolverNodePtr node_);
};

typedef std::list<ResolverChange> ResolverChangeLog;

/// <summary>
/// implements formulas context based on containment
/// </summary>
class IResolverContext : public IFormulaContext
{
public:
    virtual void GetKnownProperties(std::set<RdfProperty>& props) = NULL;
    virtual ResolverNodePtr GetValueSource(RdfProperty prop, FormulaValue* value) = NULL;
};

typedef std::shared_ptr<IResolverContext> ResolverContextPtr;

////////////////////////////////////////////////////////////////////////
// 

// create empty node
ResolverNodePtr DECL STDC   resolver_CreateNode(OwlClass  cls);

// create or update construction tree, optionally gives change information
bool            DECL STDC   resolver_UpdateNode(ResolverNodePtr& node, ResolverChangeLog* changelog);


// check if node was created successfully, and collect errors per node
bool DECL STDC resolver_GetErrors(ResolverNodePtr& node, ResolverNodeErrors* errors = NULL);

std::string DECL STDC resolver_DumpAllErrors(ResolverNodePtr& node);

// get context to evaluate formulas
ResolverContextPtr  DECL STDC   resolver_GetContext(ResolverNodePtr& node);

// get root node of the tree that contains given node, and its location in construction tree
void DECL STDC resolver_GetRootNode(ResolverNodePtr node, ResolverNodePtr* root, ResolverNodePath* path);

// print properties
extern void TracePrint(ResolverNode::PropBag const& props);


////////////////////////////////////////////////////////////////////////
// 

/// <summary>
/// Collect information which properties are required to completely define the node or can affect given node
/// </summary>

struct ResolverInterfaceProperty
{       
    RdfProperty        prop = NULL;

    bool               fromParts = false;
    std::set<OwlClass> hasDefaultValue;    //has default value (left side of ~ rule), i.e. optional property
    
    //source of requirement
    std::list<RdfRule> rules;              //requiring (undefined in) rules
    int_t              cmin = -1;          //cardinality restrictions
    int_t              cmax = -1;

    bool IsRequired() { return hasDefaultValue.empty() && (cmin > 0 || !rules.empty()); }
};

typedef std::list<ResolverInterfaceProperty> ResolverInterface;

bool DECL STDC resolver_GetClassInterfaceDetails(ResolverNodePtr node, ResolverInterface& iface);

bool DECL STDC resolver_GetExposed(OwlClass cls, std::set<OwlClass>& parents);

std::string DECL STDC resolver_FormatPath(ResolverNodePath& path, ResolverNodePtr root = NULL);
