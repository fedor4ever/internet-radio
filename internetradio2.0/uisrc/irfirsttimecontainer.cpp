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
* Description:  Find stations container.
*
*/


#include <aknlists.h>
#include <eikfrlbd.h> 

#include <barsread.h>
#include <eikenv.h>
#include <eiklabel.h>
#include <eikmenub.h>

#include <stringloader.h>
#include <internetradio.rsg>
#include <eikenv.h>
#include "irbaseview.h"


#include <eikfrlbd.h>


#include <internetradio.rsg>
#include "irui.h"
#include "ir.hrh"
#include "ircommon.h"
#include "irdebug.h"
#include "irfirsttimeview.h"
#include "irfirsttimecontainer.h"

#include "iraap.hlp.hrh"


// The granularity of the scan stations list box.

const TInt KMenubufMaxLength = 522;
const TInt KStringSize = 20;
#define KCONTROLSCOUNT 0

// ---------------------------------------------------------------------------
// C++ default constructor can NOT contain any code, that might leave.
// ---------------------------------------------------------------------------
//
CIRFirstTimeContainer::CIRFirstTimeContainer(CIRFirstTimeView& aFirstTimeView):
                                             iFirstTimeView(aFirstTimeView)
    {
    IRLOG_DEBUG( "CIRFirstTimeContainer::CIRFirstTimeContainer" );
    }

// ---------------------------------------------------------------------------
// CIRFirstTimeContainer::ConstructL
// ---------------------------------------------------------------------------
//
void CIRFirstTimeContainer::ConstructL( const TRect& aRect )
    {
    IRLOG_DEBUG( "CIRFirstTimeContainer::ConstructL - Entering" );
    CreateWindowL();
    iListbox = new(ELeave) CAknDoubleStyleListBox;
    iListbox->SetContainerWindowL(*this);
    iListbox->ConstructL(this,EAknListBoxSelectionList);

	iItemArray = static_cast<CDesCArray*>( iListbox->Model()->ItemTextArray());
    PopulateFindStationListL();

			
    iListbox->SetCurrentItemIndex(iFirstTimeView.GetCurrentFocus());
	iListbox->HandleItemAdditionL( );

    
    SetRect( aRect );
    DrawNow();
       
    iListbox->CreateScrollBarFrameL( ETrue );
    iListbox->ScrollBarFrame()->SetScrollBarVisibilityL(CEikScrollBarFrame::EOff, 
                                              CEikScrollBarFrame::EAuto);
    
    iListbox->SetListBoxObserver( this );  
	IRLOG_DEBUG( "CIRFirstTimeContainer::ConstructL - Exiting" );
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CIRFirstTimeContainer::~CIRFirstTimeContainer()
    {
    IRLOG_DEBUG( "CIRFirstTimeContainer::~CIRFirstTimeContainer - Entering" );
	if(iListbox)
		{
		delete iListbox;
		iListbox = NULL;
		}
    IRLOG_DEBUG( "CIRFirstTimeContainer::~CIRFirstTimeContainer - Exiting" );
    }

//----------------------------------------------------------------------------
// CIRFirstTimeContainer::CountComponentControls() const
// returns the number of components in the view.
//----------------------------------------------------------------------------
//
TInt CIRFirstTimeContainer::CountComponentControls() const
	{
	IRLOG_DEBUG( "CIRFirstTimeContainer::CountComponentControls - Entering" );
	TInt count(KCONTROLSCOUNT);
	if(iListbox)
		{
		count++;
		}
	IRLOG_DEBUG( "CIRFirstTimeContainer::CountComponentControls - Exiting" );
	return count;
	}
//----------------------------------------------------------------------------
// CIRFirstTimeContainer::ComponentControl() const
// returns a pointer to the control under this view depending on the index
// passed,to the framework.
//----------------------------------------------------------------------------
// 
CCoeControl* CIRFirstTimeContainer::ComponentControl(TInt aIndex) const
	{
	IRLOG_DEBUG( "CIRFirstTimeContainer::ComponentControl - Entering" );
	IRLOG_DEBUG( "CIRFirstTimeContainer::ComponentControl - Exiting" );
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
TKeyResponse CIRFirstTimeContainer::OfferKeyEventL( const TKeyEvent& aKeyEvent, TEventCode aType )
    {
    IRLOG_DEBUG( "CIRFirstTimeContainer::OfferKeyEventL" );
	return iListbox->OfferKeyEventL(aKeyEvent, aType );
    }

// ---------------------------------------------------------------------------
// From class CCoeControl.
// Opens help by context.
// ---------------------------------------------------------------------------
//
void CIRFirstTimeContainer::GetHelpContext( TCoeHelpContext& aContext ) const
    {
    IRLOG_DEBUG( "CIRFirstTimeContainer::GetHelpContext - Entering" );
	aContext = TCoeHelpContext( KIRMCVUid, KIR_HELP_RESULTS  );
	IRLOG_DEBUG( "CIRFirstTimeContainer::GetHelpContext - Exiting" );
    }

// ---------------------------------------------------------------------------
// From class MEikListBoxObserver.
// List box event handler.
// ---------------------------------------------------------------------------
//
void CIRFirstTimeContainer::HandleListBoxEventL( CEikListBox* /*aListBox*/, TListBoxEvent aEventType )
    {
    IRLOG_DEBUG( "CIRFirstTimeContainer::HandleListBoxEventL - Entering" );
    CIRUi* appUi = static_cast<CIRUi*>( iCoeEnv->AppUi( ) );
    
    switch ( aEventType )
        {
        case EEventEnterKeyPressed:
        	 /**  Keypress event. */
        case EEventItemDoubleClicked:
            /**  Item single-tap event */
            iFirstTimeView.SetCurrentFocus(iListbox->CurrentItemIndex());
            switch( iListbox->CurrentItemIndex( ) )
                {
                case 0:
					{
					TBuf8<2> dummy;
					TVwsViewId viewId(TUid::Uid(KUIDIRAPP),KIRStationDirectoryViewId);
					appUi->ActivateViewL(viewId ,TUid::Uid(1),dummy);
					}
                    break;
                case 1:
	                {
					if( !iFirstTimeView.iUi->CheckAndNotifyLowDiskSpaceL() )
						{
						HBufC *SearchTitle = StringLoader::LoadLC(R_IRAPP_SEARCH_STRING);
	                	_LIT(KNullText,"");
						RBuf string1;
						string1.Create(KStringSize);
						string1.Append(KNullText);
						iFirstTimeView.ShowQueryDialogL(*SearchTitle,string1);
						CleanupStack::PopAndDestroy(SearchTitle);
						string1.Close();
						}
	                }
                    break;
                case 2:
                	{
					if( !iFirstTimeView.iUi->CheckAndNotifyLowDiskSpaceL() )
						{
	                	appUi->SetEditStation(EFalse);
	                    appUi->ActivateLocalViewL(KIRAddManuallyStationViewID);
						}
                	}
                    break;
                default:
                    break;
                }
			/* fall through */ // pc-lint Warning -e616                
         default:
            break;
        }
    IRLOG_DEBUG( "CIRFirstTimeContainer::HandleListBoxEventL - Exiting" );
    }
    
    
// ---------------------------------------------------------------------------
// CIRSearchResultsViewContainer::Draw()
// From class CCoeControl.
// Draws the control.
// ---------------------------------------------------------------------------
//
void CIRFirstTimeContainer::Draw( const TRect& /*aRect*/ ) const
    {
    IRLOG_DEBUG( "CIRFirstTimeContainer::Draw - Entering" );
    CWindowGc& gc = SystemGc();
    gc.SetPenStyle( CGraphicsContext::ENullPen );
    gc.SetBrushColor( KRgbBlue );
    gc.SetBrushStyle( CGraphicsContext::ESolidBrush );
    IRLOG_DEBUG( "CIRFirstTimeContainer::Draw - Exiting" );
    }

// ---------------------------------------------------------------------------
// Populates listbox
// ---------------------------------------------------------------------------
//
void CIRFirstTimeContainer::PopulateFindStationListL( )
    {
    IRLOG_DEBUG( "CIRFirstTimeContainer::PopulateFindStationListL - Entering" );   
    //Fetches the data to be displayed in this view from resource.
    if(!(iItemArray->Count()))
	    {
		CreateFindStationDataL(R_IRAPP_FINDSTATION_DIRECTORY
			,R_IRAPP_MAIN_DISCOVER_EXPAND);
		CreateFindStationDataL(R_IRAPP_MAIN_MENU_SEARCH
			,R_IRAPP_FINDSTATION_SEARCH_EXPAND);
		CreateFindStationDataL(R_IRAPP_FINDSTATION_ADDSTATIONMANUALLY
			,R_IRAPP_FINDSTATION_ADDSTATIONMANUALLY_EXPAND);
	    }
	IRLOG_DEBUG( "CIRFirstTimeContainer::PopulateFindStationListL - Exiting" );
    }
    
    
// ---------------------------------------------------------------------------
// CreateDiscoverStationDataL(TInt aTitle,TInt aDescription)
// Fetches the data from stringloader that should be displayed
// and sets it to the listbox.
// ---------------------------------------------------------------------------
//
void CIRFirstTimeContainer::CreateFindStationDataL(TInt aTitle,TInt aDescription)
	{
	IRLOG_DEBUG( "CIRFirstTimeContainer::CreateFindStationDataL - Entering" );
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
	IRLOG_DEBUG( "CIRFirstTimeContainer::CreateFindStationDataL - Exiting" );
	}
// -----------------------------------------------------------------------------
// CIRFirstTimeContainer::SizeChanged()
// Called by framework when the view size is changed.
// -----------------------------------------------------------------------------
//
void CIRFirstTimeContainer::SizeChanged()
    {  
    iListbox->SetRect(Rect());
    }
// ---------------------------------------------------------------------------
// CIRFirstTimeContainer::GetCurrentItemIndex()
// returns the index of the currently selected item
// ---------------------------------------------------------------------------
//

TInt CIRFirstTimeContainer::GetCurrentItemIndex() const
    {
    IRLOG_DEBUG( "CIRStationDirectoryContainer::GetCurrentItemIndex" );
	return iListbox->CurrentItemIndex();
    }
// ---------------------------------------------------------------------------
// void CIRFirstTimeContainer::HandleResourceChange(TInt aType)
// Handles a change to the control's resources.
// ---------------------------------------------------------------------------
//
void CIRFirstTimeContainer::HandleResourceChange(TInt aType)
	{
	IRRDEBUG( "CIRFirstTimeContainer::HandleResourceChange - Entering" );
    CCoeControl::HandleResourceChange( aType );
	iFirstTimeView.RefreshView();
	IRRDEBUG( "CIRFirstTimeContainer::HandleResourceChange - Exiting" );
	}
// End of file


