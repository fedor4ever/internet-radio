/*
* Copyright (c) 2004-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Header file for CIRSystemEventDetector.
*
*/


#ifndef IRSYSTEMEVENTDETECTOR_H
#define IRSYSTEMEVENTDETECTOR_H

#include <e32base.h>

#include "irpropertyobserver2.h"

class MIRSystemEventDetectorObserver;

/**
* Catches notification of the call and network state changes.
*
* Class provides implementation to detect system events and notify UI components of the event,
* Class keeps several instances of CVPropertyObserver, and those instances send event notifications 
* about the particular system component state change. 
*/
NONSHARABLE_CLASS( CIRSystemEventDetector ) : public CBase, public MIRPropertyChangeObserver2
    {
    public:

        /**
        * Two-phased constructor.
        * @param aObserver a reference to the observer interface implementer
        */
        static CIRSystemEventDetector* NewL(MIRSystemEventDetectorObserver&  aObserver);
        
        /**
        * Destructor.
        */
        virtual ~CIRSystemEventDetector();

		/**
		* Returns network coverage state (wlan or mobile).
		*/
		TBool IsNetworkCoverage() const;

		/**
		* Returns mobile network coverage state.
		*/
		TBool IsMobileNetworkCoverage() const;

		/**
		* Returns current call state.
		*/
		TBool IsCallActive() const;
		
		/**
		* Returns current audio resources state.
		* @return audio resources state 
		*/
		TBool IsAudioResourcesAvailable() const;
		
		/**
		* Returns current Voice UI state.
		* @return ETrue if Voice UI is active, EFalse otherwise
		*/
		TBool IsVoiceUiActive() const;

    protected:
    
// from base class MIRPropertyChangeObserver2
	    void HandlePropertyChangeL(const TUid& aCategory, const TUint aKey, const TInt aValue);
	    void HandlePropertyChangeL(const TUid& aCategory, const TUint aKey, const TDesC8& aValue);
	    void HandlePropertyChangeL(const TUid& aCategory, const TUint aKey, const TDesC& aValue);
	    void HandlePropertyChangeErrorL(const TUid& aCategory, const TUint aKey, TInt aError);
   
    private:

        /**
        * By default EPOC constructor is private.
        */
        void ConstructL();
        
        /**
        * Used internally to change network coverage status and report it to observer.
        **/
        void CIRSystemEventDetector::SetNetworkCoverageL( const TBool aIsMobileNetworkCoverage,
        		 const TBool aIsWlanCoverage );

        /**
        * C++ default constructor.
        * @param aObserver a reference to the observer interface implementer     
        */
        CIRSystemEventDetector(MIRSystemEventDetectorObserver&  aObserver);
    
	private:    // Data
    
        /**Observer which will be notified of the call and network state changes*/
		MIRSystemEventDetectorObserver&  iObserver;
	
        /** handler to Publish & Subscribe interface*/        		
        CIRPropertyObserver2*    iCallStatusObserver;
        /** handler to Publish & Subscribe interface*/        		
        CIRPropertyObserver2*    iNetworkStatusObserver;
        /** handler to Publish & Subscribe interface*/        		
        CIRPropertyObserver2*    iVoiceUiObserver;
        /** handler to Publish & Subscribe interface*/        		
        CIRPropertyObserver2*    iAudioPolicyObserver;
        /** handler to Publish & Subscribe interface*/        		
        CIRPropertyObserver2*    iWlanObserver;

		/**Indicator for the mobile network status*/
		TBool iIsMobileNetworkCoverage;

		/**Indicator for the wlan network status*/
		TBool iIsWlanCoverage;
		
		/**Indicator for the call status*/
		TBool iIsCallActive;
        
        /** Array of audio policy categories that VR must not resume after. */
        RArray<TInt> iNoAutoResumeAudioCategories;
        
        /** Indicator for audio resource status. */
		TBool iIsAudioResourcesAvailable;
		
		/** Indicator for Voice UI status. */
		TBool iIsVoiceUiActive;
    };

#endif   // IRSYSTEMEVENTDETECTOR_H   
            
