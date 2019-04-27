//	© Paul B. Beeken, Work In Progress, 1994-5//	Knowledge Software Consulting.////	These files are a mindlessly simple wrapper class for the//	basic XCMD operations.  Only one instance of the xcmdBase class is//	generated per XCMD call but there may be many instances of the XCMDString//	class.  I have used these classes to whip out XCMD/XFCNs within hours of//	receiving the specs.  They work great for me but I will always consider //	suggestions.////	Please, please, please, in the unlikely event you should use this stuff//	for some commercial application I would appreciate you contacting me.  If//	its for your own use, use away. Send email: knowsoft@ios.com////	As always: this file is presented as is with no warrantees expressed or implied.//	Swim at your own risk, etc. etc.////	11/11/95	I've made some simple modifications to this file as well as xcmdString//		my understanding of c++ has matured and I have made some minor modifications//		to this implementation to take advantage of a better implementation of xcmdStrings#include	<ctype.h>#include	"xcmdBase.h"#include	<Memory.h>#include	<String.h>// Original notes:////	This file defines all the methods in the base class.//	Parameters are passed to client functions as zero terminated strings. All the//	server calls involving strings expect pascal like string arrays. What to do...////	I have just figured out why the parameters passed to this routine are null//	term strings: The parameters can be arb. long. The input strings, for example,//	can be the entire contents of a field container. Pascal strings are limited//	to 255 bytes. So...////	Any server call made by the client, involving a string will expect a "c" like//	zero terminated string. The member function converts it to a pascal string, places//	the call, and converts it back on return. This may seem inefficient but "c" strings//	are more natural for this language and the alg. for conversion is pretty fast. //	Besides, most of the time we are not tied up in client requests anyway.////	Here's the problem. For some bizare reason the developers of Hypercard//	pass zero terminated "C" like strings as parameters in the XCmdPtr.//	This would be OK by	itself until you realize that most of the callback//	routines want to see pascal like strings. We have created a special class//	to handle all the string operations.////	Changes://	¥ xcmdString no longer uses c2pstr and p2cstr.  I devised a simple way to convert//	from one to another without any calculational overhead (can you guess, no peeking!)//	¥ I still don't understand why the call backs needed pascal strings but this is not//	a problem anymore.#pragma mark	¥ Creation and DestructionÉ//	Creation routine. Registers the XCmdPtr and locks all the parameters high.//	____________________________________xcmdBase::xcmdBase( XCmdPtr xP, Boolean lockParameters ) : 	paramsLocked( lockParameters ){	paramPtr = xP;				//	our copy of the paramPtr	xcmdString::paramPtr = xP;	// common ptr for xcmdString objects.		//if ( xP )	exitNow = false;  if xp is nil we probably wouldn't be here.		// I don't automaticlly lock the handles to the parameters.		// if they are small strings then it may needlessly tie up high memory		// for other things.	if ( lockParameters )	lockParams();}//	Destruction routine. unlocks all the parameters.//	____________________________________xcmdBase::~xcmdBase( void ){	if ( paramsLocked ) unlockParams();}#pragma mark	-#pragma mark	¥ Parameter managementÉ//	Unlocks all the parameters.//	____________________________________voidxcmdBase::unlockParams( void ){	if ( paramsLocked ) {		short i;		for(i=0; i < paramPtr->paramCount; i++) {			HUnlock(paramPtr->params[i]);			}		paramsLocked = false;		}}//	Locks all the parameters.//	____________________________________voidxcmdBase::lockParams( void ){	if ( !paramsLocked ) {		short i;		for(i=0; i < paramPtr->paramCount; i++) {			MoveHHi(paramPtr->params[i]);			HLock(paramPtr->params[i]);			}		paramsLocked = true;		}}//	Check the parameter count to see if it is expected.//	____________________________________BooleanxcmdBase::checkParameters( int min, int max ){	if ( paramPtr==nil )	return false;	// no paramPtr so exit	if (max<min) max=min;	if ( nParams() < min ) {		errorMsg( "too few parameters." );		return false;		}	if ( nParams() > max ) {		errorMsg( "too many parameters." );		return false;		}	return true;}//	Array overload. returns the parameter at position i.//	____________________________________HandlexcmdBase::operator[]( const int i ){	if ( i>=0 && i<=16 ) return paramPtr->params[i];	else return nil;}shortxcmdBase::scanParams( xcmdString str, short from ){		// scan all the parameters for key.	int i;	xcmdString	p;	for( i=from; i<nParams(); i++ ) {		p = xcmdString( paramPtr->params[i] );		if ( p.contains( str ) ) return i;				}		return INVALID_INDEX;  // index > 15 is invalid}#pragma mark	-#pragma mark	¥ Return code managementÉ//	Return methods. alerts user of error.//	____________________________________voidxcmdBase::errorMsg( xcmdString msg ){	returnMsg( "¥Error: " & msg );}//	Return methods. returns string.//	____________________________________voidxcmdBase::returnMsg( xcmdString message ){	paramPtr->returnValue = Handle(message);}#pragma mark	-#pragma	mark	¥ UtilitiesÉ/****  HyperTalk Utilities  ****/shortxcmdBase::evalExpr( xcmdString expr, xcmdString* r ){	Handle h = EvalExpr( paramPtr, expr );	xcmdString	rc(h);	DisposHandle(h);	if ( r )	// we might not care about a result		*r = rc;	return	paramPtr->result;}shortxcmdBase::sendCardMessage( xcmdString msg ){	// message goes to current card.	SendCardMessage( paramPtr, msg ); 	return	paramPtr->result;}shortxcmdBase::sendHCMessage( xcmdString msg ){	// message goes to hypercard itself.	SendHCMessage( paramPtr, msg );	return	paramPtr->result;}voidxcmdBase::runScript( xcmdString handler ){	RunHandler( paramPtr, handler ); }/****  Memory Utilities  ****/voidxcmdBase::GetGlobal( xcmdString globName, xcmdString* globValue ){	Handle		h = ::GetGlobal( paramPtr, globName );	xcmdString	gv( h );	DisposHandle( h );	*globValue = gv;}voidxcmdBase::SetGlobal( xcmdString globName, xcmdString& globValue ){	Handle gVal = Handle(globValue); // create a handle of the value	::SetGlobal( paramPtr, globName, gVal );	DisposHandle( gVal );			// dispose of it.}voidxcmdBase::zeroBytes( Ptr dstPtr, long longCount ){	ZeroBytes( paramPtr, dstPtr, longCount ); }