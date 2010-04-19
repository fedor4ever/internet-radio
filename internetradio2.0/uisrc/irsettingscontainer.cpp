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
* Description:  
*
*/


#include <stringloader.h>
#include <internetradio.rsg>
#include <akntitle.h> 

#include "irsettingscontainer.h"
#include "irsettingsview.h"
#include "irdebug.h" //  PC-Lint comments :: SPP


// ======== LOCAL FUNCTIONS ========

// ======== MEMBER FUNCTIONS ========

// -----------------------------------------------------------------------------
// CIRSettingsContainer::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CIRSettingsContainer* CIRSettingsContainer::NewL(CIRSettingsView& aSettingsView)
    {
 	IRLOG_DEBUG( "CIRSettingsContainer::NewL" );
 	CIRSettingsContainer* self = CIRSettingsContainer::NewLC(aSettingsView);
    CleanupStack::Pop( self );
    IRLOG_DEBUG( "CIRSettingsContainer::NewL - Exiting" );
    return self;
    }
// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CIRSettingsContainer* CIRSettingsContainer::NewLC(CIRSettingsView& aSettingsView )
    {
    	IRLOG_DEBUG( "CIRSettingsView::NewLC - Entering" );
    	CIRSettingsContainer* self = new ( ELeave ) CIRSettingsContainer(aSettingsView);
		CleanupStack::PushL( self );
		self->ConstructL();
		IRLOG_DEBUG( "CIRSettingsContainer::NewLC - Exiting" );
		return self;
	
    }

// ---------------------------------------------------------------------------
// Constructor.
// ---------------------------------------------------------------------------
//
CIRSettingsContainer::CIRSettingsContainer( CIRSettingsView& aSettingsView ) 
    : CIRCommonSettingsContainer(), iSettingsView(aSettingsView)
    {
    IRLOG_DEBUG( "CIRSettingsContainer::CIRSettingsContainer" );
    }

// ---------------------------------------------------------------------------
// Second-phase constructor.
// ---------------------------------------------------------------------------
//
void CIRSettingsContainer::ConstructL()
    {
    	IRLOG_DEBUG( "CIRSettingsContainer::ConstructL - Entering." );
    	BaseConstructL();
    	IRLOG_DEBUG( "CIRSettingsContainer::ConstructL - Exiting." );
    }

// ---------------------------------------------------------------------------
// Destructor.
// ---------------------------------------------------------------------------
//
CIRSettingsContainer::~CIRSettingsContainer()
    {
    IRLOG_DEBUG( "CIRSettingsContainer::~CIRSettingsContainer" );
    }

// ---------------------------------------------------------------------------
// From CAknSettingItemList.
// ---------------------------------------------------------------------------
//
CAknSettingItem* CIRSettingsContainer::CreateSettingItemL( TInt aSettingId )
    {
    	IRLOG_DEBUG( "CIRSettingsContainer::CreateSettingItemL - Entering." );
		CAknSettingItem* item = CIRCommonSettingsContainer::CreateSettingItemL( aSettingId );
		IRLOG_DEBUG( "CIRSettingsContainer::CreateSettingItemL - Exiting." );
		return item;

    }
    
// ---------------------------------------------------------
// CIRSettingsContainer::OfferKeyEventL
// ---------------------------------------------------------
//
TKeyResponse CIRSettingsContainer::OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType)
    {
    	IRLOG_DEBUG( "CIRSettingsContainer::OfferKeyEventL - Entering." );
		TKeyResponse keyResp(EKeyWasNotConsumed);
		
		switch (aKeyEvent.iCode)
			{
			case EKeyIncVolume:
			case EKeyDecVolume:
			case EKeyLeftArrow:
			case EKeyRightArrow:
				{
				// Don't consume the volume keys
				keyResp = EKeyWasNotConsumed;
				break;
				}
			default:
				{
				keyResp = CAknSettingItemList::OfferKeyEventL(aKeyEvent, aType);
				break;
				}
		}
		IRLOG_DEBUG( "CIRSettingsContainer::OfferKeyEventL - Exiting." );
		return keyResp; 
    }
    
// ---------------------------------------------------------------------------
// Called when an item is being edited.
// ---------------------------------------------------------------------------
//
void CIRSettingsContainer::EditItemL( TInt aIndex, TBool aCalledFromMenu )
    {
    IRLOG_DEBUG( "CIRSettingsContainer::EditItemL - Entering." );
		
		CIRCommonSettingsContainer::EditItemL( aIndex, aCalledFromMenu );
		IRLOG_DEBUG( "CIRSettingsContainer::EditItemL - Entering." );
			
    }

// ---------------------------------------------------------------------------
// Sets window faded
// ---------------------------------------------------------------------------
//
void CIRSettingsContainer::SetFaded(TBool aFaded) const
    {
    IRLOG_DEBUG( "CIRSettingsContainer::SetFaded - Entering." );
    Window().SetFaded(aFaded, RWindowTreeNode::EFadeIncludeChildren);
    IRLOG_DEBUG( "CIRSettingsContainer::SetFaded - Exiting." );
    }

// ======== GLOBAL FUNCTIONS ========

