// This file is ment to be included inside another source file.  It is a stub for the//	function definitions that alllow for Arithmetic parsing.//	Knowledge Software Consulting	� Paul Beeken, 1999  Work in Progress.//--------------------// The idea is to build a parser that will take a simple calculation expression//	and return a result.  The input is a string.  The output a string representing//	the result of the calculations.//	I impose a couple of constraints on the lexicon, parser and compiler. 1) I can't//	have linked lists.  2) the lexicon must be simple and reentrant.  3) it need only//	be left associative.////--------------------//	Parser...//		This implementation of a string translator allows the caller to track the//		position of the indicator.  It takes a pointer to a null terminated string//		or position indicator and scans forward spining out tokens.  The caller is//		responsible for makeing space for the tokens.//	The grammar for this is://	tok -> num op//	num -> [0-9.]//	op	-> [+-*/��"']voidTuple::createTuple( char* tokStr )	{//	Tuple	tuple;		setType( Undefined );	setValue( 0.0 );	setFunc( 0L );		if ( *tokStr == EOS ) {		setType( Terminal );		}	else	if ( strcmp( tokStr, "+" ) == 0 ) {		setType( Addition );	// The compiler may change this later to Unary plus		}	else	if ( strcmp( tokStr, "-" ) == 0 ) {		setType( Subtract );	// The compiler may change this later to Unary negate		}	else	if ( strcmp( tokStr, "*" ) == 0 ) {		setType( Multiply );		}	else	if ( strcmp( tokStr, "/" ) == 0 ) {		setType( Divide );		}	else	if ( strcmp( tokStr, "%" ) == 0 ) {		setType( Percent );		}	else	if ( strcmp( tokStr, "�" ) == 0 ) {	// Macintosh opt-shift-8		setType( AngleDeg );		}	else	if ( strcmp( tokStr, "�" ) == 0 ) {	// Windows opt-0		setType( AngleDeg );		}	else	if ( strcmp( tokStr, "'" ) == 0 ) {		setType( AngleMin );		}	else	if ( strcmp( tokStr, "(" ) == 0 ) {		setType( OpenParen );		}	else	if ( strcmp( tokStr, ")" ) == 0 ) {		setType( CloseParen );		}	else	if ( strcmp( tokStr, "\"" ) == 0 ) {		setType( AngleSec );		}	else	if ( strcmp( tokStr, "^" ) == 0 ) {		setType( PowerOf );		}	else	if ( strcmp( tokStr, "," ) == 0 ) {		setType( Comma );		}	else	if ( isalpha( *tokStr ) )	{	// hash the alphanumeric string.		setType( Function );		setFunc( hash( tokStr ) );		}	else	if ( isdigit( *tokStr ) ) {	// Assume whatever is left over is a number.		setType( Number );		setValue( atof( tokStr ) );		}}ULongTuple::hash( char* str )	{	ULong h = 0;	while(*str) {		h	=	(h << 4) + toupper(*str++);	//	case insensitive		ULong g = h & 0xF0000000L;		if ( g )			h	^=	g >> 24;			h	&=	~g;		}	return h;}