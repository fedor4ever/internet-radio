/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  ?Description
*
*/


#include "irctrlcmdobserver.h"
#include "irctrlcommand.h"
#include "irdebug.h"
#include "ircontrolparams.h"

// ======== MEMBER FUNCTIONS ========
// ---------------------------------------------------------------------------
// Function : NewL
// Function returns an instance of CIRCtrlCmdObserver
// Two phase constructor
// ---------------------------------------------------------------------------
//
EXPORT_C CIRCtrlCmdObserver* CIRCtrlCmdObserver::NewL
	(MIRCtrlCmdObserver* aObserver,MIRStreamFill* aStream)
    {
    IRLOG_DEBUG( "CIRCtrlCmdObserver::NewL" );
    CIRCtrlCmdObserver* self = CIRCtrlCmdObserver::NewLC(aObserver,aStream);
    CleanupStack::Pop(self);
    IRLOG_DEBUG( "CIRCtrlCmdObserver::NewL - Exiting." );
    return self;
    }

// ---------------------------------------------------------------------------
// Function : NewLC
// Function returns an instance of CIRCtrlCmdObserver
// Two phase constructor
// ---------------------------------------------------------------------------
//
EXPORT_C CIRCtrlCmdObserver* CIRCtrlCmdObserver::NewLC
	(MIRCtrlCmdObserver* aObserver,MIRStreamFill* aStream)
    {
    IRLOG_DEBUG( "CIRCtrlCmdObserver::NewLC" );
    CIRCtrlCmdObserver* self = new (ELeave) CIRCtrlCmdObserver;
    CleanupStack::PushL(self);
    self->ConstructL(aObserver,aStream);
    IRLOG_DEBUG( "CIRCtrlCmdObserver::NewLC - Exiting." );
    return self;
    }

// ---------------------------------------------------------------------------
// Function : ~CIRCtrlCmdObserver
//  Default destructor calls Cancel function which cancel the active request 
// ---------------------------------------------------------------------------
//
CIRCtrlCmdObserver::~CIRCtrlCmdObserver()
    {
    IRLOG_DEBUG( "CIRCtrlCmdObserver::~CIRCtrlCmdObserver" );
    if ( iVolumeTimer->IsActive() )
        {
        iVolumeTimer->Cancel();            
        }
    delete iVolumeTimer;    
    IRLOG_DEBUG( "CIRCtrlCmdObserver::~CIRCtrlCmdObserver - Exiting." );
    }
    
// ---------------------------------------------------------------------------
// Function : SentRequest
// Function sends request to handle events represented by TIRControlCommmand
// ---------------------------------------------------------------------------
//
EXPORT_C void CIRCtrlCmdObserver::SentRequest( TIRControlCommmand aCommand, TInt aValue )
    {
    IRLOG_DEBUG( "CIRCtrlCmdObserver::SentRequest" );
   if(aCommand==EPlayerChanged)
	    {
	    iObserver->PlayerChanged();	
	    }
    else
	    {
	    iObserver->MCtrlCommand( aCommand, aValue );
	    }
    IRLOG_DEBUG( "CIRCtrlCmdObserver::SentRequest - Exiting." );
    }

// ---------------------------------------------------------------------------
// Function : FilltheBuffer
// Function sends request to fill the buffer
// ---------------------------------------------------------------------------
//
EXPORT_C void CIRCtrlCmdObserver::FilltheBuffer(TDes8& aInputBuffer)
    {
    IRLOG_DEBUG( "CIRCtrlCmdObserver::FilltheBuffer" );
    iStream->FilltheBuffer(aInputBuffer);
    IRLOG_DEBUG( "CIRCtrlCmdObserver::FilltheBuffer - Exiting." );
    }

// ---------------------------------------------------------------------------
// Function : FetchVolume
// Function sends request to get the current volume index
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CIRCtrlCmdObserver::FetchVolume()
    {
    IRLOG_DEBUG( "CIRCtrlCmdObserver::FetchVolume" );
    return iObserver->FetchVolume();
    }

// ---------------------------------------------------------------------------
// Function : AudioFadeInOut
// Function is called for audio Fade In Fade Out Effects
// ---------------------------------------------------------------------------
//
EXPORT_C void CIRCtrlCmdObserver::AudioFadeInOut
	(TIRControlCommmand& aCommand,TInt aValue)
    {
    IRLOG_DEBUG( "CIRCtrlCmdObserver::AudioFadeInOut" );
    iPersistentVolume = aValue;    
    if ( EBufferFadeIn == aCommand )
        {
		//Audio Fade In effect
        if ( iVolumeTimer->IsActive() )
            {
			//cancels the previous request if pending
            iVolumeTimer->Cancel();            
            }
        if ( !iVolumeTimer->IsActive() )
            {
			//starts the fade in effect
            iVolumeLevel = 0;            
            TTimeIntervalMicroSeconds32 interval(KVolumeTime);
            iVolumeTimer->Start(interval,interval,TCallBack(
			CIRCtrlCmdObserver::StartAudioFadeIn,this));
            }
        }
    else if ( EBufferFadeOut == aCommand )
        {
		//Audio Fade Out effect
        if ( iVolumeTimer->IsActive() )
            {
			//cancels the previous request if pending
            iVolumeTimer->Cancel();            
            }
        if ( !iVolumeTimer->IsActive() )
            {
			//starts the fade out effect
            iVolumeLevel = iPersistentVolume;
            TTimeIntervalMicroSeconds32 interval(KDownVolumeTime);
            iVolumeTimer->Start(interval,interval,TCallBack(
			CIRCtrlCmdObserver::StartAudioFadeOut,this));
            }
        }
    else
		{
        IRLOG_DEBUG( "CIRCtrlCmdObserver::AudioFadeInOut - Exiting." );    
        return;    
		}        
    IRLOG_DEBUG( "CIRCtrlCmdObserver::AudioFadeInOut - Exiting." );   
    }

// ---------------------------------------------------------------------------
// Function : DoAudioFadeOut
// Function is used to sent request start audio Fade Out
// ---------------------------------------------------------------------------
//
EXPORT_C void CIRCtrlCmdObserver::DoAudioFadeOut()
    {
    IRLOG_DEBUG( "CIRCtrlCmdObserver::DoAudioFadeOut" );
    //requests to start Audio Fade Out
	iObserver->DoAudioFadeOut();
    }

// ---------------------------------------------------------------------------
// Function : ConstructL
// Function does all the initializations
// Two phase constructor
// ---------------------------------------------------------------------------
//
void CIRCtrlCmdObserver::ConstructL(MIRCtrlCmdObserver* aObserver,
    MIRStreamFill* aStream)
    {
    IRLOG_DEBUG( "CIRCtrlCmdObserver::ConstructL" );
    iObserver = aObserver;
    iStream = aStream;
    iVolumeTimer = CPeriodic::NewL(CPeriodic::EPriorityStandard);
    IRLOG_DEBUG( "CIRCtrlCmdObserver::ConstructL - Exiting." );
    }

// ---------------------------------------------------------------------------
// Function : CIRCtrlCmdObserver
// Function is the default constructor, sets the CActive priority, Line status
// ---------------------------------------------------------------------------
//
CIRCtrlCmdObserver::CIRCtrlCmdObserver()
    {
    //No implementation    
    }    
    
// ---------------------------------------------------------------------------
// Function : StartAudioFadeIn
// Function Starts audio Fade In effects
// ---------------------------------------------------------------------------
//
TInt CIRCtrlCmdObserver::StartAudioFadeIn(TAny* aPtr)     
    {
    IRLOG_DEBUG( "CIRCtrlCmdObserver::StartAudioFadeIn" );
    CIRCtrlCmdObserver* self = static_cast<CIRCtrlCmdObserver*>(aPtr);
    if( self )
		{
		self->UpdateVolumeInc();
		}
    IRLOG_DEBUG( "CIRCtrlCmdObserver::StartAudioFadeIn - Exiting." );
    return KErrNone;        
    }
 
// ---------------------------------------------------------------------------
// Function : StartAudioFadeOut
// Function Starts audio Fade Out effects
// ---------------------------------------------------------------------------
//
TInt CIRCtrlCmdObserver::StartAudioFadeOut(TAny* aPtr)
    {
    IRLOG_DEBUG( "CIRCtrlCmdObserver::StartAudioFadeOut" );
    CIRCtrlCmdObserver* self = static_cast<CIRCtrlCmdObserver*>(aPtr);
    if( self )
		{
		self->UpdateVolumeDec();
		}
    IRLOG_DEBUG( "CIRCtrlCmdObserver::StartAudioFadeOut - Exiting." );
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// Function : UpdateVolumeInc
// Function does the Fade In effect
// ---------------------------------------------------------------------------
//
void CIRCtrlCmdObserver::UpdateVolumeInc()     
    {
    IRLOG_DEBUG( "CIRCtrlCmdObserver::UpdateVolumeInc" );
    //less than zero condition check the value will never be less than zero
	//even when when we start 
    //primary check is volume becoming greater than equal to current volume 
	//level stored in iPersistentVolume
    if ( iVolumeTimer->IsActive()
		&& ( ( 0 <= iVolumeLevel ) 
		&& ( iPersistentVolume >= iVolumeLevel ) ) )
        {
		//if the time is volume level is not equal to current volume stored in
		// iPersistentVolume
		//it will initiates fade in effect
        iCommand = EBufferFadeIn;
        iObserver->MCtrlCommand(iCommand,iVolumeLevel);
        iVolumeLevel++;
        }
    else
        {
        if ( iVolumeTimer->IsActive() )
            {
            iVolumeTimer->Cancel();            
            }
        }
    IRLOG_DEBUG( "CIRCtrlCmdObserver::UpdateVolumeInc - Exiting." );
    }

// ---------------------------------------------------------------------------
// Function : UpdateVolumeDec
// Function does the Fade In effect 
// ---------------------------------------------------------------------------
//
void  CIRCtrlCmdObserver::UpdateVolumeDec()     
    {
	IRLOG_DEBUG( "CIRCtrlCmdObserver::UpdateVolumeDec" );
	//primary check is volume becoming lesser than equal to zero level 
	if ( iVolumeTimer->IsActive()
		&& ( ( 0 <= iVolumeLevel ) 
		&& ( iPersistentVolume >= iVolumeLevel ) ) )
		{
		//if the time is volume level is not equal to current zero
		//it will initiates fade out effect
		iCommand = EBufferFadeOut;
		iObserver->MCtrlCommand(iCommand,iVolumeLevel);
		iVolumeLevel--;
		}
	else
		{
		if ( iVolumeTimer->IsActive() )
			{
			iVolumeTimer->Cancel();
			}
		}
    IRLOG_DEBUG( "CIRCtrlCmdObserver::UpdateVolumeDec - Exiting." );    
    }


