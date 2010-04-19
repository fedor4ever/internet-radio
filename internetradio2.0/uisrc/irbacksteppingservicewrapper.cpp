/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Wrapper for Back Stepping Service.
*
*/


#include <liwcommon.h>
#include <liwservicehandler.h>
#include <mmf/common/mmfcontrollerpluginresolver.h>

#include "irbsserviceconstants.h" 
#include "irbacksteppingservicewrapper.h"

_LIT8( KIRBackSteppingPrefix, "IRBS" );

// ======== MEMBER FUNCTIONS =================================================

// ---------------------------------------------------------------------------
// CIRBackSteppingServiceWrapper::NewL
// ---------------------------------------------------------------------------
//
CIRBackSteppingServiceWrapper* CIRBackSteppingServiceWrapper::NewL( TUid aUid )
    {
    CIRBackSteppingServiceWrapper* self = new( ELeave ) CIRBackSteppingServiceWrapper;
    CleanupStack::PushL( self );
    self->ConstructL( aUid );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CIRBackSteppingServiceWrapper::~CIRBackSteppingServiceWrapper
// ---------------------------------------------------------------------------
//
CIRBackSteppingServiceWrapper::~CIRBackSteppingServiceWrapper()
    {
    if ( iBsInterface )
        {
        iBsInterface->Close();
        }
    if ( iServiceHandler )
        {
        delete iServiceHandler;
        }
    }

// ---------------------------------------------------------------------------
// CIRBackSteppingServiceWrapper::HandleViewActivationEventL
// ---------------------------------------------------------------------------
//
void CIRBackSteppingServiceWrapper::HandleViewActivationEventL( const TVwsViewId aViewId,
                                                                TBool aEnter )
    {
    // Make Service Handler param lists.
    iInParamList->Reset();
	iOutParamList->Reset();
	
    RBuf8 buffer;
    CleanupClosePushL( buffer );
    buffer.CreateL( KIRBackSteppingPrefix().Length() + KDefaultRealWidth );
    buffer.Copy( KIRBackSteppingPrefix );
    // Convert 32-bit signed integer (TInt32) view uid to TInt.
    TInt64 viewUid = static_cast< TInt64 >( aViewId.iViewUid.iUid );
    buffer.AppendNum( viewUid );
        
	TLiwGenericParam state( KIRBSInParamState, TLiwVariant( buffer ) );
	iInParamList->AppendL( state );
	TLiwGenericParam enter( KIRBSInParamEnter, TLiwVariant( aEnter ) );
	iInParamList->AppendL( enter );

    // Execute view activation event.
	iBsInterface->ExecuteCmdL( KIRBSCmdForwardActivationEvent, *iInParamList, *iOutParamList );
    CleanupStack::PopAndDestroy(&buffer);

    // Check if BS Service consumed the event.
    HandleResultL();

	iInParamList->Reset();
	iOutParamList->Reset();
    }


// ---------------------------------------------------------------------------
// CIRBackSteppingServiceWrapper::HandleBackCommandL
// ---------------------------------------------------------------------------
//
TBool CIRBackSteppingServiceWrapper::HandleBackCommandL( const TVwsViewId aViewId )
    {
    // Make Service Handler param lists.
    iInParamList->Reset();
	iOutParamList->Reset();
	
    // Create message for back stepping wrapper to handle back command event.
    RBuf8 buffer;
    CleanupClosePushL( buffer );
    buffer.CreateL( KIRBackSteppingPrefix().Length() + KDefaultRealWidth );
    buffer.Copy( KIRBackSteppingPrefix );
    // Convert 32-bit signed integer (TInt32) view uid to TInt64.
    TInt64 viewUid = static_cast< TInt64 >( aViewId.iViewUid.iUid );
    buffer.AppendNum( viewUid );
    
	TLiwGenericParam state( KIRBSInParamState, TLiwVariant( buffer ) );
	iInParamList->AppendL( state );

    // Execute back event.
	iBsInterface->ExecuteCmdL( KIRBSCmdHandleBackCommand, *iInParamList, *iOutParamList );
    CleanupStack::PopAndDestroy(&buffer);

    // Check if BS Service consumes the event.
    TBool retVal = HandleResultL();

	iInParamList->Reset();
	iOutParamList->Reset();

	return retVal;
    }

// ---------------------------------------------------------------------------
// CIRBackSteppingServiceWrapper::CIRBackSteppingServiceWrapper
// ---------------------------------------------------------------------------
//
CIRBackSteppingServiceWrapper::CIRBackSteppingServiceWrapper()
    {
    // No implementation required.
    }

// ---------------------------------------------------------------------------
// CIRBackSteppingServiceWrapper::ConstructL
// ---------------------------------------------------------------------------
//
void CIRBackSteppingServiceWrapper::ConstructL( TUid aUid )
    {
    // Create Service Handler and keep as long as access to BS Service needed.
    iServiceHandler = CLiwServiceHandler::NewL();
    // For convenience keep pointers to Service Handler param lists.
	iInParamList = &iServiceHandler->InParamListL();
	iOutParamList = &iServiceHandler->OutParamListL();

    // Create AIW criteria.
    RCriteriaArray criteriaArray;
	CleanupResetAndDestroyPushL( criteriaArray );
	CLiwCriteriaItem* criterion = CLiwCriteriaItem::NewLC(
	    KLiwCmdAsStr,
	    KIRBSInterface,
	    KIRBSServiceID );
	criterion->SetServiceClass( TUid::Uid( KLiwClassBase ) );
	criteriaArray.AppendL( criterion );
	CleanupStack::Pop( criterion );

    // Attach AIW criteria.
	iServiceHandler->AttachL( criteriaArray );
	// Get BS Service interface.
	iServiceHandler->ExecuteServiceCmdL( *criterion, *iInParamList, *iOutParamList );
	CleanupStack::PopAndDestroy( &criteriaArray );

	// Check if BS interface can be found from output params.
	TInt pos( 0 );
	iOutParamList->FindFirst( pos, KIRBSInterface );
	if( KErrNotFound != pos )
		{
		// Extract BS interface from output params.
		ASSERT( pos >= 0 && pos < iOutParamList->Count() );
		iBsInterface = ( *iOutParamList )[pos].Value().AsInterface();	
		}
    
    if ( !iBsInterface )
        {
        // No BS Service available.
        User::Leave( KErrNotSupported );
        }

    // Initialize BS Service.
    InitializeL( aUid );
    }

// ---------------------------------------------------------------------------
// CIRBackSteppingServiceWrapper::InitializeL
// ---------------------------------------------------------------------------
//
void CIRBackSteppingServiceWrapper::InitializeL( const TUid aUid )
    {
    // Initialize BS Service.
    iInParamList->Reset();
	iOutParamList->Reset();

	TLiwGenericParam appUid( KIRBSInParamAppUid, TLiwVariant( aUid.iUid ) );
	iInParamList->AppendL( appUid );
	
	iBsInterface->ExecuteCmdL( KIRBSCmdInitialize, *iInParamList, *iOutParamList );

    if ( !HandleResultL() )
        {
        // Initialize command was not consumed.
        User::Leave( KErrArgument );
        }

	iInParamList->Reset();
	iOutParamList->Reset();
    }

// ---------------------------------------------------------------------------
// CIRBackSteppingServiceWrapper::HandleResultL
// ---------------------------------------------------------------------------
//
TInt CIRBackSteppingServiceWrapper::HandleResultL()
    {
    TBool retVal( EFalse );
    // Check if error can be found from outparams.
    TInt posErr( 0 );
	iOutParamList->FindFirst( posErr, LIW::EGenericParamError );
	if( KErrNotFound != posErr )
		{
		// Error code found - extract and handle
		ASSERT( posErr >= 0 && posErr < iOutParamList->Count() );
		TInt errorCode( KErrNone );
		( *iOutParamList )[posErr].Value().Get( errorCode );
		User::LeaveIfError( errorCode );
		}
    // No errors found. Check if status info can be found from outparams.
    TInt posStat( 0 );
    iOutParamList->FindFirst( posStat, KIRBSOutParamStatusInfo );
    if( KErrNotFound != posStat )
		{
		// Status info present - extract.
		ASSERT( posStat >= 0 && posStat < iOutParamList->Count() );
		( *iOutParamList )[posStat].Value().Get( retVal );
		}
	else
	    {
	    // No status info found.
	    User::Leave( KErrNotFound );
	    }
    return retVal;
    }
