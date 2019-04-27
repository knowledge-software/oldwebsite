//	Knowledge Software Consulting	� Paul Beeken, 1999  Work in Progress.//--------------------// The idea is to build a parser that will take a simple calculation expression//	and return a result.  The input is a string.  The output a string representing//	the result of the calculations.//	I impose a couple of constraints on the lexicon, parser and compiler. 1) I can't//	have linked lists.  2) the lexicon must be simple and reentrant.  3) is need only//	be left associative.//#include	<string.h>#include	<stdlib.h>#include	<ctype.h>#include	"LexicalParser.h"char*	getToken( char* str, char* tok );struct stack	{//	struct	stack*	next;	token	value;	};typedef	struct stack	stack, *stackPtr;////	This routine will parse the string into a linear list of tokens//void	parseString( char* str )	{	token	theStack[100];	short	stackPtr = 0;	char*	strPtr;	char tok[100]; // = strtok( str, " \t\r\n" );	strPtr = getToken( str, tok );	while ( tok[0] != '\0' ) {		theStack[ stackPtr ].value = 0.0;		if ( strcmp( tok, "+" ) == 0 ) {			theStack[ stackPtr ].type = Add;			}		else		if ( strcmp( tok, "-" ) == 0 ) {			theStack[ stackPtr ].type = Subtract;			}		else		if ( strcmp( tok, "*" ) == 0 ) {			theStack[ stackPtr ].type = Multiply;			}		else		if ( strcmp( tok, "/" ) == 0 ) {			theStack[ stackPtr ].type = Divide;			}		else		if ( strcmp( tok, "�" ) == 0 ) {	// Macintosh opt-shift-8			theStack[ stackPtr ].type = AngleDeg;			}		else		if ( strcmp( tok, "�" ) == 0 ) {	// Windows opt-0			theStack[ stackPtr ].type = AngleDeg;			}		else		if ( strcmp( tok, "'" ) == 0 ) {			theStack[ stackPtr ].type = AngleMin;			}		else		if ( strcmp( tok, "\"" ) == 0 ) {			theStack[ stackPtr ].type = AngleSec;			}		if ( *tok == '(' ) {			// We are dealing with a sub expression.			}		else	{	// Assume whatever is left over is a number.			theStack[ stackPtr ].type = Number;			theStack[ stackPtr ].value = atof( tok );			}		stackPtr++;		strPtr = getToken( strPtr, tok );		}	// At this point we have a linear array of tokens as they appear in the parser.	// Up end the stack an begin to build the result.	float result = 0.0;	short	err = noErr;	while( stackPtr >= 0 ) {		switch( theStack[ stackPtr ].type ) {			case Number:	result = theStack[ stackPtr ].value;							break;			case Add:		stackPtr--;							if ( stackPtr >= 0 )								if ( theStack[ stackPtr ].type == Number )									result += theStack[ stackPtr ].value;								// otherwise it is a uniary plus, we ignore.							break;			case Subtract:	stackPtr--;							if ( stackPtr >= 0 )								if ( theStack[ stackPtr ].type == Number )									result -= theStack[ stackPtr ].value;								else									result = -result;								// otherwise it is a uniary negate, we ignore.							break;			case Multiply:		stackPtr--;							if ( stackPtr >= 0 )								if ( theStack[ stackPtr ].type == Number )									result *= theStack[ stackPtr ].value;								else									err = expresionErr;								// otherwise it is a uniary plus, we ignore.							break;			case Divide:	stackPtr--;							if ( stackPtr >= 0 )								if ( theStack[ stackPtr ].type == Number )									result /= theStack[ stackPtr ].value;								else									err = expresionErr;								// otherwise it is a uniary plus, we ignore.							break;			case AngleSec:	stackPtr--;							if ( stackPtr >= 0 )								if ( theStack[ stackPtr ].type == Number )									theStack[ stackPtr ].value /= 3600;								// otherwise it is a uniary plus, we ignore.							break;			}			stackPtr--;		}}