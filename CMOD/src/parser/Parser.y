/**
*  Parser.y
**/

%{
//typedef char * cstr;

//---------------------------------------------------------------------------//

extern "C" {
  int yyparse(void);
  int yylex(void);
  int yywrap();

}

typedef char * cstr;

#include "../Libraries.h"
#include "../FileValue.h"

//---------------------------------------------------------------------------//

#define YYSTYPE cstr
//#define PARSER_DEBUG
#define YYERROR_VERBOSE
#define YY_USER_INIT yylloc.first_line=1;

//---------------------------------------------------------------------------//

std::list<FileValue*> deleteme;

//---------------------------------------------------------------------------//

/**
*  converts v to a list
**/
FileValue *ToList(FileValue *v) {
  if (v == NULL) return NULL;

  FileValue *ret = new FileValue;
  std::list<FileValue> l;
  l.push_back(*v);
  ret->setOrigType(FVAL_LIST);
  ret->setReturnType(FVAL_LIST);
  ret->setList(l);
  deleteme.push_back(v);
  return ret;
}

//---------------------------------------------------------------------------//

/**
*  Concatenates two lists returning a new list
**/
FileValue *ConcatList(FileValue *v1, FileValue *v2) {
  FileValue *ret;
  
  if (v1 == NULL || v2 == NULL) return NULL;
  if (!(v1->isList()) || !(v2->isList())) return NULL;
  
  ret = new FileValue;
  ret->setOrigType(FVAL_LIST);
  ret->setReturnType(FVAL_LIST);
  ret->setList(v1->getList());
  std::list<FileValue>::const_iterator it;

  for (it = v2->getList().begin(); it != v2->getList().end(); it++) {
    ret->getListPtr(NULL)->push_back((*it));
  }

  deleteme.push_back(v1);
  deleteme.push_back(v2);
  return ret;
}

//---------------------------------------------------------------------------//

/**
*  Prints a filevalue
**/
void printFileValue(const FileValue *v);

//---------------------------------------------------------------------------//

void printList(const FileValue *v)
{
	if (v == NULL) {
		printf("error: tried to print a NULL list\n");
		return;
	}

	if (!(v->isList()) && !(v->isFunction()))
		return;

        if (v->isList()) {
	  std::list<FileValue>::const_iterator it;
	  printf("<");
	  for (it=v->getList().begin(); it!=v->getList().end(); it++) {
	    printFileValue(&(*it));
	    printf(" ");
	  }
	  printf(">");
        } else if (v->isFunction()) {
          std::list<FileValue>::const_iterator it;
          printf("<");
          for (it=v->getFtnArgs().begin(); it!=v->getFtnArgs().end(); it++) {
            printFileValue(&(*it));
            printf(" ");
          }
          printf(">");
        }
}

//---------------------------------------------------------------------------//

/**
*  Prints a filevalue
**/
void printFileValue(const FileValue *v)
{	
	switch(v->getOrigType()) {
	case FVAL_FUNC:
		printf("%s ( ", v->getFtnString().c_str());
		printList(v);
		printf(" )");
		break;
	case FVAL_STRING:
		printf("%s", v->getString().c_str());
		break;
	case FVAL_NUMBER:
		printf("%f", v->getFloat());
		break;
	case FVAL_LIST:
		printList(v);
		break;
	default:
		break;
	}
}

FileValue * newNull()
{
	FileValue *v = new FileValue;
	v->setOrigType(FVAL_NULL);
        v->setReturnType(FVAL_NULL);
	return v;
}

FileValue * newFunc(char *s, FileValue * params)
{
	FileValue *v = new FileValue;
	v->setOrigType(FVAL_FUNC);
	v->setFtnString(s);
	v->setFtnArgs(params->getList());
	deleteme.push_back(params);
	return v;
}

FileValue * newNumber(double n)
{
	FileValue *v = new FileValue;
	v->setOrigType(FVAL_NUMBER);
        v->setReturnType(FVAL_NUMBER);
	v->setNumber(n);
	return v;
}

FileValue * newString(char *s)
{
	FileValue *v = new FileValue;
	v->setOrigType(FVAL_STRING);
        v->setReturnType(FVAL_STRING);
	v->setString(string(s));
	return v;
}

#define TOVALUE(a) ((FileValue*)(a))

struct ltstr
{
  bool operator()(const char* s1, const char* s2) const
  {
    return strcmp(s1, s2) < 0;
  }
};

//extern map<const char*, FileValue*, ltstr> file_data;
int yyerror(const char *msg);

%}
%token SFUNCTOK EQUALTOK NUMBERTOK LISTTOK STRTOK TYPETOK SEMITOK RBRAC LBRAC COMMA SCOLON RPAREN LPAREN RBRACE LBRACE FUNCTOK GLOBALKEYWORDTOK EVENTKEYWORDTOK
%start config
%%


config : 
	one_or_more_eventconfigs { 
		#ifdef PARSER_DEBUG 
		cout << "event config!!!" << endl;
		#endif
		}
	| one_or_more_globalconfigs {
		#ifdef PARSER_DEBUG
			cout << "global config!!!" << endl;
		#endif
		file_data["_global"] = newNumber(1); }
	|
;

/* This is a recursive definition, so that we can accept a string of many eventconfigs */
one_or_more_eventconfigs:
	one_or_more_eventconfigs eventconfig
	| eventconfig
;

eventconfig :
	function SEMITOK { 
		#ifdef PARSER_DEBUG
		printf("saw lone function");
		#endif
		}
	| statement SEMITOK { 
		#ifdef PARSER_DEBUG
		printf("event Statement over\n");
		#endif
		}
;

/* This is a recursive definition, so that we can accept a string of many globalconfigs */
one_or_more_globalconfigs:
	one_or_more_globalconfigs globalconfig
	| globalconfig
;

globalconfig : 
	gstatement SEMITOK { 
		#ifdef PARSER_DEBUG
		printf("global Satement over\n"); 
		#endif
		}
;


gstatement : GLOBALKEYWORDTOK EQUALTOK arg { 
						 #ifdef PARSER_DEBUG
						 printf("saw gstatement\n"); 
						 #endif
					     file_data[$1] = TOVALUE($3); }
;

statement : EVENTKEYWORDTOK EQUALTOK arg  { 
					#ifdef PARSER_DEBUG
					printf(stderr,"Line %d\n",@1.first_line);
					printf("saw statement\n");
					#endif
					file_data[$1] = TOVALUE($3);
					#ifdef PARSER_DEBUG
				    printFileValue(file_data[$1]);
					#endif
				  }
;

arg : NUMBERTOK  { 
		   #ifdef PARSER_DEBUG
		   printf("Saw a number\n");
		   #endif
		   $$ = (char*)newNumber(atof($1)); }
	| list {
			#ifdef PARSER_DEBUG
			printf("saw a list\n"); $$ = $1; 
			#endif
			}
	| STRTOK { 
		   #ifdef PARSER_DEBUG
		   printf("p:Saw a string\n");
		   #endif
		   $$ = (char*)newString($1); }
	| function {
		   #ifdef PARSER_DEBUG
		   printf("saw a function\n"); 
		   #endif
		   $$ = $1; 
		   }
	| SFUNCTOK { 
		#ifdef PARSER_DEBUG
		printf("saw a static function\n"); 
		#endif
		$$ = (char*)newFunc($1, newNull()); }
;

list : LBRAC list_element RBRAC { 
		#ifdef PARSER_DEBUG
		printf("Saw a list list %s\n", $2); 
		#endif
		$$ = $2; }
;

funclist : LPAREN list_element RPAREN { 
		#ifdef PARSER_DEBUG
		cout << "saw function list" << endl; 
		#endif
		$$=$2; }
;

list_element : arg { $$ = (char*)ToList((FileValue*)$1);
			 #ifdef PARSER_DEBUG
		     printf("saw list element ");
		     printFileValue(TOVALUE($$)); printf("\n");
			 #endif
			 }
	| arg COMMA list_element { FileValue * v1 = ToList(TOVALUE($1));
				   #ifdef PARSER_DEBUG
				   printf("inside list ");
				   printFileValue(TOVALUE($1));
				   printf(" ");
				   printFileValue(TOVALUE($3));
				   printf("\n");
				   #endif
				   $$ = (char*)ConcatList(v1, TOVALUE($3)); }
;

function : FUNCTOK funclist  {  
				#ifdef PARSER_DEBUG
				cout << "saw function " << $1 << endl;
				#endif
				$$ = (char*)newFunc($1, TOVALUE($2)); }
;

%%
/*
int main()
{
	printf("Before setting yyin\n");
	extern FILE *yyin;
	yyin = fopen("test.in", "r");
	printf("Set yyin\n");
	yyparse();
	// ef = new EventFactory;
	// PARSER_ARG_STRUCT *ps;
	// ps = new PARSER_ARG_STRUCT;
	// ps->method = 0;
	// ps->args = (void*)file_data["density"];
	// ef->setDensity(ps);
	// printf("yay!\n");
	// delete ps;
	// delete ef;
	return 0;
}
*/

int yyerror (char *msg)
{
	return printf("Error encountered, line %d: %s\n", yylloc.first_line, msg);
}

int yywrap()
{
	return 1;
}
