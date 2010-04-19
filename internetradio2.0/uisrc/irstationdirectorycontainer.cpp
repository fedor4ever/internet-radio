/*
* Copyright (c)  Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Station Directory container.
*
*/

/* ---------------------------------------------------------------------------
*  Version history:
*  Template version:
*  <ccm_history>
*
*  Version: 2, Fri March 21 17:20:00 2008 by Prabina
*  Ref:
*  Added WorkAround for supporting different Resolutions , Added Wait Dialog for Loading of stations , 
*  Handled Single/Double tapping on the list itmes
*
*  </ccm_history>
* ============================================================================
*/
/* ---------------------------------------------------------------------------
*  Version history:
*  Template version:
*  <ccm_history>
*
*  Version: 3, Tues March 25 17:30:00 2008 by Prabina
*  Ref:
*  Fixing the PCLint errors
*
*
*  </ccm_history>
* ============================================================================
*/
/* ---------------------------------------------------------------------------
*  Version history:
*  Template version:
*  <ccm_history>
*
*  Version: 4, Tues April 04 16:40:00 2008 by Prabina
*  Ref:
*  Added WorkAround for retaining of focus & Page retaining
*
*
*  </ccm_history>
* ============================================================================
*/
/* ---------------------------------------------------------------------------
*  Version history:
*  Template version:
*  <ccm_history>
*
*  Version: 5, Thurs July 18 12:00:00 2008 by Prabina
*  Ref:
*  Code Review Comments Fixed
*
*
*  </ccm_history>
* ============================================================================
*/
// SYSTEM INCLUDE FILES
#include <aknlists.h>
#include <aknview.h>
#include <barsread.h>
#include <eikenv.h>
#include <eiklabel.h>
#include <eikmenub.h>
#include <stringloader.h>
#include <internetradio.rsg>
#include <eikenv.h>
#include <eikfrlbd.h>


// INCLUDE FILES
#include "irbaseview.h"
#include "iraap.hlp.hrh"
#include "irui.h"
#include "irstationdirectorycontainer.h"
#include "irstationdirectoryview.h"
#include "irdocument.h"
#include "irdebug.h"
#include "ircommon.h"
#include "ir.hrh"

// LITERALS
const TInt KMenubufMaxLength = 522;
#define KCONTROLSCOUNT 0

// ==================== LOCAL FUNCTIONS ====================

// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// CIRStationDirectoryContainer::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CIRStationDirectoryContainer* CIRStationDirectoryContainer::NewL( 
			CIRStationDirectoryView& aStationsDirView, const TRect& aRect )
    {
 	IRLOG_DEBUG( "CIRStationDirectoryContainer::NewL" );
    CIRStationDirectoryContainer* self = CIRStationDirectoryContainer::NewLC(
    				 aStationsDirView, aRect );
    CleanupStack::Pop( self );
    IRLOG_DEBUG( "CIRStationDirectoryContainer::NewL - Exiting" );
    return self;
    }

// -----------------------------------------------------------------------------
// CIRStationDirectoryContainer::NewLC()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CIRStationDirectoryContainer* CIRStationDirectoryContainer::NewLC( 
			CIRStationDirectoryView& aStationsDirView, const TRect& aRect )
    {
 	IRLOG_DEBUG( "CIRStationDirectoryContainer::NewLC" );
    CIRStationDirectoryContainer* self = new ( ELeave ) CIRStationDirectoryContainer(
    												aStationsDirView);
    CleanupStack::PushL( self );
    self->ConstructL( aRect );
    IRLOG_DEBUG( "CIRStationDirectoryContainer::NewLC - Exiting" );
    return self;
    }
// ---------------------------------------------------------------------------
// C++ default constructor can NOT contain any code, that might leave.
// ---------------------------------------------------------------------------
//
CIRStationDirectoryContainer::CIRStationDirectoryContainer(
					CIRStationDirectoryView& aStationsDirView ):iStationDirView(aStationsDirView)
    {
	IRLOG_DEBUG( "CIRStationDirectoryContainer::CIRStationDirectoryContainer" );
	}

// ---------------------------------------------------------------------------
// CIRStationDirectoryContainer::ConstructL
// ---------------------------------------------------------------------------
//
void CIRStationDirectoryContainer::ConstructL( const TRect& aRect )
    {
	IRLOG_DEBUG( "CIRStationDirectoryContainer::ConstructL - Entering" );
    CreateWindowL();
    
    iListbox = new(ELeave) CAknDoubleStyleListBox;
    iListbox->SetContainerWindowL(*this);
    iListbox->ConstructL(this,EAknListBoxSelectionList);

	iItemArray = static_cast<CDesCArray*>( iListbox->Model()->ItemTextArray());
    PopulateStationListL();

    iListbox->SetCurrentItemIndex(iStationDirView.GetCurrentFocus());
	iListbox->HandleItemAdditionL( );

    
    SetRect( aRect );
    DrawNow();
       
    iListbox->CreateScrollBarFrameL( ETrue );
    iListbox->ScrollBarFrame()->SetScrollBarVisibilityL(CEikScrollBarFrame::EOff, 
                                              CEikScrollBarFrame::EAuto);
    
    iListbox->SetListBoxObserver( this );  
	IRLOG_DEBUG( "CIRStationDirectoryContainer::ConstructL - Exiting" );
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CIRStationDirectoryContainer::~CIRStationDirectoryContainer()
    {
	IRLOG_DEBUG( "CIRStationDirectoryContainer::~CIRStationDirectoryContainer - Entering" );
	if(iListbox)
		{
		delete iListbox;
		iListbox = NULL;
		}
	IRLOG_DEBUG( "CIRStationDirectoryContainer::~CIRStationDirectoryContainer - Exiting" );
    }
//----------------------------------------------------------------------------
// CIRStationDirectoryContainer::CountComponentControls() const
// returns the number of components in the view.
//----------------------------------------------------------------------------
//
TInt CIRStationDirectoryContainer::CountComponentControls() const
	{
	IRLOG_DEBUG( "CIRStationDirectoryContainer::CountComponentControls - Entering" );
	TInt count(KCONTROLSCOUNT);
	if(iListbox)
		{
		count++;
		}
	IRLOG_DEBUG( "CIRStationDirectoryContainer::CountComponentControls - Exiting" );
	return count;
	}
//----------------------------------------------------------------------------
// CIRStationDirectoryContainer::ComponentControl() const
// returns a pointer to the control under this view depending on the index
// passed,to the framework.
//----------------------------------------------------------------------------
// 
CCoeControl* CIRStationDirectoryContainer::ComponentControl(TInt aIndex) const
	{
	IRLOG_DEBUG( "CIRStationDirectoryContainer::ComponentControl - Entering" );
	IRLOG_DEBUG( "CIRStationDirectoryContainer::ComponentControl - Exiting" );
	switch(aIndex)
		{
		case 0:
			{
			if(iListbox)
			return iListbox;
			}
		default:
			return NULL;
		}
	}
// ---------------------------------------------------------------------------
// From class CCoeControl.
// We need to catch the selection key event in order to do a couple of things 
// in moving state make the movement, and in normal state open channel 
// specific popup menu. Up and down buttons are forwarded to the list.
// ---------------------------------------------------------------------------
//
TKeyResponse CIRStationDirectoryContainer::OfferKeyEventL( const TKeyEvent& aKeyEvent,
					 TEventCode aType )
	{
	IRLOG_DEBUG( "CIRStationDirectoryContainer::OfferKeyEventL - Entering" );
	if((aType)==EEventKey)
		{
		if((aKeyEvent.iCode)==EKeyDevice3)
			{
			iStationDirView.SetCurrentFocus(iListbox->CurrentItemIndex());
			iStationDirView.HandleCommandL(ESelectCmd);
			return EKeyWasConsumed;
			}
		else
			{
			return iListbox->OfferKeyEventL(aKeyEvent, aType );
			}
		}
	IRLOG_DEBUG( "CIRStationDirectoryContainer::OfferKeyEventL - Exiting" );
	return EKeyWasNotConsumed;
	}

// ---------------------------------------------------------------------------
// From class CCoeControl.
// Opens help by context.
// ---------------------------------------------------------------------------
//
void CIRStationDirectoryContainer::GetHelpContext( TCoeHelpContext& aContext) const
    {
	IRLOG_DEBUG( "CIRStationDirectoryContainer::GetHelpContext - Entering" );
    aContext = TCoeHelpContext( KIRMCVUid, KIR_HELP_DISCOVER  );
	IRLOG_DEBUG( "CIRStationDirectoryContainer::GetHelpContext - Exiting" );
    }
// ---------------------------------------------------------------------------
// From class MEikListBoxObserver.
// List box event handler.
// ---------------------------------------------------------------------------
//
void CIRStationDirectoryContainer::HandleListBoxEventL( CEikListBox* /*aListBox*/, TListBoxEvent aEventType )
	{
	IRLOG_DEBUG( "CIRStationDirectoryContainer::HandleListBoxEventL - Entering" );
	switch ( aEventType )
		{
		case EEventEnterKeyPressed: // Keypress event
		case EEventItemDoubleClicked:
			{
			iStationDirView.SetCurrentFocus(iListbox->CurrentItemIndex());
			iStationDirView.HandleCommandL(ESelectCmd);
			}
		break;
		default:
		break;	
		}
	IRLOG_DEBUG( "CIRStationDirectoryContainer::HandleListBoxEventL - Exiting" );
	}

// ---------------------------------------------------------------------------
// Polulates listbox
// ---------------------------------------------------------------------------
//
void CIRStationDirectoryContainer::PopulateStationListL( )
    {
	IRLOG_DEBUG( "CIRStationDirectoryContainer::PopulateStationListL - Entering" );
    if(!(iItemArray->Count()))
	    {
	    //Fetches the data to be displayed in this view from resource.
		CreateDiscoverStationDataL(R_IRAPP_STAT_DIR_GENRE,R_IRAPP_STAT_DIR_GENRE_EXPAND);
		CreateDiscoverStationDataL(R_IRAPP_STAT_DIR_LANGUAGE,R_IRAPP_STAT_DIR_LANGUAGE_EXPAND);
		CreateDiscoverStationDataL(R_IRAPP_STAT_DIR_COUNTRY,R_IRAPP_STAT_DIR_COUNTRY_EXPAND);
		CreateDiscoverStationDataL(R_IRAPP_STAT_DIR_TOP,R_IRAPP_STAT_DIR_TOP_EXPAND);
	    }
	IRLOG_DEBUG( "CIRStationDirectoryContainer::PopulateStationListL - Exiting" );
    }
    
    
// ---------------------------------------------------------------------------
// CreateDiscoverStationDataL(TInt aTitle,TInt aDescription)
// Fetches the data from stringloader that should be displayed
// and sets it to the listbox.
// ---------------------------------------------------------------------------
//
void CIRStationDirectoryContainer::CreateDiscoverStationDataL(TInt aTitle,TInt aDescription)
	{
	IRLOG_DEBUG( "CIRStationDirectoryContainer::CreateDiscoverStationDataL - Entering" );
	_LIT(KTabSpace,"\t");
	
	HBufC *titleData = StringLoader::LoadLC(aTitle);
    HBufC *descriptionData = StringLoader::LoadLC(aDescription);
    TBuf<KMenubufMaxLength> menubuf;
   	menubuf.Append(KTabSpace);
  	menubuf.Append(*titleData);
  	menubuf.Append(KTabSpace);
  	menubuf.Append(*descriptionData);
  	iItemArray->AppendL(menubuf);
  	CleanupStack::PopAndDestroy(descriptionData);
	CleanupStack::PopAndDestroy(titleData);
	IRLOG_DEBUG( "CIRStationDirectoryContainer::CreateDiscoverStationDataL - Exiting" );
	}
    
// ---------------------------------------------------------------------------
// CIRSearchResultsViewContainer::Draw()
// From class CCoeControl.
// Draws the control.
// ---------------------------------------------------------------------------
//
void CIRStationDirectoryContainer::Draw( const TRect& /*aRect*/ ) const
    {
	IRLOG_DEBUG( "CIRStationDirectoryContainer::Draw - Entering" );
	
    CWindowGc& gc = SystemGc();
    gc.SetPenStyle( CGraphicsContext::ENullPen );
    gc.SetBrushColor( KRgbBlue );
    gc.SetBrushStyle( CGraphicsContext::ESolidBrush );
    
	IRLOG_DEBUG( "CIRStationDirectoryContainer::Draw - Exiting" );
    }

// ---------------------------------------------------------------------------
// CIRStationDirectoryContainer::GetCurrentItemIndex()
// returns the index of the currently selected item
// ---------------------------------------------------------------------------
//

TInt CIRStationDirectoryContainer::GetCurrentItemIndex() const
    {
    IRLOG_DEBUG( "CIRStationDirectoryContainer::GetCurrentItemIndex" );
	return iListbox->CurrentItemIndex();
    }
    
// -----------------------------------------------------------------------------
// CIRStationDirectoryContainer::SizeChanged()
// Called by framework when the view size is changed.
// -----------------------------------------------------------------------------
//
void CIRStationDirectoryContainer::SizeChanged()
    {  
    iListbox->SetRect(Rect());
    }

// ---------------------------------------------------------------------------
// void CIRStationDirectoryContainer::HandleResourceChange(TInt aType)
// Handles a change to the control's resources.
// ---------------------------------------------------------------------------
//
void CIRStationDirectoryContainer::HandleResourceChange(TInt aType)
	{
	IRRDEBUG( "CIRStationDirectoryContainer::HandleResourceChange - Entering" );
    CCoeControl::HandleResourceChange( aType );
	iStationDirView.RefreshView();
	IRRDEBUG( "CIRStationDirectoryContainer::HandleResourceChange - Exiting" );
	}
