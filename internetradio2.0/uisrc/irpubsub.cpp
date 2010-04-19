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


#include <s32strm.h>

#include "ircommon.h"
#include "irdebug.h"
#include "irmetadata.h"
#include "irpropertyobserver.h"
#include "irpubsub.h"

/**
*C++ default constructor can NOT contain any code, that
* might leave.
*/
CIRPubSub::CIRPubSub()
    {
	IRLOG_DEBUG( "CIRPubSub::CIRPubSub" );
    }

/**
* Function: ConstructL
* Symbian 2nd phase constructor can leave.
*/

void CIRPubSub::ConstructL()
    {
	IRLOG_DEBUG( "CIRPubSub::ConstructL - Entering" );
    // Define data properties.
    DefinePropertyL( KIRPSDataApplicationRunning, RProperty::EInt, ETrue );
    DefinePropertyL( KIRPSPresetIndex, RProperty::EInt, ETrue );
    DefinePropertyL( KIRPSDataChannel, RProperty::ELargeText, ETrue );
    DefinePropertyL( KIRPSMetaData, RProperty::ELargeText, ETrue );
    DefinePropertyL( KIRPSVolume, RProperty::EInt, ETrue );
    DefinePropertyL( KIRPSPlayStop, RProperty::EInt, ETrue );
    DefinePropertyL( KIRPSChannelUrl, RProperty::ELargeText, ETrue );
    DefinePropertyL( KIRPSBitrate, RProperty::EInt, ETrue );
    DefinePropertyL( KIRPSMultiplePlay, RProperty::EInt, ETrue );
    DefinePropertyL( KIRPSStationName, RProperty::ELargeText, ETrue );
    DefinePropertyL( KIRPSAppInstanceRunning, RProperty::EInt, ETrue );
    DefinePropertyL( KIRPSChannelType, RProperty::EInt, ETrue );
    DefinePropertyL( KIRPSChannelId, RProperty::EInt, ETrue );
    DefinePropertyL( KIRPSDataChannelDesc, RProperty::ELargeText, ETrue );
    DefinePropertyL( KIRPSChannelMusicFlag, RProperty::ELargeText, ETrue );
    DefinePropertyL( KIRPSBufferingState, RProperty::EInt, ETrue );
    //Gets the integer value for the property KIRPSAppInstanceRunning
    //which gives the number of instances of iRapp running
    RProperty::Get( KUidActiveInternetRadioApp,KIRPSAppInstanceRunning,iApplicationInstanceCount);
    //Increment the count and set the integer value for the property 
    //KIRPSAppInstanceRunning to track the number of instances of iRapp
    //running
    iApplicationInstanceCount++;
    RProperty::Set( KUidActiveInternetRadioApp,KIRPSAppInstanceRunning,iApplicationInstanceCount);
    DefinePropertyL( KIRPSControlSetPlayState, RProperty::EInt, EFalse );

    IRLOG_DEBUG( "CIRPubSub::ConstructL - Exiting." );	
    }

/**
* Function: NewL
* Two-phased constructor.
* @return An Instance of CIRBufferingControl
*/
CIRPubSub* CIRPubSub::NewL()
    {
	IRLOG_DEBUG( "CIRPubSub::NewL - Entering" );
    CIRPubSub* self = new (ELeave) CIRPubSub();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
	IRLOG_DEBUG( "CIRPubSub::NewL - Exiting." );
    return self;
    }

    
/**
* Function: ~CIRPubSub
* Destructs all the Property Instances
*/
CIRPubSub::~CIRPubSub()
    {
	IRLOG_DEBUG( "CIRPubSub::~CIRPubSub - Entering" );
    iPropertyArray.ResetAndDestroy();
	iPropertyArray.Close();
    RProperty::Get( KUidActiveInternetRadioApp,KIRPSAppInstanceRunning,iApplicationInstanceCount);
    iApplicationInstanceCount--;

    //Delete the property only when one instance of iRapp is running 
    if(!iApplicationInstanceCount )
    	{
        DeleteProperty( KIRPSDataApplicationRunning );
        DeleteProperty( KIRPSDataChannel );
        DeleteProperty( KIRPSMetaData );
        DeleteProperty( KIRPSPresetIndex );
 	    DeleteProperty( KIRPSVolume );
        DeleteProperty( KIRPSPlayStop );
        DeleteProperty( KIRPSChannelUrl );
        DeleteProperty( KIRPSBitrate );
        DeleteProperty( KIRPSMultiplePlay );
        DeleteProperty( KIRPSStationName );
        DeleteProperty( KIRPSAppInstanceRunning );
        DeleteProperty( KIRPSChannelType );
        DeleteProperty( KIRPSChannelId );
        DeleteProperty( KIRPSDataChannelDesc );
        DeleteProperty( KIRPSChannelMusicFlag );
        DeleteProperty( KIRPSControlSetPlayState );
        DeleteProperty( KIRPSBufferingState );
    	}
    RProperty::Set( KUidActiveInternetRadioApp,KIRPSAppInstanceRunning,iApplicationInstanceCount);
	
    IRLOG_DEBUG( "CIRPubSub::~CIRPubSub - Exiting." );	
    }

/*
* CIRPubSub::DefinePropertyL
* Defines a property.
*/
void CIRPubSub::DefinePropertyL( TUint aKey, TInt aAttr, TBool aDataProperty ) 
				const
    {
	IRLOG_DEBUG( "CIRPubSub::DefinePropertyL - Entering" );
    _LIT_SECURITY_POLICY_PASS(KIRExternalAccessPolicy);
    _LIT_SECURITY_POLICY_S0(KIRInternalAccessPolicy, KUIDIRAPP);
    
    TInt err = KErrNone;
    if(aDataProperty)
        {   
        // For data properties, grant read access for all, but write access only for iRAPP
       	err = RProperty::Define( KUidActiveInternetRadioApp, aKey, aAttr, 
       			KIRExternalAccessPolicy, KIRInternalAccessPolicy );
        }
    else
        {
        // For control properties, grant read access only for IRA, but write access for all
	    err = RProperty::Define( KUidActiveInternetRadioApp, aKey, aAttr,
	    		 KIRInternalAccessPolicy, KIRExternalAccessPolicy );
	    }
	if ( err && err != KErrAlreadyExists )
        {
        User::Leave( err );
        }
	IRLOG_DEBUG( "CIRPubSub::DefinePropertyL - Exiting." );
     }

/*
* CIRPubSub::DeleteProperty
* Deletes a property
*/

void CIRPubSub::DeleteProperty( TUint aKey ) const
    {
	IRLOG_DEBUG( "CIRPubSub::DeleteProperty Entering" );
    RProperty::Delete( KUidActiveInternetRadioApp, aKey );
	IRLOG_DEBUG( "CIRPubSub::DeleteProperty - Exiting." );
    }


/*
* CIRPubSub::PublishChannel
* Publishes the property value for key KIRPSDataChannel
*/
	
void CIRPubSub::PublishChannel(const TDesC16& aChannelName ) const
    {
	IRLOG_DEBUG( "CIRPubSub::PublishChannel - Entering" );	
	RProperty::Set( KUidActiveInternetRadioApp, KIRPSDataChannel,
					 aChannelName );
	IRLOG_DEBUG( "CIRPubSub::PublishChannel - Exiting." );	
    }

/*
* CIRPubSub::PublishVolumeL
* Publishes the property value for key KIRPSVolume
*/

void CIRPubSub::PublishVolume( TInt aVol ) const
    {
	IRLOG_DEBUG( "CIRPubSub::PublishVolumeL - Entering" );	
	RProperty::Set( KUidActiveInternetRadioApp, KIRPSVolume , aVol );
	IRLOG_DEBUG( "CIRPubSub::PublishVolumeL - Exiting." );	
    }
    
/*
* CIRPubSub::PublishPlayStop
* Publishes the property value for playing status
*/
void CIRPubSub::PublishPlayStop ( TIRStopPlayState aPlayingState ) const
	{
	IRLOG_DEBUG( "CIRPubSub::PublishPlayStop - Entering" );	
	RProperty::Set( KUidActiveInternetRadioApp, KIRPSPlayStop, aPlayingState );	
	IRLOG_DEBUG( "CIRPubSub::PublishPlayStop - Exiting." );	
	}

/*
* CIRPubSub::PublishApplicationRunningStateL
* Publishes the property value for key KIRPSVolume
*/
	
void CIRPubSub::PublishApplicationRunningStateL( TIRPSApplicationRunningState
	 			aRunningState ) const
	    {
	IRLOG_DEBUG( "CIRPubSub::ConstructL - Entering" );	
    User::LeaveIfError( RProperty::Set( KUidActiveInternetRadioApp, 
    		KIRPSDataApplicationRunning, aRunningState ) );
    IRLOG_DEBUG( "CIRPubSub::ConstructL - Exiting." );	
	}

// ---------------------------------------------------------------------------
// Publishes meta data information.
// ---------------------------------------------------------------------------
//
void CIRPubSub::PublishMetaDataL( const CIRMetaData& aMetaData ) const
    {
    IRLOG_INFO4( "CIRPubSub::PublishMetaDataL( aMetaData.Artist()=%S, aMetaData.Song()=%S, aMetaData.StreamUrl()=%S )",
                 &aMetaData.Artist(), &aMetaData.Song(), &aMetaData.StreamUrl() );

    HBufC8* buffer = HBufC8::NewLC( aMetaData.Artist().Size() + aMetaData.Song().Size() + 
    				aMetaData.StreamUrl().Size() );
    
    TPtr8 bufferPtr( buffer->Des() );
    
    RDesWriteStream stream( bufferPtr );
    CleanupClosePushL( stream );

    aMetaData.ExternalizeL( stream );
    User::LeaveIfError( RProperty::Set( KUidActiveInternetRadioApp, KIRPSMetaData, bufferPtr ) );

    CleanupStack::PopAndDestroy(&stream);
    CleanupStack::PopAndDestroy( buffer );

    IRLOG_DEBUG( "CIRPubSub::PublishMetaDataL - Exit" );
    }
   
/*
* CIRPubSub::PublishChannelType
* Publishes the property value for  KIRPSChannelType
*/
void CIRPubSub::PublishChannelType ( TInt aChannelType ) const
	{
	IRLOG_DEBUG( "CIRPubSub::PublishPlayStop - Entering" );	
	RProperty::Set( KUidActiveInternetRadioApp, KIRPSChannelType, aChannelType );	
	IRLOG_DEBUG( "CIRPubSub::PublishPlayStop - Exiting." );	
	}
    
/*
* CIRPubSub::PublishChannelId
* Publishes the property value for  KIRPSChannelId
*/
void CIRPubSub::PublishChannelId ( TInt aChannelId ) const
	{
	IRLOG_DEBUG( "CIRPubSub::PublishPlayStop - Entering" );	
	RProperty::Set( KUidActiveInternetRadioApp, KIRPSChannelId, aChannelId );	
	IRLOG_DEBUG( "CIRPubSub::PublishPlayStop - Exiting." );	
	}

/*
* CIRPubSub::PublishChannelDesc
* Publishes the property value for key KIRPSDataChannel
*/
	
void CIRPubSub::PublishChannelDesc(const TDesC16& aChannelDesc ) const
    {
	IRLOG_DEBUG( "CIRPubSub::PublishChannel - Entering" );	
	RProperty::Set( KUidActiveInternetRadioApp, KIRPSDataChannelDesc,
					 aChannelDesc );
	IRLOG_DEBUG( "CIRPubSub::PublishChannel - Exiting." );	
    }
/*
* CIRPubSub::PublishChannelMusicFlag
* Publishes the property value for key KIRPSChannelMusicFlag
*/
	
void CIRPubSub::PublishChannelMusicFlag(const TDesC16& aChannelMusicFlag ) const
    {
	IRLOG_DEBUG( "CIRPubSub::PublishChannelMusicFlag - Entering" );	
	RProperty::Set( KUidActiveInternetRadioApp, KIRPSChannelMusicFlag,
					 aChannelMusicFlag );
	IRLOG_DEBUG( "CIRPubSub::PublishChannelMusicFlag - Exiting." );	
    }
    /*
* CIRPubSub::PublishBufferingState
* Publishes the property value for key KIRPSBufferingState
*/
	
void CIRPubSub::PublishBufferingState(TIRBufferingState aState) const
    {
	IRLOG_DEBUG( "CIRPubSub::PublishBufferingState - Entering" );	
	RProperty::Set( KUidActiveInternetRadioApp, KIRPSBufferingState,
					 aState );
	IRLOG_DEBUG( "CIRPubSub::PublishBufferingState - Exiting." );	
    }
//  End of File  
