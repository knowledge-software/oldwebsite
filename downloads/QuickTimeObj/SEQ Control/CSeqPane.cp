//	© Paul B. Beeken, Work In Progress, 1994-5//	Knowledge Software Consulting.////	Please, please, please, in the unlikely event you should use this stuff//	for some commercial application I would appreciate you contacting me.  If//	its for your own use, use away. Send email: knowsoft@idt.net//	My personal philosophy closely adheres to that of GNU software.  I offer this//	in the hope that others will improve and expand upon it.  Post your additions,//	leave this notice in place and add your own comments.////	As always: this file is presented as is with no warrantees expressed or implied.//	Swim at your own risk, etc. etc.////	CSeqPane.cp////	A c++ extention of a powerplant™ (CodeWarrior) pane for the Quicktime component library //	associated with sequencers Instantiating a pane of this class will automatically open//	the default sequencerfor preview. This pane will also captutre a pict.  You can modify the//	code to suit your own purposes.//////	Current version 1.0		© Paul B. Beeken, Knowledge Software Consulting.#include "CSeqPane.h"#include	<limits.h>#include	<LStream.h>// ---------------------------------------------------------------------------//		• Constant declarations// ---------------------------------------------------------------------------#pragma mark • Constructors/Destructors// ---------------------------------------------------------------------------//		• CreateGraphPane// ---------------------------------------------------------------------------//	Static constructor used in URegistrarCSeqrPane*CSeqrPane::CreateSeqrPaneStream( LStream* inStream ){	return	( new CSeqrPane(inStream) );}CSeqrPane::CSeqrPane( LStream* inStream ) : LPane( inStream ){	makeSeqPane( 1 );}CSeqrPane::CSeqrPane( void ){	makeSeqPane( 1 );}CSeqrPane::~CSeqrPane( void ){	StopRepeating();	if ( seqGrabber != nil ) {		SGStop( seqGrabber );				CloseComponent( seqGrabber );		}}#pragma mark -#pragma mark • LPeriodical overridevoidCSeqrPane::SpendTime(const EventRecord &inMacEvent){	if ( IsVisible() ) {//		Rect	paneFrame;//		CalcPortFrameRect( paneFrame );					SGIdle( seqGrabber ); // captures to the offscreen world		// This will only get called if we call idleTime with nonzero number.		}}#pragma mark -#pragma mark • LPane overridesvoidCSeqrPane::DrawSelf(){	Rect	paneFrame;	CalcPortFrameRect( paneFrame );		// Set the videoFrame to an appropriate size.	try {		OSErr rc;			// do some drawing.		Rect	theRect;		CalcPortFrameRect( theRect );		// Turn off		Pause();		rc = SGSetChannelBounds( videoChannel, &theRect );		if( rc != noErr ) throw rc;		::InsetRect( &theRect, 20, 20 );		::FrameRect( &theRect );		Continue();		}	catch( OSErr rc ) {		StopRepeating();		}	// I don't actually draw anything here since it is done at 	//	Repeat time.	}voidCSeqrPane::ClickSelf(const SMouseDownEvent &inMouseDown){}////	Fire up the sequencer.voidCSeqrPane::FinishCreateSelf(){	Rect	destRect;	CalcPortFrameRect( destRect );	SGSetChannelBounds( videoChannel, &destRect );	SGSetChannelUsage( videoChannel, seqGrabPreview );	updateValues();		ComponentResult			rc		= noErr;	// Assign the sequencer the window	SGSetGWorld( seqGrabber, (CGrafPtr)GetMacPort(), nil);	rc = SGPrepare( seqGrabber, true, false);	rc = SGStartPreview( seqGrabber );	StartRepeating();}#pragma mark -#pragma mark • Public MethodsvoidCSeqrPane::Pause( void ){	SGPause( seqGrabber, true );}voidCSeqrPane::Continue( void ){	SGPause( seqGrabber, false );}PicHandleCSeqrPane::Snap( FSSpec fspec ){	PicHandle pic;	Rect	destRect;		CalcPortFrameRect( destRect );	OSErr	rc = SGGrabPict( seqGrabber, &pic, &destRect, 0, grabPictOffScreen);	if ( rc == noErr ) return pic;	//	if ( rc == noErr ) {//		HLock( (Handle) pic );//		if ( false ) {	// Do to clipboard.//			ZeroScrap();//			PutScrap( GetHandleSize( (Handle)pic ), 'PICT', *pic );//			}//		else {								// Do to file.//			OSErr rc = FSpCreate(&fspec,'ttxt','PICT', smSystemScript );//			if ( rc ) throw rc;//			short	refNum;//			rc = FSpOpenDF( &fspec, fsRdWrPerm, &refNum ); //<//			if ( rc ) throw rc;//			long data = 0L, count = 4;//			for( int i=0; i<128; i++ )//				FSWrite ( refNum, &count, &data );//			//			count = GetHandleSize( (Handle)pic );//			rc = FSWrite ( refNum, &count, *pic );//			if ( rc ) throw rc;//			rc = FSClose( refNum );		//			}//		HUnlock( (Handle)pic );		KillPicture( pic );//		}}voidCSeqrPane::PopDialog( void ){//	Component	pl[2] = { SeqGrabPanelType, SeqGrabCompressionPanelType };//	short		ps = sizeof pl;	StopRepeating();	SGSettingsDialog( seqGrabber, videoChannel, 0, nil, 0, NewSGModalFilterProc( modalFilter ), (long)seqGrabber );	StartRepeating();	updateValues();}void	CSeqrPane::Brightness( long b ){	vdigBrightness = b;	VDSetBrightness( vdig, &vdigBrightness );	SGVideoDigitizerChanged( videoChannel );}long	CSeqrPane::Brightness( void ){	updateValues();	return vdigBrightness;}void	CSeqrPane::Contrast( long c ){	vdigContrast = c;	VDSetBrightness( vdig, &vdigContrast );	SGVideoDigitizerChanged( videoChannel );}long	CSeqrPane::Contrast( void ){	updateValues();	return vdigContrast;}void	CSeqrPane::Hue( long h ){	vdigHue = h;	VDSetBrightness( vdig, &vdigHue );	SGVideoDigitizerChanged( videoChannel );}long	CSeqrPane::Hue( void ){	updateValues();	return vdigHue;}void	CSeqrPane::Saturation( long s ){	vdigSaturation = s;	VDSetBrightness( vdig, &vdigSaturation );	SGVideoDigitizerChanged( videoChannel );}longCSeqrPane::Saturation( void ){	updateValues();	return vdigSaturation;}void	CSeqrPane::Sharpness( long s ){	vdigSharpness = s;	VDSetBrightness( vdig, &vdigSharpness );	SGVideoDigitizerChanged( videoChannel );}long	CSeqrPane::Sharpness( void ){	updateValues();	return vdigSharpness;}#pragma mark -#pragma mark • Protected MethodsvoidCSeqrPane::makeSeqPane( short port ){			ComponentResult			result		= noErr;	// Here is where we initialize sequencer...	try	 {			// Instantiate a component			ComponentDescription	theDesc;		theDesc.componentType			= SeqGrabComponentType;		theDesc.componentSubType		= nil;		theDesc.componentManufacturer	= 'appl';		theDesc.componentFlags			= nil;		theDesc.componentFlagsMask		= nil;			Component				sgCompID	= nil;					// Find a component		sgCompID = FindNextComponent(nil, &theDesc);		if ( sgCompID == nil ) throw cantFindHandler;					// Open the component		seqGrabber = OpenComponent(sgCompID);		if ( seqGrabber == nil ) throw cantOpenHandler;					// Initialize it.		result = SGInitialize( seqGrabber );		if ( result!=noErr ) throw result;				// Assign an output channel.		result = SGNewChannel( seqGrabber, VideoMediaType, &videoChannel );		if ( result!=noErr ) throw result;		if ( videoChannel==nil ) throw result;			// Set the port		vdig = SGGetVideoDigitizerComponent( videoChannel );		VDSetInput( vdig, port );		SGVideoDigitizerChanged( videoChannel );				// Initializes the sources rectangle.					result = SGGetSrcVideoBounds( videoChannel, &videoBounds );		// open the windows		}	catch ( OSErr rc ) {		if ( seqGrabber )	CloseComponent(seqGrabber);		seqGrabber = nil;		vdig = nil;		throw  rc;		}}voidCSeqrPane::updateValues( void ){	VDGetVideoDefaults( vdig, &vdigBlackLevel, &vdigWhiteLevel, &vdigBrightness, &vdigHue, 							  &vdigSaturation, &vdigContrast, &vdigSharpness );}pascal Boolean CSeqrPane::modalFilter(DialogPtr theDialog, EventRecord *theEvent, short *itemHit, long refCon){//	SeqGrabComponent		sg = (SeqGrabComponent)refCon;		// Instance of a grabber////	if ( theEvent->what == nullEvent )//		if (sg ) SGIdle( sg ); // captures to the offscreen world	return false;}