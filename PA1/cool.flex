/*
 *  The scanner definition for COOL.
 */

/*
 *  Stuff enclosed in %{ %} in the first section is copied verbatim to the
 *  output, so headers and global definitions are placed here to be visible
 * to the code in the file.  Don't remove anything that was here initially
 */
%{
#include <cool-parse.h>
#include <stringtab.h>
#include <utilities.h>

/* The compiler assumes these identifiers. */
#define yylval cool_yylval
#define yylex  cool_yylex

/* Max size of string constants */
#define MAX_STR_CONST 1025
#define YY_NO_UNPUT   /* keep g++ happy */

extern FILE *fin; /* we read from this file */

/* define YY_INPUT so we read from the FILE fin:
 * This change makes it possible to use this scanner in
 * the Cool compiler.
 */
#undef YY_INPUT
#define YY_INPUT(buf,result,max_size) \
	if ( (result = fread( (char*)buf, sizeof(char), max_size, fin)) < 0) \
		YY_FATAL_ERROR( "read() in flex scanner failed");

char string_buf[MAX_STR_CONST]; /* to assemble string constants */
char *string_buf_ptr;

extern int curr_lineno;
extern int verbose_flag;

extern YYSTYPE cool_yylval;

/*
 *  Add Your own definitions here
 */
std::string string_const_buf;   // String buffer
static int n_comments = 0;      // For nested comments
%}

/*
 * Define names for regular expressions here.
 */

DIGIT           [0-9]
DARROW          =>
ASSIGN_OP       <-
LESS_THAN_E     <=
LINE_BREAK_OP   \n

%x STRING_SC
%x SL_COMMENT_SC
%x ML_COMMENT_SC


%%

 /*
  * Multi-line comment.
  */
<INITIAL,ML_COMMENT_SC>"(*"   { BEGIN(ML_COMMENT_SC); n_comments++; }
<ML_COMMENT_SC>"*)"           { n_comments--; if (n_comments == 0) BEGIN(INITIAL); }
<ML_COMMENT_SC>"\n"           { curr_lineno++; }
<ML_COMMENT_SC><<EOF>>        { cool_yylval.error_msg = "EOF in comment"; BEGIN(INITIAL); return ERROR; }
<ML_COMMENT_SC>.              { /* Discard everything inside comment. */ }

 /*
  * Unmatched comment.
  */
"*)"                        {
                              cool_yylval.error_msg = "Unmatched *)";
                              return ERROR;
                            }

 /*
  * Single-line comment.
  */
<INITIAL>"--"               { BEGIN(SL_COMMENT_SC); }
<SL_COMMENT_SC>["\n"]       { BEGIN(INITIAL); curr_lineno++; }
<SL_COMMENT_SC><<EOF>>      { BEGIN(INITIAL); }
<SL_COMMENT_SC>.*           { /* Discard comment. */ }

 /*
  * The multiple-character operators.
  */

{DARROW}          { return (DARROW);  }
{ASSIGN_OP}       { return (ASSIGN);  }
{LESS_THAN_E}     { return (LE);      }

 /*
  * Keywords are case-insensitive except for the values true and false,
  * which must begin with a lower-case letter.
  */

(?i:"class")      { return CLASS;     }
(?i:"else")       { return ELSE;      }
(?i:"fi")         { return FI;        }
(?i:"if")         { return IF;        }
(?i:"in")         { return IN;        }
(?i:"inherits")   { return INHERITS;  }
(?i:"let")        { return LET;       }
(?i:"loop")       { return LOOP;      }
(?i:"pool")       { return POOL;      }
(?i:"then")       { return THEN;      }
(?i:"while")      { return WHILE;     }
(?i:"case")       { return CASE;      }
(?i:"esac")       { return ESAC;      }
(?i:"of")         { return OF;        }
(?i:"new")        { return NEW;       }
(?i:"isvoid")     { return ISVOID;    }
(?i:"not")        { return NOT;       }

 /*
  *  Boolean constant - The values start with a lower-case letter.
  */

"t"(?i:"rue")   { cool_yylval.boolean = true; return BOOL_CONST;  }
"f"(?i:"alse")  { cool_yylval.boolean = false; return BOOL_CONST; }

 /*
  * Integer constant.
  */

{DIGIT}+    { cool_yylval.symbol = inttable.add_string(yytext); return INT_CONST; }

 /*
  *  String constants (C syntax)
  *  Escape sequence \c is accepted for all characters c. Except for 
  *  \n \t \b \f, the result is c.
  */

 /*
  * String begins
  */
<INITIAL>"\""         {
                        BEGIN(STRING_SC);
                        string_const_buf = "";
                      }

 /*
  * String ends
  */
<STRING_SC>"\""       {
                        BEGIN(INITIAL);
                        if (string_const_buf.length() >= MAX_STR_CONST)
                        {

                          cool_yylval.error_msg = "String constant too long";
                          return ERROR; 
                        }
                        cool_yylval.symbol = stringtable.add_string((char *)string_const_buf.c_str());
                        return STR_CONST;
                      }

 /*
  * Unterminated string constant (\n, EOF, null character)
  */
<STRING_SC>"\n"       {
                  	curr_lineno++;
			cool_yylval.error_msg = "Unterminated string constant";
                        BEGIN(INITIAL);
                        return ERROR;
                      }


<STRING_SC>"\0"	      {
                        cool_yylval.error_msg = "String contains null character";
                        BEGIN(INITIAL);
                        return ERROR;
                      }

<STRING_SC><<EOF>>    {
                        cool_yylval.error_msg = "EOF in string constant";
                        BEGIN(INITIAL);
                        return ERROR;
                      }

 /*
  * Multi-line string comment i.e. '\\n'
  */
<STRING_SC>"\\\n"     {
                        string_const_buf += "\n";
                        curr_lineno++;
                      }

 /*
  * Escape characters.
  */
<STRING_SC>"\\".      {
                        char escaped_char = yytext[1];
                        switch (escaped_char)
                        {
                          /* Backspace */
                          case 'b':
                          {
                            string_const_buf += "\b";
                            break;
                          }

                          /* Form feed */
                          case 'f':
                          {
                            string_const_buf += "\f";
                            break;
                          }

                          /* New line */
                          case 'n':
                          {
                            string_const_buf += "\n";
                            break;
                          }

                          /* Tab */
                          case 't':
                          {
                            string_const_buf += "\t";
                            break;
                          }

                          default:
                          {
                            string_const_buf += escaped_char;
                          }
                        }
                      }

 /*
  * All other characters.
  */
<STRING_SC>.          {
                        string_const_buf += yytext[0];
                      }
 
 /*
  * Type ID. Starts with a upper case letter. Can contain letters,
  * digits and underscore.
  */
[A-Z][a-zA-Z_0-9]*    {
                        cool_yylval.symbol = idtable.add_string(yytext);
                        return TYPEID;
                      }

 /*
  * Object ID. Begins with a lower case letter. Can contain letters,
  * digits and underscore.
  */
[a-z][a-zA-Z_0-9]*    {
                        cool_yylval.symbol = idtable.add_string(yytext);
                        return OBJECTID;
                      }

 /*
  * Single character constant. Return the ascii value of the character.
  * Last preference because of multi-line comments with (, *, - etc.
  */
";" { return (';' - 0); }   /* Semi Colon         */
"{" { return ('{' - 0); }   /* Open curly braces  */
"}" { return ('}' - 0); }   /* Close curly braces */
"(" { return ('(' - 0); }   /* Open paranthesis   */
")" { return (')' - 0); }   /* Close paranthesis  */
"+" { return ('+' - 0); }   /* Addition           */
"-" { return ('-' - 0); }   /* Subtraction        */
"*" { return ('*' - 0); }   /* Multiply           */
"/" { return ('/' - 0); }   /* Divide             */
":" { return (':' - 0); }   /* Colon              */
"." { return ('.' - 0); }   /* Dot                */
"~" { return ('~' - 0); }   /* Tilde              */
"<" { return ('<' - 0); }   /* LT                 */ 
"=" { return ('=' - 0); }   /* Equal              */ 
"," { return (',' - 0); }   /* Comma              */
"@" { return ('@' - 0); }   /* At operator        */

 /*
  *  Whitespace. Sample lexer does not print to stdout.
  */
[ \r\f\v\t]* { /* Discard */ }

{LINE_BREAK_OP}   { curr_lineno++; }

. { cool_yylval.error_msg = yytext; return ERROR; }

%%
