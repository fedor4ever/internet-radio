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
#include <internetradio.rsg>
#include "irnetworkcontroller.h"
#include "iraap.hlp.hrh"
#include "irdebug.h" //  PC-Lint comments :: SPP

#define KUIDIRAPP 0x2000B499
const TUid KIRMCVUid = {KUIDIRAPP};

const TInt KBufSize = 256;

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
    ConstructFromResourceL( R_IR_SETTING_LIST );
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
	TCmSettingSelection userSelection;
	userSelection.iResult = GetUserSelectionL() ;
	if(userSelection.iResult == EDestination)
		{
		userSelection.iId = iIRSettings->GetDestinationIdL();	
		}
	else if(userSelection.iResult == EConnectionMethod)
		{
		userSelection.iId = iIRSettings->GetApIdL();	
		}
	else
		{
		userSelection.iId =0;	
		}

	CCmApplicationSettingsUi* settings = CCmApplicationSettingsUi::NewL();
	CleanupStack::PushL( settings );

	TUint listedItems = EShowAlwaysAsk | EShowDefaultConnection |
						EShowDestinations | EShowConnectionMethods;

	TBearerFilterArray filter;
	TBool selected = EFalse;

	TRAPD( settingsErr, selected = settings->RunApplicationSettingsL( userSelection, 
																		listedItems, filter ) );

	CleanupStack::PopAndDestroy( settings );

	if((selected == EFalse) || (settingsErr != KErrNone) )
		{
		return;
		}

	switch ( userSelection.iResult )
		{
		case EAlwaysAsk:
			{
			iIRSettings->SetUserDefinedSelectionL(EUserSelectionAlwaysAsk);
			iIRSettings->SetDisplayAccessPointL();
			break;
			}
		case EDefaultConnection:
			{
			iIRSettings->SetUserDefinedSelectionL(EUserSelectionDefaultConnection);
			break;
			} 
		
		case EDestination:
			{
			iIRSettings->SetUserDefinedSelectionL(EUserSelectionDestination);
			iIRSettings->SetDestinationIdL(userSelection.iId);
			iDestinationSelected = ETrue;
			break;
			}
			
		case EConnectionMethod:
			{
			iIRSettings->SetUserDefinedSelectionL(EUserSelectionConnectionMethod);
			SetAccessPointDetailsL(userSelection.iId);
			iDestinationSelected = EFalse;
			break;
			}
		default:
			{
			break;
			} 
		}
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
// GetUserSelectionL()
// Queries the Settings and returns the user selected IAP/SNAP
// ---------------------------------------------------------------------------
//
TCmSettingSelectionMode CIRCommonSettingsContainer::GetUserSelectionL()
	{
	IRLOG_DEBUG( "CIRCommonSettingsContainer::GetUserSelectionL - Entering" );
	TUint32 userSelection;
	userSelection = iIRSettings->GetUserDefinedSelectionL();
	TCmSettingSelectionMode ret = EAlwaysAsk;	
	switch(userSelection)
		{
		case EUserSelectionAlwaysAsk:
			ret = EAlwaysAsk;
			break;
			
		case EUserSelectionDefaultConnection:
			ret = EDefaultConnection ;
			break;
			
		case EUserSelectionDestination:
			ret = EDestination;
			break;
			
		case EUserSelectionConnectionMethod:
			ret = EConnectionMethod;
			break;
			
		default:
			ret = EAlwaysAsk;
			break;	
		
		}
	    IRLOG_DEBUG( "CIRCommonSettingsContainer::GetUserSelectionL - Exiting" );
		return ret;
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

	//This stores the settings in Central Repository
	iIRSettings->SetUserSavedApSettingsL(bearerId, networkId,accessPointId);

	// Save the settings for use by IRNetworkContorller			
	iIRSettings->SetBearerIdL(bearerId);
	iIRSettings->SetNetworkIdL(networkId);
	iIRSettings->SetApIdL(accessPointId);
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
	if(SettingItemArray()->Count()>0)
		    {
		    switch(GetUserSelectionL())
				{
				case EAlwaysAsk:
					{
					HBufC* stringholder = StringLoader::LoadLC( R_IR_ACCESS_ASK);
					SettingItemArray()->At( 0 )->SetEmptyItemTextL(*stringholder);
					CleanupStack::PopAndDestroy(stringholder);
					HandleChangeInItemArrayOrVisibilityL();
					ActivateL();		
					break;	
					}
					
				case EDefaultConnection:
					{
					HBufC* stringholder = StringLoader::LoadLC( R_IR_SETTINGS_SNAP_DEFAULT);
					SettingItemArray()->At( 0 )->SetEmptyItemTextL(*stringholder);
					CleanupStack::PopAndDestroy(stringholder);
					HandleChangeInItemArrayOrVisibilityL();
					ActivateL();
					break;
					}
					
				case EDestination:
					{
					RCmManager cmManager;
					cmManager.OpenL();
			TRAPD(err, RCmDestination dest = cmManager.DestinationL( iIRSettings->GetDestinationIdL() ));
		    		if (err == KErrNone)
			    		{
				RCmDestination dest = cmManager.DestinationL(iIRSettings->GetDestinationIdL());
						CleanupClosePushL( dest );
					
						HBufC* destName = dest.NameLC();
						TBuf<KBufSize> item;
						_LIT( KStringFormat, "%S" );
						item.Format( KStringFormat,destName );
						
						SettingItemArray()->At( 0 )->SetEmptyItemTextL(item);
						HandleChangeInItemArrayOrVisibilityL();
						ActivateL();
						CleanupStack::PopAndDestroy( destName ); // destName, dest
						CleanupStack::PopAndDestroy( &dest);
			    		}
			    	cmManager.Close();
					break;
					}
				
				case EConnectionMethod:
					{
					if(!iDestinationSelected)
						{
						iIapArray = iNetworkController->GetAccessPointList();
						const RArray<TUint32>& accessPointIDArray = iNetworkController->GetApList();
		
						TInt index = 0;
						//Get access point index, that matches the access point stored in 
						// settings previously
						for(TInt i=0; i<accessPointIDArray.Count(); i++ )
							{
					if(accessPointIDArray[i] == iIRSettings->GetApIdL())	
								{
								index = i;	
								}
							}		
						TPtrC list;
						list.Set(iIapArray->MdcaPoint(index));
						HBufC* itemText = HBufC::NewLC(list.Length());
						itemText->Des().Copy(list);	
								
						SettingItemArray()->At( 0 )->SetEmptyItemTextL(list);
						HandleChangeInItemArrayOrVisibilityL();
						ActivateL();
						CleanupStack::PopAndDestroy(itemText);	
						}
			
					break;
					}
				default:
					break;
				
				} 
		    }
	IRLOG_DEBUG( "CIRCommonSettingsContainer::SetListBoxTextL - Exiting" );
	}



// ======== GLOBAL FUNCTIONS ========

