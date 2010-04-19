/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Container class for Pls list container
*
*/


#include <barsread.h>
#include <internetradio.rsg>
#include <stringloader.h>

#include "irhistory.h"
#include "irisdspreset.h"
#include "irui.h"
#include "ir.hrh"
#include "ircommon.h"
#include "irdebug.h"
#include "irbaseview.h"
#include "irplsview.h"
#include "irplscontainer.h"
#include "irapplication.h"
#include "irfavoritesdb.h"
#include "irdialoglauncher.h"
#include "iraap.hlp.hrh" //for HELP
#include "irnowplayingwrapper.h"
#include "irsettings.h"     //Added to maintain the IfAnyFav variable in settings


const TInt KTwo = 2;
const TInt KChannelNameSize = 200;

#define KPLSLISTCOUNT  0

//-----------------------------------------------------------------------------
//  CIRPlsContainer::NewL
//  Creates a new instance of pls Container
//-----------------------------------------------------------------------------
//
CIRPlsContainer* CIRPlsContainer::NewL(const TRect& aRect ,CIRPlsView& aView)
    {
	IRLOG_DEBUG( "CIRPlsContainer::NewL - Entering" );
    CIRPlsContainer* self = CIRPlsContainer::NewLC(aRect,aView);
    CleanupStack::Pop(self);
	IRLOG_DEBUG( "CIRPlsContainer::NewL - Exiting." );
    return self;
    }

//-----------------------------------------------------------------------------
//  CIRPlsContainer::NewLC
//  Creates a new instance of pls Container and leaves it on the cleanup stack.
//-----------------------------------------------------------------------------
//
CIRPlsContainer* CIRPlsContainer::NewLC(const TRect& aRect ,CIRPlsView& aView)
    {
    IRLOG_DEBUG( "CIRPlsContainer::NewLC - Entering" );
    CIRPlsContainer* self = new (ELeave) CIRPlsContainer(aView);
    CleanupStack::PushL(self);
    self->ConstructL(aRect);
	IRLOG_DEBUG( "CIRPlsContainer::NewLC - Exiting." );
    return self;
    }
// ----------------------------------------------------------------------------
// CIRPlsContainer::CIRPlsContainer()
// C++ default constructor can NOT contain any code, that might leave.
// ----------------------------------------------------------------------------
//
CIRPlsContainer::CIRPlsContainer( CIRPlsView& aView ) 
        : iPlsView( aView )
    {
	IRLOG_DEBUG( "CIRPlsContainer::CIRPlsContainer" );
    }

// ----------------------------------------------------------------------------
// CIRPlsContainer::~CIRPlsContainer()
// Destructor.
// Destructs all the Label Instances.
// ----------------------------------------------------------------------------
//
CIRPlsContainer::~CIRPlsContainer()
	{
	IRLOG_DEBUG( "CIRPlsContainer::~CIRPlsContainer - Entering" );
	iPresetList.ResetAndDestroy();
	iIsPresetSaved.Close();
	iPresetList.Close();
	if( iPlsListBox )
		{
		delete iPlsListBox;
		iPlsListBox = NULL;
		}
	IRLOG_DEBUG( "CIRPlsContainer::~CIRPlsContainer - Exiting." );
	}

// ---------------------------------------------------------------------------
// CIRPlsContainer::ConstructL()
// ---------------------------------------------------------------------------
//
void CIRPlsContainer::ConstructL( const TRect& aRect )
    {
    IRLOG_DEBUG( "CIRPlsContainer::ConstructL - Entering" );
    CreateWindowL();
    SetRect( aRect );
    iPlsListBox = new (ELeave) CAknSingleStyleListBox;

    // Construct list.
    TResourceReader rr;
    iCoeEnv->CreateResourceReaderLC( rr, R_IR_CHANNEL_LIST );
    ConstructFromResourceL( rr );
    CleanupStack::PopAndDestroy();                // rr
    
	iPlsListBox->SetContainerWindowL(*this);
	iPlsListBox->ConstructL(this ,EAknListBoxSelectionList);
	
	iListArray = static_cast<CDesCArray*>(iPlsListBox->Model()->ItemTextArray() );

    iPlsListBox->CreateScrollBarFrameL( ETrue );
    iPlsListBox->ScrollBarFrame()->SetScrollBarVisibilityL(CEikScrollBarFrame::EOff, 
                                              CEikScrollBarFrame::EAuto);
	iPlsListBox->ItemDrawer()->ColumnData()->EnableMarqueeL(ETrue); 
    iPlsListBox->SetRect( Rect());
    iPlsListBox->ActivateL();
    iPlsListBox->SetListBoxObserver(this); 

    UpdateAllL();
	IRLOG_DEBUG( "CIRPlsContainer::ConstructL - Exiting" );
    }



// ---------------------------------------------------------------------------
// From class CCoeControl.
// We need to catch the selection key event in order to do a couple of things 
// in moving state make the movement, and in normal state open channel 
// specific popup menu. Up and down buttons are forwarded to the list.
// ---------------------------------------------------------------------------
//
TKeyResponse CIRPlsContainer::OfferKeyEventL( 
        const TKeyEvent& aKeyEvent, TEventCode aType )
    {
    IRLOG_DEBUG( "CIRPlsContainer::OfferKeyEventL - Entering" );
    TKeyResponse keyResp( EKeyWasNotConsumed );
                 
    switch ( aKeyEvent.iCode )
        {
        case EKeyLeftArrow: // fall-through intended here
        case EKeyRightArrow:
        case EKeyIncVolume:
        case EKeyDecVolume:
            {
            keyResp = EKeyWasNotConsumed;
            break;
            }
        case EKeyOK:        
            {
            iPlsView.SetCurrentFocus(iPlsListBox->CurrentItemIndex());
			iPlsView.HandleCommandL(EListenCmd);
            break;
            }
        case EKeyBackspace:        // "c"-key
            {
            // delete current channel with "c"-key
            keyResp = EKeyWasConsumed;
            break;
            }
        default:
            {
            iPlsListBox->OfferKeyEventL(aKeyEvent,aType);
            break;
            }
        }
    IRLOG_DEBUG( "CIRPlsContainer::OfferKeyEventL - Exiting" );   
    return keyResp; 
    }

void CIRPlsContainer::HandlePointerEventL( const TPointerEvent& aPointerEvent )
    {
    IRLOG_DEBUG( "CIRPlsContainer::HandlePointerEventL - Entering" );
    iPlsListBox->HandlePointerEventL( aPointerEvent );
    IRLOG_DEBUG( "CIRPlsContainer::HandlePointerEventL - Exiting" );
    }

// ---------------------------------------------------------------------------
// CIRPlsContainer::UpdateAllL() 
// Fills list with preset data read from data base.
// ---------------------------------------------------------------------------
//
void CIRPlsContainer::UpdateAllL() 
    {
    IRLOG_DEBUG( "CIRPlsContainer::UpdateAllL - Entering" );
	iListArray->Reset();
	CIRUi* ui = reinterpret_cast<CIRUi*>( iCoeEnv->AppUi() );
	TInt NoOfEntries = (ui->iURLs.Count()) / KTwo;
	TInt titleIndex = 1;    
	_LIT(KTab,"\t");
	for(TInt i=0 ; i < NoOfEntries ; i++ )
		{

		HBufC8* title;
		title = HBufC8::NewLC( (ui->iURLs[titleIndex]->Des()).Length() + KTwo );
		(title->Des()).Append(KTab);
		(title->Des()).Append((ui->iURLs[titleIndex])->Des());
		(title->Des()).Append(KTab);
		HBufC* title1 = HBufC::NewLC((title->Des()).Length());
		title1->Des().Copy(title->Des());
		iListArray->AppendL( (title1->Des()));
		CleanupStack::PopAndDestroy(title1);
		CleanupStack::PopAndDestroy(title);
		titleIndex +=KTwo;    
		}
	if(iListArray->Count())
		{
		iPlsListBox->SetCurrentItemIndex(iPlsView.GetCurrentFocus());
		iPlsListBox->View()->VScrollTo(iPlsView.GetCurrentTopItem());	
		}
	else
		{
         HBufC* headerText = StringLoader::LoadLC(R_IRAPP_STREAMING_LINKS);
         iPlsListBox->View()->SetListEmptyTextL(*headerText);
         CleanupStack::PopAndDestroy(headerText); 
		}	
	iPlsListBox->HandleItemAdditionL();
	DrawNow();	
	IRLOG_DEBUG( "CIRPlsContainer::UpdateAllL - Exiting" );      
    }


// ---------------------------------------------------------------------------
// From class CCoeControl.
// Draws the control.
// ---------------------------------------------------------------------------
//
void CIRPlsContainer::Draw( const TRect& aRect ) const
    {
    IRLOG_DEBUG( "CIRPlsContainer::Draw - Entering" );
    CWindowGc& gc = SystemGc();
    gc.SetPenStyle( CGraphicsContext::ENullPen );
    gc.DrawRect( aRect );
    IRLOG_DEBUG( "CIRPlsContainer::Draw - Exiting" );
    }


// ---------------------------------------------------------------------------
// From class CCoeControl.
// Opens help by context.
// ---------------------------------------------------------------------------
//
void CIRPlsContainer::GetHelpContext( TCoeHelpContext& aContext ) const
    {
    IRLOG_DEBUG( "CIRPlsContainer::GetHelpContext - Entering" );
    aContext = TCoeHelpContext( KIRMCVUid, KIR_HELP_PLS );
    IRLOG_DEBUG( "CIRPlsContainer::GetHelpContext - Exiting" );
    }

    
// ---------------------------------------------------------------------------
// HandleListBoxEventL().
// From MEikListBoxObserver.
// ---------------------------------------------------------------------------
//
void CIRPlsContainer::HandleListBoxEventL( CEikListBox* /*aListBox*/, TListBoxEvent aEventType )
	{
	IRLOG_DEBUG( "CIRPlsContainer::HandleListBoxEventL - Entering" );
	switch ( aEventType )
		{
		/*  Keypress event. */
		case EEventEnterKeyPressed:
		/*  Item two-taps event*/
		case EEventItemDoubleClicked:
			{
			iPlsView.SetCurrentFocus(iPlsListBox->CurrentItemIndex());
			iPlsView.HandleCommandL(EListenCmd);
			break;
			} 
		default:
		break;
		}
	IRLOG_DEBUG( "CIRPlsContainer::HandleListBoxEventL - Exiting" );
	}

	
// ---------------------------------------------------------------------------
// GetCurrentItemIndex.
// 
// ---------------------------------------------------------------------------
//	
TInt CIRPlsContainer::GetCurrentItemIndex() const
    {
    IRLOG_DEBUG( "CIRPlsContainer::GetCurrentItemIndex " );
	return iPlsListBox->CurrentItemIndex();
    }

    
// ---------------------------------------------------------------------------
// GetCount().
// Gets the count of the ListArray.
// ---------------------------------------------------------------------------
//   
TInt CIRPlsContainer::GetCount() const
    {
    IRLOG_DEBUG( "CIRPlsContainer::GetCount " );
    return iListArray->Count();	
    }  

      
// ---------------------------------------------------------------------------
// ListenToStationL().
// Listen to channel (URL)
// ---------------------------------------------------------------------------
//
void CIRPlsContainer::ListenToStationL()
	{
	IRLOG_DEBUG( "CIRPlsContainer::ListenToStationL - Entering" );
		iPlsView.iNowPlayingWrapper->SetView(EPls);
		CIRIsdsPreset* preset = CIRIsdsPreset::NewL();
		CleanupStack::PushL( preset );
		if(iListArray->Count()>0)
			{
			CreatePresetListL();
			TInt ListBoxIndex = GetCurrentItemIndex();	
			TInt UrlIndex = (ListBoxIndex * KTwo);
			iPlsView.iUi->SetPlsIndex(ListBoxIndex);
			iPlsView.iUi->SetListenFromSecondPls(ETrue);
				
			iPlsView.iUi->iNPPrevViewId = KIRPlsViewID;  
			iPlsView.iNowPlayingWrapper->SetConnectToServerState(ETrue);

			//copy channel name
			TBuf<KChannelNameSize> channelName;
			if(UrlIndex>=0 && UrlIndex <iPlsView.iUi->iURLs.Count())
				{
				channelName.Copy(*( iPlsView.iUi->iURLs[UrlIndex+1]));	
				}
			preset->SetName(channelName);
			preset->SetChannelType(0);
			channelName.Zero();
			//copy channel url
			channelName.Copy(*( iPlsView.iUi->iURLs[UrlIndex] ) );
			_LIT(KServerName,"NA");

			preset->SetUrlL(KServerName,channelName,0);
			preset->SetUrlCount();
			// Set the channel as last played and currently playing
			*iPlsView.iNowPlayingWrapper->iNowPlayingPreset = *preset;
			CleanupStack::PopAndDestroy( preset );

			iPlsView.iUi->iNPPrevViewId = KIRPlsViewID;

			//connecting to server	
			iPlsView.iNowPlayingWrapper->SetCurrentConnectionSource(EIRAdhocExternal);
			iPlsView.iNowPlayingWrapper->ConnectToChannelL(*iPlsView.iNowPlayingWrapper->iNowPlayingPreset);
		}
	                                        
	IRLOG_DEBUG( "CIRPlsContainer::ListenToStationL - Exiting." );
	}

	
// ---------------------------------------------------------------------------
// CreatePresetL().
// Creates the Preset from the playlist data.
// ---------------------------------------------------------------------------
//	
CIRIsdsPreset* CIRPlsContainer::CreatePresetL()
	{
	IRLOG_DEBUG( "CIRPlsContainer::CreatePresetL - Entering." );
	// Create a preset
	CIRIsdsPreset *newpreset;
	newpreset = CIRIsdsPreset::NewL(); 	
	// Indicates that the preset user added
	newpreset->SetChannelType(0);
	// Indicates that the preset user added		
	newpreset->SetId(0);
	IRLOG_DEBUG( "CIRPlsContainer::CreatePresetL - Exiting." );
	return newpreset;
	}

		
// ---------------------------------------------------------------------------
// CreatePresetListL().
// Creates the PresetList from the playlist data.
// ---------------------------------------------------------------------------
//	
void CIRPlsContainer::CreatePresetListL()
	{
	IRLOG_DEBUG( "CIRPlsContainer::CreatePresetListL - Entering." );
	_LIT(KServerName,"NA");
	CIRUi* ui = reinterpret_cast<CIRUi*>( iCoeEnv->AppUi() );	
	// The index for the .pls file info present in iUrls
	if( !iPresetListPresent )
		{
		iPresetListPresent = ETrue;
		TInt urlIndex = 0;
		TInt titleIndex = 1;
		// Loop through all the .pls entries
		if(iListArray->Count()>0)
			{
			for( TInt index=0;index < ui->iURLs.Count()/2 ; index++ , titleIndex += 2 , urlIndex += 2 )
				{
				// Get the title of the first pls file entry indicated by titleIndex
				HBufC16* title = HBufC16::NewLC(ui->iURLs[titleIndex]->Length()+1);
				title->Des().Copy(ui->iURLs[titleIndex]->Des());
				// Get the title of the first pls file entry indicated by titleIndex
				HBufC16* url = HBufC16::NewLC(ui->iURLs[urlIndex]->Length()+1);
				url->Des().Copy(ui->iURLs[urlIndex]->Des());
				// Loop through all the presets in the 
				// preset list to check if the title is already saved
				CIRIsdsPreset* tempPreset;
				TBool present = EFalse;
				for ( TInt presetIndex = 0 ; presetIndex < iPresetList.Count() ; presetIndex++ )
					{
					present = EFalse;
					// get the pointer to preset at locationindex
					tempPreset = iPresetList[presetIndex];
					if( tempPreset->GetName().Compare(*title) == 0 )
						{
						present = ETrue;
						break;							
						}
					}
				// The title is already there as a preset
				// hence add the url to the same
				if( present )
					{
					// Set url for the preset
					tempPreset->SetUrlL(KServerName,*url,0);		
					}
				else
					{
					CIRIsdsPreset* preset = CreatePresetL();
					CleanupStack::PushL(preset);
					preset->SetName(*title);
					// Set url for the preset
					preset->SetUrlL(KServerName,*url,0);
					// Append the preset object to the iPresetList
					iPresetList.Append(preset);
					iIsPresetSaved.Append(0);
					CleanupStack::Pop(preset);
					}
				CleanupStack::PopAndDestroy(url);
				CleanupStack::PopAndDestroy(title);
				}
			}

	}
	IRLOG_DEBUG( "CIRPlsContainer::CreatePresetListL - Exiting." );
}


 
    
// ---------------------------------------------------------------------------
// SaveL().
// Saves all the entries in the list to the favorites .
// ---------------------------------------------------------------------------
//    
void CIRPlsContainer::SaveL()
	{
	IRLOG_DEBUG( "CIRPlsContainer::SaveL - Entering" );
	CIRUi* ui = reinterpret_cast<CIRUi*>( iCoeEnv->AppUi() );
		
    if( !ui->iFavPresets->EmptyPresetCount() )
        {
		DisplayErroronDBL();
		return ;
        }
     
     if(iListArray->Count()>0)
     {
     	
     	TBool saved = ETrue;
		TInt selectedIndex = GetCurrentItemIndex();		
		CreatePresetListL();
		// Get the title of the first pls file entry indicated by titleIndex
		HBufC16* title = HBufC16::NewLC(ui->iURLs[(selectedIndex*2)+1]->Length()+1);
		title->Des().Copy(ui->iURLs[(selectedIndex*2)+1]->Des());
		// Get the title of the first pls file entry indicated by titleIndex
		HBufC16* url = HBufC16::NewLC(ui->iURLs[(selectedIndex*2)]->Length()+1);
		url->Des().Copy(ui->iURLs[(selectedIndex*2)]->Des());
		
		RBuf urlName;
		urlName.Create(*url);
		
		RBuf titleName;
		titleName.Create(*title);
		
		for ( TInt index = 0 ; index < iPresetList.Count() ; index++ )
			{
			// get the pointer to preset at locationindex
			CIRIsdsPreset* tempPreset = iPresetList[index];
			if( tempPreset->GetName().Compare(*title) == 0 )
				{
				tempPreset->SetUrlCount();
				TInt res = KErrNone;
				if(index >=0 && index <iIsPresetSaved.Count())
					{
					// check is the preset is already added
					if( iIsPresetSaved[index] == 0 )
						{
						ui->iFavPresets->AddPresetL(*tempPreset,res);
						if( res == KErrNoMemory )
							{
							iIsPresetSaved[index] = 0;
							DisplayErroronDBL();
							saved = EFalse;
							break;
							}
						else
							{					
							iIsPresetSaved[index] = 1;	
							iPlsView.iUi->GetPlsSaved()[index] = 1 ;
							}				
						}
					}
			    }
		    }
		if( saved )
			{
		    TInt com=urlName.Compare(titleName);
		    if(com==0)
			    {
			    ui->iDialogNote->ShowConfirmationNoteL( R_IRAPP_STATION_SAVED,ETrue );
			    }
		    else
			    {
				ui->DisplayInformationL( R_IRAPP_STATIONS_POPUP_SAVED,titleName );
				iPlsView.iUi->iIRSettings->SetFlagIfAnyFavL();
			    }
			}
		CleanupStack::PopAndDestroy(url);
		CleanupStack::PopAndDestroy(title);	
		titleName.Close();
	    urlName.Close();
        }
	IRLOG_DEBUG( "CIRPlsContainer::SaveL - Exiting." ); 
	}


// ---------------------------------------------------------------------------
// SaveAllL()
// Saves all the entries in the list to the favorites .
// ---------------------------------------------------------------------------
//
void CIRPlsContainer::SaveAllL()
	{
	IRLOG_DEBUG( "CIRPlsContainer::SaveAllL - Entering" );
	// Create the preset list if it doesnt exist                                 
	CreatePresetListL();
	CIRUi* ui = reinterpret_cast<CIRUi*>( iCoeEnv->AppUi() );
	
    TBool allSaved = ETrue;	
	TInt res = KErrNone;
    if( iPresetList.Count() > ui->iFavPresets->EmptyPresetCount() )
        {
        res = KErrNoMemory;
        allSaved = EFalse;
        }
	
    else
        {
        
	    // Now add all the presets in the iPresetList to fav db
	    for ( TInt index = 0 ; index < iPresetList.Count() ; index++ )	
			{			
			CIRIsdsPreset* preset = iPresetList[index];
			preset->SetUrlCount();
			if(index >=0 && index <iIsPresetSaved.Count())
				{
				// check is the preset is already added
				if( iIsPresetSaved[index] == 0 )
					{
					ui->iFavPresets->AddPresetL(*preset,res);
					//KErrNoMemory is returned when there are 20 Stations in the Favourites. 
					if( res == KErrNoMemory )
						{
						iIsPresetSaved[index] = 0;					
						allSaved = EFalse;
						break;
						}
					else
						{					
						iPlsView.iUi->GetPlsSaved()[index] = 1;						
						iIsPresetSaved[index] = 1;	
						}
					}
				}
			}
        }
    
    if( res == KErrNoMemory )
        {            
        DisplayErroronDBL();
        }
			
	// Check if all the presets have been saved	
	if( allSaved )
		{
		//display message if save successful
		if(iPresetList.Count()!=0)
			{
		    ui->DisplayInformationL(R_IRAPP_STATIONS_SAVED,iPresetList.Count());	
			iSavedAllUrl = ETrue;
			iPlsView.iUi->iIRSettings->SetFlagIfAnyFavL();
			}
		}
	IRLOG_DEBUG( "CIRPlsContainer::SaveAllL - Exiting." );
	}

	
// ---------------------------------------------------------------------------
// DisplayErroronDBL.
// Displays error if favorites db is full.
// ---------------------------------------------------------------------------
//  
void CIRPlsContainer::DisplayErroronDBL()
	{
	IRLOG_DEBUG( "CIRPlsContainer::DisplayErroronDBL - Entering." );
	CIRUi* ui = reinterpret_cast<CIRUi*>( iCoeEnv->AppUi() );

	ui->iDialogNote->ShowErrorNoteL(R_IRAPP_ADDFAVOURITES_ERROR,ETrue);
	IRLOG_DEBUG( "CIRPlsContainer::DisplayErroronDBL - Exiting." );	
	}

	
// ---------------------------------------------------------------------------
// DisplayErrorOnNoItemsL()
// Displays error if no streaming links
// ---------------------------------------------------------------------------
//
void CIRPlsContainer::DisplayErrorOnNoItemsL()
	{
	IRLOG_DEBUG( "CIRPlsContainer::DisplayErrorOnNoItemsL - Entering." );
	CIRUi* ui = reinterpret_cast<CIRUi*>( iCoeEnv->AppUi() );
	ui->iDialogNote->ShowErrorNoteL(R_IRAPP_STREAMING_LINKS,ETrue);
	IRLOG_DEBUG( "CIRPlsContainer::DisplayErrorOnNoItemsL - Exiting." );
	}

	
// ---------------------------------------------------------------------------
// GetListBoxTopIndex().
// 
// ---------------------------------------------------------------------------
//
TInt CIRPlsContainer::GetListBoxTopIndex() const
	{
	IRLOG_DEBUG( "CIRPlsContainer::GetListBoxTopIndex" );
	return iPlsListBox->View()->TopItemIndex ();
	}
// ---------------------------------------------------------------------------
// SecondPls().
// To be called when Second Pls file is opened.
// ---------------------------------------------------------------------------
//
void CIRPlsContainer::SecondPls()
	{
	iPresetList.ResetAndDestroy();
	iIsPresetSaved.Close();
	iPresetList.Close();
	iPresetListPresent = EFalse;
	TRAPD(err,UpdateAllL());
	if(err)
		{
		return ;
		}
		
	DrawNow();
    }
//----------------------------------------------------------------------------
// CIRPlsContainer::CountComponentControls() const
// returns the number of components in the view.
//----------------------------------------------------------------------------
//
TInt CIRPlsContainer::CountComponentControls() const
	{
	IRLOG_DEBUG( "CIRPlsContainer::CountComponentControls - Entering" );
	TInt count( KPLSLISTCOUNT );
	if (iPlsListBox)
		{
		count++;
		}
	IRLOG_DEBUG( "CIRPlsContainer::CountComponentControls - Exiting" );
	return count;     // returns the no. of controls inside this container
	}
//----------------------------------------------------------------------------
// CIRPlsContainer::ComponentControl() const
// returns a pointer to the control under this view depending on the index
// passed,to the framework.
//----------------------------------------------------------------------------
// 
CCoeControl* CIRPlsContainer::ComponentControl(TInt aIndex) const
	{
	IRLOG_DEBUG( "CIRPlsContainer::ComponentControl - Entering" );
	IRLOG_DEBUG( "CIRPlsContainer::ComponentControl - Exiting" );
	switch ( aIndex )
		{
		case 0:
		       return iPlsListBox;
		       
		default:
		      return NULL;
		}
	}
// -----------------------------------------------------------------------------
// CIRPlsContainer::SizeChanged()
// Called by framework when the view size is changed.
// -----------------------------------------------------------------------------
//
void CIRPlsContainer::SizeChanged()
    {  
    if(iPlsListBox)
	    {
	    iPlsListBox->SetRect(Rect());
	    }
    }
// ---------------------------------------------------------------------------
// void CIRPlsContainer::HandleResourceChange(TInt aType)
// Handles a change to the control's resources.
// ---------------------------------------------------------------------------
//
void CIRPlsContainer::HandleResourceChange(TInt aType)
	{
	IRLOG_DEBUG( "CIRMainContainer::HandleResourceChange - Entering" );
    CCoeControl::HandleResourceChange( aType );
	SetRect( iPlsView.ClientRect() );
	DrawDeferred();
    IRLOG_DEBUG( "CIRMainContainer::HandleResourceChange - Exiting" );
	}
