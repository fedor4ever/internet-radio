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
* Description:  view class for IRHistoryListView.cpp
*
*/

#include <stringloader.h>
#include <internetradio.rsg>

#include "irui.h"
#include "ir.hrh"
#include "ircommon.h"
#include "irdebug.h"
#include "irbaseview.h"
#include "irhistorylistview.h"
#include "irhistorylistcontainer.h"
#include "irapplication.h"
#include "irsonghistoryinfo.h"
#include "irsonghistoryengine.h"
#include "irnowplayingwrapper.h"
#include "irisdswrapper.h"
#include <hlplch.h>

//For Touch Toolbar
#include <internetradio.mbg>
#include <akntoolbar.h>
#include <aknbutton.h>

#ifdef MUSICSHOP_AVAILABLE
#include <mpxfindinmusicshop.h> //For Find in Shop
#include <apgcli.h>             //For Find in Shop
#endif

//to be removed after UT
#include "irmetadata.h"
#include "irdialoglauncher.h"

#include "irnetworkcontroller.h" 	
#include "iractivenetworkobserver.h"


// ---------------------------------------------------------------------------
// C++ default constructor can NOT contain any code, that might leave.
// ---------------------------------------------------------------------------
//

CIRHistoryListView::CIRHistoryListView():iCallRequestPending(EFalse)
    {
    IRLOG_DEBUG( "CIRHistoryListView::CIRHistoryListView" );
    // Nothing here
    }

// ---------------------------------------------------------------------------
// EPOC default constructor can leave.
// ---------------------------------------------------------------------------
//

void CIRHistoryListView::ConstructL()
{
	IRLOG_DEBUG( "CIRHistoryListView::ConstructL - Entering" );
	CIRBaseView::ConstructL(R_IR_HISTORY);
	
	MAknsSkinInstance * skin=AknsUtils::SkinInstance () ;
	TRgb color;
	TInt skinColor=AknsUtils::GetCachedColor(skin, color, KAknsIIDQsnTextColors,
				                                      EAknsCIQsnTextColorsCG6 );
	if(color.Gray2()!=0)
		{
		CreateAndSetToolbarL(R_IR_HISTORY_WHITE_TOOLBAR);
		}
	else
		{
		CreateAndSetToolbarL(R_IR_HISTORY_BLACK_TOOLBAR);	
		}
	if( AknLayoutUtils::PenEnabled() && Toolbar() )
		{
		Toolbar()->SetToolbarObserver( this );
		}
	
	iHistoryEngine = CIRSongHistoryEngine::NewL(*this);
  	GetAllListL();

  	//For Find in Shop
  	iMPXFindInShop = NULL;
#ifdef MUSICSHOP_AVAILABLE  	
	TRAPD(error, iMPXFindInShop = CMPXFindInMShop::NewL());
    if (error != KErrNotFound && error != KErrNone)
    {
       User::Leave(error);
    }
#endif    

    iRequestPending = EFalse;
    iAddToFav= EFalse;
    iListenRequest = EFalse;
    IRLOG_DEBUG( "CIRHistoryListView::ConstructL - Exiting" );
}
// ---------------------------------------------------------------------------
// Static constructor.
// CIRHistoryListView::NewL
// ---------------------------------------------------------------------------
//
CIRHistoryListView* CIRHistoryListView::NewL()
    {
    IRLOG_DEBUG("CIRHistoryListView::NewL - Entering" );
    CIRHistoryListView* self = CIRHistoryListView::NewLC();
    CleanupStack::Pop(self);
    IRLOG_DEBUG( "CIRHistoryListView::NewL - Exiting." );
    return self;
    }
// ---------------------------------------------------------------------------
// Two-phased constructor.
// CIRHistoryListView::NewLC
// ---------------------------------------------------------------------------
//
CIRHistoryListView* CIRHistoryListView::NewLC()
    {
       
    IRLOG_DEBUG( "CIRHistoryListView::NewLC - Entering" );
    CIRHistoryListView* self = new (ELeave) CIRHistoryListView();
    CleanupStack::PushL(self);
    self->ConstructL();
    IRLOG_DEBUG( "CIRHistoryListView::NewLC - Exiting." );
    return self;
    }


// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//

CIRHistoryListView::~CIRHistoryListView()
	{
	IRLOG_DEBUG( "CIRHistoryListView::~CIRHistoryListView - Entering" );
    if ( iContainer )
        {
        	AppUi()->RemoveFromViewStack( *this, iContainer );
        	iContainer->MakeVisible( EFalse );
        	delete iContainer;
        	iContainer=NULL;
        }

    iHistoryDataArr.ResetAndDestroy();

    if(iHistoryEngine)
        {
        	delete iHistoryEngine;
        	iHistoryEngine = NULL;
        }

#ifdef MUSICSHOP_AVAILABLE
    if (iMPXFindInShop)
		{
			delete iMPXFindInShop;
			iMPXFindInShop = NULL;
		}
#endif

	if( AknLayoutUtils::PenEnabled() && Toolbar() )
		{
		Toolbar()->SetToolbarVisibility(EFalse, EFalse);
		Toolbar()->SetToolbarObserver( NULL );
		}
    IRLOG_DEBUG( "CIRHistoryListView::~CIRHistoryListView - Exiting" );
	}

// ---------------------------------------------------------------------------
// From class CAknView.
// Returns view id.
// ---------------------------------------------------------------------------
//

TUid CIRHistoryListView::Id() const
    {
    IRLOG_DEBUG( "CIRHistoryListView::Id" );
    return KIRHistoryListViewId;
    }

// ---------------------------------------------------------------------------
// From class CAknView.
// Handles view activation.
// ---------------------------------------------------------------------------
//

void CIRHistoryListView::DoActivateL( const TVwsViewId& aPrevViewId,
        TUid aCustomMessageId, const TDesC8& aCustomMessage )
    {
    IRLOG_DEBUG( "CIRHistoryListView::DoActivateL - Entering" );
    iUi->iPresentViewID = KIRHistoryListViewId;
    CIRUi* appUi = static_cast<CIRUi*>( AppUi() );
     if ( !iContainer )
		{
		iContainer = CIRHistoryListContainer::NewL(ClientRect(),*this,iHistoryDataArr );
		iContainer->SetMopParent( this );
		appUi->AddToViewStackL( *this, iContainer );
		}
    else
       {
		// we must notify a resource change in order to update the skin context
		// This is a flaw of CEikFormattedCellListBox
			iContainer->SetRect(ClientRect());
        	iContainer->HandleResourceChange(KEikDynamicLayoutVariantSwitch);
       }

    ConstructToolbarL();
    iContainer->ActivateL();
    iContainer->MakeVisible( ETrue );
    SetTitleL( R_IRAPP_HISTORY_TITLE );
    CIRBaseView::DoActivateL( aPrevViewId, aCustomMessageId, aCustomMessage );
    HandleStatusPaneSizeChange();
    iUi->iNetworkController->RegisterActiveNetworkObserverL(*this);
    EnableObserverL(EIRSystemEventObserver);
    IRLOG_DEBUG( "CIRHistoryListView::DoActivateL - Exiting" );
    }

// ---------------------------------------------------------------------------
// From class CAknView.
// Handles view deactivation.
// ---------------------------------------------------------------------------
//

void CIRHistoryListView::DoDeactivate()
   {
   IRLOG_DEBUG( "CIRHistoryListView::DoDeactivate - Entering" );
    CIRUi* appUi = static_cast<CIRUi*>( AppUi() );
    if(&(appUi->SystemEventCollector()))
	    {
		DisableObserver(EIRSystemEventObserver);
	    }

    if ( iContainer )
        {
        appUi->RemoveFromViewStack( *this, iContainer );
        iContainer->MakeVisible( EFalse );
        delete iContainer;
    	iContainer  = NULL;
        }

	if( AknLayoutUtils::PenEnabled() && Toolbar() )
		{
		Toolbar()->SetToolbarVisibility(EFalse, EFalse);
		Toolbar()->SetToolbarObserver( NULL );
		}
    iUi->iNetworkController->DeleteActiveNetworkObserver(*this);
    IRLOG_DEBUG( "CIRHistoryListView::DoDeactivate - Exiting" );
   }


// ---------------------------------------------------------------------------
// From class CAknView.
// Command handling function.
// ---------------------------------------------------------------------------
//

void CIRHistoryListView::HandleCommandL( TInt aCommand )
    {
    IRLOG_DEBUG( "CIRHistoryListView::HandleCommandL - Entering" );
	iCommand = aCommand;
    switch ( aCommand )
        {
       case EAknSoftkeyOptions:
            {

            Cba()->DrawDeferred();
            break;
            }

        case EAknSoftkeyCancel:
            {
            break;
            }

        case EClearCmd:
			{
			
			RBuf sumvalue;
			HBufC* textResource = StringLoader::LoadLC( R_IRAPP_HISTORY_QUERY );
			sumvalue.Create(*textResource);
			CIRDialogLauncher* queryNote = CIRDialogLauncher::NewL();
			CleanupStack::PushL( queryNote );
			TInt dlgValue;
			queryNote->ShowQueryDialogDeletionL(sumvalue,dlgValue);
			if(dlgValue)
				{
				ClearSongHistoryItemL();
				}
			CleanupStack::PopAndDestroy( queryNote );
			CleanupStack::PopAndDestroy( textResource );
			sumvalue.Close();
			Toolbar()->DrawNow();

			break;

			}
        case EHelpCmd:
            {
			if ( !iUi->CheckAndNotifyLowDiskSpaceL() )
				{
				HlpLauncher::LaunchHelpApplicationL( iEikonEnv->WsSession(), AppUi()->AppHelpContextL() );
				}
          		break;
            }

        case EGotoNowPlayingViewCmd:
            {
            	iUi->SetHistory(ETrue);
			    iUi->SavePrevIDL(KIRHistoryListViewId,aCommand);
			    break;
            }

        case EListenCmd:
            {
			if ( !iUi->CheckAndNotifyLowDiskSpaceL() )
				{
				iUi->SetListenFromPls(EFalse);
				if( iUi->VerifyNetworkConnectivityL() )
					{
					iRequestPending = ETrue;
					iListenRequest  = ETrue;
					iAddToFav=EFalse;
					break;
					}
				else
					{
	                iNowPlayingWrapper->SetView(EHistoryView);
					TInt index=iContainer->GetCurrentFocussedNode();
					iContainer->ListenUrlL(index );
					}
				}
		        break;
            }

        case ESettingsCmd:
            {
		        AppUi()->ActivateLocalViewL(KIRSettingsViewId);
           	    break;
            }

        case EAddtoFavouritesCmd:
            {
				if ( iUi->CheckAndNotifyLowDiskSpaceL() )
					{
					break;
					}
				
			   if(iContainer->IsdsOrNot())
					{
					if( iUi->VerifyNetworkConnectivityL() )
						{
						iRequestPending = ETrue;
						iAddToFav = ETrue;
						iListenRequest=EFalse;
						break;
						}
					else 
						{
						iAddToFav = ETrue;
	                    iContainer->AddStationToFavoritesL(aCommand);
						}
					}
				else
					{
					iContainer->AddStationToFavoritesL(aCommand);
					}
			            
            }
        break;    
        case EFindInShop:
        	{
			if ( !iUi->CheckAndNotifyLowDiskSpaceL() )
				{
				_LIT(KFind, "find");
	        	FindInShopOptionL();
	        	 
	        	/**
	        	 * Increment the Find In Store count by One
	        	 */
	        	iUi->FindInMusicStoreCounter();
	        	iNowPlayingWrapper->UpdateNmsLogEventsL(KFind);
				}
        	}
        break;
       	case EAddStationManuallyCmd:
			{
			if ( !iUi->CheckAndNotifyLowDiskSpaceL() )
				{
				iUi->SetEditStation(EFalse);
				AppUi()->ActivateLocalViewL(KIRAddManuallyStationViewID);
				}
			}
		break;

        default:
            {
            CIRBaseView::HandleCommandL( aCommand );
            break;
            }
        }
    IRLOG_DEBUG( "CIRHistoryListView::HandleCommandL - Exiting" );
    }



// ---------------------------------------------------------------------------
// From class CAknView.
// Event handler for status pane size changes.
// ---------------------------------------------------------------------------
//
void CIRHistoryListView::HandleStatusPaneSizeChange()
    {
    IRLOG_DEBUG( "CIRHistoryListView::HandleStatusPaneSizeChange - Entering" );
    // the client rect is also set here after screen orientation change
    if ( iContainer )
        {
        iContainer->SetRect( ClientRect() );
        }
    IRLOG_DEBUG( "CIRHistoryListView::HandleStatusPaneSizeChange - Exiting" );
    }

// ---------------------------------------------------------------------------
// CIRHistoryListView::NextSongReceivedL()
// Adds the currently playing song to the history's array.
// ---------------------------------------------------------------------------
//

void CIRHistoryListView::NextSongReceivedL( const TDesC& /*aSongName*/,
                                            const TDesC& /*aArtistName*/,
                                            const TDesC& /*aChannelName*/, 
                                            const TDesC& /*aChannelUrl*/ )
{
	IRLOG_DEBUG( "CIRHistoryListView::NextSongReceivedL - Entering" );
	iHistoryDataArr.ResetAndDestroy();

	if(iContainer)
	{
		iContainer->ClearHistoryListL();
	}

	GetAllListL();
	if(iContainer)
	{
		iContainer->UpdateAllL(iHistoryDataArr);
	}
	IRLOG_DEBUG( "CIRHistoryListView::NextSongReceivedL - Exiting" );
}



// ---------------------------------------------------------------------------
// CIRHistoryListView::ChannelChangedL()
// Moves the currently playing channel to the top.
// ---------------------------------------------------------------------------
//
void CIRHistoryListView::ChannelChangedL( const TDesC& /*aSongName*/,const TDesC& /*aArtistName*/,
		 const TDesC& /*aChannelName*/, const TDesC& /*aChannelUrl*/ )
{
	IRLOG_DEBUG( "CIRHistoryListView::ChannelChangedL - Entering" );
	iHistoryDataArr.ResetAndDestroy();

	if(iContainer)
	{
		iContainer->ClearHistoryListL();
	}

	GetAllListL();
	if(iContainer)
	{
		iContainer->UpdateAllL(iHistoryDataArr);
	}
	IRLOG_DEBUG( "CIRHistoryListView::ChannelChangedL - Exiting" );
}


// ---------------------------------------------------------------------------
// CIRHistoryListView::RemoveSongEntry()
// Removes the particular  from the List Array
// ---------------------------------------------------------------------------
//
TInt CIRHistoryListView::RemoveSongEntry(const TDesC & /*aSongName*/,
                                         const TDesC & /*aArtistName*/, 
                                         const TDesC & /*aChannelName*/,
                                         const TDesC& /*aChannelUrl*/)
 {
 	IRLOG_DEBUG( "CIRHistoryListView::RemoveSongEntry - Entering" );
    IRLOG_DEBUG( "CIRHistoryListView::RemoveSongEntry - Exiting" );
	return KErrNone;
 }


// ---------------------------------------------------------------------------
// CIRHistoryListView::RemoveChannelEntry()
// ---------------------------------------------------------------------------
//
TInt CIRHistoryListView::RemoveChannelEntry( const TDesC& /*aSongName*/, 
                                             const TDesC& /*aArtistName*/,
                                             const TDesC& /*aChannelName*/,
                                             const TDesC& /*aChannelUrl*/ )
{
	IRLOG_DEBUG( "CIRHistoryListView::RemoveChannelEntry" );
	// No implementation required for DoubleStyleListbox as no seperate channel entries exist to be deleted.
	return KErrNone;
}


// ---------------------------------------------------------------------------
// CIRHistoryListView::NotifyDataChangeToUIError()
// Not Implemented
// ---------------------------------------------------------------------------
//
void CIRHistoryListView::NotifyDataChangeToUIError(TInt /*aError*/)
{
	IRLOG_DEBUG( "CIRHistoryListView::NotifyDataChangeToUIError" );
}

// ---------------------------------------------------------------------------
// CIRHistoryListView::ClearSongHistoryItemL()
// Clears the Song History array
// ---------------------------------------------------------------------------
//
void CIRHistoryListView::ClearSongHistoryItemL()
{
	IRLOG_DEBUG( "CIRHistoryListView::ClearSongHistoryItemL - Entering" );
	if(iHistoryEngine->ClearAllHistory() != KErrNone)
	{
		iHistoryDataArr.ResetAndDestroy();
		iContainer->ClearHistoryListL();
		Toolbar()->SetItemDimmed(EFindInShop,ETrue,ETrue);
		Toolbar()->SetItemDimmed(EListenCmd,ETrue,ETrue);
		GetAllListL();
		if(iContainer)
		{
			iContainer->UpdateAllL(iHistoryDataArr);
		}
		IRLOG_DEBUG( "CIRHistoryListView::ClearSongHistoryItemL - Exiting" );
		return;
	}
	iHistoryDataArr.ResetAndDestroy();

	if(iContainer)
	{
		iContainer->ClearHistoryListL();
		Toolbar()->SetItemDimmed(EFindInShop,ETrue,ETrue);
		Toolbar()->SetItemDimmed(EListenCmd,ETrue,ETrue);
		iContainer->UpdateAllL(iHistoryDataArr);
	}
	IRLOG_DEBUG( "CIRHistoryListView::ClearSongHistoryItemL - Exiting" );
}


// ---------------------------------------------------------------------------
// CIRHistoryListView::GetAllList()
// gets the List which was stored earlier
//---------------------------------------------------------------------------
void CIRHistoryListView::GetAllListL()
{
	IRLOG_DEBUG( "CIRHistoryListView::GetAllListL - Entering" );
	TInt songCount = iHistoryEngine->GetHistoryCount();

	if(songCount == 0)
		{
		IRLOG_DEBUG( "CIRHistoryListView::GetAllListL - Exiting" );
		return;
		}

	for(TInt arrCount=0 ; arrCount<songCount ; arrCount++ )
		{
		CIRSongHistoryInfo *songHistory = CIRSongHistoryInfo::NewL();
		CleanupStack::PushL( songHistory );
		iHistoryDataArr.Append(songHistory);
		CleanupStack::Pop(songHistory);
		}

	//to be implemented after getting the engine class..
	iHistoryEngine->GetAllHistoryL(iHistoryDataArr);
	IRLOG_DEBUG( "CIRHistoryListView::GetAllListL - Exiting" );
}

// ---------------------------------------------------------------------------
// CIRHistoryListView::DynInitMenuPaneL()
// Dynamically handles the Menu Pane
//---------------------------------------------------------------------------
void CIRHistoryListView::DynInitMenuPaneL(TInt aResourceId,CEikMenuPane* aMenuPane)
	{
	IRLOG_DEBUG( "CIRHistoryListView::DynInitMenuPaneL - Entering" );
	if(aResourceId == R_IR_MENU_HISTORY)
		{
		if(!aMenuPane) User::Leave(KErrNotFound);
			{
			if (  iHistoryDataArr.Count() == 0 )
				{
				aMenuPane->SetItemDimmed( EClearCmd, ETrue );
				}
			else
				{
				aMenuPane->SetItemDimmed( EClearCmd, EFalse );
				}
			// Now Playing option
			if(iNowPlayingWrapper->iPlaying)
				{
				aMenuPane->SetItemDimmed(EGotoNowPlayingViewCmd,EFalse);
				}
			else
				{
				aMenuPane->SetItemDimmed(EGotoNowPlayingViewCmd,ETrue);
				}
            if(iContainer->ShowFocus()|| !(iHistoryDataArr.Count() ))
	            {
	            aMenuPane->SetItemDimmed(EAddtoFavouritesCmd,ETrue);
	            }
            else
	            {
	            aMenuPane->SetItemDimmed(EAddtoFavouritesCmd,EFalse);
	            }
			}

		}
	IRLOG_DEBUG( "CIRHistoryListView::DynInitMenuPaneL - Exiting" );

	}
// ---------------------------------------------------------------------------
// CIRHistoryListView::HandleMetaDataReceived()
// Handles the change in metadata.
//---------------------------------------------------------------------------

void CIRHistoryListView::HandleMetaDataReceived( const CIRMetaData& aMetaData )
{
	IRLOG_DEBUG( "CIRHistoryListView::HandleMetaDataReceived - Entering" );
	if( iHistoryEngine )
	{
		TRAPD( error, iHistoryEngine->HandleMetaDataReceivedL( aMetaData ) );
		if( error )
		{
			IRLOG_DEBUG( "CIRHistoryListView::HandleMetaDataReceivedL - Failed." );
		}
	}
	IRLOG_DEBUG( "CIRHistoryListView::HandleMetaDataReceived - Exiting" );
}


// ---------------------------------------------------------------------------
// CIRHistoryListView::ConstructToolbarL()
// Constructs Touch Toolbar
//---------------------------------------------------------------------------

void CIRHistoryListView::ConstructToolbarL()
	{
	IRLOG_DEBUG( "CIRHistoryListView::ConstructToolbarL - Entering" );
    if(Toolbar())
	    {
		if (  iHistoryDataArr.Count() == 0 )
			{
			Toolbar()->SetItemDimmed(EFindInShop,ETrue,ETrue);
			Toolbar()->SetItemDimmed(EListenCmd,ETrue,ETrue);
			}

		if(iMPXFindInShop)
			{
			if(iContainer->GetButtonDimmed())
				{
				Toolbar()->SetItemDimmed(EFindInShop,ETrue,ETrue);
				}
			}
		else
			{
			Toolbar()->SetItemDimmed(EFindInShop,ETrue,ETrue);
			}
		Toolbar()->SetToolbarVisibility( ETrue );
		if(iContainer)
			{
		    iContainer->SetRect( ClientRect() );
		    iContainer->DrawDeferred();
			}
	    }
	IRLOG_DEBUG( "CIRHistoryListView::ConstructToolbarL - Exiting" );
}

// --------------------------------------------------------------------------
// CIRHistoryListView::DynInitToolbarL()
// Dynamically Updates the toolbar
// --------------------------------------------------------------------------
//
void CIRHistoryListView::DynInitToolbarL( TInt /*aResourceId*/, CAknToolbar* /*aToolbar*/ )
{
	IRLOG_DEBUG( "CIRHistoryListView::DynInitToolbarL - Entering" );
	
	IRLOG_DEBUG( "CIRHistoryListView::DynInitToolbarL - Exiting" );
}

// --------------------------------------------------------------------------
// CIRHistoryListView::OfferToolbarEventL
// Handles key events
// --------------------------------------------------------------------------
//
void CIRHistoryListView::OfferToolbarEventL( TInt aCommand )
	{
	IRLOG_DEBUG( "CIRHistoryListView::OfferToolbarEventL - Entering" );
	HandleCommandL(aCommand);
	IRLOG_DEBUG( "CIRHistoryListView::OfferToolbarEventL - Exiting" );
	}




// ---------------------------------------------------------------------------
// void CIRHistoryListView::LaunchMusicShopL()
// To Launch Find in Shop Option
// ---------------------------------------------------------------------------
//
void CIRHistoryListView::LaunchMusicShopL()
	{
#ifdef MUSICSHOP_AVAILABLE
	  IRLOG_DEBUG( "Entered CIRHistoryListView::LaunchMusicShopL() Entering" );

	  TApaTaskList tasList( iCoeEnv->WsSession() );
	  TApaTask task = tasList.FindApp( KUidMusicStore );

	  if ( task.Exists() )
		  {
		    task.BringToForeground();
		  }
	  else
		  {
		    RApaLsSession session;
		    if ( KErrNone == session.Connect() )
			    {
			      CleanupClosePushL( session );
			      TThreadId threadId;
			      session.CreateDocument( KNullDesC, KUidMusicStore, threadId );
			      CleanupStack::PopAndDestroy(&session);
			    }
		  }

	  IRLOG_DEBUG( "Leaving CIRHistoryListView::LaunchMusicShopL() Exiting" );
#endif
   }

// ---------------------------------------------------------------------------
// CIRHistoryListView::DoFindInMusicShopL()
// To Launch Find in Shop Option
// ---------------------------------------------------------------------------
//
void CIRHistoryListView::DoFindInMusicShopL( const TDesC& aSongName,
                                             const TDesC& aArtistName,
                                             const TDesC& aAlbumName )
	{
	IRLOG_DEBUG( "Entered CIRHistoryListView::DoFindInMusicShopL() - Entering" );
#ifdef MUSICSHOP_AVAILABLE
	HBufC* url = NULL;
	url = iMPXFindInShop->CreateSearchURLL( aSongName,
	             							 aArtistName,
	             							 aAlbumName,
	             							 KNullDesC,     // Composer - Not used
	              							 KNullDesC );   // Genre - Not used
	CleanupStack::PushL(url);   // the pointer will be poped
	RProperty::Set( KUidMusicStore,
	KMShopCategoryId,
	KFindInMShopKeyValid );  // Set Key to Valid

	RProperty::Set( KUidMusicStore,
	KMShopCategoryName,
	*url );
	LaunchMusicShopL();
	if(url)
		{
		CleanupStack::PopAndDestroy(url);
		}
	REComSession::FinalClose();
#else
    if(aSongName.Length() > 0 || aArtistName.Length() > 0 || aAlbumName.Length() > 0)
        {
        // do nothing, just remove build warnings.
        }
	IRLOG_DEBUG( "Leaving CIRHistoryListView::DoFindInMusicShopL() - Exiting" );
#endif
	}

// ---------------------------------------------------------------------------
// CIRHistoryListView::FindInShopOption()
// To Launch Find in Shop Option when selected from the toolbar
// ---------------------------------------------------------------------------
//

void CIRHistoryListView:: FindInShopOptionL()
	{
	IRLOG_DEBUG( "CIRHistoryListView::FindInShopOptionL - Entering" );
	TInt itemIndex = iContainer->GetCurrentFocussedLeaf();
	if(itemIndex==-1)
		{
	    IRLOG_DEBUG( "CIRHistoryListView::FindInShopOptionL - Exiting" );
		return;
		}
	if(itemIndex>=0 && itemIndex<iHistoryDataArr.Count())
		{	
		DoFindInMusicShopL(iHistoryDataArr[itemIndex]->GetSongInfo(),
		iHistoryDataArr[itemIndex]->GetArtistInfo(),
		KNullDesC);
		}
	IRLOG_DEBUG( "CIRHistoryListView::FindInShopOptionL - Exiting" );
	}
// ---------------------------------------------------------------------------
//Notifies all observers whose network request is active
// to reissue the request  
//NotifyActiveNetworkObserversL()
// ---------------------------------------------------------------------------

void CIRHistoryListView::NotifyActiveNetworkObserversL(TIRNetworkEvent aEvent)
	{
	IRLOG_DEBUG( "CIRHistoryListView::NotifyActiveNetworkObserversL - Entering" );
	
	switch(aEvent)
	{
	case ENetworkConnectionDisconnected:
		{
		iNowPlayingWrapper->HandleNetworkDisconnected();
		}
		break;
	case ENetworkConnectionEstablished:
		{
		if( !iUi->CheckAndNotifyLowDiskSpaceL() && iContainer )
			{
			if(iRequestPending || iUi->iNetworkController->IsHandingOverConnection())
				{
				if(iAddToFav)
					{
					iAddToFav=ETrue;
					iListenRequest=EFalse;
					iContainer->AddStationToFavoritesL(EAddtoFavouritesCmd);
					}
				else if(iListenRequest)
					{
					iAddToFav=EFalse;
					iListenRequest=EFalse;
					iNowPlayingWrapper->SetView(EHistoryView);
					TInt index=iContainer->GetCurrentFocussedNode();
					iContainer->ListenUrlL(index );
					}
				else
					{
						
					}
				}
			}
		if(iUi->iNetworkController->IsHandingOverConnection())
			{
			ResetPendingRequests(EFalse);
			}
		}
	}

	IRLOG_DEBUG( "CIRHistoryListView::NotifyActiveNetworkObserversL - Exiting" );
	}
	
	

// -----------------------------------------------------------------------------
// Notified by network controller when user cancels network connection, to reset 
// the pending requests  
// ResetPendingRequests()
// -----------------------------------------------------------------------------
void CIRHistoryListView::ResetPendingRequests(TBool aValue)
	{
	iRequestPending = aValue;
	iListenRequest  = aValue;
	iAddToFav = aValue;
	}

	
	
	
// ---------------------------------------------------------------------------
// From class CAknView.
// Foreground event handling function.
// ---------------------------------------------------------------------------
//
void CIRHistoryListView::HandleForegroundEventL( TBool aForeground )
    {
	IRLOG_DEBUG( "CIRHistoryListView::HandleForegroundEventL - Entering" );
	CIRBaseView::HandleForegroundEventL( aForeground );
   
	if ( aForeground )
		{
		if(iContainer)
			{
			MAknsSkinInstance * skin=AknsUtils::SkinInstance () ;
		    TRgb color;
		    TInt skinColor=AknsUtils::GetCachedColor(skin, color, KAknsIIDQsnTextColors,
		    				                                      EAknsCIQsnTextColorsCG6 );
			if(color.Gray2()!=0)
				{
				CreateAndSetToolbarL(R_IR_HISTORY_WHITE_TOOLBAR);
				}
			else
				{
				CreateAndSetToolbarL(R_IR_HISTORY_BLACK_TOOLBAR);	
				}
	        if( AknLayoutUtils::PenEnabled() && Toolbar() )
				{
				Toolbar()->SetToolbarObserver( this );
				}
			ConstructToolbarL();
			}
		}
	IRLOG_DEBUG( "CIRHistoryListView::HandleForegroundEventL" );
    }

// ---------------------------------------------------------------------------
// GetFindInShopInstance()
// Get the FindInShop Instance is null or not.
// ---------------------------------------------------------------------------
//
TBool CIRHistoryListView::GetFindInShopInstance()
	{
	TBool ret = EFalse;
	if(iMPXFindInShop)
		{
		ret = ETrue;	
		}
	IRLOG_DEBUG( "CIRHistoryListView::GetFindInShopInstance" );
	return ret;	
	}
// ---------------------------------------------------------
// CIRHistoryListView::HandleSystemEventL
// from base class MIRSystemEventObserver
// ---------------------------------------------------------
//
void CIRHistoryListView::HandleSystemEventL(TIRSystemEventType aEventType)
	{
	IRLOG_DEBUG(" CIRHistoryListView::HandleSystemEventL - Entering");
	
	switch (aEventType)
		{
		case EIRCallActivated:
			{
			if( iUi->GetNetworkControllerInstance()->IdentifyConnectionType() != EWiFi )
				{
				if(iUi->iIsdsWrapper->iIsdsWaitDialog)
					{
					TRAP_IGNORE( iUi->iIsdsWrapper->DestroyWaitDialogL() );
					}
				if(!iCallRequestPending && iCommand==EAddtoFavouritesCmd)
					{
					iUi->iIsdsWrapper->IsdsCancelRequest();
					iCallRequestPending = ETrue;
					}
				if(iUi->iIsdsWrapper->GetListenRequest())
					{
					iNowPlayingWrapper->DestroyWaitDialogL();
					iUi->iIsdsWrapper->SetListenRequest(EFalse);
					iUi->iIsdsWrapper->IsdsCancelRequest();
					}
				}
			break;
			}
		case EIRCallDeactivated:
			{
			if(iCallRequestPending && iCommand==EAddtoFavouritesCmd)
				{
				iCallRequestPending = EFalse;
				HandleCommandL(iCommand);
				}
			break;
			}
		default:
			{
			break;
			}
		}
	IRLOG_DEBUG( "CIRHistoryListView::HandleSystemEventL - Exiting." );
	}	
// ---------------------------------------------------------------------------
// RefreshHistoryL()
// Refreshes the History view while Synchronisation.
// ---------------------------------------------------------------------------
//
void CIRHistoryListView::RefreshHistoryL()
{
	IRLOG_DEBUG( "CIRHistoryListView::RefreshHistoryL - Entering" );
	iHistoryDataArr.ResetAndDestroy();

	if(iContainer)
	{
		iContainer->ClearHistoryListL();
	}

	GetAllListL();
	if(iContainer)
	{
		iContainer->UpdateAllL(iHistoryDataArr);
	}
	IRLOG_DEBUG( "CIRHistoryListView::RefreshHistoryL - Exiting" );
}
// ---------------------------------------------------------------------------
// HistoryEngineInstance()
// Gets the history Engine object Instance.
// ---------------------------------------------------------------------------
//
CIRSongHistoryEngine* CIRHistoryListView::HistoryEngineInstance()
{
	IRLOG_DEBUG( "CIRHistoryListView::HistoryEngineInstance - Exiting" );
	return iHistoryEngine ;
}
