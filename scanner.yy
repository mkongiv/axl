%{ 
#include <cerrno>
#include <climits>
#include <cstdlib>
#include <string>
#include "aqcsl.h"

// Work around an incompatibility in flex (at least versions
// 2.5.31 through 2.5.33): it generates code that does
// not conform to C89.  See Debian bug 333231
// <http://bugs.debian.org/cgi-bin/bugreport.cgi?bug=333231>.
# undef yywrap
# define yywrap() 1


using namespace std;
using namespace yy;

#undef YY_DECL
#define YY_DECL yy::aqcsl_parser::symbol_type yylex()
YY_DECL;

yy::location loc;

// Code run each time a pattern is matched.
# define YY_USER_ACTION  loc.columns (yyleng);



%}

%option yylineno
%option noyywrap 

DIGIT [0-9] 
ALPHA [a-zA-Z]

%%

%{
  // Code run each time yylex is called.
  loc.step ();
%}




\/\/.*$   
[ \t]+						{ loc.step (); }
[\n]+							{ loc.lines (yyleng); loc.step (); }

"(+)"						  { 
										//yylval->build (yytext);
										return yy::aqcsl_parser::make_T_COMP_T(loc); 
									}

"[+]"							{ 
										//yylval->build (yytext);
										return yy::aqcsl_parser::make_T_COMP_S(loc); 
                  }

"(*)"						  { 
										//yylval->build (yytext);
										return yy::aqcsl_parser::make_T_ITER_T(loc); 
									}

"[*]"							{ 
										//yylval->build (yytext);
										return yy::aqcsl_parser::make_T_ITER_S(loc); 
                  }

"="							  { 
										//yylval->build (yytext);
										return yy::aqcsl_parser::make_T_EQUAL(loc); 
                  }

":="							{ 
										//yylval->build (yytext);
										return yy::aqcsl_parser::make_T_ASSIGN(loc); 
                  }

"<>"							{ 
										//yylval->build (yytext);
										return yy::aqcsl_parser::make_T_EMPTY_QG(loc); 
                  }

"<"							  { 
										//yylval->build (yytext);
										return yy::aqcsl_parser::make_T_LT(loc); 
                  }

"<="							  { 
										//yylval->build (yytext);
										return yy::aqcsl_parser::make_T_LEQ(loc); 
                  }

">"							  { 
										//yylval->build (yytext);
										return yy::aqcsl_parser::make_T_GT(loc); 
                  }

">="							  { 
										//yylval->build (yytext);
										return yy::aqcsl_parser::make_T_GEQ(loc); 
                  }

";"							  { 
										//yylval->build (yytext);
										return yy::aqcsl_parser::make_T_SEP(loc); 
                  }

":"							  { 
										//yylval->build (yytext);
										return yy::aqcsl_parser::make_T_COLON(loc); 
                  }


","							  { 
										//yylval->build (yytext);
										return yy::aqcsl_parser::make_T_COMMA(loc); 
                  }

"+"							  { 
										//yylval->build (yytext);
										return yy::aqcsl_parser::make_T_ADD(loc); 
                  }

"-"							  { 
										//yylval->build (yytext);
										return yy::aqcsl_parser::make_T_SUB(loc); 
                  }

"*"							  { 
										//yylval->build (yytext);
										return yy::aqcsl_parser::make_T_MUL(loc); 
                  }

"/"							  { 
										//yylval->build (yytext);
										return yy::aqcsl_parser::make_T_DIV(loc); 
                  }

"|"							  { 
										//yylval->build (yytext);
										return yy::aqcsl_parser::make_T_PIPE(loc); 
                  }

"%"							  { 
										//yylval->build (yytext);
										return yy::aqcsl_parser::make_T_ATCH_DOM_B(loc); 
                  }

"(%)"							  { 
										//yylval->build (yytext);
										return yy::aqcsl_parser::make_T_ATCH_DOM_T(loc); 
                  }

"[%]"							  { 
										//yylval->build (yytext);
										return yy::aqcsl_parser::make_T_ATCH_DOM_S(loc); 
                  }


"("							  { 
										//yylval->build (yytext);
										return yy::aqcsl_parser::make_T_LPAR(loc); 
                  }

")"							  { 
										//yylval->build (yytext);
										return yy::aqcsl_parser::make_T_RPAR(loc); 
                  }

"{"							  { 
										//yylval->build (yytext);
										return yy::aqcsl_parser::make_T_LCUR(loc); 
                  }

"}"							  { 
										//yylval->build (yytext);
										return yy::aqcsl_parser::make_T_RCUR(loc); 
                  }

"["							  { 
										//yylval->build (yytext);
										return yy::aqcsl_parser::make_T_LBRA(loc); 
                  }

"]"							  { 
										//yylval->build (yytext);
										return yy::aqcsl_parser::make_T_RBRA(loc); 
                  }

"reverse"				  { 
										//yylval->build (yytext);
										return yy::aqcsl_parser::make_T_REVERSE(loc); 
                  }

"apply"						{ 
										//yylval->build (yytext);
										return yy::aqcsl_parser::make_T_APPLY(loc); 
                  }

"with"						{ 
										//yylval->build (yytext);
										return yy::aqcsl_parser::make_T_WITH(loc); 
                  }

"pi"						  { 
										//yylval->build (yytext);
										return yy::aqcsl_parser::make_T_PI(loc); 
                  }

"codegen"					{ 
										//yylval->build (yytext);
										return yy::aqcsl_parser::make_T_CODEGEN(loc); 
                  }

"backend"				  { 
										//yylval->build (yytext);
										return yy::aqcsl_parser::make_T_BACKEND (loc); 
                  }

"print"						{ 
										//yylval->build (yytext);
										return yy::aqcsl_parser::make_T_PRINT(loc); 
                  }

"export_scop"			{ 
										//yylval->build (yytext);
										return yy::aqcsl_parser::make_T_EXPORT_SCOP(loc); 
                  }

"classic"			    { 
										//yylval->build (yytext);
										return yy::aqcsl_parser::make_T_CLASSIC(loc); 
                  }

"osl"			        { 
										//yylval->build (yytext);
										return yy::aqcsl_parser::make_T_OSL(loc); 
                  }

"int"							{ 
										//yylval->build (yytext);
										return yy::aqcsl_parser::make_T_DT_INT(loc); 
                  }

"circ"						{ 
										//yylval->build (yytext);
										return yy::aqcsl_parser::make_T_DT_CIRC(loc); 
                  }

"statement"				{ 
										//yylval->build (yytext);
										return yy::aqcsl_parser::make_T_DT_STMT(loc); 
                  }

"param"						{ 
										//yylval->build (yytext);
										return yy::aqcsl_parser::make_T_DT_PARAM(loc); 
                  }


"#U"				      { 
										//yylval->build (yytext);
										return yy::aqcsl_parser::make_T_GATE_U(loc); 
                  }


"#ccnot"				  { 
										//yylval->build (yytext);
										return yy::aqcsl_parser::make_T_GATE_CCX(loc); 
                  }

"#CCNOT"				  { 
										//yylval->build (yytext);
										return yy::aqcsl_parser::make_T_GATE_CCX(loc); 
                  }

"#ccX"						{ 
										//yylval->build (yytext);
										return yy::aqcsl_parser::make_T_GATE_CCX(loc); 
                  }

"#CCX"						{ 
										//yylval->build (yytext);
										return yy::aqcsl_parser::make_T_GATE_CCX(loc); 
                  }

"#rtnot"				  { 
										//yylval->build (yytext);
										return yy::aqcsl_parser::make_T_GATE_RTNOT(loc); 
                  }

"#cswap"				  { 
										//yylval->build (yytext);
										return yy::aqcsl_parser::make_T_GATE_CSWAP(loc); 
                  }

"#CSWAP"				  { 
										//yylval->build (yytext);
										return yy::aqcsl_parser::make_T_GATE_CSWAP(loc); 
                  }

"#SQRT_SWAP"		  { 
										//yylval->build (yytext);
										return yy::aqcsl_parser::make_T_GATE_SQRT_SWAP(loc); 
                  }

"#meas"				    { 
										//yylval->build (yytext);
										return yy::aqcsl_parser::make_T_GATE_MEAS(loc); 
                  }

"#swap"				    { 
										//yylval->build (yytext);
										return yy::aqcsl_parser::make_T_GATE_SWAP(loc); 
                  }

"#SWAP"				    { 
										//yylval->build (yytext);
										return yy::aqcsl_parser::make_T_GATE_SWAP(loc); 
                  }

"#QFT"						{ 
										//yylval->build (yytext);
										return yy::aqcsl_parser::make_T_GATE_QFT(loc); 
                  }

"#H"							{ 
										//yylval->build (yytext);
										return yy::aqcsl_parser::make_T_GATE_H(loc); 
                  }

"#R"							{ 
										//yylval->build (yytext);
										return yy::aqcsl_parser::make_T_GATE_R(loc); 
                  }

"#XX"							{ 
										//yylval->build (yytext);
										return yy::aqcsl_parser::make_T_GATE_XX(loc); 
                  }

"#YY"							{ 
										//yylval->build (yytext);
										return yy::aqcsl_parser::make_T_GATE_YY(loc); 
                  }

"#ZZ"							{ 
										//yylval->build (yytext);
										return yy::aqcsl_parser::make_T_GATE_ZZ(loc); 
                  }


"#cX"							{ 
										//yylval->build (yytext);
										return yy::aqcsl_parser::make_T_GATE_CX(loc); 
                  }

"#CX"							{ 
										//yylval->build (yytext);
										return yy::aqcsl_parser::make_T_GATE_CX(loc); 
                  }

"#CNOT"						{ 
										//yylval->build (yytext);
										return yy::aqcsl_parser::make_T_GATE_CX(loc); 
                  }

"#cY"							{
										//yylval->build (yytext);
										return yy::aqcsl_parser::make_T_GATE_CY(loc); 
                  }

"#CY"							{
										//yylval->build (yytext);
										return yy::aqcsl_parser::make_T_GATE_CY(loc); 
                  }

"#cZ"							{ 
										//yylval->build (yytext);
										return yy::aqcsl_parser::make_T_GATE_CZ(loc); 
                  }

"#CZ"							{ 
										//yylval->build (yytext);
										return yy::aqcsl_parser::make_T_GATE_CZ(loc); 
                  }

"#X"							{ 
										//yylval->build (yytext);
										return yy::aqcsl_parser::make_T_GATE_X(loc); 
                  }

"#NOT"					  { 
										//yylval->build (yytext);
										return yy::aqcsl_parser::make_T_GATE_X(loc); 
                  }

"#Y"							{ 
										//yylval->build (yytext);
										return yy::aqcsl_parser::make_T_GATE_Y(loc); 
                  }

"#Z"							{ 
										//yylval->build (yytext);
										return yy::aqcsl_parser::make_T_GATE_Z(loc); 
                  }




{DIGIT}+[.]{DIGIT}+	{ 
                      //yylval->build (yytext);
                      return yy::aqcsl_parser::make_T_DOUBLE (string(yytext),loc);
									  }

{DIGIT}+					{ 
										//yylval->build (yytext);
										return yy::aqcsl_parser::make_T_INTEGER (string(yytext),loc);
									}

({ALPHA}|[_])({DIGIT}|{ALPHA}|[_])*     { 
																					//yylval->build (yytext);
																					return yy::aqcsl_parser::make_T_ID (string(yytext),loc);
																				}

<<EOF>>						{ return yy::aqcsl_parser::make_T_EOF (loc); }
.									{ printf ("Unexpected character\n"); exit (1); }





%%
