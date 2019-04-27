#include <UException.h>//	© Paul B. Beeken, Work In Progress, 1994-5//	Knowledge Software Consulting.////	These files are a mindlessly simple wrapper class for the//	basic XCMD operations.  Only one instance of the xcmdBase class is//	generated per XCMD call but there may be many instances of the XCMDString//	class.  I have used these classes to whip out XCMD/XFCNs within hours of//	receiving the specs.  They work great for me but I will always consider //	suggestions.////	Please, please, please, in the unlikely event you should use this stuff//	for some commercial application I would appreciate you contacting me.  If//	its for your own use, use away. Send email: knowsoft@ios.com////	As always: this file is presented as is with no warrantees expressed or implied.//	Swim at your own risk, etc. etc.////	A serious improvement over the previous version.//	11/11/95	because of some problems with stricter type checking under//               CW 7 I have changed the scheme for string storage.  I //               allocate a single storage pointer which will hold the pascal //               length byte and the null terminator.  If I need a c string I //               simply pass a pointer to the beginning of the c like //               portion.  A pascal string request gets a pointer to the very //               beginning.  Designing this way required more attention to //               details over the previous brain-dead method.//#include	<strings.h>#include	"xcmdStrings.h"const	int MAX_PSIZE	= 255;#define NoCLIB 1#ifdef NoCLIB//	A local function to avoid need for clib;intstrlen( char* a );intstrlen( char* a ){	int i=0;	for( ; *a; a++ ) i++;	return i;}voidstrcpy( char* a, char* b );voidstrcpy( char* a, char* b ){	for( ; *b; b++,a++ ) *a = *b;	*a = *b;}voidstrncpy( char* a, char* b, int i );voidstrncpy( char* a, char* b, int i ){	for( ; i; b++,a++,i-- ) *a = *b;	// *a = '\0';	}char*strcat( char* a, char* b );char*strcat( char* a, char* b ){	char*	c = a;	while( *a++ ) {}	--a;	// we've overstepped, backup one.	for( ; *b; b++,a++ ) *a = *b;	*a = *b;	return c;}#else#include	<string.h>#endif// allocated when xcmdBase object is instantiatedXCmdPtr	xcmdString::paramPtr	= nil;// This is the real length of the string.  Sometimes it doesn't//	correspond to the pascal length (limited to 255 characters)intxcmdString::length( void ) const{	return len;		// The string length.}//	Wipe pointer, return storage.xcmdString::~xcmdString(){	if ( str != nil )	delete [] str;}xcmdString::xcmdString() : len(0){	str = nil;}#pragma	mark -#pragma mark • Creation Operators…// from null length handle.xcmdString::xcmdString( Handle s, Boolean useHandLen ){	if ( useHandLen )	// handle size determines length		len = ::GetHandleSize( s );	else				// nil termination determines length		len = strlen(*s);		str = new char[len+2];		ThrowIfNil_( str );	// Handle does not move so I can get away with this.	strncpy( cStr(), *s, len+1 );	str[0] = len>MAX_PSIZE ? MAX_PSIZE : len;}// copy constructorxcmdString::xcmdString( xcmdString& s ){ 	len = s.length();	str = new char[len+2];		ThrowIfNil_( str );	// copy strlen pointer and all.	strncpy( str, (char*)s.pStr(), len+2 );}// from c stringxcmdString::xcmdString( char* s ){ 	len = strlen(s);	str = new char[len+2];		ThrowIfNil_( str );	strcpy( cStr(), s );	str[0] = len>MAX_PSIZE ? MAX_PSIZE : len;	str[len+1]	= '\0';	// null terminate}// from pointerxcmdString::xcmdString( char* s, long n ){ 	len = n;	str = new char[len+2];		ThrowIfNil_( str );	strncpy( cStr(), s, n );	str[0] = len>MAX_PSIZE ? MAX_PSIZE : len;	str[len+1]	= '\0';	// null terminate}// from pascal stringxcmdString::xcmdString( StringPtr s ){ 	len = s[0];	str = new char[len+2];		ThrowIfNil_( str );	strncpy( str, (char*)s, len+1 );	str[len+1]	= '\0';	// null terminate}/****  String Conversion Constructors  ****/xcmdString::xcmdString( Boolean bool ){	str = new char[8];		ThrowIfNil_( str );		BoolToStr( paramPtr, bool, StringPtr(str) );	len = str[0];	str[len+1]	= '\0';	// null terminate}xcmdString::xcmdString( extended num ){	str = new char[12];		ThrowIfNil_( str );	ExtToStr( paramPtr, num, StringPtr(str) ); 	len = str[0];	str[len+1]	= '\0';	// null terminate}xcmdString::xcmdString( unsigned long posNum ){	str = new char[12];		ThrowIfNil_( str );	LongToStr( paramPtr, long(posNum), StringPtr(str) ); 	len = str[0];	str[len+1]	= '\0';	// null terminate}xcmdString::xcmdString( long num, short nd ){	str = new char[nd+4];		ThrowIfNil_( str );	NumToHex( paramPtr, num, nd, StringPtr(str) );	len = str[0];	str[len+1]	= '\0';	// null terminate}xcmdString::xcmdString( long num ){	str = new char[16];		ThrowIfNil_( str );	NumToStr( paramPtr, num, StringPtr(str) ); 	len = str[0];	str[len+1]	= '\0';	// null terminate}xcmdString::xcmdString( Point pt ){	str = new char[15];		ThrowIfNil_( str );	PointToStr( paramPtr, pt, StringPtr(str) );	len = str[0];	str[len+1]	= '\0';	// null terminate}xcmdString::xcmdString( Rect& rct ){	str = new char[30];		ThrowIfNil_( str );	RectToStr( paramPtr, &rct, StringPtr(str) ); 	len = str[0];	str[len+1]	= '\0';	// null terminate}#pragma	mark -#pragma	mark • Operators…// conversion operators for useful types:xcmdString::operator Boolean() const{	Boolean rc = StrToBool( paramPtr, StringPtr(str) );	return rc;}xcmdString::operator Rect() const{		Rect	rct;	StrToRect( paramPtr, StringPtr(str), &rct );	return rct;}xcmdString::operator Point() const{		Point	pt;	StrToPoint( paramPtr, pStr(), &pt );	return pt;}xcmdString::operator extended() const{	extended e = StrToExt( paramPtr, pStr() ); 	return e;}xcmdString::operator unsigned long() const{	unsigned long d = StrToLong( paramPtr, pStr() ); 	return d;}xcmdString::operator long() const{	unsigned long d = StrToNum( paramPtr, pStr() ); 	return d;}xcmdString::operator StringPtr() const{	return pStr();}xcmdString::operator char*() const{	return cStr();}// I create a handle, copy data, hand it off. You must manage it!xcmdString::operator Handle() const{	Handle h;		h = NewHandleClear (length() + 1);	if ( h )		strcpy( (char *) *h, cStr() );		return h;}#pragma	mark -#pragma mark • Testing operations…// Some key testing operators. == != contains//	____________________________________long			xcmdString::contains( const xcmdString& s2 ) const{	// this is a case insensitive search, if you wand\t case sensitivity, 	// override.	char*	op	=	StringMatch( paramPtr, s2, cStr() );	if ( op == nil ) return 0;	return op-str;}Booleanoperator!=( const xcmdString& s1, const xcmdString& s2 ){	Boolean	rc	=	StringEqual( xcmdString::paramPtr, s1.pStr(), s2.pStr() );	return !rc;}// Equate.  Is a friend so that "" clauses are handled automaticlly//	____________________________________Booleanoperator==( const xcmdString& s1, const xcmdString& s2 ){	Boolean	rc	=	StringEqual( xcmdString::paramPtr, s1.pStr(), s2.pStr() );	return rc;}// overloading of assignment.//	____________________________________xcmdString&xcmdString::operator=( const xcmdString& s2 ){	if ( this != &s2 ) {		if ( str != nil )	delete [] str;		len =  s2.length();		str = new char[len+2];			ThrowIfNil_( str );		// copy strlen pointer and all.		strncpy( str, (char*)s2.pStr(), len+2 );		}	return *this;}// self concatenation.//	____________________________________xcmdString&xcmdString::operator&=( const xcmdString& s2 ){	char*	temp	=	str;	len +=  s2.length();	str = new char[len+2];		ThrowIfNil_( str );	if ( temp != nil ) {		strcpy( cStr(), &temp[1] );		delete [] temp;		}		strcat( cStr(), s2.cStr() );		str[0] = len>MAX_PSIZE ? MAX_PSIZE : len;	return *this;}// overloading of index to return specific element.//	____________________________________charxcmdString::operator[]( const int i ){	if ( i>length() ) return '\0';	return cStr()[i];}// Catenation.  Is a friend so that "" are handled automaticlly//	____________________________________xcmdStringoperator&( const xcmdString& s1, const xcmdString& s2 ){	char*	s	=	new	char[ s1.length() + s2.length() + 2 ];		ThrowIfNil_( s );	strcpy( s, s1.cStr() );	xcmdString	rs( strcat( s, s2.cStr() ) );	delete [] s;	return rs;}#pragma	mark -#pragma mark • Conditioning operations…// This function will trim leading and trailing whitespace characters//	It does not alter the size of the allocated memoryvoidxcmdString::trimWhiteSpace( void ){	char*	sp	=	cStr();		while( *sp == ' ' || *sp == '\t' || *sp == '\r' ) sp++;	::BlockMove( sp, cStr(), length() );		len = strlen(cStr());	while( str[len] == ' ' || str[len] == '\t' || str[len] == '\r' ) len--;	str[0] = len>MAX_PSIZE ? MAX_PSIZE : len;	str[len+1]	= '\0';	// null terminate}// Just as it sez. Uppercase the string.//voidxcmdString::upperCase( void ){	short	tc;	for( int i=1; i<=len; i++ ) {		tc = str[i] - 'a';		if ( tc >= 0 && tc <= 26 )			str[i] -= 32;		}}// Just as it sez. LowerCase the string.//voidxcmdString::lowerCase( void ){	short	tc;	for( int i=1; i<=len; i++ ) {		tc = str[i] - 'A';		if ( tc >= 0 && tc <= 26 )			str[i] += 32;		}}