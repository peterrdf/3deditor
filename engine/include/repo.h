
////////////////////////////////////////////////////////////////////////
// References
//   RDF Turtle: https://www.w3.org/TR/turtle/
//   IRI: https://www.ietf.org/rfc/rfc3987.txt
// 
// The Repo component is intended to extend ontology embedded in Geometry Kernel.
// It includes new classes, new inheritances and properties.
// 
// Information is presented as of set of knowleges of various types.
// The knowledges are stored in a set of RdfFiles, and can be serialized as RDF Turtle or similar format
// 
// In calls to Geometry Kernel new classes and properties are referenced by fully qualified IRI notation "<absolute_uri_path>#<local_name>",
// Existing (embedded) Geometry Kernel entities are still identified by their clear/short/local names, like "Box", "Cylinder".
// 
// Repo.h includes functions to manage prefixed names, which are readable shortcuts IRI reference.
// Prefixes are defined in the context of particular file.
// Two types of separators are supported: RDF syntax ':' and expression-style '.'
// Prefix mapped to "http://rdf.bg//geometry.ttl" can be used to identify embedded entities if you want to map empty prefix to another namespace.
// Examples:
//      "pfx:myClass"
//      "pfx.myClass" 
//      ":myClass" - follows RDF syntax, when prefix is empty string it requires to start with ':'
//      "myClass" - when separator is '.' and prefix is empty string, starting separator can be skipped 
//      "geom.Box" and "geom:Box" - will identify Geometry Kernel Box if prefix "geom" is mapped to "http:://rdf.bg/geom.ttl"
// 
// If an error happens while calling repo_* function, use repo_GetErrors to get more information


#ifndef __RDF_LTD__REPO_H
#define __RDF_LTD__REPO_H

#include "engine.h"

#define DECL /*nothing while we use .def on Windows*/
#ifdef _WINDOWS
#define STDC __stdcall
#else
#define STDC /*nothing*/
#endif

///////////////////////////////////////////////////////////////////////////////////////////
/// 
/// 

#define REPO_GEOM_URI        "http://rdf.bg/geometry.ttl"


// base type for all types defined in Repo.h
#ifdef __cplusplus
struct RepoResource_s {};
typedef RepoResource_s* RepoResource;
#else
typedef void* RepoResource;
#endif

// atomic knowledge
#ifdef __cplusplus
struct RepoKnowledge_s : public RepoResource_s {};
typedef RepoKnowledge_s* RepoKnowledge;
#else
typedef struct RepoKnowledge_s {} * RepoKnowledge;
#endif

// RdfFile contains set of knowledges and may define prefixes
#ifdef __cplusplus
struct RdfFile_s : public RepoResource_s {};
typedef RdfFile_s* RdfFile;
#else
typedef struct RdfFile_s {} * RdfFile;
#endif


#ifdef __cplusplus
extern "C" {
#endif

///////////////////////////////////////////////////////////////////////////////////////////
/// 
/// RDF Files

// Creates RdfFile object associated with given URI.
// It does not read content of the file and does not require or check URI exists.
// Returns NULL if the URI was already added or in case of error.
//
RdfFile         DECL  STDC  repo_AddFile                    (OwlModel model, const char* uri);

// Get RdfFile associated with given URI by early call repo_AddFile.
// It's allowed to pass iriRef started with required URI. 
// Returns NULL if there is no RdfFile associated with given URU or in case of error.
//
RdfFile         DECL  STDC  repo_GetFile                    (OwlModel model, const char* uri);

// Returns URI of RdfFile, created early by repo_AddFile
// Returns NULL in case of error
//
const char      DECL* STDC  repo_GetFileURI                 (RdfFile file);

// Changes URI associated with RdfFile and rename all classes, properties and prefixes based on old URI
// Returns success flag
//
bool            DECL STDC   repo_SetFileURI                 (RdfFile file, const char* uri);

// Iterates RdfFiles
//
RdfFile         DECL  STDC  repo_GetFileByIterator          (OwlModel model, RdfFile prev);

///////////////////////////////////////////////////////////////////////////////////////////
/// 
/// Prefixes control

// Defines prefix and sets its URI value.
// If the prefix is already set it will change its value.
// If uri is NULL it will remove (unset) the prefix.
// Returns success flag.
//
bool           DECL STDC    repo_SetPrefix  (RdfFile file, const char* prefix, const char* uri);

// Changes the prefix keeping its URI value.
// Returns success flag.
//
bool           DECL STDC    repo_ChangePrefix(RdfFile file, const char* oldprefix, const char* newprefix);

// Returns URI value of prefix.
// Returns NULL if prefix was not set.
//
const char     DECL* STDC   repo_GetPrefixURI  (RdfFile file, const char* prefix);

// Iterates prefixes defined in context of the RdfFile
//
const char     DECL * STDC  repo_GetPrefixByIterator   (RdfFile file, const char* prev);


// Returns name that can used in calls to Geometry Kernel as class or property name.
// 
// The input prefixedOrIriName can be:
// 
// - fully classified IRIREF name in the format "<absolute_uri_path>#<local_name>" 
//      -- for fully classified Geometry Kernel name "http://rdf.bg/geom.ttl#<name>" the function will return <name> 
//      -- in other cases the it will return IRIRF name unchanged
// (separator is ignored in this case)
// 
// - prefixed name with separator '.':
//      -- for "<prefix>.<local_name>" the function will return URI value of the prefix concatenated with <local_name>
//      -- when separator is missed ("<local_name>") it will assume prefix is empty string
// 
// - prefixed name with separator ':'
//      -- for "<prefix>:<local_name>" the function will return URI value of the prefix concatenated with <local_name>
//      -- when separator is missed ("<local_name>") it will return <local_name>
//
// Return NULL if fails
// !!! Returned pointer is valid until next call to the function
//
const char     DECL * STDC  repo_ResolveName           (RdfFile file, const char* prefixedOrIriName, char separator);

// Returns prefixed name.
// 
// The input iriName can be
// - fully qualified IRIREF name in the format "<absolute_uri_path>#<local_name>"
// - Geometry Kernel embedded name
// 
// The function will return suitable prefixed name or (when no suitable prefix found) fully qualified IFREF name.
//
// Return NULL if fails
// !!! Returned pointer is valid until next call to the function
//
const char     DECL * STDC  repo_GetPrefixedName        (RdfFile file, const char* iriName, char separator);

///////////////////////////////////////////////////////////////////////////////////////////
/// 
/// Classes and Properties control

// Adds information about the property to the RdfFile and calls Geometry Kernel CreateProperty if necessary
// See repo_ResolveName for interpretation of prefixedOrIriName argument
//
// Returns NULL in case of failure 
//
RdfProperty     DECL STDC   repo_AddProperty        (RdfFile file, RdfPropertyType type, const char* prefixedOrIriName, char separator);

//TODO?? rename Add/Remove do Define/Undefine

// Adds information about the class to the RdfFile and calls Geometry Kernel CreateClass if necessary
// See repo_ResolveName for interpretation of prefixedOrIriName argument
//
// Returns NULL in case of failure 
//
OwlClass        DECL STDC   repo_AddClass            (RdfFile file, const char* prefixedOrIriName, char separator);


// Sets new name for a property defined in the RdfFile
// See repo_ResolveName for interpretation of prefixedOrIriName argument
// Returns success flag
//
bool            DECL STDC   repo_RenameProperty         (RdfFile file, RdfProperty prop, const char* prefixedOrIriName, char separator);

// Sets new name for a class defined in the RdfFile
// See repo_ResolveName for interpretation of prefixedOrIriName argument
// Returns success flag
//
bool            DECL STDC   repo_RenameClass            (RdfFile file, OwlClass cls, const char* prefixedOrIriName, char separator);

// Removes knowledge about the property from the RdfFile 
// and calls Geometry Kernel RemoveProperty if no other property definition are known
// Returns:
//      0 - the fact was not known in this file or other failure
//      1 - knowledge was removed but the fact known from other sources
//      2 - knowledge was removed and RemoveProperty was called
//
int_t           DECL STDC   repo_RemoveProperty(RdfFile file, RdfProperty prop);

// Removes knowledge about the class from the RdfFile 
// and calls Geometry Kernel RemoveProperty if no other property definition are known
// Returns:
//      0 - the fact was not known in this file or other failure
//      1 - knowledge was removed but the fact known from other sources
//      2 - knowledge was removed and RemoveProperty was called
//
int_t           DECL STDC   repo_RemoveClass(RdfFile file, OwlClass cls);


// Add class parent knowledge in the RdfFile and calls Geometry Kernel SetClassParent if necessary.
// Returns success flag
//
bool            DECL STDC   repo_AddClassParent        (RdfFile file, OwlClass cls, OwlClass parent);

// Removes class parent knowledge from the RdfFile 
// and calls Geometry Kernel UnsetClassParent if the parenthood is not known anymore
// Returns:
//      0 - the fact was not known in this file or other failure
//      1 - knowledge was removed but the fact known from other sources
//      2 - knowledge was removed and UnsetClassParent was called
//
int_t            DECL STDC   repo_RemoveClassParent(RdfFile file, OwlClass cls, OwlClass parent);


// When the parametric class will be referenced from another model as black box, it can act as child of 'exposed' parent
// Adds the knowledge to RdfFile
// Returns success flag
//
bool            DECL STDC   repo_AddExposedParent       (RdfFile file, OwlClass cls, OwlClass parent);


// When the parametric class will be referenced from another model as black box, it can act as child of 'exposed' parent
// Removes the knowledge to RdfFile
// Returns success flag
//
bool            DECL STDC   repo_RemoveExposedParent(RdfFile file, OwlClass cls, OwlClass parent);

// Specify cardinality restriction and knowledge record in RdfFile
// If the RdfFile already contains knowledge about this class-property pair, it will be updated
// Returns success flag
//
bool            DECL STDC   repo_SetClassPropertyCardRestriction    (RdfFile file, OwlClass cls, RdfProperty prop, int64_t cmin, int64_t cmax);

// Returns cardinality restriction known only in this RdfFile, not taking into account other files and embedded knowledge
// Returns success flag
//
bool            DECL STDC   repo_GetClassPropertyCardRestriction   (RdfFile file, OwlClass cls, RdfProperty prop, int64_t* cmin, int64_t* cmax);

///////////////////////////////////////////////////////////////////////////////////////////
/// 
/// Go through information presented in RdfFile

// Iterate information records in the RdfFile
//
RepoKnowledge   DECL STDC   repo_GetFileKnowledgeByIterator        (RdfFile file, RepoKnowledge prev);

// Iterate information records across all loaded RdfFiles
//
RepoKnowledge   DECL STDC   repo_GetModelKnowledgeByIterator       (OwlModel model, RepoKnowledge prev);

// Returns RdfFile which contains the information record 
//
RdfFile         DECL STDC   repo_GetKnowledgeFile                  (RepoKnowledge knowledge);

// Checks if the 'knowledge' information record is a class declaration
// If yes, *pCls will return the class  
//
bool            DECL STDC   repo_IsKnowledgeCreateClass             (RepoKnowledge knowledge, OwlClass* pCls);

// Checks if the 'knowledge' information record is a property declaration
// If yes, *pProp will return the class  
//
bool            DECL STDC   repo_IsKnowledgeCreateProperty          (RepoKnowledge knowledge, RdfProperty* pProp);

// Checks if the 'knowledge' information record is about class inheritance
// If yes, *pCls and *pParent will return related classes  
//
bool            DECL STDC   repo_IsKnowledgeSetClassParent             (RepoKnowledge knowledge, OwlClass* pCls, OwlClass* pParent);

// Checks if the 'knowledge' information record is about class exposing
// If yes, *pCls and *pParent will return related classes  
//
bool            DECL STDC   repo_IsKnowledgeExposedParent               (RepoKnowledge knowledge, OwlClass* pCls, OwlClass* pParent);

// Checks if the 'knowledge' information record is cardinality restriction
// If yes, *pCls and *pParent will return related classes  
//
bool            DECL STDC   repo_IsKnowledgeSetClassPropertyCardinalityRestriction    (RepoKnowledge knowledge, OwlClass* pCls, RdfProperty* pProp, int64_t* cmin, int64_t* cmax);

///////////////////////////////////////////////////////////////////////////////////////////
/// 
// Utility functions
//

//Gets OwlModel from RdfFile, RepoKnowledge, OwlClass or RdfProperty
//
OwlModel        DECL STDC   repo_GetModel(RepoResource owlResource);

// Check if the 'name' can be used for class, property or prefix.
// 'name' can be fully qualified IRIREF or its local_name part.
// nameType: 'F' prefix, 'C' class, 'P' property
// Returns true if name is valid
//
bool            DECL STDC   repo_IsCorrectName(const char* name, char nameType);

// If 'name' is correct, returns it unchanged, 
// else returns correct name based on suggested
// !!! Returned pointer is valid until next call to the function
//
const char     DECL* STDC   repo_MakeCorrectName(RdfFile file, const char* name, char nameType);

///////////////////////////////////////////////////////////////////////////////////////////
/// 
// Utility functions
//

// Repo supports errors messages buffer.
// This function returns all messages in the buffer.
// !!! Returned pointer us valid until next call to any function from repo.h
//
const char     DECL* STDC   repo_GetErrors(OwlModel model);

// Removes all messages from the error buffer
//
void           DECL  STDC   repo_ClearErrors(OwlModel model);

// Adds message to the error buffer.
//
void           DECL  STDC   repo_AddError(OwlModel model, const char* error);

// Splits iriName in the form <uri_path>#<local_name>
// and returns starting position of local_name in the iriName string.
// uriPathBuff is optional return, caller should provide buffer size at least max (strlen(iriName)+1, 27);
// When input IRI has no '#' assumes uri_path is "http://rdf.bg/geometry.ttl"
// Returns NULL in case of error.
//
const char     DECL* STDC  repo_SplitIRI(const char* iriName, char* uriPathBuff = NULL, int_t uriPathBuffSize = 0);

#ifdef __cplusplus
}

#endif //__cplusplus

#undef DECL
#undef STDC
#endif // __RDF_LTD__REPO_H


