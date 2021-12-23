#include "argz/argz.h"
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

/**
 * Determine if two strings, a and b, are equal
 * @param a string 1
 * @param b string 2
 */
#define strequ(a,b) (!(strcmp((a),(b))))


#define maximize( a, b ) ( a = (((a) > (b)) ? (a) : (b)) )

/**
 * Determine if string a begins with strin b
 * e.g : a: "Funnyhaha" b: "Funny" -> true
 *       a: "abc"       b: "def"   -> false
 * @param a string 1
 * @param b string 2
 */
static bool starts_with( const char* a, const char* b )
{
    return memcmp( a, b, strlen(b) ) == 0;
}

/**
 * Get the flag pointer for a given name from a parser
 */
static ArgzFlag* getFlag( ArgzParser* parser, const char* name )
{
    ArgzFlag* ptr = parser->m_flag_base;
    while (ptr){
        if ( strequ( ptr->longkey, name ) || strequ( ptr->shortkey, name ) )
        {
            return ptr;
        }
        ptr = ptr->_m_node.next;
    }
    return NULL;
}

/**
 * Get the keyword pointer for a given name from a parser
 */
static ArgzKeyword* getKeyword( ArgzParser* parser, const char* name )
{
    ArgzKeyword* ptr = parser->m_keyword_base;
    while (ptr){
        if ( starts_with( name, ptr->longkey ) || starts_with( name,ptr->shortkey ) )
        {
            return ptr;
        }
        ptr = ptr->_m_node.next;
    }
    return NULL;
}

/**
 * Get the next unfilled positional from a parser
 */
static ArgzPositional* getPositional( ArgzParser* parser )
{
    ArgzPositional* ptr = parser->m_positional_base;
    while (ptr){
        if (ptr->value == NULL)
        {
            return ptr;
        }
        ptr = ptr->_m_node.next;
    }
    return NULL;
}

char formatKeywordNameBuffer[256];
static const char* formatKeywordName( const char* name )
{
    memset(formatKeywordNameBuffer, 0, 256);
    for(char* fmt = formatKeywordNameBuffer ; *name ; name++, fmt++)
    {
        if (*name != '-' && *name != '\\' && *name != '/')
        {
            *fmt = toupper(*name);
        }
        else
        {
            fmt--;
        }
    }
    return formatKeywordNameBuffer;
}


/**
 * Terminate parsing, and throw an error to stderr
 */
static void error( ArgzParser* parser, const char* message, const char* subject )
{
    const char* progname = parser->progname ? parser->progname : parser->argv[0];
    if (subject)
    {
        printf("%s: %s: %s\n", progname, message, subject);
    }
    else
    {
        printf("%s: %s\n", progname, message);
    }

    printf("usage: %s ", progname);



    {
        ArgzFlag* fptr = parser->m_flag_base;
        while (fptr)
        {
            printf("[ %s ] ", fptr->shortkey);
            fptr = fptr->_m_node.next;
        }

        ArgzKeyword* kptr = parser->m_keyword_base;
        while (kptr)
        {
            printf("[ %s %s ] ", kptr->shortkey, formatKeywordName( kptr->longkey ));
            kptr = kptr->_m_node.next;
        }

        ArgzPositional* pptr = parser->m_positional_base;
        while (pptr)
        {
            printf("%s ", pptr->name);
            pptr = pptr->_m_node.next;
        }
    }

    putc('\n', stdout);

    // descriptions 
    printf("Flag Arguments: \n");
    ArgzFlag* fptr = parser->m_flag_base;
    while (fptr)
    {
        printf("\t%s, %s, \t%s\n", fptr->shortkey, fptr->longkey, fptr->description);
        fptr = fptr->_m_node.next;
    }

    printf("Keyword Arguments: \n");
    ArgzKeyword* kptr = parser->m_keyword_base;
    while (kptr)
    {
        printf("\t%s, %s, \t%s\n", kptr->shortkey, kptr->longkey, kptr->description);
        kptr = kptr->_m_node.next;
    }

    printf("Positional Arguments: \n");
    ArgzPositional* pptr = parser->m_positional_base;
    while (pptr)
    {
        printf("\t%s \t%s\n", pptr->name, pptr->description);
        pptr = pptr->_m_node.next;
    }




    


    exit(1);
}

/**
 * Determine if an argument is a flag or a keyword, 
 * and parse accordingly
 */
static int parseFlagOrKeyword( ArgzParser* parser, size_t i, const char** argv )
{
    ArgzFlag* flag;
    if ( flag = getFlag(parser, argv[i]) )
    {
        // flag
        flag->present = true;
    }
    else 
    {
        ArgzKeyword* keyword = getKeyword( parser, argv[i] );
        if (keyword == NULL)
        {
            // Error
            error(parser, "Invalid Keyword", argv[i]);
        }
        if (strequ(keyword->shortkey, argv[i]) || strequ(keyword->longkey, argv[i]))
        {
            // Value is in next arg
            if (i >= parser->argc)
            {
                // Error
                error(parser, "Unexpected end of arguments", NULL);
            }
            keyword->value = argv[i+1];
        }
        else
        {
            // Value is in the same arg
            if (starts_with(argv[i], keyword->shortkey))
            {
                keyword->value = &argv[i][strlen(keyword->shortkey)];
            }
            else
            {
                keyword->value = &argv[i][strlen(keyword->longkey)];
            }
        }
        // keyword
    }

}

/**
 * Parsing entrypoint
 */
int argzParse( ArgzParser* parser )
{
    for ( size_t i = 1; i < parser->argc; i++ )
    {
        if (parser->argv[i][0] == '-')
        {
            parseFlagOrKeyword( parser, i, parser->argv );
            i++;
        }
        else
        {
            ArgzPositional* pos = getPositional( parser );
            if (pos == NULL)
            {
                error(parser, "Invalid argument", parser->argv[i]);
            }
            pos->value = parser->argv[i];
        }
    }

    ArgzPositional* testPositional;
    if ( testPositional = getPositional(parser) )
    {
        error(parser, "Missing positional", testPositional->name );
    }

    for ( ArgzKeyword* kit = parser->m_keyword_base; kit->_m_node.next ; kit = kit->_m_node.next )
    {
        if (kit->required &&! kit->value)
        {
            error(parser, "Missing required keyword", kit->shortkey);
        }
    }

}


void argzAddFlag( ArgzParser* parser, ArgzFlag* flag )
{
    // Init to null
    flag->present = false;
    flag->_m_node = (ArgzNode) { NULL, NULL };

    // Add to linked list
    if (parser->m_flag_base)
    {
        flag->_m_node.next = parser->m_flag_base;
        parser->m_flag_base->_m_node.prev = ( void* ) flag;
        flag->_m_node.prev = NULL;
    }
    parser->m_flag_base = flag;

}

void argzAddKeyword( ArgzParser* parser, ArgzKeyword* keyword )
{
    // Init to null
    keyword->value = NULL;
    keyword->_m_node = (ArgzNode) { NULL, NULL };

    // Add to linked list
    if (parser->m_keyword_base)
    {
        keyword->_m_node.next = parser->m_keyword_base;
        parser->m_keyword_base->_m_node.prev = ( void* ) keyword;
        keyword->_m_node.prev = NULL;
    }
    parser->m_keyword_base = keyword;
}

void argzAddPositional( ArgzParser* parser, ArgzPositional* positional )
{
    // Init to null
    positional->value = NULL;
    positional->_m_node = (ArgzNode) { NULL, NULL };

    // Add to linked list
    // -note: added in correct order, because positionals need to be in order
    if (parser->m_positional_base)
    {
        if (parser->m_positional_base->_m_node.prev)
        {
            ArgzNode* last = parser->m_positional_base->_m_node.prev;
            last->next = positional;
            positional->_m_node.prev = last;
        } 
        else
        {
            parser->m_positional_base->_m_node.next = positional;
        }
        parser->m_positional_base->_m_node.prev = positional;
    }
    else
    {
        parser->m_positional_base = positional;
    }
}


void argzCreateParser( ArgzParser* parser, int argc, const char** argv, const char* program_name )
{
    memset(parser, 0, sizeof(ArgzParser));
    parser->argc = argc;
    parser->argv = argv;
    parser->progname = program_name;
}