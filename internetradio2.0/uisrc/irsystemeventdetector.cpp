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
* Description:  
*   Class provides implementation to detect system events and notify UI 
*   components of the event, Class keeps several instances of 
*   CIRPropertyObserver2, and those instances send event notifications 
*   about the particular system component state change. 
*
*/
	

#include <e32def.h> // Define before audiopolicy defs
#include <e32std.h> // Define before audiopolicy defs
#include <eikdef.h>

#ifndef __WINS__

//#include <audiopolicypubsubdata.h>
//#include <audiosw_pubsubkeys.h>

#endif //__WINS__

#include <ctsydomainpskeys.h>
#include <wlaninternalpskeys.h>
#include <sacls.h>
#ifdef MUSICSHOP_AVAILABLE
#include <voiceuidomainpskeys.h>
#endif
#include <featmgr.h>
//d #include <vrdebug.h>

#include "irsystemeventdetector.h"
#include "irsystemeventdetectorobserver.h"
#include "irdebug.h"

/** Granularity for audio category arrays. */
const TInt KVRAudioCategoryArrayGranularity = 3;

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------------------------
// C++ default constructor can NOT contain any code, that might leave.
// ---------------------------------------------------------------------------
//
CIRSystemEventDetector::CIRSystemEventDetector(MIRSystemEventDetectorObserver&  aObserver) 
    :    iObserver(aObserver),
         iIsMobileNetworkCoverage(EFalse),
         iIsWlanCoverage(EFalse),
         iIsCallActive(EFalse),
         iIsAudioResourcesAvailable(ETrue),
         iIsVoiceUiActive(EFalse)
    {
    IRLOG_DEBUG( "CIRSystemEventDetector::CIRSystemEventDetector" );
    }

// ---------------------------------------------------------------------------
// EPOC default constructor can leave.
// ---------------------------------------------------------------------------
//
void CIRSystemEventDetector::ConstructL()
    {
    IRLOG_DEBUG( "CIRSystemEventDetector::ConstructL - Entering" );
    FeatureManager::InitializeLibL();
    TBool wlanSupported = FeatureManager::FeatureSupported(KFeatureIdProtocolWlan);
    FeatureManager::UnInitializeLib();
    

        iNetworkStatusObserver = CIRPropertyObserver2::NewL(*this, 
                                 KUidSystemCategory, 
                                 KUidNetworkStatusValue, 
                                 CIRPropertyObserver2::EIRPropertyInt);
        iNetworkStatusObserver->ActivateL();
        
    iIsMobileNetworkCoverage = (iNetworkStatusObserver->ValueInt() == ESANetworkAvailable) ? 
    										ETrue : EFalse;

    // On S60 platform, there is no guaranteed way of seeing whether WLAN is explicitly turned off
    // in the settings, or whether the network is available. We use the P&S key KPSUidWlan/KPSWlanIndicator
    // because it is the best we can get.
    if (wlanSupported)
        {
        iWlanObserver = CIRPropertyObserver2::NewL(*this,
                        KPSUidWlan,
                        KPSWlanIndicator,
                        CIRPropertyObserver2::EIRPropertyInt);
        iWlanObserver->ActivateL();

        TInt wlanStatus = iWlanObserver->ValueInt();
        iIsWlanCoverage = (wlanStatus == EPSWlanIndicatorActive || wlanStatus == 
        			EPSWlanIndicatorActiveSecure) ? ETrue : EFalse;
        }
    else
        {
        iIsWlanCoverage = EFalse;
        }

    // Initialize call state observer.
    iCallStatusObserver = CIRPropertyObserver2::NewL(*this, 
                                                      KPSUidCtsyCallInformation, 
                                                      KCTsyCallState, 
                                                      CIRPropertyObserver2::EIRPropertyInt);
    iCallStatusObserver->ActivateL();
    iIsCallActive = iCallStatusObserver->ValueInt() != EPSCTsyCallStateNone;
    
#ifdef MUSICSHOP_AVAILABLE
    // Initialize voice ui observer.
    iVoiceUiObserver = CIRPropertyObserver2::NewL(*this, KPSUidVoiceUiAccMonitor, KVoiceUiOpenKey,
    					 CIRPropertyObserver2::EIRPropertyInt);
    iVoiceUiObserver->ActivateL();    					    
#endif
    IRLOG_DEBUG( "CIRSystemEventDetector::ConstructL - Exiting" );

#ifndef __WINS__
    // Define audio types for not resuming.
    iNoAutoResumeAudioCategories = RArray<TInt>( KVRAudioCategoryArrayGranularity );
//    iNoAutoResumeAudioCategories.AppendL(ECatMediaPlayer)
//    iNoAutoResumeAudioCategories.AppendL(ECatMobileTv)
//    iNoAutoResumeAudioCategories.AppendL(ECatUnknownPlayer)
    iNoAutoResumeAudioCategories.Compress();
    // Start listening audio client events
//	iAudioPolicyObserver = CIRPropertyObserver2::NewL(*this, KPSUidMMFAudioServer, KAudioPolicyAudioClients, CIRPropertyObserver2::EIRPropertyByteArray)
//	iAudioPolicyObserver->ActivateL()
#endif
    }


// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CIRSystemEventDetector::~CIRSystemEventDetector()
    {
    IRLOG_DEBUG( "CIRSystemEventDetector::~CIRSystemEventDetector - Entering" );
    FeatureManager::UnInitializeLib();
    delete iCallStatusObserver;
    delete iNetworkStatusObserver;
    delete iVoiceUiObserver;
//    delete iAudioPolicyObserver
    delete iWlanObserver;

    iNoAutoResumeAudioCategories.Close();
    IRLOG_DEBUG( "CIRSystemEventDetector::~CIRSystemEventDetector - Exiting" );
    }
    
// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CIRSystemEventDetector* CIRSystemEventDetector::NewL(
                                    MIRSystemEventDetectorObserver&  aObserver)
    {
    IRLOG_DEBUG( "CIRSystemEventDetector::NewL - Entering" );
    CIRSystemEventDetector* self = new (ELeave) CIRSystemEventDetector(aObserver);
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    IRLOG_DEBUG( "CIRSystemEventDetector::NewL - Exiting" );
    return self;
    }
    
// ---------------------------------------------------------------------------
// Handling of the int property changes is done here. 
// Observer components are getting notifications in correspondence with what 
// has changed
// ---------------------------------------------------------------------------
//
void CIRSystemEventDetector::HandlePropertyChangeL( const TUid& aCategory, 
                                                    const TUint aKey, 
                                                    const TInt aValue )
	{
	IRLOG_DEBUG( "CIRSystemEventDetector::HandlePropertyChangeL - Entering" );
    if (aCategory == KUidSystemCategory && aKey == KUidNetworkStatusValue)
    	{
    	switch (aValue)
    		{
    		case ESANetworkAvailable:
    			{
                SetNetworkCoverageL( ETrue, iIsWlanCoverage );
    			break;
    			}
    		case ESANetworkUnAvailable:
    			{
                SetNetworkCoverageL( EFalse, iIsWlanCoverage );
    			break;
    			}
    		default:
    			{
    			break;   			
    			}
    		}
    	}
    else if (aCategory == KPSUidCtsyCallInformation && aKey == KCTsyCallState)
    	{
		if ((!iIsCallActive) && (aValue > EPSCTsyCallStateNone)) 
			{
			iIsCallActive = ETrue;
			iObserver.CallActivatedCallbackL();
			}
		else if ((iIsCallActive) && (aValue <= EPSCTsyCallStateNone))
			{
			iIsCallActive = EFalse;
			iObserver.CallDeactivatedCallbackL();	
			}
        }
    else if ( aCategory == KPSUidWlan && aKey == KPSWlanIndicator )
        {
        switch (aValue)
            {
            case EPSWlanIndicatorActive:
            case EPSWlanIndicatorActiveSecure:
                {
                SetNetworkCoverageL( iIsMobileNetworkCoverage, ETrue );
                break;
                }
            case EPSWlanIndicatorNone:
            case EPSWlanIndicatorAvailable:
                {
                SetNetworkCoverageL( iIsMobileNetworkCoverage, EFalse );
                break;
                }
            default:
                {
                break;
                }
			}
    	}
#ifdef MUSICSHOP_AVAILABLE
    else if ( aCategory == KPSUidVoiceUiAccMonitor && aKey == KVoiceUiOpenKey )
        {
        switch (aValue)
    		{
    		case KVoiceUiIsClose:
    			{
				if (iIsVoiceUiActive)
				    {
				    iIsVoiceUiActive = EFalse;
				    if (iIsAudioResourcesAvailable)
				        {
            		    iObserver.AudioResourcesAvailableL();
				        }
				    }
    			break;
    			}
    		case KVoiceUiIsOpen:
    			{
    			if (!iIsVoiceUiActive)
    			    {
    			    iIsVoiceUiActive = ETrue;
    			    }
    			break;
    			}
    		default:
    			{
    			break;   			
    			}
    		}
        }
#endif
    else // NOP
    	{
    	}
    IRLOG_DEBUG( "CIRSystemEventDetector::HandlePropertyChangeL - Exiting" );
 	}

#ifndef __WINS__	
// ---------------------------------------------------------------------------
// Handling of the byte array property changes is done here. 
// Observer components are getting notifications in correspondence with what 
// has changed
// ---------------------------------------------------------------------------
//
void CIRSystemEventDetector::HandlePropertyChangeL( const TUid& aCategory, 
                                                    const TUint aKey, 
                                                    const TDesC8& aValue)
	{
/*    if (aCategory == KPSUidMMFAudioServer)
        {        
        if (aKey == KAudioPolicyAudioClients)
            {
            TBool atLeastOneAutoResumeAudioPlaying(EFalse);
            TBool atLeastOneNoAutoResumeAudioPlaying(EFalse);
            TBool radioPlaying(EFalse);
            TAudioClientList audioClients;
            audioClients.Copy(aValue);
            // Check all playing audios
            for ( TInt i = 0; i < audioClients().iNumOfProcesses ; i++ )
                {
                TInt cat = audioClients().iClientCategoryList[i];
                VRDEBUG2(" *** S60VisualRadio -- Check audio cat %d", cat);
                if (cat == ECatFmRadio)
                    {
                    radioPlaying = ETrue;
                    }
                else if (iNoAutoResumeAudioCategories.Find(cat) != KErrNotFound)
                    {
                    atLeastOneNoAutoResumeAudioPlaying = ETrue;
                    }
                else
                    {
                    atLeastOneAutoResumeAudioPlaying = ETrue;
                    }
                }
                
            if (!radioPlaying)
                {
                // Decide audio resource availability from audio category info.
                if (atLeastOneNoAutoResumeAudioPlaying)
            		{
            		VRDEBUG(" *** S60VisualRadio -- Audio resources not available. Change informed.");
            		iIsAudioResourcesAvailable = EFalse;
            		iObserver.AudioAutoResumeForbiddenL();
            		}
            	else if (!atLeastOneAutoResumeAudioPlaying)
            		{
            		if (!iIsVoiceUiActive)
            		    {
            		    VRDEBUG(" *** S60VisualRadio -- Audio resources available. Change informed.");
            		    iIsAudioResourcesAvailable = ETrue;
            		    iObserver.AudioResourcesAvailableL();
            		    }
            		else
            		    {
            		    VRDEBUG(" *** S60VisualRadio -- Audio resources available. Change not informed.");
            		    iIsAudioResourcesAvailable = ETrue;
            		    }
            		}
            	else
            	    {
            	    VRDEBUG(" *** S60VisualRadio -- Audio resources not available. Change not informed.");
            		iIsAudioResourcesAvailable = EFalse;
            	    }
                }
            else // audio resources are considered to be available when radio is playing
                {
                iIsAudioResourcesAvailable = ETrue;
                }
            }
        }
 */	}
 	
#else //__WINS__
// ---------------------------------------------------------------------------
// Dummy version for WINS in order to avoid compiler warnings. 
// The real implementation of function is above.
// ---------------------------------------------------------------------------
//
void CIRSystemEventDetector::HandlePropertyChangeL( const TUid& /*aCategory*/, 
                                                    const TUint /*aKey*/, 
                                                    const TDesC8& /*aValue*/)
	{
	IRLOG_DEBUG( "CIRSystemEventDetector::HandlePropertyChangeL" );
 	}
#endif

// ---------------------------------------------------------------------------
// Handling of the text property changes is done here. 
// Observer components are getting notifications in correspondence with what 
// has changed
// ---------------------------------------------------------------------------
//
void CIRSystemEventDetector::HandlePropertyChangeL( const TUid& /*aCategory*/, 
                                                    const TUint /*aKey*/, 
                                                    const TDesC& /*aValue*/)
	{
	IRLOG_DEBUG( "CIRSystemEventDetector::HandlePropertyChangeL" );
	}

// ---------------------------------------------------------------------------
// This is a callback function which is called when a P&S components returns 
// an error
// ---------------------------------------------------------------------------
//
void CIRSystemEventDetector::HandlePropertyChangeErrorL( const TUid& aCategory, 
                                                         const TUint aKey, 
                                                         TInt aError )
	{
	IRLOG_DEBUG( "CIRSystemEventDetector::HandlePropertyChangeErrorL - Entering" );
#ifdef MUSICSHOP_AVAILABLE
	if ( aCategory == KPSUidVoiceUiAccMonitor && aKey == KVoiceUiOpenKey && aError == KErrNotFound )
        {
        HandlePropertyChangeL( KPSUidVoiceUiAccMonitor, KVoiceUiOpenKey, KVoiceUiIsClose );
        }
    else
        {
        iObserver.ErrorCallbackL(aError);
        }
#else  // remove warnings
    if ( aCategory.iUid == 0x101F8543 || aKey || aError)
        {
        // do nothing
        }
#endif
	IRLOG_DEBUG( "CIRSystemEventDetector::HandlePropertyChangeErrorL - Exiting" );
	}

// ---------------------------------------------------------------------------
TBool CIRSystemEventDetector::IsMobileNetworkCoverage() const
    {
    IRLOG_DEBUG( "CIRSystemEventDetector::IsMobileNetworkCoverage" );
    return iIsMobileNetworkCoverage;
    }
// CIRSystemEventDetector::IsNetworkCoverage
// ---------------------------------------------------------------------------
//
TBool CIRSystemEventDetector::IsNetworkCoverage() const
	{
	 IRLOG_DEBUG( "CIRSystemEventDetector::IsNetworkCoverage" );
    return iIsWlanCoverage || iIsMobileNetworkCoverage;
	}

// ---------------------------------------------------------------------------
// CIRSystemEventDetector::IsCallActive
// ---------------------------------------------------------------------------
//
TBool CIRSystemEventDetector::IsCallActive() const
	{
	IRLOG_DEBUG( "CIRSystemEventDetector::IsCallActive" );
	return iIsCallActive;
	}
	
// ---------------------------------------------------------------------------
// CIRSystemEventDetector::IsAudioResourcesAvailable
// ---------------------------------------------------------------------------
//
TBool CIRSystemEventDetector::IsAudioResourcesAvailable() const
	{
	IRLOG_DEBUG( "CIRSystemEventDetector::IsAudioResourcesAvailable" );
	return iIsAudioResourcesAvailable;
	}

// ---------------------------------------------------------------------------
// CIRSystemEventDetector::IsVoiceUiActive
// ---------------------------------------------------------------------------
//
TBool CIRSystemEventDetector::IsVoiceUiActive() const
    {
    IRLOG_DEBUG( "CIRSystemEventDetector::IsVoiceUiActive" );
    return iIsVoiceUiActive;
    }
    

// ---------------------------------------------------------------------------
// CIRSystemEventDetector::SetNetworkCoverageL
// ---------------------------------------------------------------------------
//
void CIRSystemEventDetector::SetNetworkCoverageL( const TBool aIsMobileNetworkCoverage,
					 const TBool aIsWlanCoverage )
    {
    IRLOG_DEBUG( "CIRSystemEventDetector::SetNetworkCoverageL - Entering" );

    TBool wasCoverage = IsNetworkCoverage();
    iIsMobileNetworkCoverage = aIsMobileNetworkCoverage;
    iIsWlanCoverage = aIsWlanCoverage;
    TBool isCoverage = IsNetworkCoverage();

    if (isCoverage != wasCoverage)
        {
        if (isCoverage)
            {
            iObserver.NetworkUpCallbackL();
            }
        else
            {
            iObserver.NetworkDownCallbackL();
            }
        IRLOG_DEBUG( "CIRSystemEventDetector::SetNetworkCoverageL - Exiting" );
        }
    }
