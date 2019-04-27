//	Knowledge Software Consulting	� Paul Beeken, 1999  Work in Progress.//--------------------// The idea is to build a parser that will take a simple calculation expression//	and return a result.  The input is a string.  The output a string representing//	the result of the calculations.//	I impose a couple of constraints on the lexicon, parser and compiler. 1) I don't//	want linked lists.  2) the lexicon must be simple.  3) it need only//	be left associative.//	It should be obvious to anyone familiar with parsing that I got alot of help from//	the "Dragon Book", Thanks guys.  //--------------------//	Parser...//		This implementation of a string translator allows the caller to track the//		position of the indicator.  It takes a pointer to a null terminated string//		or position indicator and scans forward spining out tokens.  The caller is//		responsible for makeing space for the tokens.//	The grammar for this is://	tok -> num op//	num -> [0-9.]//	op	-> [+-*/��"']#include	<cstring>#include	<cstdlib>#include	<cctype>#include	<cmath>#include	<stdio.h>#ifdef DEBUG#undef DEBUG#endif#define	DEBUG				0#define	DONT_USE_THROW		0#define	DEBUG_STACK_TRACE	0	// produces a trace stack for the functions#include	"LR1LA.h"using namespace std;#if DEBUG || DEBUG_STACK_TRACE#include <iostream>#endif//***********//*** Function List//***********char*	gFuncList[]	=	 {	"sinh", "asinh", "cosh", "acosh", "tanh", "atanh", 		"sin", "asin", "cos", "acos", "tan", "atan", 		"exp", "sqrt", "ln", "log", "pi" };enum {	SINH=0,	ASNH ,	COSH,	ACSH,	TANH,	ATNH,			SIN ,	ASIN,	COS ,	ACOS,	TAN ,	ATAN,		EXP ,	SQRT,	LN  ,	LOG ,	PI	};//***********//*** Function List//***********//*******************************************************************//	LR1LA				constructor							[public]//		Builds the inital token stream from a supplied string.//*******************************************************************LR1LA::LR1LA( char* inString ) : Parser( inString ){	errorMsg[0] = '\0';	// initially empty}LR1LA::~LR1LA(){}//*******************************************************************//	Answer			method									[public]//	Produces the result in a string form.//*******************************************************************boolLR1LA::Answer( char* theResult )	{	if ( *errorMsg ) {		strcpy( theResult, errorMsg );		return false;		}			// stack index will be 1 unless there was a syntax error.	double val = 	tupStack[stackIndex];//		ostrstream out( theResult, 200 );//		out << double( tupStack[stackIndex] );//		//		if ( europeanStandard ) { // European settings.//			long sign = copysign ( 1, val );//			double ival = floor( abs( val ) );//			long mant = roundtol( 1000000.0 * fmod( abs(val), ival ) );//			sprintf( theResult, "%ld,%ld", sign*(long)ival, mant );//			}//		else	sprintf( theResult, GetFormat(), val );	return true;				}//*******************************************************************//	Compile					method							[public]//	This is the public method that initiates the action.//*******************************************************************void	LR1LA::Compile( char* inString )	{		if ( inString != nil )		SetStream( inString );		errorMsg[0] = '\0';	// initially empty#if DONT_USE_THROW											// no try...catch		#else	try {									// standard try...catch#endif		parse();		if ( stackIndex != 1 )			error( "expression syntax error" );#if DONT_USE_THROW											// no try...catch		#else		}									// standard try...catch	catch( ... ) {							// standard try...catch		}									// standard try...catch#endif#if DEBUG	DumpToken( result(), true );#endif}#if DEBUG || DEBUG_STACK_TRACE		//*******************************************************************//	DumpToken				method							[public]//	This is the public method that helps in debugging.//*******************************************************************voidLR1LA::DumpToken( Tuple t, bool sIdx )	{	cout << "Tuple Dump... ";	if( sIdx )		cout << "[" << stackIndex << "]";			cout << "\t\t tk:";		unsigned short typ = t.getType();	cout.write( (char*)&typ, 2 );	cout << " ";		unsigned long fun = t.getFunc();	if ( t.is( Function ) )		cout << ULong( t.getFunc() );	else if ( t.is( Number ) || t.is( AngleDeg ) )		cout <<  t;	cout << "..." << endl;}#endif//*******************************************************************//	parse					method							[private]// We wish to implement the following grammar://		Under the rules of this grammar there is a set of special high priority postfix operators//		for identifying angles.  There is a special addition operator which is coupled to this called//		the Delta operator.  Syntaxes such as 14�13' produce streams that look like: 14 deg � 13 min//		whose operators should have priority over the standard aritmetic operators +,-,*, and /.//		The basis for this results in the following expectation://		2 * 14�32'16"  -> 2 * (14�+32'+16")  i.e. the angle components stay together.//	expr	->	expr + term//				expr - term//			*	� + term	// unary plus	// These productions are generated dynamically//			*	� - term	// unary minus	// if + and - descend to factor, see below.//				term//	term	->	term * value//				term / value//				value//	value	->	term ^ factor//				term %//				term �//				term '		//				term "//	factor	->	( expr )//				� + term	// unary plus	// These productions are generated dynamically.//				� - term	// unary minus	// if addition and subtraction descend to this level.//				FUNC//				NUM [ deg // special terminals//			       || min // which modify the number.//				   || sec ]//				;			// prefactor terminates all expressions.////*******************************************************************voidLR1LA::parse()	{	stackIndex = 0;	// make sure that the stack has been reset.	currentToken = GetToken();	while ( currentToken.isnot( Terminal ) ) {		expr();		}}//*******************************************************************//	expr					method							[private]voidLR1LA::expr()	{#if DEBUG_STACK_TRACE	cout << "expr()...";	DumpToken( currentToken );	cout << endl;#endif	term();	for(;;) {		switch( currentToken.getType() ) {			case	Addition:			case	Subtract:				Tuple t = currentToken;				match( currentToken ); 				term(); 				emit( t );				continue;			default:				return;			}		}}//*******************************************************************//	term					method							[private]voidLR1LA::term()	{#if DEBUG_STACK_TRACE	cout << "term()..." ;	DumpToken( currentToken );	cout << endl;#endif	value();	for(;;) {		switch( currentToken.getType() ) {			case	Multiply:			case	Divide:				Tuple t = currentToken;				match( currentToken );				value();				emit( t );				continue;			default:				return;			}		}}//*******************************************************************//	value					method							[private]voidLR1LA::value()	{#if DEBUG_STACK_TRACE	cout << "value()..." ;	DumpToken( currentToken );	cout << endl;#endif	factor();	for(;;) {		switch( currentToken.getType() ) {			case	PowerOf:	{				Tuple t = currentToken;				match( PowerOf );				factor();				emit( t );				}				continue;			case AngleDeg:			case AngleMin:			case AngleSec:				emit( currentToken );				match( currentToken );				if ( currentToken.is( Number ) ) {					value();	// there is a trailing number.								// it is probably a construct like: 13�14'32"								// or such.  13�15 will give unpredictable results.					}				continue;			case Percent:				emit( currentToken ); 				match( Percent );				continue;			default:				return;			}		}}//*******************************************************************//	factor					method							[private]voidLR1LA::factor()	{#if DEBUG_STACK_TRACE	cout << "factor()..." ;	DumpToken( currentToken );	cout << endl;#endif	switch( currentToken.getType() ) {		case	OpenParen:			match( OpenParen ); 			expr();			match( CloseParen ); 			break;		case	Number:			emit( currentToken ); 			match( Number );			break;		case	Function:			Tuple	f = currentToken;			match( Function );			match( OpenParen );			expr();			match( CloseParen );			emit( f );			break;//		case	UnaryNeg:	// These tokens are never actually generated. They are simply markers//		case	UnaryPlus:	// These tokens are never actually generated. They are simply markers		case	Subtract:	// Subtract and Add at this level are really unary operations.		case	Addition:			Tuple	op = currentToken;			match( currentToken );			expr();			if ( op.getType() == Subtract )	op.setType( UnaryNeg );		// convert the value.			if ( op.getType() == Addition )	op.setType( UnaryPlus );	// convert the value.			emit( op );			break;		default:			error( "syntax error (factor)" );		}}//*******************************************************************//	match					method							[private]voidLR1LA::match( unsigned short t )	{	if ( currentToken.is( t ) )		currentToken = GetToken();	else {		UShrt typ = currentToken.getType();#if DEBUG		cout << "Expected:";		DumpToken( currentToken );#endif				//	 		   1		 2		   3	     4		   5					//	 012345678901234567890123456789012345678901234567890		char msg[100] = "syntax error (match), expected: -- and got: --";		msg[32] = ((char*)&t)[0];		msg[33] = ((char*)&t)[1];		msg[44] = ((char*)&typ)[0];		msg[45] = ((char*)&typ)[1];				error( msg );//		error( "syntax error (match)" );		}}//*******************************************************************//	emit					method							[private]voidLR1LA::emit( Tuple& t )	{#if DEBUG	DumpToken( t );#endif	switch( t.getType() ) {		case 	Undefined:					break;//		case 	DeltaOp:	// fall through intentional		case 	Addition:					checkStack();					tupStack[ stackIndex-1 ].setValue( tupStack[ stackIndex-1 ].getValue() + tupStack[ stackIndex ].getValue() );					tupStack[ stackIndex-1 ].setType( tupStack[ stackIndex ].getType() );					stackIndex--;					break;		case 	UnaryPlus:				//	tupStack[ stackIndex ].setValue( +tupStack[ stackIndex ].getValue() );					break;		case 	Subtract:					checkStack();					tupStack[ stackIndex-1 ].setValue( tupStack[ stackIndex-1 ].getValue() - tupStack[ stackIndex ].getValue() );					tupStack[ stackIndex-1 ].setType( tupStack[ stackIndex ].getType() );					stackIndex--;					break;		case 	UnaryNeg:					checkStack();					tupStack[ stackIndex ].setValue( -tupStack[ stackIndex ].getValue() );					break;		case 	Multiply:					checkStack();					tupStack[ stackIndex-1 ].setValue( tupStack[ stackIndex-1 ].getValue() * tupStack[ stackIndex ].getValue() );					tupStack[ stackIndex-1 ].setType( tupStack[ stackIndex ].getType() );					stackIndex--;					break;		case 	PowerOf:					checkStack();					tupStack[ stackIndex-1 ].setValue( pow(tupStack[ stackIndex-1 ].getValue(), tupStack[ stackIndex ].getValue()) );					stackIndex--;					break;		case 	Divide	:					checkStack();					tupStack[ stackIndex-1 ].setValue( tupStack[ stackIndex-1 ].getValue() / tupStack[ stackIndex ].getValue() );					tupStack[ stackIndex-1 ].setType( tupStack[ stackIndex ].getType() );					stackIndex--;					break;		case 	Function:					// operate based on the function's name.					checkStack();					operateFunction( findFunction( t ) );					break;		case 	Number	:					tupStack[ ++stackIndex ] = t;					break;		case 	AngleDeg:					tupStack[ stackIndex ].setValue( tupStack[ stackIndex ].getValue()/57.2957795131 );					tupStack[ stackIndex ].setType( AngleDeg );					break;		case 	AngleMin:					tupStack[ stackIndex ].setValue( tupStack[ stackIndex ].getValue()/(60*57.2957795131) );					tupStack[ stackIndex ].setType( AngleMin );					if ( stackIndex > 0 && tupStack[ stackIndex-1 ].is( AngleDeg ) ) {						Tuple addition(Addition);						emit( addition );						tupStack[ stackIndex ].setType( AngleDeg );						}					break;		case 	AngleSec:					tupStack[ stackIndex ].setValue( tupStack[ stackIndex ].getValue()/(60*60*57.2957795131) );					tupStack[ stackIndex ].setType( AngleSec );					if ( stackIndex > 0 && tupStack[ stackIndex-1 ].is( AngleDeg ) ) {						Tuple addition(Addition);						emit( addition );						tupStack[ stackIndex ].setType( AngleDeg );						}					break;		case 	Percent:					tupStack[ stackIndex ].setValue( tupStack[ stackIndex ].getValue()/100. );					break;		case 	OpenParen:	// Never happens		case 	CloseParen:	// Never happens but should // flags function execution		case 	Initial	:	// Never happens		case 	Terminal:	// Should never happen.					break;		default:					;		}}//*******************************************************************//	result					method							[private]voidLR1LA::operateFunction( short f )	{	switch( f ) {		// Hyperbolics		case SINH :			tupStack[ stackIndex ].setValue( sinh( tupStack[ stackIndex ].getValue() ) );			break;		case ASNH :			tupStack[ stackIndex ].setValue( asinh( tupStack[ stackIndex ].getValue() ) );			break;		case COSH :			tupStack[ stackIndex ].setValue( cosh( tupStack[ stackIndex ].getValue() ) );			break;		case ACSH :			tupStack[ stackIndex ].setValue( acosh( tupStack[ stackIndex ].getValue() ) );			break;		case TANH :			tupStack[ stackIndex ].setValue( tanh( tupStack[ stackIndex ].getValue() ) );			break;		case ATNH :			tupStack[ stackIndex ].setValue( atanh( tupStack[ stackIndex ].getValue() ) );			break;		// Trig Functions		case SIN  :			tupStack[ stackIndex ].setValue( sin( tupStack[ stackIndex ].getValue() ) );			break;		case ASIN :			tupStack[ stackIndex ].setValue( asin( tupStack[ stackIndex ].getValue() ) );			break;		case COS  :			tupStack[ stackIndex ].setValue( cos( tupStack[ stackIndex ].getValue() ) );			break;		case ACOS :			tupStack[ stackIndex ].setValue( acos( tupStack[ stackIndex ].getValue() ) );			break;		case TAN  :			tupStack[ stackIndex ].setValue( tan( tupStack[ stackIndex ].getValue() ) );			break;		case ATAN :			tupStack[ stackIndex ].setValue( atan( tupStack[ stackIndex ].getValue() ) );			break;		// Exp functions		case EXP  :			tupStack[ stackIndex ].setValue( exp( tupStack[ stackIndex ].getValue() ) );			break;		case SQRT :			tupStack[ stackIndex ].setValue( sqrt( tupStack[ stackIndex ].getValue() ) );			break;		case LN   :			tupStack[ stackIndex ].setValue( log( tupStack[ stackIndex ].getValue() ) );			break;		case LOG:			tupStack[ stackIndex ].setValue( log10( tupStack[ stackIndex ].getValue() ) );			break;		case PI:			tupStack[ stackIndex ].setValue( 3.14159265359*tupStack[ stackIndex ].getValue() );			break;		default:						error( "Unrecognized function." );		}}//*******************************************************************//	checkStack				method							[private]voidLR1LA::checkStack()	{	if ( stackIndex < 0 )		error( "stack underflow" );	if ( stackIndex > kTupStackSize )		error( "stack overflow" );	return;}//*******************************************************************//	error					method							[private]voidLR1LA::error( char* msg ) {	strcpy( errorMsg, msg );#if DONT_USE_THROW//	SetStream( nil );  // finalizes stream.#else	throw	errorMsg;						// standard try...catch#endif}//*******************************************************************//	findFunction				method						[private]//	if I were serious about this I would write a short program to //	pre-compile the standard functions but this will begin the process//	of user defined functions.shortLR1LA::findFunction( Tuple& t ) {	short e = sizeof( gFuncList )/ sizeof( char* );	for( short i=0; i<e; i++ ) {		Tuple funcName( gFuncList[i] );		if ( funcName.getFunc() == t.getFunc() ) {			return i;			}		}//	error( "Unrecogized function." );	return -1;	// invalid function}