/*
* Copyright (c) 2006-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Container class for the settings UI
*
*/


#include <aknnotewrappers.h>
#include <e32property.h>
#include <stringloader.h>
#include <akntitle.h> 
#include <cmmanager.h>
#include <cmdestination.h>
#include "ircommonsettingscontainer.h"
#include "ir.hrh"
#include "irnetworkcontroller.h"
#include "iraap.hlp.hrh"
#include "irdebug.h" //  PC-Lint comments :: SPP

#define KUIDIRAPP 0x2000B499
const TUid KIRMCVUid = {KUIDIRAPP};


// ======== LOCAL FUNCTIONS ========

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// NewL()
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
EXPORT_C CIRCommonSettingsContainer* CIRCommonSettingsContainer::NewL()
    {
    IRLOG_DEBUG( "CIRCommonSettingsContainer::NewL - Entering" );
    CIRCommonSettingsContainer* self = new ( ELeave ) CIRCommonSettingsContainer(/*aAccessPointList, aDefaultAccessPoint*/);
    CleanupStack::PushL( self );
    self->BaseConstructL();
    CleanupStack::Pop( self );
    IRLOG_DEBUG( "CIRCommonSettingsContainer::NewL - Exiting" );
    return self;
	}

// ---------------------------------------------------------------------------
// Constructor.
// ---------------------------------------------------------------------------
//
EXPORT_C CIRCommonSettingsContainer::CIRCommonSettingsContainer( ) 
    {
    IRLOG_DEBUG( "CIRCommonSettingsContainer::CIRCommonSettingsContainer" );
    }

// ---------------------------------------------------------------------------
// BaseConstructL()
// ---------------------------------------------------------------------------
//
EXPORT_C void CIRCommonSettingsContainer::BaseConstructL()
    {
    IRLOG_DEBUG( "CIRCommonSettingsContainer::BaseConstructL - Entering" );
    iNetworkController = CIRNetworkController::OpenL();
    iIRSettings = CIRSettings::OpenL();
    iDestinationSelected = EFalse;
    SetListBoxTextL() ;
    IRLOG_DEBUG( "CIRCommonSettingsContainer::BaseConstructL - Exiting" );
    }

// ---------------------------------------------------------------------------
// Destructor.
// ---------------------------------------------------------------------------
//
EXPORT_C CIRCommonSettingsContainer::~CIRCommonSettingsContainer()
    {
    IRLOG_DEBUG( "CIRCommonSettingsContainer::~CIRCommonSettingsContainer - Entering" );
	if(iIRSettings) 
		{
		iIRSettings->Close();
		iIRSettings = NULL;
		}
	if(iNetworkController) 
		{
		iNetworkController->Close();
		iNetworkController = NULL;	
		}
	IRLOG_DEBUG( "CIRCommonSettingsContainer::~CIRCommonSettingsContainer - Exiting" );
    }
    
// ---------------------------------------------------------------------------
// EditCurrentItemL()
// Starts editing of the current item.
// ---------------------------------------------------------------------------
//
void CIRCommonSettingsContainer::EditCurrentItemL( TBool aCalledFromMenu )
    {
    IRLOG_DEBUG( "CIRCommonSettingsContainer::EditCurrentItemL - Entering" );
    EditItemL( ListBox()->CurrentItemIndex(), aCalledFromMenu );
    DrawDeferred();
    IRLOG_DEBUG( "CIRCommonSettingsContainer::EditCurrentItemL - Exiting" );
    }
 
TInt CIRCommonSettingsContainer::GetCurrentSelection()
	{
	IRLOG_DEBUG( "CIRCommonSettingsContainer::GetCurrentSelection - Entering" );
	TInt current = ListBox()->CurrentItemIndex();
	IRLOG_DEBUG( "CIRCommonSettingsContainer::GetCurrentSelection - Exiting" );
	if((current >= 0) && (current < SettingItemArray()->Count()))
		{
		return SettingItemArray()->At( current )->Identifier();
		}
	else 
		{
		return 0;
		}
	}

// ---------------------------------------------------------------------------
// CreateSettingItemL()
// From CAknSettingItemList.
// Creates a setting item based upon the user id aSettingId
// ---------------------------------------------------------------------------
//
EXPORT_C CAknSettingItem* CIRCommonSettingsContainer::CreateSettingItemL( TInt aSettingId )
    {
    IRLOG_DEBUG( "CIRCommonSettingsContainer::CreateSettingItemL - Entering" );
    CAknSettingItem* item = NULL;
 
    switch ( aSettingId )
        {
        case EIRSettingItemSnapSelectionID:
			{
			iAccessPointIndex = 0;
			// Need to use this class later
			item = new (ELeave) CAknEnumeratedTextPopupSettingItem(aSettingId, 
													iAccessPointIndex);
			}
        break;
            
        case EIRSettingItemGprsConnBitRateID:
			{
			iGprsBitRateIndex = 0;
			item = new (ELeave) CAknEnumeratedTextPopupSettingItem(aSettingId, 
															iGprsBitRateIndex);
			iGprsBitRateIndex = iIRSettings->GetGPRSBitrateQuality();
			}
		break;
	           
        case EIRSettingItem3GConnBitRateID:
			{
			i3GBitRateIndex = 0;
			item = new (ELeave) CAknEnumeratedTextPopupSettingItem(aSettingId, 
															i3GBitRateIndex);                
			i3GBitRateIndex = iIRSettings->Get3GBitrateQuality();
			}
        break;
               
        case EIRSettingItemWiFiConnBitRateID:
			{
			iWifiBitRateIndex = 0;
			item = new (ELeave) CAknEnumeratedTextPopupSettingItem(aSettingId, 
														iWifiBitRateIndex);                
			iWifiBitRateIndex = iIRSettings->GetWiFiBitrateQuality();
			}
        break;
        
        default:
            break;
        }
     
    LoadSettingsL();
    IRLOG_DEBUG( "CIRCommonSettingsContainer::CreateSettingItemL - Exiting" );
    return item;

    }

// ---------------------------------------------------------------------------
// EditItemL()
// Called when an item is being edited.
// ---------------------------------------------------------------------------
//

EXPORT_C void CIRCommonSettingsContainer::EditItemL( TInt aIndex, TBool aCalledFromMenu )
    {
    IRLOG_DEBUG( "CIRCommonSettingsContainer::EditItemL - Entering" );
	if((aIndex >= 0) && (aIndex < SettingItemArray()->Count()))
    {
	    TInt settingId = SettingItemArray()->At( aIndex )->Identifier();
	    
	    switch ( settingId )
	        {
	        case EIRSettingItemGprsConnBitRateID:
	        	{
	           	CAknSettingItemList::EditItemL( aIndex, aCalledFromMenu );
	        	SettingItemArray()->At( aIndex )->StoreL();
            iIRSettings->SetGPRSBitrateQualityL(iGprsBitRateIndex);
	            break;
	        	}
	        case EIRSettingItem3GConnBitRateID:
	        	{
	        	CAknSettingItemList::EditItemL( aIndex, aCalledFromMenu );
	  	   		SettingItemArray()->At( aIndex )->StoreL();
            iIRSettings->Set3GBitrateQualityL(i3GBitRateIndex);
	            break;
	        	}
	        case EIRSettingItemWiFiConnBitRateID:
	        	{
	        	CAknSettingItemList::EditItemL( aIndex, aCalledFromMenu );
	  			SettingItemArray()->At( aIndex )->StoreL();
            iIRSettings->SetWiFiBitrateQualityL(iWifiBitRateIndex);
	            break;
	        	}
	        case EIRSettingItemSnapSelectionID:
	        	{
	           	ShowConnectionSettingsUiL();
	        	break;
	        	}
	        default:
	            break;
	        }
	    IRLOG_DEBUG( "CIRCommonSettingsContainer::EditItemL - Exiting" );
	    }
    }

// ---------------------------------------------------------------------------
// Gets the help context.
// ---------------------------------------------------------------------------
//
EXPORT_C void CIRCommonSettingsContainer::GetHelpContext( TCoeHelpContext& aContext ) const
    {
    IRLOG_DEBUG( "CIRCommonSettingsContainer::GetHelpContext - Entering" );
    aContext = TCoeHelpContext( KIRMCVUid, KIR_HELP_SETTINGS );
    IRLOG_DEBUG( "CIRCommonSettingsContainer::GetHelpContext - Exiting" );
    }
  
// -----------------------------------------------------------------------------
// CIRCommonSettingsContainer::SizeChanged
// -----------------------------------------------------------------------------
//
EXPORT_C void CIRCommonSettingsContainer::SizeChanged()
    {
    IRLOG_DEBUG( "CIRCommonSettingsContainer::SizeChanged - Entering" );
    CAknSettingItemList::SizeChanged();
    CEikListBox* lb = ListBox();
    if( lb )
        {
        lb->SetRect( Rect() );  // Needed to update listbox after portrait-landscape change
        }
    IRLOG_DEBUG( "CIRCommonSettingsContainer::SizeChanged - Exiting" );
    }


// ---------------------------------------------------------------------------
// ShowConnectionSettingsUiL
// Displays the connection settings ui
// ---------------------------------------------------------------------------
//
void CIRCommonSettingsContainer::ShowConnectionSettingsUiL()
	{
	IRLOG_DEBUG( "CIRCommonSettingsContainer::ShowConnectionSettingsUiL - Entering" );
		SetListBoxTextL();
		IRLOG_DEBUG( "CIRCommonSettingsContainer::ShowConnectionSettingsUiL - Exiting" );
	}




    
// ---------------------------------------------------------------------------
// UpdateStatusL()
// Fetches the new settings values from iIRSettings and updates the list
// ---------------------------------------------------------------------------
//
void CIRCommonSettingsContainer::UpdateStatusL()
	{	
	IRLOG_DEBUG( "CIRCommonSettingsContainer::UpdateStatusL - Entering" );
	iGprsBitRateIndex = iIRSettings->GetGPRSBitrateQuality();
    i3GBitRateIndex = iIRSettings->Get3GBitrateQuality();
    iWifiBitRateIndex = iIRSettings->GetWiFiBitrateQuality();
    LoadSettingsL();
    SetListBoxTextL();
    IRLOG_DEBUG( "CIRCommonSettingsContainer::UpdateStatusL - Exiting" );
	}



// ---------------------------------------------------------------------------
// SetAccessPointDetailsL()
// Sets the details of the chosen IAP in cenrep
// ---------------------------------------------------------------------------
//
void CIRCommonSettingsContainer::SetAccessPointDetailsL(TUint aId)
	{
	IRLOG_DEBUG( "CIRCommonSettingsContainer::SetAccessPointDetailsL - Entering" );
	//Get All the bearer IDs(RArray)
	const RArray<TUint32>& bearerIDArray = iNetworkController->GetBearerList();
	const RArray<TUint32>& networkIDArray = iNetworkController->GetNetworkList();
	const RArray<TUint32>& accessPointIDArray = iNetworkController->GetApList();
	
	TInt index =0;
	//Get access point index, that matches the access point stored in 
	// settings previously
	for(TInt i=0; i<accessPointIDArray.Count(); i++ )
		{
		if(accessPointIDArray[i] == aId )	
			{
			index = i;	
			}
		}	

	TUint32 bearerId = bearerIDArray[index];
	TUint32 networkId = networkIDArray[index];
	TUint32 accessPointId = accessPointIDArray[index];

	IRLOG_DEBUG( "CIRCommonSettingsContainer::SetAccessPointDetailsL - Exiting" );
	}
	
// ---------------------------------------------------------------------------
// SetListBoxTextL()
// Sets the text of the Network Selection in the Settings UI
// ---------------------------------------------------------------------------
//	
void CIRCommonSettingsContainer::SetListBoxTextL()
	{
	IRLOG_DEBUG( "CIRCommonSettingsContainer::SetListBoxTextL - Entering" );

	IRLOG_DEBUG( "CIRCommonSettingsContainer::SetListBoxTextL - Exiting" );
	}



// ======== GLOBAL FUNCTIONS ========

