#ifndef LIBARGZH
#define LIBARGZH

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    void* prev;
    void* next;
} ArgzNode;


typedef struct
{
    const char* shortkey;
    const char* longkey;
    const char* description;
    const char* value;
    ArgzNode _m_node;
} ArgzKeyword;

typedef struct
{
    const char* shortkey;
    const char* longkey;
    const char* description;
    bool present;
    ArgzNode _m_node;
} ArgzFlag;

typedef struct
{
    const char* name;
    const char* description;
    const char* value;
    ArgzNode _m_node;
} ArgzPositional;


typedef struct
{
    const char* option;
    void* parser;
} ArgzSubswitch;

typedef struct
{
    int argc;
    const char** argv;
    const char* progname;
    ArgzKeyword* m_keyword_base;
    ArgzFlag* m_flag_base;
    ArgzPositional* m_positional_base;
} ArgzParser;

/**
 * Once all argument types have been added to a parser,
 * use argzParse to populate them with values, and check
 * for errors.
 * @param parser the parser to use
 */
int argzParse( ArgzParser* parser );

/**
 * Add a flag to a parser
 */
void argzAddFlag( ArgzParser* parser, ArgzFlag* flag );

/**
 * Add a keyword to a parser
 */
void argzAddKeyword( ArgzParser* parser, ArgzKeyword* keyword );

/**
 * Add a positional to a parser
 */
void argzAddPositional( ArgzParser* parser, ArgzPositional* positional );

void argzRequireFlag( ArgzFlag* flag );
void argzRequireKeyword( ArgzKeyword* keyword );
void argzRequirePositional( ArgzPositional* positional );



#ifdef __cplusplus
}
#endif

#endif