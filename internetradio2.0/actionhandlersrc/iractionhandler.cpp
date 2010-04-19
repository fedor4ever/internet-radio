/*
* Copyright (c) 2008-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  
*
*/


#include <e32cmn.h>
#include <e32property.h>
#include <e32std.h>
#include <w32std.h>
#include <implementationproxy.h>
#include <liwvariant.h>
#include "irdebug.h"
#include <apgtask.h>
#include <apgcli.h>
#include <apacmdln.h>


#include "iractionhandler.h"
#include "iractionhandler.hrh"
#include "iractionhandlerdefs.h"
#include "irpubsubkeys.h"

_LIT8( KIRSpace, " " );
_LIT8( KIRCommandLineActionHandler, "-a" );
_LIT8( KIRCommandLineActionHandlerCommandStartNowPlayingView, "startnowplaying" );

// ---------------------------------------------------------
// CIRActionHandler::NewL
// ---------------------------------------------------------
//
CIRActionHandler* CIRActionHandler::NewL()
    {
    CIRActionHandler* self = new ( ELeave ) CIRActionHandler(); 
    CleanupStack::PushL( self );
    
    self->ConstructL();
    
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------
// CIRActionHandler::~CIRActionHandler
// ---------------------------------------------------------
//
CIRActionHandler::~CIRActionHandler()
    {
    }

// ---------------------------------------------------------
// CIRActionHandler::CIRActionHandler
// ---------------------------------------------------------
//
CIRActionHandler:: CIRActionHandler()
    {
    
    }
    
// ---------------------------------------------------------
// CIRActionHandler::ConstructL
// ---------------------------------------------------------
//
void CIRActionHandler::ConstructL()
    {
    }

// ---------------------------------------------------------
// CIRActionHandler::ExecuteActionL
// ---------------------------------------------------------
//
TInt CIRActionHandler::ExecuteActionL( const CLiwMap* aMap )
    {
    IRRDEBUG2("InternetRadio -- CIRActionHandler::ExecuteActionL Entering", KNullDesC);

    TInt retValue( KErrNone );
    RBuf command;
    CleanupClosePushL( command );
    
	retValue  = ExtractStringL( aMap, command, KIRCommand);    
    
    if  (retValue== KErrNone )
        {
        if ( !command.CompareF( KIRCommandValueStop) )
            {
            Stop();
            }
        else if ( !command.CompareF( KIRCommandValuePlay ) )
            {
            Play();
            }

        else if ( !command.CompareF( KIRCommandValueStartNowPlaying ) )
            {
            StartToNowPlayingL();
            }
        else
            {
            retValue = KErrNotSupported;
            }
        }
    CleanupStack::PopAndDestroy( &command );
    IRRDEBUG2("InternetRadio -- CIRActionHandler::ExecuteActionL exiting", KNullDesC);
    return retValue;
    }



// ---------------------------------------------------------
// CIRActionHandler::Mute
// ---------------------------------------------------------
//
void CIRActionHandler::Stop()
    {
    IRRDEBUG2("CIRActionHandler::Stop entering", KNullDesC);
      
   	TInt err = RProperty::Set(KUidActiveInternetRadioApp, KIRPSControlSetPlayState, EIRStateStop );
   	
    IRRDEBUG2("CIRActionHandler::Stop exiting err =%d", err);
	
    }

// ---------------------------------------------------------
// CIRActionHandler::Unmute
// ---------------------------------------------------------
//
void CIRActionHandler::Play()
    {
    IRRDEBUG2("CIRActionHandler::Play entering", KNullDesC);

    TInt err = RProperty::Set(KUidActiveInternetRadioApp, KIRPSControlSetPlayState, EIRStatePlay);	
  	
    IRRDEBUG2("CIRActionHandler::Play exiting err=%d", err);

    }



// ---------------------------------------------------------
// CIRActionHandler::StartToNowPlayingL
// ---------------------------------------------------------
//
void CIRActionHandler::StartToNowPlayingL()
    {
    
    IRRDEBUG2("CIRActionHandler::StartToNowPlayingL entering", KNullDesC);

    RWsSession wsSession;
                
    User::LeaveIfError(wsSession.Connect() );
    TApaTaskList taskList( wsSession );

    TApaTask irTask = taskList.FindApp(KUidActiveInternetRadioApp);
   
   
    IRRDEBUG2("CIRActionHandler::StartToNowPlayingL irTask.Exists()=%d", irTask.Exists());

    TPtrC8 actionHandlerCmdId( KIRCommandLineActionHandler );
    TPtrC8 spacePtr( KIRSpace );
    
    TPtrC8 startNowPlayingCmdvalue(
    KIRCommandLineActionHandlerCommandStartNowPlayingView );

    HBufC8* buf = HBufC8::NewLC(actionHandlerCmdId.Length()
    			 + spacePtr.Length() + startNowPlayingCmdvalue.Length());
    TPtr8 tail( buf->Des() );
       
    tail.Append( actionHandlerCmdId );
    tail.Append( spacePtr );
    tail.Append( startNowPlayingCmdvalue );

    if ( !irTask.Exists() )
        {
        IRRDEBUG2("CIRActionHandler::StartToNowPlayingL !irTask.Exists", KNullDesC);

        RApaLsSession apaSession;
        CleanupClosePushL( apaSession );
           
        User::LeaveIfError( apaSession.Connect() );
        User::LeaveIfError( apaSession.GetAllApps() );
       
        TApaAppInfo appInfo;
        User::LeaveIfError( apaSession.GetAppInfo( appInfo, KUidActiveInternetRadioApp ) );

      
           
        CApaCommandLine* internetRadioApplicationStartParams = CApaCommandLine::NewLC();
        internetRadioApplicationStartParams->SetExecutableNameL( appInfo.iFullName );
        // Start the application to the foreground
        internetRadioApplicationStartParams->SetCommandL( EApaCommandRun );
        internetRadioApplicationStartParams->SetTailEndL( tail );
       
        User::LeaveIfError( apaSession.StartApp( *internetRadioApplicationStartParams ) );
    
        IRRDEBUG2("CIRActionHandler::StartToNowPlayingL StartApp", KNullDesC);

        CleanupStack::PopAndDestroy( internetRadioApplicationStartParams );
        CleanupStack::PopAndDestroy( &apaSession );
        }
    else
        {
        IRRDEBUG2("CIRActionHandler::StartToNowPlayingL SendMessage", KNullDesC);

        User::LeaveIfError( irTask.SendMessage( KUidActiveInternetRadioApp , tail ) );

        }
    CleanupStack::PopAndDestroy( buf );
    wsSession.Close();

    IRRDEBUG2("InternetRadio -- CIRActionHandler::StartToNowPlayingL exiting", KNullDesC);

    }

// ---------------------------------------------------------
// CIRActionHandler::ExtractStringLC
// ---------------------------------------------------------
//
TInt CIRActionHandler::ExtractStringL( const CLiwMap* aMap, RBuf& aString,
										 const TDesC8& aMapName)
    {
    IRRDEBUG2("InternetRadio -- CIRActionHandler::ExtractStringL entering", KNullDesC);

    TInt err( KErrNotFound );
    TLiwVariant variant;
    variant.PushL();
    TPtrC tempString( KNullDesC );    
    if ( aMap->FindL( aMapName, variant ) )
        {
        variant.Get( tempString );
        aString.Close();
        aString.CreateL( tempString );
        err = KErrNone;
        }
    CleanupStack::PopAndDestroy( &variant );    
    IRRDEBUG2("InternetRadio -- CIRActionHandler::ExtractStringL exiting", KNullDesC);
   
    return err;

    }

/** Provides a key-value pair table, that is used to identify the correct construction function for the requested interface. */
const TImplementationProxy KIRActionHandlerImplementationTable[] =
    {
    /*lint -save -e611 (Warning -- Suspicious cast)*/
    IMPLEMENTATION_PROXY_ENTRY( KIRActionHandlerImplementationUid, CIRActionHandler::NewL )
    /*lint -restore*/
    };

const TImplementationProxy* ImplementationGroupProxy( TInt& aTableCount );

// ---------------------------------------------------------------------------
// Returns the implementations provided by this ECOM plugin DLL.
// ---------------------------------------------------------------------------
//
EXPORT_C const TImplementationProxy* ImplementationGroupProxy( TInt& aTableCount )
    {
    IRRDEBUG2("InternetRadio -- CIRActionHandler::ImplementationGroupProxy", KNullDesC);

    aTableCount = sizeof( KIRActionHandlerImplementationTable ) / sizeof( TImplementationProxy );
    return KIRActionHandlerImplementationTable;
    }
