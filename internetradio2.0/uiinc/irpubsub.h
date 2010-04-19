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
* Description:  Header file for CIRPubSub
*
*/


#ifndef CIRPUBSUB_H
#define CIRPUBSUB_H

#include <e32base.h>

#include "irpubsubkeys.h"

class CIRMetaData;
class CIRPropertyObserver;

/**
 *  Publish&Subscribe interface of Internet Radio Application.
 */
class CIRPubSub : public CBase
    
	//public MIRPropertyChangeObserver
    {
    
public:  // Methods

    /**
     * Static constructor.
     */
    static CIRPubSub* NewL();
    
    /**
     * Destructor.
     */
    virtual ~CIRPubSub();

    /**
     * Publishes active channel number to P&S.
     *
     * @param aChannelName The channel name to publish.
     */
    void PublishChannel( const TDesC16& aChannelName ) const;
    
    /**
     * Publishes volume level number to P&S.
     *
     * @param aVol The volume level to publish.
     */
    void PublishVolume( TInt aVol ) const;
    
    /**
     * Publishes active channel number to P&S.
     *
     * @param aPlayingState The state of palying to publish.
     */
    void PublishPlayStop ( TIRStopPlayState aPlayingState ) const;
    
    /**
     * Publishes application running state to P&S.
     *
     * @param aRunningState The application running state.
     */
    void PublishApplicationRunningStateL( 
        TIRPSApplicationRunningState aRunningState ) const;

    /**
     * Published meta data information.
     *
     * @param   aMetaData   Meta data information to publish.
     */
    void PublishMetaDataL( const CIRMetaData& aMetaData ) const;
    
    /**
     * Publishes active channel Type to P&S.
     *
     * @param aChannelName The channel Type to publish.
     */
    void PublishChannelType ( TInt aChannelType ) const;
    
    /**
     * Publishes active channel Id to P&S.
     *
     * @param aChannelId The channel Id to publish.
     */
    void PublishChannelId ( TInt aChannelId ) const;
    
    void PublishChannelDesc(const TDesC16& aChannelDesc ) const ;
    
    void PublishChannelMusicFlag(const TDesC16& aChannelMusicFlag ) const ;
    
    
    void PublishBufferingState(TIRBufferingState aState) const;

private: //Methods

	/**
     * C++ default constructor.
     */
    CIRPubSub( );
    
   void ConstructL();

    /**
     * Defines a property.
     *
     * @param aKey The key of the property.
     * @param aAttr Attributes of the property.
     * @param aDataProperty ETrue if the property is a data property, EFalse 
     * if it is a control property
     */
    void DefinePropertyL( 
        TUint aKey, 
        TInt aAttr, 
        TBool aDataProperty ) const;

    /**
     * Deletes a property.
     *
     * @param aKey The key of the property.
     */
    void DeleteProperty( TUint aKey ) const;        

    
private:    // Data

	/** 
	 * An array of handlers to P&S interface. 
	 */ 
	RPointerArray<CIRPropertyObserver> iPropertyArray;
public :

    //iRapp application instance count
    TInt iApplicationInstanceCount;
	
    };

#endif      // CIRPUBSUB_H   

 
