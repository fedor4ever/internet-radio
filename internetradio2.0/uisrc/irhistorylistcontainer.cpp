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
* Description:  Container class for IRHistoryListContainer
*
*/


/* ---------------------------------------------------------------------------
*  Version history:
*  Template version:
*  <ccm_history>
*
*  Version: 2, Thu Apr 10 20:00:00 2008 by Rohit
*  Ref:
*  Implemented SongHistory changes for channelwise songs
*
*  </ccm_history>
* ============================================================================
*/

#include <stringloader.h>
#include <internetradio.rsg>
#include "iraap.hlp.hrh"
#include <aknsinglestyletreelist.h>
#include <aknsbasicbackgroundcontrolcontext.h>
#include <akntoolbar.h>

#include "irsonghistoryengine.h"
#include "irui.h"
#include "ir.hrh"
#include "ircommon.h"
#include "irdebug.h"
#include "irbaseview.h"
#include "irhistorylistview.h"
#include "irhistorylistcontainer.h"
#include "irapplication.h"
#include "irsonghistoryinfo.h"
#include "irnowplayingwrapper.h"
#include "irfavoritesdb.h"
#include "irisdswrapper.h"
#include "irisdspreset.h"
#include "irpubsub.h"

const TInt KMaxSize = 5;
const TInt KTwo = 2 ;
_LIT(KNo,"No");

//-----------------------------------------------------------------------------
//  CIRHistoryListContainer::NewL
//  Creates a new instance of History Container
//-----------------------------------------------------------------------------
//
CIRHistoryListContainer* CIRHistoryListContainer::NewL(
			const TRect& aRect,CIRHistoryListView& aView,
			RPointerArray<CIRSongHistoryInfo>& aHistoryDataArr)
    {
	IRLOG_DEBUG( "CIRHistoryListContainer::NewL - Entering" );
    CIRHistoryListContainer* self = CIRHistoryListContainer::NewLC(aRect,aView,aHistoryDataArr);
    CleanupStack::Pop(self);
	IRLOG_DEBUG( "CIRHistoryListContainer::NewL - Exiting." );
    return self;
    }

//-----------------------------------------------------------------------------
//  CIRHistoryListContainer::NewLC
//  Creates a new instance of History Container and leaves it on the cleanup stack.
//-----------------------------------------------------------------------------
//
CIRHistoryListContainer* CIRHistoryListContainer::NewLC(const TRect& aRect ,
			CIRHistoryListView& aView ,RPointerArray<CIRSongHistoryInfo>& aHistoryDataArr)
    {
    IRLOG_DEBUG( "CIRHistoryListContainer::NewLC - Entering" );
    CIRHistoryListContainer* self = new (ELeave) CIRHistoryListContainer(aView);
    CleanupStack::PushL(self);
    self->ConstructL(aRect,aHistoryDataArr);
	IRLOG_DEBUG( "CIRHistoryListContainer::NewLC - Exiting." );
    return self;
    }

// ---------------------------------------------------------------------------
// C++ default constructor can NOT contain any code, that might leave.
// ---------------------------------------------------------------------------
//
CIRHistoryListContainer::CIRHistoryListContainer( CIRHistoryListView& aHistoryListView):
                                                  iHistoryListView( aHistoryListView )
    {
    IRLOG_DEBUG( "CIRHistoryListContainer::CIRHistoryListContainer" );
    iDimmed=ETrue;
    }


// ---------------------------------------------------------------------------
// CIRHistoryListContainer::ConstructL
// ---------------------------------------------------------------------------
//
void CIRHistoryListContainer::ConstructL( const TRect& aRect, 
						RPointerArray<CIRSongHistoryInfo>& aHistoryDataArr
						 )
    {
   IRLOG_DEBUG( "CIRHistoryListContainer::ConstructL - Entering" );
   CreateWindowL();
   
   InitComponentArrayL();
   Components().SetControlsOwnedExternally( ETrue );
   CAknSingleStyleTreeList* list =CAknSingleStyleTreeList::NewL( *this );
    // Use list as a component control of the view.
    Components().AppendLC( list, KCoeNoControlId );
    CleanupStack::Pop( list );
    iTreeListBox = list;
    iTreeListBox->SetContainerWindowL(*this);
    iTreeListBox->SetFlags( iTreeListBox->Flags() ^ KAknTreeListLooping ^ 
                            KAknTreeListNoStructureLines ^ KAknTreeListMarqueeScrolling);
    iTreeListBox->AddObserverL( this );
    iBgContext = CAknsBasicBackgroundControlContext::NewL(
        KAknsIIDQsnBgAreaMain, iAvkonAppUi->ApplicationRect(), EFalse );
    
    
	iChannelArray=new ( ELeave ) CDesC16ArrayFlat (KMaxSize);
	iUrlArray=new ( ELeave ) CDesC16ArrayFlat (KMaxSize);
	iChannelDescriptionArray=new ( ELeave ) CDesC16ArrayFlat (KMaxSize);
	iMusicStoreStatus=new ( ELeave ) CDesC16ArrayFlat (KMaxSize);
    
    UpdateAllL(aHistoryDataArr);
    iTreeListBox->SetRect(aRect);
	iTreeListBox->SetFocus(ETrue);
    iTreeListBox->MakeVisible(ETrue);
    SetRect( aRect );
    DrawNow();
    iTreeListBox->DrawNow();
    IRLOG_DEBUG( "CIRHistoryListContainer::ConstructL - Exiting" );
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CIRHistoryListContainer::~CIRHistoryListContainer()
    {
        
    IRLOG_DEBUG( "CIRHistoryListContainer::~CIRHistoryListContainer - Entering" );
        if(iTreeListBox)
    	{
    	delete iTreeListBox;
    	iTreeListBox = NULL;
    	}
    	
    	iNodeArray.Close();
    	
    	iLeafArray.Close();
    	
    	iChannelTypeArray.Close();
    	
    	iChannelIdArray.Close();
    	
    	if(iBgContext)
	    	{
	    	delete iBgContext;
	    	iBgContext = NULL;	
	    	}
    	
    	if(iUrlArray)
	    	{
	    	iUrlArray->Reset();
	    	delete iUrlArray;	
	    	iUrlArray=NULL;
	    	}
    	if(iChannelArray)
	    	{
	    	iChannelArray->Reset();
	    	delete iChannelArray;
	    	iChannelArray=NULL;	
	    	}
    	if(iChannelDescriptionArray)
	    	{
	    	iChannelDescriptionArray->Reset();
	    	delete iChannelDescriptionArray;	
	    	iChannelDescriptionArray=NULL;
	    	}
    	if(iMusicStoreStatus)
	    	{
	    	iMusicStoreStatus->Reset();
	    	delete iMusicStoreStatus;	
	    	iMusicStoreStatus=NULL;
	    	}
        IRLOG_DEBUG( "CIRHistoryListContainer::~CIRHistoryListContainer - Exiting" );
    }

// ---------------------------------------------------------------------------
// From class CCoeControl.
// We need to catch the selection key event in order to do a couple of things 
// in moving state make the movement, and in normal state open channel 
// specific popup menu. Up and down buttons are forwarded to the list.
// ---------------------------------------------------------------------------
//
TKeyResponse CIRHistoryListContainer::OfferKeyEventL( 
        const TKeyEvent& aKeyEvent, TEventCode aType )
    {
    IRLOG_DEBUG( "CIRHistoryListContainer::OfferKeyEventL -Entering" );
    if(iTreeListBox)
	    {
        IRLOG_DEBUG( "CIRHistoryListContainer::OfferKeyEventL -Exiting" );
	    return iTreeListBox->OfferKeyEventL(aKeyEvent, aType);
	    }
	return EKeyWasNotConsumed;   
    }

void CIRHistoryListContainer::HandlePointerEventL( const TPointerEvent& aPointerEvent )
	{
	IRLOG_DEBUG( "CIRHistoryListContainer::HandlePointerEventL - Entering" );
	if(iTreeListBox)
		{
		iTreeListBox->HandlePointerEventL( aPointerEvent ); 
		}
	IRLOG_DEBUG( "CIRHistoryListContainer::HandlePointerEventL - Exiting" );
	}

// ---------------------------------------------------------------------------
// CIRHistoryListContainer::UpdateAllL()
// Fills list with  data read from data base.
// ---------------------------------------------------------------------------
//
void CIRHistoryListContainer::UpdateAllL(RPointerArray<CIRSongHistoryInfo> & aHistoryDataArr)
	{
	IRLOG_DEBUG( "CIRHistoryListContainer::UpdateAllL - Entering" );
	if(aHistoryDataArr.Count()==0)
		{
        iTreeListBox->DrawNow();
	    HBufC* headerText = StringLoader::LoadLC(R_IRAPP_CLEAR_HISTORY);
		iTreeListBox->SetEmptyTextL(*headerText);
		CleanupStack::PopAndDestroy(headerText); 

		}
	else
		{
		
		
        TUint32 flags = CAknSingleStyleTreeList::EPersistent;
		TBool drawNow = EFalse;

		for(TInt songCount = 0; songCount < aHistoryDataArr.Count(); songCount++)  
			{
			if(songCount==0)     
				{
				iChannelArray->AppendL(aHistoryDataArr[songCount]->GetChannelName());
			    iUrlArray->AppendL(aHistoryDataArr[songCount]->GetStreamUrl());
                iChannelTypeArray.Append(aHistoryDataArr[songCount]->GetChannelType());
                iChannelIdArray.Append(aHistoryDataArr[songCount]->GetChannelId());
                iChannelDescriptionArray->AppendL(aHistoryDataArr[songCount]->GetChannelDesc()); 
                iMusicStoreStatus->AppendL(aHistoryDataArr[songCount]->GetChannelMusicStatus());	
                }
			else
				{
				TBool nextChannel=EFalse;
				for(TInt comp=0;comp<iChannelArray->Count();comp++)
					{
					if((aHistoryDataArr[songCount]->GetChannelName()==iChannelArray->MdcaPoint(comp)) &&
					         (aHistoryDataArr[songCount]->GetStreamUrl()==iUrlArray->MdcaPoint(comp)) &&
					         (aHistoryDataArr[songCount]->GetChannelType()==iChannelTypeArray[comp]))
						{
						nextChannel=ETrue;	
						}
						
						
							
					}
					if(!nextChannel)
							{
													
							iChannelArray->AppendL(aHistoryDataArr[songCount]->GetChannelName());
						    iUrlArray->AppendL(aHistoryDataArr[songCount]->GetStreamUrl());
		                    iChannelTypeArray.Append(aHistoryDataArr[songCount]->GetChannelType());
		                    iChannelIdArray.Append(aHistoryDataArr[songCount]->GetChannelId());
		                    iChannelDescriptionArray->AppendL(aHistoryDataArr[songCount]->
		                                                      GetChannelDesc()); 
		                    iMusicStoreStatus->AppendL(aHistoryDataArr[songCount]->
		                                                      GetChannelMusicStatus());	
		                                                      
							}
                    
				}

			}
		for(TInt index=0;index<iChannelArray->Count();index++) 
			{
			
			TAknTreeItemID subtitle = iTreeListBox->AddNodeL( 1,
			                                                 iChannelArray->MdcaPoint(index), 
			                                                 flags,
			                                                 drawNow );
			iNodeArray.Append(subtitle);	
			}
		for(TInt songCount = 0; songCount < aHistoryDataArr.Count(); songCount++)            
			{
			for(TInt index=0;index<iChannelArray->Count();index++)
				{
					if((aHistoryDataArr[songCount]->GetChannelName()==iChannelArray->MdcaPoint(index)) &&
					         (aHistoryDataArr[songCount]->GetStreamUrl()==iUrlArray->MdcaPoint(index)) &&
					         (aHistoryDataArr[songCount]->GetChannelType()==iChannelTypeArray[index]))
						{
				        HBufC* text ;
						CDesCArray* strings = new ( ELeave ) CDesCArrayFlat( 2 );
						CleanupStack::PushL( strings );
				        if(aHistoryDataArr[songCount]->GetArtistInfo() != KNullDesC &&
				           aHistoryDataArr[songCount]->GetSongInfo() != KNullDesC)
					        {
							strings->AppendL(aHistoryDataArr[songCount]->GetArtistInfo());
							strings->AppendL(aHistoryDataArr[songCount]->GetSongInfo());
						    text = StringLoader::LoadLC( R_IR_HISTORY_ARTIST_SONG, *strings );
					        }
                        else if(aHistoryDataArr[songCount]->GetArtistInfo() != KNullDesC)
					        {
	                     	text = HBufC::NewLC( aHistoryDataArr[songCount]->
	                     						GetArtistInfo().Length() + KTwo );
	                     	text->Des().Copy(aHistoryDataArr[songCount]->GetArtistInfo());
					        }
				        else
					        {
	                     	text = HBufC::NewLC( aHistoryDataArr[songCount]->
	                     						GetSongInfo().Length() + KTwo );
	                     	text->Des().Copy(aHistoryDataArr[songCount]->GetSongInfo());
					        }
						if(index>=0 && index<iNodeArray.Count())
							{
							TAknTreeItemID coreItem = iTreeListBox->AddLeafL( iNodeArray[index],
							*text, flags, drawNow );
							iTreeListBox->SetIcon( coreItem, CAknSingleStyleTreeList::ELeaf, NULL, ETrue );
							iLeafArray.AppendL(coreItem);   	
							}
						CleanupStack::PopAndDestroy( text );	
                        CleanupStack::PopAndDestroy( strings );
						}
					
						
				}
         
			}
		iTreeListBox->DrawNow();
		if(iHistoryListView.Toolbar())
			{
            iHistoryListView.Toolbar()->SetItemDimmed(EListenCmd,EFalse,ETrue);
            iHistoryListView.Toolbar()->DrawNow();
          	}
		}	
	IRLOG_DEBUG( "CIRHistoryListContainer::UpdateAllL - Exiting" );


	}
// ---------------------------------------------------------------------------
// CIRHistoryListContainer::RemoveListItem() 
// Removes  data  from the list read from data base.
// ---------------------------------------------------------------------------
//
void CIRHistoryListContainer::RemoveListItem(const TDesC &/*aSongName*/ , const TDesC & /*aArtistName*/, const TDesC & /*aChannelName*/)
{
IRLOG_DEBUG( "CIRHistoryListContainer::RemoveListItem" );	 
}


// ---------------------------------------------------------------------------
// From class CCoeControl.
// Draws the control.
// ---------------------------------------------------------------------------
//
void CIRHistoryListContainer::Draw( const TRect& /*aRect*/ ) const
    {
    IRLOG_DEBUG( "CIRHistoryListContainer::Draw" );
    }


// ---------------------------------------------------------------------------
// From class CCoeControl.
// Opens help by context.
// ---------------------------------------------------------------------------
//
void CIRHistoryListContainer::GetHelpContext( TCoeHelpContext& aContext ) const
    {
    IRLOG_DEBUG( "CIRHistoryListContainer::GetHelpContext - Entering" );
    aContext = TCoeHelpContext( KIRMCVUid, KIR_HELP_STATIONS  );
    IRLOG_DEBUG( "CIRHistoryListContainer::GetHelpContext - Exiting" );
    }

// ---------------------------------------------------------------------------
// CIRHistoryListContainer::ClearHistoryListL().
// Clears the List
// ---------------------------------------------------------------------------
//
void CIRHistoryListContainer::ClearHistoryListL()
	{
	IRLOG_DEBUG( "CIRHistoryListContainer::ClearHistoryListL - Entering" );
	HBufC* headerText = StringLoader::LoadLC(R_IRAPP_CLEAR_HISTORY);
	iTreeListBox->SetEmptyTextL(* headerText);
	CleanupStack::PopAndDestroy(headerText); 
	for(TInt index=0;index<iNodeArray.Count();index++)
		{
		iTreeListBox->RemoveItem( iNodeArray[index], EFalse );
		}

	iNodeArray.Reset();
	iLeafArray.Reset();
	iChannelTypeArray.Reset();
	iChannelIdArray.Reset();
	if(iUrlArray)
		{
		iUrlArray->Reset();
		}
	if(iMusicStoreStatus)
		{
		iMusicStoreStatus->Reset();
		}
	if(iChannelArray)
		{
		iChannelArray->Reset();
		}
	if(iChannelDescriptionArray)
	{
	iChannelDescriptionArray->Reset();
	}
	
	IRLOG_DEBUG( "CIRHistoryListContainer::ClearHistoryListL - Exiting" );
	}

// ---------------------------------------------------------------------------
// CIRHistoryListContainer::ListenUrlL()
// Listen a URL and displays Now Playing
// ---------------------------------------------------------------------------
//
void CIRHistoryListContainer::ListenUrlL( TInt aIndex )
	{
	IRLOG_DEBUG( "CIRHistoryListContainer::ListenUrlL - Entering" );
	iHistoryListView.iNowPlayingWrapper->SetView(EHistoryView);
	
	iListenFrmHistory = ETrue ;
    iAddFrmHistory = EFalse;
	// Listen the URL
	if(aIndex>=0 && aIndex<iChannelTypeArray.Count())
		{
		if(iChannelTypeArray[aIndex]==0)
			{
			CIRIsdsPreset* preset = CIRIsdsPreset::NewL();
			CleanupStack::PushL( preset );
			preset->SetName(iChannelArray->MdcaPoint(aIndex));
			if(aIndex>=0 && aIndex<iChannelTypeArray.Count())
				{
				preset->SetChannelType(iChannelTypeArray[aIndex]);
				}
			preset->SetShortDesc(iChannelDescriptionArray->MdcaPoint(aIndex ));
	        preset->SetDescription(iChannelDescriptionArray->MdcaPoint(aIndex ));
			//Set the URL data.
			_LIT(KNA,"NA");
			preset->SetUrlL( KNA, iUrlArray->MdcaPoint(aIndex ), 0 );
			preset->SetUrlCount();
			// Set the channel as last played and currently playing
			*iHistoryListView.iNowPlayingWrapper->iNowPlayingPreset = *preset;
			CleanupStack::PopAndDestroy( preset );

			iHistoryListView.iUi->iNPPrevViewId = KIRHistoryListViewId;

			//connecting to server	
			iHistoryListView.iNowPlayingWrapper->SetCurrentConnectionSource(EIRHistoryAdhoc);
			iHistoryListView.iNowPlayingWrapper->ConnectToChannelL(*iHistoryListView.
			                                 iNowPlayingWrapper->iNowPlayingPreset);
			}
		else
			{
			iHistoryListView.iUi->iNPPrevViewId = KIRHistoryListViewId;
			if(aIndex>=0 && aIndex<iChannelIdArray.Count())
				{
				iChannelSyncId = iChannelIdArray[aIndex] ;
				iChangedIndex = aIndex;
				iHistoryListView.iNowPlayingWrapper->SetWhenUserCancelsBufferingWaitBar(EFalse);
		 		iHistoryListView.iNowPlayingWrapper->SetListenFromIsdsValue(EFalse);
		    	iHistoryListView.iNowPlayingWrapper->CreateWaitDialogL();
	    		iHistoryListView.iUi->GetPubSubManagerInstance()->PublishBufferingState(
	    								EIRStateBufferingStart);
				iHistoryListView.iIsdsWrapper->IsdsListenRequestL(this,iChannelIdArray[aIndex],ETrue);
				}
			}
		}
	IRLOG_DEBUG( "CIRHistoryListContainer::ListenUrlL - Exiting" );
    }


// ---------------------------------------------------------------------------
// void CIRHistoryListContainer::SizeChanged(const TRect& aRect)
// 
// ---------------------------------------------------------------------------
//

void CIRHistoryListContainer::SizeChanged()
	{
	IRLOG_DEBUG( "CIRHistoryListContainer::SizeChanged - Entering" );
    TRect rect = Rect();
    if (iTreeListBox)
        {
        iTreeListBox->SetRect( rect );
        }
    if ( iBgContext )
        {
        iBgContext->SetRect( rect );
        }
    IRLOG_DEBUG( "CIRHistoryListContainer::SizeChanged - Exiting" );
	}
	
//----------------------------------------------------------------------------
// CIRHistoryListContainer::CountComponentControls() const
// returns the number of components in the view.
//----------------------------------------------------------------------------
//
TInt CIRHistoryListContainer::CountComponentControls() const
	{
	IRLOG_DEBUG( "CIRHistoryListContainer::CountComponentControls" );
	return 1;
	}
//----------------------------------------------------------------------------
// CIRHistoryListContainer::ComponentControl() const
// returns a pointer to the control under this view depending on the index
// passed,to the framework.
//----------------------------------------------------------------------------
// 
CCoeControl* CIRHistoryListContainer::ComponentControl(TInt /*aIndex*/) const
	{
		IRLOG_DEBUG( "CIRHistoryListContainer::ComponentControl" );
		return iTreeListBox;
	}
	
// ----------------------------------------------------
// CIRHistoryListContainer::HandleTreeListEvent
// From MAknTreeListObserver .
// Handles the events on the TreeList
// ----------------------------------------------------
//
TBool CIRHistoryListContainer::HandleTreeListEvent( CAknTreeList& aList,
    TAknTreeItemID /* aItem */, TEvent aEvent )
    {
    IRLOG_DEBUG( "CIRHistoryListContainer::HandleTreeListEvent - Entering" );
    if ( &aList != iTreeListBox )
        {
        return KErrNone;
        }

    switch ( aEvent )
        {
        // One of the nodes in the list is being expanded.
        case ENodeExpanded:
        // One of the nodes in the list is being collapsed.
        case ENodeCollapsed:
        // Simple data row or core data row is selected.
        case EItemSelected:
			break;
        // Focused item changes
        case EItemFocused:
            {
			_LIT(KMusicStoreEnabled, "yes");
			if(iHistoryListView.Toolbar())
				{
				if(!ShowFocus())
					{
					iHistoryListView.Toolbar()->SetItemDimmed(EFindInShop,ETrue,ETrue);
					iDimmed=ETrue;
						
					}
				else
					{
					TInt itemIndex =GetCurrentFocussedNode();
					if(itemIndex>=0 && itemIndex<iMusicStoreStatus->MdcaCount() )
						{
						TInt comp=(iMusicStoreStatus->MdcaPoint(itemIndex)).Compare(KMusicStoreEnabled);
						if(comp)    
							{
							iHistoryListView.Toolbar()->SetItemDimmed(EFindInShop,ETrue,ETrue);
							iDimmed=ETrue;	
							}
						else
							{
							if(iHistoryListView.GetFindInShopInstance())
								{
								iHistoryListView.Toolbar()->SetItemDimmed(EFindInShop,EFalse,ETrue);
								iDimmed=EFalse;	
								}
							else
								{
								iHistoryListView.Toolbar()->SetItemDimmed(EFindInShop,ETrue,ETrue);
								iDimmed=ETrue;	
								}
							}    
						}    
					}
				iHistoryListView.Toolbar()->DrawNow();	
				}
				
            }
            break;
        // Item is being removed from the list.
        case EItemRemoved:
        case EMarkingModeEnabled:
        case EMarkingModeDisabled:
        case EItemUnmarked:
        case EItemMarked:
            break;
        
        default:
            break;
        }
    IRLOG_DEBUG( "CIRHistoryListContainer::HandleTreeListEvent - Exiting" );
    return KErrNone;
    }
	
// ----------------------------------------------------
// CIRHistoryListContainer::MopSupplyObject()
// ----------------------------------------------------
//
TTypeUid::Ptr CIRHistoryListContainer::MopSupplyObject( TTypeUid aId )
    {
    IRLOG_DEBUG( "CIRHistoryListContainer::MopSupplyObject - Entering" );
    if ( aId.iUid == MAknsControlContext::ETypeId && iBgContext )
        {
        IRLOG_DEBUG( "CIRHistoryListContainer::MopSupplyObject - Exiting" );
        return MAknsControlContext::SupplyMopObject( aId, iBgContext );
        }
    IRLOG_DEBUG( "CIRHistoryListContainer::MopSupplyObject - Exiting" );
    return CCoeControl::MopSupplyObject( aId );
    }
    
// ----------------------------------------------------
// CIRHistoryListContainer::HandleResourceChange()
// Handles Changes in the resources 
// ----------------------------------------------------
//
void CIRHistoryListContainer::HandleResourceChange( TInt aType )
    {
    IRLOG_DEBUG( "CIRHistoryListContainer::HandleResourceChange - Entering" );
    CCoeControl::HandleResourceChange( aType );
    CIRUi* ui = reinterpret_cast<CIRUi*>( iCoeEnv->AppUi() );
    TRect rect = ui->ApplicationRect();
    if( aType == KAknsMessageSkinChange )
        {
        }
	if( aType == KEikDynamicLayoutVariantSwitch )
		{
		if ( iBgContext )
			{
			iBgContext->SetRect( Rect() );
			}
		iTreeListBox->SetRect(rect);
		SizeChanged();
		DrawNow();
        
		}
	IRLOG_DEBUG( "CIRHistoryListContainer::HandleResourceChange - Exiting" );
   } 
   
// ----------------------------------------------------
// CIRHistoryListContainer::GetCurrentFocussed()
// To get the focus of Node of the treeList
// ----------------------------------------------------
//
     
TInt CIRHistoryListContainer::GetCurrentFocussedNode()
	{
	IRLOG_DEBUG( "CIRHistoryListContainer::GetCurrentFocussedNode - Entering" );
	TInt focusItem=iTreeListBox->FocusedItem();
	TInt isNode=iTreeListBox->IsNode( focusItem );
	TInt retValue = -1;
	if(!isNode)  
		{
		focusItem=iTreeListBox->Parent(focusItem);	
		}
	for(TInt index=0;index<iUrlArray->Count();index++)
		{
		if(index>=0 && index<iNodeArray.Count())
			{
			if(focusItem==iNodeArray[index])
			retValue=index;
			}
		}
	IRLOG_DEBUG( "CIRHistoryListContainer::GetCurrentFocussedNode - Exiting" );
	return retValue;
	}
	
// ----------------------------------------------------
// CIRHistoryListContainer::GetCurrentFocussedLeaf()
// To get the focus of Leaf of the treeList
// ----------------------------------------------------
//	
TInt CIRHistoryListContainer::GetCurrentFocussedLeaf()
	{
	IRLOG_DEBUG( "CIRHistoryListContainer::GetCurrentFocussedLeaf - Entering" );
	TInt focusItem=iTreeListBox->FocusedItem();
	TInt isLeaf=iTreeListBox->IsLeaf( focusItem );
	TInt retValue = -1;
	if(isLeaf)
		{
		for(TInt index=0;index<iLeafArray.Count();index++)
			{
			if(focusItem==iLeafArray[index])
				{
				retValue = index;
				break;
				}
			}
		}
	    
	IRLOG_DEBUG( "CIRHistoryListContainer::GetCurrentFocussedLeaf - Exiting" );
	return retValue;
	}
	
// ----------------------------------------------------
// CIRHistoryListContainer::AddStationToFavorites()
// For Adding the Station name to the favorites DB.
// ----------------------------------------------------
//	

void CIRHistoryListContainer::AddStationToFavoritesL(TInt aCommand)
	{
	IRLOG_DEBUG( "CIRHistoryListContainer::AddStationToFavoritesL - Entering" );
    iLoadingCancelled = EFalse;
    
	iAddFrmHistory = ETrue;
	iListenFrmHistory = EFalse ;
	TInt focusItem=iTreeListBox->FocusedItem();
	TInt isNode=iTreeListBox->IsNode( focusItem );
	TInt favouriteIndexChannel = -1;
	if(!isNode)  
		{
		focusItem=iTreeListBox->Parent(focusItem);	
		}
	for(TInt index=0;index<iUrlArray->Count();index++)
		{
		if(index>=0 && index<iNodeArray.Count())
			{
			if(focusItem==iNodeArray[index])
			favouriteIndexChannel=index;
			}
		}
    _LIT(KNA,"NA");
  	TInt res = KErrNone;
	if(favouriteIndexChannel>=0 && favouriteIndexChannel<iChannelTypeArray.Count())
		{
		if(iChannelTypeArray[favouriteIndexChannel]==0)
			{
			CIRIsdsPreset* preset = CIRIsdsPreset::NewL();
			CleanupStack::PushL( preset );
			preset->SetName(iChannelArray->MdcaPoint(favouriteIndexChannel));
			preset->SetUrlL( KNA, iUrlArray->MdcaPoint(favouriteIndexChannel ), 0 );
			preset->SetUrlCount();
			if(favouriteIndexChannel >=0 && favouriteIndexChannel<iChannelTypeArray.Count())
				{
				preset->SetChannelType(iChannelTypeArray[favouriteIndexChannel]);
				}
			if(favouriteIndexChannel >=0 && favouriteIndexChannel<iChannelIdArray.Count())
				{
				preset->SetId(iChannelIdArray[favouriteIndexChannel]);
				}
			preset->SetShortDesc(iChannelDescriptionArray->MdcaPoint(favouriteIndexChannel ));
	        preset->SetDescription(iChannelDescriptionArray->MdcaPoint(favouriteIndexChannel ));
			iHistoryListView.iUi->iFavPresets->AddPresetL(*preset,res);
			if( res == KErrNoMemory )
				{
				DisplayErroronDBL();
				}
			else
				{
				iHistoryListView.iUi->DisplayInformationL( R_IRAPP_STATIONS_POPUP_SAVED,
				                                           iChannelArray->
				                                           MdcaPoint(favouriteIndexChannel) );
				}
			CleanupStack::PopAndDestroy( preset );
			}
		else
			{
			//Display a loading Progress Bar
			iChannelSyncId = iChannelIdArray[favouriteIndexChannel] ;
			iChangedIndex = favouriteIndexChannel;
			iHistoryListView.iIsdsWrapper->IsdsPresetRequestL(this,
			                                              iChannelIdArray[favouriteIndexChannel],
			                                              aCommand,EFalse,ETrue);
			}
		}
	IRLOG_DEBUG( "CIRHistoryListContainer::AddStationToFavoritesL - Exiting" );  
	
	}
	
// ----------------------------------------------------
// CIRHistoryListContainer::DisplayErroronDBL()
// To show error note whenever the Fovorites DB is full
// ----------------------------------------------------
//	
void CIRHistoryListContainer::DisplayErroronDBL()
	{
	IRLOG_DEBUG( "CIRHistoryListContainer::DisplayErroronDBL - Entering" );
	iHistoryListView.iUi->iDialogNote->ShowErrorNoteL(R_IRAPP_ADDFAVOURITES_ERROR,ETrue);
	IRLOG_DEBUG( "CIRHistoryListContainer::DisplayErroronDBL - Exiting" );
	}

// ----------------------------------------------------
// CIRHistoryListContainer::ShowFocus()
// To return whether a leaf or node is focused 
// ----------------------------------------------------
//	
TInt CIRHistoryListContainer::ShowFocus()
	{
	IRLOG_DEBUG( "CIRHistoryListContainer::ShowFocus - Entering" );
	TInt focusItem=iTreeListBox->FocusedItem();
	TInt isLeaf=0;
	TInt retValue;
	if(focusItem)
		{
		isLeaf=iTreeListBox->IsLeaf( focusItem );
		}
	if(isLeaf)
		{
		retValue = ETrue;
		}
	else
		{
		retValue = EFalse;
		}
	IRLOG_DEBUG( "CIRHistoryListContainer::ShowFocus - Exiting" );
	return retValue ;

	}

// ---------------------------------------------------------------------------
// void CIRHistoryListContainer::ResponseL()
// Activates the Stations view after getting the IsdsResponse
// ---------------------------------------------------------------------------
//
void CIRHistoryListContainer::ResponseL( CIRIsdsPreset* aPreset )
	{
	IRLOG_DEBUG( "CIRHistoryListContainer::ResponseL - Entering" );
	iHistoryListView.ResetPendingRequests(EFalse);

	iHistoryListView.HistoryEngineInstance()->UpdateSongHistoryDb(aPreset->GetId(),
	                                                     aPreset->GetName(),
	                                                     KNo,
	                                                     aPreset->GetMusicStoreStatus());
	iHistoryListView.RefreshHistoryL();
	IRLOG_DEBUG( "CIRHistoryListContainer::ResponseL - Exiting" );
	return;
	}
// ---------------------------------------------------------------------------
// void CIRHistoryListContainer::PresetResponseL()
// Called when Preset Data is received
// Used to Listen to a channel with given preset.
// ---------------------------------------------------------------------------
//	
void CIRHistoryListContainer::PresetResponseL(CIRIsdsPreset* aPreset)
	{
	IRLOG_DEBUG( "CIRHistoryListContainer::PresetResponseL - Entering" );
	if ( !iHistoryListView.iUi->CheckAndNotifyLowDiskSpaceL() )
		{
		iHistoryListView.ResetPendingRequests(EFalse);
		if(!iHistoryListView.iNowPlayingWrapper->GetWhenUserCancelsIsdsBufferingWaitBar())
			{
			iHistoryListView.iNowPlayingWrapper->SetListenFromIsdsValue(ETrue);
			iHistoryListView.iNowPlayingWrapper->ListenToChannelL(aPreset);
			}
		}
	IRLOG_DEBUG( "CIRHistoryListContainer::PresetResponseL - Exiting" );
	}
// ---------------------------------------------------------------------------
// void CIRHistoryListContainer::ErrorL()
// Handles error Conditions
// ---------------------------------------------------------------------------
//	
void CIRHistoryListContainer::ErrorL()
	{
	IRLOG_DEBUG( "CIRHistoryListContainer::ErrorL - Entering" );
	if(iHistoryListView.iIsdsWrapper->GetListenRequest())
		{
		iHistoryListView.iNowPlayingWrapper->DestroyWaitDialogL();
		iHistoryListView.iUi->GetPubSubManagerInstance()->PublishBufferingState(EIRStateBufferingError);

		iHistoryListView.iIsdsWrapper->SetListenRequest(EFalse);
		}
	if(iHistoryListView.iIsdsWrapper->iConnTimeOut)
		{
		iHistoryListView.iIsdsWrapper->iConnTimeOut = EFalse;	
		}
	RemovedIsdsChannelL();	
	IRLOG_DEBUG( "CIRHistoryListContainer::ErrorL - Exiting" );	
	return;
	}

// -----------------------------------------------------------------------------
// CIRHistoryListContainer::IsdsOrNot()
// Whether ISDS channel or not.
// -----------------------------------------------------------------------------
// 
TBool CIRHistoryListContainer::IsdsOrNot()
	{
	IRLOG_DEBUG( "CIRHistoryListContainer::IsdsOrNot - Entering" );
	TInt focusItem=iTreeListBox->FocusedItem();
	TInt isNode=iTreeListBox->IsNode( focusItem );
	TInt itemIndex = -1;
	TBool retValue = EFalse;
	if(!isNode)  
		{
		focusItem=iTreeListBox->Parent(focusItem);	
		}
	for(TInt index=0;index<iUrlArray->Count();index++)
		{
		if(index>=0 && index<iNodeArray.Count())
			{
			if(focusItem==iNodeArray[index])
			itemIndex=index;
			}
		}
   	if(itemIndex >=0 && itemIndex<iChannelTypeArray.Count())
	    {
	    if(iChannelTypeArray[itemIndex]==1)
		    {
		    retValue = ETrue;	
		    }
	    }
	IRLOG_DEBUG( "CIRHistoryListContainer::IsdsOrNot - Exiting" );
	return retValue;    
	}

// -----------------------------------------------------------------------------
// CIRHistoryListContainer::GetButtonDimmed()
// Whether Find in Shop Button is dimmed or not.
// -----------------------------------------------------------------------------
// 
TBool CIRHistoryListContainer::GetButtonDimmed()
	{
	return iDimmed;
   	}
// -----------------------------------------------------------------------------
// CIRHistoryListContainer::RemovedIsdsChannelL()
// To add or play a channel which has been removed from ISDS .
// -----------------------------------------------------------------------------
// 
void CIRHistoryListContainer::RemovedIsdsChannelL()
	{
	IRLOG_DEBUG( "CIRHistoryListContainer::RemovedIsdsChannelL - Entering" );
	if(iHistoryListView.iIsdsWrapper->GetErrorCode())
		{
		iHistoryListView.HistoryEngineInstance()->SyncHistory( iChannelSyncId );
		
		_LIT(KNA,"NA");
		CIRIsdsPreset* preset = CIRIsdsPreset::NewL();
		CleanupStack::PushL( preset );
		preset->SetName(iChannelArray->MdcaPoint(iChangedIndex));
		preset->SetUrlL( KNA, iUrlArray->MdcaPoint(iChangedIndex ), 0 );
		preset->SetUrlCount();
		preset->SetChannelType(0);
		preset->SetId(0);
		preset->SetShortDesc(iChannelDescriptionArray->MdcaPoint(iChangedIndex ));
	    preset->SetDescription(iChannelDescriptionArray->MdcaPoint(iChangedIndex ));
        TInt res = KErrNone;

		if(iAddFrmHistory)
			{
			iHistoryListView.iUi->iFavPresets->AddPresetL(*preset,res);
			if( res == KErrNoMemory )
				{
				DisplayErroronDBL();
				}
			else
				{
				iHistoryListView.iUi->DisplayInformationL( R_IRAPP_STATIONS_POPUP_SAVED,
				                                           iChannelArray->
				                                           MdcaPoint(iChangedIndex) );
				}
			CleanupStack::PopAndDestroy( preset );
		    iAddFrmHistory = EFalse ;
			}
		if(iListenFrmHistory)
			{
			
			// Set the channel as last played and currently playing
			*iHistoryListView.iNowPlayingWrapper->iNowPlayingPreset = *preset;
			CleanupStack::PopAndDestroy( preset );

			iHistoryListView.iUi->iNPPrevViewId = KIRHistoryListViewId;

			//connecting to server	
			iHistoryListView.iNowPlayingWrapper->SetCurrentConnectionSource(EIRHistoryAdhoc);
			iHistoryListView.iNowPlayingWrapper->ConnectToChannelL(*iHistoryListView.
			                                 iNowPlayingWrapper->iNowPlayingPreset);
			
		    iListenFrmHistory = EFalse ;
			}
		iHistoryListView.RefreshHistoryL();  
		}
	IRLOG_DEBUG( "CIRHistoryListContainer::RemovedIsdsChannelL - Exiting" );
	}
                                                
                                                

