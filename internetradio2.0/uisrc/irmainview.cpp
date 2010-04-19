/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Main View
*
*/
/*
* ============================================================================
*  Name        : irmainview.cpp
*  Part of     : InternetRadio
*  Description : Main View
*  Version    :
* ============================================================================
*
*/

/* ---------------------------------------------------------------------------
*  Version history:
*  Template version:
*  <ccm_history>
*
*  Version: 2, Tue Feb 28 18:00:00 2008 by Neelima
*  Ref:
*  Added Workaround in CIRMainView::HandleCommandL() Listen/Add to Favourates is implemented and the dummy functionality is removed
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
*  Version: 2, Sat Feb 18:12:00 2008 by Neelima
*  Ref:
*  Added Workaround in CIRMainView::Handlecommandl() for deleting the stations and necessary dialogs  .
*  and also handled necessary cases for add to favourites.
*  </ccm_history>
* ============================================================================
*/
/* ---------------------------------------------------------------------------
*  Version history:
*  Template version:
*  <ccm_history>
*
*  Version: 1, Thrus Feb 20 07:35:00 2008 by Neelima
*  Ref:
*  Added Workaround in CIRMainView::DynInitMenuPaneL() for displaying "Go to Now playing" & "update" options .
*  </ccm_history>
* ============================================================================
*/
/* ---------------------------------------------------------------------------
*  Version history:
*  Template version:
*  <ccm_history>
*
*  Version: 10, July 8 tue 18:44:00 2008 by Rohit
*  Implemented delayed activation for Station Information view in case of no network connectivity
*
*  Version: 9, May 28 wed 16:34:00 2008 by Rohit
*  Fixed bug to verify network before invoking Station Information view
*
*  Version: 8, May 5 wed 19:14:00 2008 by Rohit
*  Fixed bug to show Song Information view for last played station
*
*  Version: 7, Apr 30 wed 14:10:00 2008 by Rohit
*  Added handler to invoke Song Information view
*
*  Version: 6, April 29 tue 12:15:00 2008 by Neelima
*  Changed the container to CCoeControl and implemented dynamic filtering
*
*  Version: 5, April 18 wed 10:41:00 2008 by Neelima
*  Updated after pc-lint run
*
*  Version: 4, April 17 wed 08:06:00 2008 by Neelima
*  Ref:
*  Customisation of  mark/unmark
*
*  Version: 3, April 9 wed 19:12:00 2008 by Neelima
*  Ref:
*  Added Workaround in CIRMainContainer::DeleteMultiStationsL() the code for deleting the stations and necessary dialogs  .
*
*  Version: 2, Thrus Feb 20 08:52:00 2008 by Neelima
*  Ref:
*  Added Workaround in CIRMainView::Handlecommandl() Added marm/unmark commands and necessary functionality
*  also implemented delete for marked items
*  </ccm_history>
* ============================================================================
*/
/* ---------------------------------------------------------------------------
*  Version history:
*  Template version:
*  <ccm_history>
*
*  Version: 9, may 13 Wed 05:55:00 2008 by Neelima
* Implemeted Listen on dynamic filtered stations.
* ============================================================================
*/


#include <stringloader.h>
#include <hlplch.h>
#include <internetradio.rsg>
#include <internetradio.mbg>
//For Touch Toolbar
#include <akntoolbar.h>



#include "irui.h"
#include "ir.hrh"
#include "ircommon.h"
#include "irdebug.h"
#include "irbaseview.h"
#include "irmainview.h"
#include "irmaincontainer.h"
#include "irisdswrapper.h"
#include "irstationinfoview.h"
#include "irchannelserverurl.h"
#include "irhistory.h"
#include "irnowplayingview.h"
#include "irdialoglauncher.h"
#include "irnowplayingwrapper.h"
#include "irnetworkcontroller.h"
#include "irfilteredmodel.h"
#include "iradvertisinglistbox.h"
#include "msyncpresetobserver.h"	// MSyncPresetObserver
#include "irlogodownloadengine.h"
#include "irdataprovider.h"
#include "irhttpdataprovider.h"
#include "irpubsub.h"


const TInt KUrlbufMaxLength = 256;
const TInt KDummyLength = 10;
const TInt KNull = 0;
const TInt KIsdsPreset = 1;
const TInt KLPLogoSize = 59;
const TInt KLPReqFrom = 0;

// ---------------------------------------------------------------------------
// CIRMainView::CIRMainView()
// C++ default constructor can NOT contain any code, that might leave.
// ---------------------------------------------------------------------------
//
CIRMainView::CIRMainView()
    {
	IRLOG_DEBUG( "CIRMainView::CIRMainView - Entering" );
    SetCurrentFocus(0);
   	SetCurrentTopItem(0);
   	iMarkedFlag = EFalse;
   	iRequestPending = EFalse;
   	iFavListen = EFalse;
   	iStationDetailsCmd = EFalse;
   	iAddtoStationsCmd = EFalse;
	iMarkedAllFlag = EFalse;
	iMarkedFlag = EFalse;
	iMarkedItemCount = 0;
	iMarkedIndexArray.Reset();
	iUnMarkedArray.Reset();
	IRLOG_DEBUG( "CIRMainView::CIRMainView - Exiting" );
    }


// ---------------------------------------------------------------------------
// void CIRMainView::ConstructL()
// EPOC default constructor can leave.
// ---------------------------------------------------------------------------
//
void CIRMainView::ConstructL()
    {
	IRLOG_DEBUG( "CIRMainView::ConstructL - Entering" );
    CIRBaseView::ConstructL( R_IR_FAV );
	MAknsSkinInstance * skin=AknsUtils::SkinInstance () ;
	TRgb color;
	TInt skinColor=AknsUtils::GetCachedColor(skin, color, KAknsIIDQsnTextColors,
					                                      EAknsCIQsnTextColorsCG6 );
	if(color.Gray2()!=0)
		{
		CreateAndSetToolbarL(R_IR_MAINVIEW_WHITE_TOOLBAR);
		}
	else
		{
		CreateAndSetToolbarL(R_IR_MAINVIEW_BLACK_TOOLBAR);
		}
	if( AknLayoutUtils::PenEnabled() && Toolbar() )
		{
		Toolbar()->SetToolbarObserver( this );
		}
	IRLOG_DEBUG( "CIRMainView::ConstructL - Exiting" );
   	}
// -----------------------------------------------------------------------------
// CIRMainView::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CIRMainView* CIRMainView::NewL()
    {
 	IRLOG_DEBUG( "CIRMainView::NewL" );
    CIRMainView* self = CIRMainView::NewLC();
    CleanupStack::Pop( self );
    IRLOG_DEBUG( "CIRMainView::NewL - Exiting" );
    return self;
    }

// ---------------------------------------------------------------------------
// CIRMainView* CIRMainView::NewLC()
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CIRMainView* CIRMainView::NewLC()
    {
    IRLOG_DEBUG( "CIRMainView::NewLC - Entering" );
    CIRMainView* self = new( ELeave ) CIRMainView;
    CleanupStack::PushL( self );
    self->ConstructL();
    IRLOG_DEBUG( "CIRMainView::NewL - Exiting." );
    return self;
    }


// ---------------------------------------------------------------------------
// CIRMainView::~CIRMainView()
// Destructor
// ---------------------------------------------------------------------------
//
CIRMainView::~CIRMainView()
	{
	IRLOG_DEBUG( "CIRMainView::~CIRMainView - Entering" );
	if ( iContainer )
		{
		AppUi()->RemoveFromViewStack( *this, iContainer );
		delete iContainer;
		iContainer = NULL;
		}
	if( AknLayoutUtils::PenEnabled() && Toolbar() )
		{
		Toolbar()->SetToolbarVisibility(EFalse, EFalse);
		Toolbar()->SetToolbarObserver( NULL );
		}
	iMarkedIndexArray.Close();
	iUnMarkedArray.Close();
	IRLOG_DEBUG( "CIRMainView::~CIRMainView - Exiting" );
    }

// ---------------------------------------------------------------------------
// TUid CIRMainView::Id() const
// From class CAknView.
// Returns view id.
// ---------------------------------------------------------------------------
//
TUid CIRMainView::Id() const
    {
	IRLOG_DEBUG( "CIRMainView::Id" );
    return KIRMainChoiceViewID;
    }

// ---------------------------------------------------------------------------
// void CIRMainView::DoActivateL( )
// From class CAknView.
// Handles view activation.
// ---------------------------------------------------------------------------
//
void CIRMainView::DoActivateL( const TVwsViewId& aPrevViewId,
        TUid aCustomMessageId, const TDesC8& aCustomMessage )
	{
	IRLOG_DEBUG( "CIRMainView::DoActivateL - Entering" );
	iMarkedItemCount =0;
	iMarkedFlag = EFalse;
	iAddToFavouratesFlag = EFalse;

	if(GetCurrentFocus() == -1)
		{
		SetCurrentFocus(0);
		SetCurrentTopItem(0);
		}
	iUi->iFavPresets->AddObserver( *this );
	iUi->iPresentViewID = KIRMainChoiceViewID;
	if ( !iContainer )
		{
 	    iContainer = CIRMainContainer::NewL( *this, ClientRect() );
     	iContainer->CreateCustomListBoxL();
        AppUi()->AddToViewStackL( *this, iContainer);
        TInt index = GetCurrentFocus();
        if( aPrevViewId.iViewUid == KIRNowPlayingViewID )
	        {
	        if(iNowPlayingWrapper->GetFavFocus()!= GetCurrentFocus()
	        	&& iNowPlayingWrapper->GetFavFocus()>=0 )
		        {
		        index = iNowPlayingWrapper->GetFavFocus();
		        }
	        }
       iContainer->GetListPointer()->SetCurrentItemIndex(index);
       if( aPrevViewId.iViewUid != KIRAddManuallyStationViewID )
	       {
	       iContainer->HandleResourceChange(KEikDynamicLayoutVariantSwitch);
	       }
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
   	MenuBar()->SetContextMenuTitleResourceId(R_IR_MENUBAR_FAV);
   	SetTitleL( R_IRAPP_MAIN_MENU_TITLE);
	if(	iUi->GetPlsActiavted())
		{
		CIRBaseView::DoActivateL( aPrevViewId, aCustomMessageId, aCustomMessage );
		}
	else
		{
		CIRBaseView::DoActivateL( aPrevViewId, TUid::Uid(1), aCustomMessage );
		}
	HandleStatusPaneSizeChange();
	iUi->iNetworkController->RegisterActiveNetworkObserverL(*this);
	HandleCommandL(EUnmarkAll);
	IRLOG_DEBUG( "CIRMainView::DoActivateL - Exiting" );
	}

// ---------------------------------------------------------------------------
// void CIRMainView::DoDeactivate()
// From class CAknView.
// Handles view deactivation.
// ---------------------------------------------------------------------------
//
void CIRMainView::DoDeactivate()
    {
	IRLOG_DEBUG( "CIRMainView::DoDeactivate - Entering" );
	CIRBaseView::DoDeactivate();
	if ( iContainer )
		{
        if(iContainer->GetListPointer() && iContainer->GetSearchBoxPointer())
		    {
		    TBuf<KDummyLength> findstring;
			iContainer->GetSearchBoxPointer()->GetSearchText(findstring);
			if( findstring.Length()!=0)
				{
				SetCurrentFocus(iContainer->GetCurrentItemIndex());
				if(GetCurrentFocus() != -1)
					{
					iContainer->Comparestrings();
					SetCurrentFocus(iFilteredIndex);
					SetCurrentTopItem(iFilteredIndex);
					}
				}
			else
				{
				SetCurrentFocus(iContainer->GetCurrentItemIndex());
				SetCurrentTopItem(iContainer->GetListBoxTopIndex());
				}
			}
		iUi->RemoveFromStack(iContainer );
		delete iContainer;
		iContainer = NULL;
		}
	if( AknLayoutUtils::PenEnabled() && Toolbar() )
		{
		Toolbar()->SetToolbarVisibility(EFalse, EFalse);
		Toolbar()->SetToolbarObserver( NULL );
		}
	iMarkedIndexArray.Close();
	iUnMarkedArray.Close();
    iUi->iNetworkController->DeleteActiveNetworkObserver(*this);

	IRLOG_DEBUG( "CIRMainView::DoDeactivate - Exiting" );
    }

// ---------------------------------------------------------------------------
// void CIRMainView::HandleCommandL( TInt aCommand )
// From class CAknView.
// Command handling function.
// ---------------------------------------------------------------------------
//
void CIRMainView::HandleCommandL( TInt aCommand )
	{
	IRLOG_DEBUG( "CIRMainView::HandleCommandL - Entering" );
	switch ( aCommand )
		{
		case EMark:
			{
			TInt currentIndex = iContainer->GetCurrentItemIndex();
			TInt index = 0;
			if(currentIndex <=iUi->iFavPresets->iFavPresetList.Count())
				{
				if(iUi->iLastPlayed->FileExists())
					{
					if(currentIndex ==0)
						{
						index = iUi->iFavPresets->iFavPresetList[currentIndex]->Index();
						}
					else
						{
						index = iUi->iFavPresets->iFavPresetList[currentIndex-1]->Index();
						}
					}
				else
					{
					index = iUi->iFavPresets->iFavPresetList[currentIndex]->Index();
					}
				}
			if(currentIndex !=-1)
				{
				if(currentIndex ==0 &&iUi->iLastPlayed->FileExists())
					{
					// if the current item index is Zero and the Last palyed exists i.e., it is the lastplayed station then do nothing on mark/unmark.
					}
				else
					{
					iContainer->cmdMarkL(currentIndex,ETrue);
					iMarkedFlag = ETrue;
					}
				}

			if (index != KErrNotFound )
				{
				if(iMarkedIndexArray.Count())
					{
					currentIndex = GetCurrentIndex();
					for(TInt i=0;i<iMarkedIndexArray.Count();i++)
						{
						// check whether the marked index already exists in the marked array.
						if(iMarkedIndexArray[i]==currentIndex)
							{
							iIndexExisting = ETrue;
							}
						}
					}
				currentIndex = iContainer->GetCurrentItemIndex();
				if(!(currentIndex ==0 &&iUi->iLastPlayed->FileExists()))
					{
					if(iIndexExisting)
						{
						//If the index is already marked do nothing.
						}
					else
						{
						//If the index is not marked before then insert that into marked array.
						currentIndex = GetCurrentIndex();
						iMarkedIndexArray.InsertInOrderL(currentIndex);
						(iMarkedItemCount)++ ;
						}
					}
				}
			if(iUnMarkedArray.Count())
				{
				for(TInt i=0;i<iUnMarkedArray.Count();i++)
					{
					if(iUnMarkedArray[i]==currentIndex)
						{
						iUnMarkedArray.Remove(i);
						}
					}
				}
			iIndexExisting = EFalse;
			SetToolbarItems();
			SetCurrentFocus(currentIndex);
			}
		break;
		case EMarkAll:
			{
			iMarkedAllFlag = ETrue;
			iMarkedIndexArray.Reset();
			if(iUi->iLastPlayed->FileExists())
				{
				// If Last Played station exists then mark all the stations in the favourate list except the Last played.
				iContainer->cmdMarkL(0,ETrue);
				}
			else
				{
				// If Last Played station doesn't exist, then mark all the stations in the favourite list .
				iContainer->cmdMarkL(-1,ETrue);
				}
			TBuf<KDummyLength> findString ;
			// if elements in the listbox r not filtered
			if(iContainer->GetSearchBoxPointer())
				{
				iContainer->GetSearchBoxPointer()->GetSearchText(findString);
				}
			// If the elements in the listbox are not filtered
			if((findString.Length()==0))
				{
				TInt listboxCount ;
				// If last played exists
				if(iUi->iLastPlayed->FileExists())
					{
					// Except Last played mark all the elements in the favourite list
					// and  all the marked elements are to be inserted into the array.
					listboxCount = iContainer->GetListBoxCount()-1;
					}
				else
					{
					// If last played doesn't exist Mark all the elements in the list and all the
					// elements are to be inserted into the array.
					listboxCount = iContainer->GetListBoxCount();
					}
				for(TInt i=0;i<listboxCount;i++)
					{
					iMarkedIndexArray.InsertInOrderL(i);
					}
				iMarkedItemCount = iContainer->GetListPointer()->Model()->NumberOfItems();
				}
			// If the elements in the listbox are filtered
			else
				{
				iContainer->FilteredIndexL();
				iMarkedItemCount = iMarkedIndexArray.Count();
				}
			SetToolbarItems();
			SetCurrentFocus(iContainer->GetCurrentItemIndex());
			}
		break;
		case EUnmark:
			{
			TInt currentIndex = iContainer->GetCurrentItemIndex();
			if(currentIndex != -1)
				{
				if(currentIndex ==0 &&iUi->iLastPlayed->FileExists())
					{
					iContainer->cmdMarkL(currentIndex,EFalse);
					}
				else
					{
					iContainer->cmdMarkL(currentIndex,EFalse);
					}
				currentIndex = GetCurrentIndex();
				for(TInt j=0;j<iMarkedIndexArray.Count();j++)
					{
					// check which element of marked array is unmarked and remove from the marked array.
					if(iMarkedIndexArray[j] ==currentIndex )
						{
						iMarkedIndexArray.Remove(j);
						}
					}
				if(iMarkedAllFlag)
					{
					iUnMarkedArray.InsertInOrderL(currentIndex);
					}
				(iMarkedItemCount)-- ;
				}
			if(iMarkedIndexArray.Count()==0)
				{
				iMarkedFlag = EFalse;
				iMarkedAllFlag = EFalse;
				iContainer->SetContextSenstiveFlag(EFalse) ;
				}
			SetToolbarItems();
			SetCurrentFocus(currentIndex);
			}
		break;
		case EUnmarkAll:
			{
			iMarkedAllFlag = EFalse;
			iMarkedFlag = EFalse;
			// If Last played Exists
			if(iUi->iLastPlayed->FileExists())
				{
				iContainer->cmdMarkL(0,EFalse);
				}
			// If last played doesn't exist
			else
				{
				iContainer->cmdMarkL(-1,EFalse);
				}
			iMarkedItemCount = 0;
			iMarkedIndexArray.Reset();
			iUnMarkedArray.Reset();
			iContainer->SetContextSenstiveFlag(EFalse) ;
			SetToolbarItems();
		//	SetCurrentFocus(iContainer->GetCurrentItemIndex())
			}
		break;
		case EAknSoftkeyOptions:
			{
			Cba()->DrawDeferred();
			break;
			}
		case EListenCmd:
			{
			if( iUi->CheckAndNotifyLowDiskSpaceL() )
				{
				break;
				}

			iUi->SetListenFromPls(EFalse);
			if(iContainer->GetListBoxCount()!=-1)
				{
				SetCurrentFocus(iContainer->GetCurrentItemIndex());
				}

			if( iUi->VerifyNetworkConnectivityL() )
				{
				iRequestPending = ETrue;
				iListenRequest = ETrue;
				break;
				}

			ConnectToSelectedStationL();
			break;
			}
		case ESelectCmd:
			{
			break;
			}
		case EFindStationsCmd:
			{
			if( iUi->CheckAndNotifyLowDiskSpaceL() )
				{
				break;
				}

			if(iContainer->GetListBoxCount()!=-1)
				{
				if((iContainer->GetListBoxCount()!=0)&&(iContainer->GetCurrentItemIndex()
					 <= iContainer->GetListBoxCount()))
					{
					SetCurrentFocus(iContainer->GetCurrentItemIndex());
					}
				}
			TBuf8<2> dummy;
			TVwsViewId viewId(TUid::Uid(KUIDIRAPP),KIRFirstTimeViewId);
			AppUi()->ActivateViewL(viewId ,TUid::Uid(1),dummy);
			break;
			}
		case EHistoryCmd:
			{
			if(iContainer->GetListBoxCount()!=-1)
				{
				if((iContainer->GetListBoxCount()!=0)&&(iContainer->GetCurrentItemIndex()
					 <= iContainer->GetListBoxCount()))
					{
					SetCurrentFocus(iContainer->GetCurrentItemIndex());
					}
				}
			AppUi()->ActivateLocalViewL( KIRHistoryListViewId );
			break;
			}
		case EDeleteCmd:
			{
			TInt dlgValue ;
			if(iContainer->GetCurrentItemIndex()!=-1)
				{
				RBuf deleteStation;
				HBufC* textResource ;
				// If elements are marked in the list
				if(iMarkedItemCount)
					{
					// If only one station in the list is marked.
					if(iMarkedIndexArray.Count()==1)
						{
						TInt index = iMarkedIndexArray[0];
						textResource = StringLoader::LoadLC( R_IRAPP_NOTE_DELETE,
										iUi->iFavPresets->iFavPresetList[index]->Name() );
						}
					// If more than one station in the list is marked
					else
						{
						textResource = StringLoader::LoadLC( R_IRAPP_NOTE_DELETE_MANY,
													iMarkedIndexArray.Count());
						}
					deleteStation.Create(*textResource);
					iIsdsWrapper->GetDialogLauncherInstance()->
							ShowQueryDialogDeletionL(deleteStation,dlgValue);
					if(dlgValue)
						{
						iContainer->DeleteMultiStationsL();
						}
					}
				// If elements are not marked
				else
					{
					TBuf<KDummyLength> findString;
					TInt currentIndex;
					if(iContainer->GetSearchBoxPointer())
						{
						iContainer->GetSearchBoxPointer()->GetSearchText(findString);
						}
					// If elements in the listbox are not filtered
					if((findString.Length()==0))
						{
						TInt index;
						currentIndex = iContainer->GetListPointer()->CurrentItemIndex();
						SetCurrentFocus(iContainer->GetCurrentItemIndex());
						SetCurrentTopItem(iContainer->GetListBoxTopIndex());
						// If the last played exists
						if(iUi->iLastPlayed->FileExists())
							{
							index = currentIndex-1;
							textResource = StringLoader::LoadLC( R_IRAPP_NOTE_DELETE,
								iUi->iFavPresets->iFavPresetList[currentIndex-1]->Name() );
							}
						// If the last played doesn't exist
						else
							{
							index = currentIndex;
							textResource = StringLoader::LoadLC( R_IRAPP_NOTE_DELETE,
							iUi->iFavPresets->iFavPresetList[currentIndex]->Name() );
							}
						deleteStation.Create(*textResource);
						iIsdsWrapper->GetDialogLauncherInstance()->
								ShowQueryDialogDeletionL(deleteStation,dlgValue);
						if(dlgValue)
							{
							iContainer->DeleteChannelL(index );
							}
						}
					// If elements are  filtered
					else
						{
						currentIndex = GetCurrentIndex();
						textResource = StringLoader::LoadLC( R_IRAPP_NOTE_DELETE,
									iUi->iFavPresets->iFavPresetList[currentIndex]->Name() );
						deleteStation.Create(*textResource);
						iIsdsWrapper->GetDialogLauncherInstance()->
							ShowQueryDialogDeletionL(deleteStation,dlgValue);
						if(dlgValue)
							{
							iContainer->DeleteChannelL( currentIndex);
							}
						}
					}
				CleanupStack::PopAndDestroy( textResource );
				deleteStation.Close();
				SetToolbarItems();
				}
			break;
			}
		case EGotoNowPlayingViewCmd:
			{
			iUi->SetHistory(EFalse);
			iUi->SetMainView(ETrue);
			iUi->SavePrevIDL(KIRMainChoiceViewID,aCommand);
			break;
			}
		case EExportFavCmd:
			{
			break;
			}
		case EEditStationsCmd:
			{
			if(iContainer->GetListBoxCount()!=-1)
				{
				SetCurrentFocus(iContainer->GetCurrentItemIndex());
				}
			iUi->SetEditStation(ETrue);
			iUi->iEditStationIndex = iContainer->GetListPointer()->CurrentItemIndex();
			if(iUi->iLastPlayed->FileExists())
				{
				iUi->iFavPresets->iFavPresetList[iUi->iEditStationIndex-1]->
				CopyPresetData(*iNowPlayingWrapper->iEditStationPreset);
				}
			else
				{
				iUi->iFavPresets->iFavPresetList[iUi->iEditStationIndex]->
				CopyPresetData(*iNowPlayingWrapper->iEditStationPreset);
				}

			iUi->SetEditStation(ETrue);
			AppUi()->ActivateLocalViewL(KIRAddManuallyStationViewID);
			break;
			}
		case EStationsCmd:
			{
			break;
			}
		case EAddtoStationsCmd:
			{
			if ( iUi->CheckAndNotifyLowDiskSpaceL() )
				{
				break;
				}
			iAddToFavouratesFlag = ETrue;

			////////verifying network connectivity/////////////////////////////////
			TBool bNoNetwork = iUi->VerifyNetworkConnectivityL();
			if( bNoNetwork )
				{
				iRequestPending = ETrue;
				iAddtoStationsCmd = ETrue;

				break;
				}
			else
				{
				AddStationtoFavoritesL();
				}


			break;
			}
		case EAddStationManuallyCmd:
			{
			if ( !iUi->CheckAndNotifyLowDiskSpaceL() )
				{
				iUi->SetEditStation(EFalse);
				AppUi()->ActivateLocalViewL(KIRAddManuallyStationViewID);
				}
			break;
			}
		case EHelpCmd:
			{
			if ( !iUi->CheckAndNotifyLowDiskSpaceL() )
				{
				HlpLauncher::LaunchHelpApplicationL( iEikonEnv->WsSession(),
							AppUi()->AppHelpContextL() );
				}
			}
		break;
		case ESettingsCmd:
			{
			if(iContainer->GetListBoxCount()!=-1)
				{
				SetCurrentFocus(iContainer->GetCurrentItemIndex());
				}
			AppUi()->ActivateLocalViewL(KIRSettingsViewId);
			break;
			}
		case EAknSoftkeyBack:
			{
			if(	iUi->GetPlsActiavted() )
				{
				CIRBaseView::HandleCommandL( EAknSoftkeyBack );
				}
			else
				{
				CIRBaseView::HandleCommandL( EAknSoftkeyExit );
				}
			break;
			}

		case EStationDetailsCmd:
			{
			if( iUi->CheckAndNotifyLowDiskSpaceL() )
				{
				break;
				}

			TInt presetIndex = -1;
			if(iContainer->GetListBoxCount()!=-1)
				{
				SetCurrentFocus(iContainer->GetCurrentItemIndex());
				}
			// Last Played channel selected
			if(iContainer->GetCurrentItemIndex()==0 && iContainer->GetLastPlayedChannel())
				{
				iUi->iStationInfoView->SetStationPresetL( iUi->iLastPlayed->iLastPlayedStation );
				}
			else
				{
				presetIndex = GetCurrentIndex();
				iUi->iStationInfoView->SetStationPresetL( iUi->iFavPresets->
														  iFavPresetList[presetIndex] );
				}

			if( iUi->iStationInfoView->GetStationPresetL()->GetChannelType() == CIRIsdsPreset::EIsdsPreset
				&& iUi->VerifyNetworkConnectivityL() )
				{
				iRequestPending = ETrue;
				iStationDetailsCmd = ETrue;
				}
			else
				{
				AppUi()->ActivateLocalViewL(KIRStationInfoViewId);
				}
			break;
			}
		case EMoveCmd:
			{
			TInt currentIndex = iContainer->GetCurrentItemIndex();

			Toolbar()->SetItemDimmed(EFindStationsCmd,ETrue,ETrue);
			Toolbar()->SetItemDimmed(EListenCmd,ETrue,ETrue);
			Toolbar()->SetItemDimmed(EHistoryCmd,ETrue,ETrue);
			Toolbar()->DrawNow();

			if(iMarkedIndexArray.Count()==1)
				{
				if(!iUi->iLastPlayed->FileExists())
					{
					iIndexFrom = iMarkedIndexArray[0];
					iMovedItemIndex=iMarkedIndexArray[0];
					iChannelIdFrom = iUi->iFavPresets->iFavPresetList[iIndexFrom]->Id();
					}
				else
					{
					iIndexFrom = iMarkedIndexArray[0];
					iMovedItemIndex=iMarkedIndexArray[0] + 1;
					iChannelIdFrom = iUi->iFavPresets->iFavPresetList[iIndexFrom]->Id();
					}
				iMarkedIndexArray.Reset();
				SetListBoxMarkedCount(0);
				iMarkedFlag = EFalse;
				iMoveMarkedFlag = ETrue;
				}
			else
				{
				if(!iUi->iLastPlayed->FileExists())
					{
					iIndexFrom = currentIndex;
					iMovedItemIndex=currentIndex;
					iChannelIdFrom = iUi->iFavPresets->iFavPresetList[iIndexFrom]->Id();
					}
				else
					{
					iIndexFrom = currentIndex-1;
					iMovedItemIndex=currentIndex;
					iChannelIdFrom = iUi->iFavPresets->iFavPresetList[iIndexFrom]->Id();
					}
				if(currentIndex !=-1)
					{
					if(currentIndex ==0 &&iUi->iLastPlayed->FileExists())
						{
						// if the current item index is Zero and the Last palyed exists i.e., it is the lastplayed station then do nothing on mark/unmark.
						}
					else
						{
						iContainer->cmdMarkL(currentIndex,ETrue);
						iMoveMarkedFlag = ETrue;
						}
					}
				}
			Cba()->SetCommandSetL( R_IR_CBA_DONE_CANCEL );
			Cba()->DrawDeferred();
			break;
			}
		case EAknSoftkeyCancel:
		case EIRCancel:
			{
			TInt currentIndex =iMovedItemIndex;
			if(currentIndex != -1)
				{
				iContainer->cmdMarkL(currentIndex,EFalse);
				iMoveMarkedFlag = EFalse;
				}
			Toolbar()->SetItemDimmed(EFindStationsCmd,EFalse,ETrue);
			Toolbar()->SetItemDimmed(EListenCmd,EFalse,ETrue);
			Toolbar()->SetItemDimmed(EHistoryCmd,EFalse,ETrue);
			Toolbar()->DrawNow();
			Cba()->SetCommandSetL( R_IRAPP_SOFTKEYS_OPTIONS_BACK_CBA );
			Cba()->DrawDeferred();
			break;
			}
		case EAknSoftkeyDone:
			{
			TInt movedIndex =iMovedItemIndex;
			TInt currentIndex = iContainer->GetCurrentItemIndex();
			SetCurrentFocus(currentIndex);
			if(currentIndex==0)
				{
				iIndexTo=0;
				iChannelIdTo = iUi->iFavPresets->iFavPresetList[iIndexTo]->Id();
				}
			else
				{
				if(!iUi->iLastPlayed->FileExists())
					{
					iIndexTo = currentIndex;
					iChannelIdTo = iUi->iFavPresets->iFavPresetList[iIndexTo]->Id();
					}
				else
					{
					iIndexTo = currentIndex-1;
					iChannelIdTo = iUi->iFavPresets->iFavPresetList[iIndexTo]->Id();
					}
				}
			iUi->iFavPresets->SwapPresetsInDbL(iChannelIdFrom,iChannelIdTo,iIndexFrom,iIndexTo);
			if(movedIndex != -1)
				{
				iContainer->cmdMarkL(movedIndex,EFalse);
				}
			Toolbar()->SetItemDimmed(EFindStationsCmd,EFalse,ETrue);
			Toolbar()->SetItemDimmed(EListenCmd,EFalse,ETrue);
			Toolbar()->SetItemDimmed(EHistoryCmd,EFalse,ETrue);
			Toolbar()->DrawNow();
			Cba()->SetCommandSetL( R_IRAPP_SOFTKEYS_OPTIONS_BACK_CBA );
			Cba()->DrawDeferred();
			break;
			}
		default:
			{
			CIRBaseView::HandleCommandL( aCommand );
			break;
			}
		}
	IRLOG_DEBUG( "CIRMainView::HandleCommandL - Exiting." );
	}
// ---------------------------------------------------------------------------
// void CIRMainView::DynInitMenuPaneL()
// Dynamically initialises a menu pane
// ---------------------------------------------------------------------------
//
void CIRMainView::DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane )
	{
	IRLOG_DEBUG( "CIRMainView::DynInitMenuPaneL - Entering" );
	if(iContainer->GetListBoxCount()!=-1)
		{
		AknSelectionService::HandleMarkableListDynInitMenuPane
		(aResourceId, aMenuPane, iContainer->GetListPointer());
		}
	if(aResourceId == R_IR_MENU_FAV)
		{
		if(!aMenuPane) User::Leave(KErrNotFound);
			{
			if ( iContainer )
				{
				if(!(iContainer->GetContextSenstiveFlag()))
					{
					TInt filteredCount = iContainer->GetFilterdModelPointer()->NumberOfItems();
					if(filteredCount == 0)
						{
						// After dynamic filtering if no matches are present, if user presses
						// option menu only add stations manually,settings,help,
						// exit should be displayed
						aMenuPane->SetItemDimmed(EAddStationManuallyCmd, EFalse);
						aMenuPane->SetItemDimmed(EEditStationsCmd, ETrue);
						if(iContainer->GetLastPlayedChannel())
							{
							aMenuPane->SetItemDimmed(EAddtoStationsCmd, EFalse);
							}
						else
							{
							aMenuPane->SetItemDimmed(EAddtoStationsCmd, ETrue);
							aMenuPane->SetItemDimmed(EMarkUnmark,ETrue );
							aMenuPane->SetItemDimmed(EDeleteCmd,ETrue );
							aMenuPane->SetItemDimmed(EStationsCmd, ETrue);
							aMenuPane->SetItemDimmed(ESettingsCmd, EFalse);
							aMenuPane->SetItemDimmed(EHelpCmd, EFalse);
							aMenuPane->SetItemDimmed(EExitCmd, EFalse);
							}
						}
					else
						{
						if(filteredCount == iMarkedIndexArray.Count()|| iMarkedIndexArray.Count())
							{
							aMenuPane->SetItemDimmed(EEditStationsCmd, ETrue);
							}
						else
							{
							aMenuPane->SetItemDimmed(EEditStationsCmd, GetEditDimmingStatus());
							}
						aMenuPane->SetItemDimmed(EAddtoStationsCmd, GetAddDimmingStatus());
						aMenuPane->SetItemDimmed(EMarkUnmark,GetLastPlayedStatus() );
						aMenuPane->SetItemDimmed(EDeleteCmd,ETrue );
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
					}
				else
					{
					// After marking of any station, if user  taps on any station
					// only delete and mark/unmark options should be shown.
					aMenuPane->SetItemDimmed(EDeleteCmd,EFalse );
					aMenuPane->SetItemDimmed(EMarkUnmark, EFalse);
					aMenuPane->SetItemDimmed(EAddStationManuallyCmd, ETrue);
					aMenuPane->SetItemDimmed(EAddtoStationsCmd, ETrue);
					aMenuPane->SetItemDimmed(EEditStationsCmd, ETrue);
					aMenuPane->SetItemDimmed(EStationsCmd, ETrue);
					aMenuPane->SetItemDimmed(ESettingsCmd, ETrue);
					aMenuPane->SetItemDimmed(EHelpCmd, ETrue);
					aMenuPane->SetItemDimmed(EExitCmd, ETrue);
					aMenuPane->SetItemDimmed(EGotoNowPlayingViewCmd,ETrue);
					iContainer->SetContextSenstiveFlag(EFalse) ;
					}
				} /*iContainer */
			} /*aMenuPane */
		} /*aResource */

	else if(aResourceId == R_IR_MENU_STATIONS_SUB_MENU)
		{
		aMenuPane->SetItemDimmed(EDeleteCmd,GetLastPlayedStatus() );
		aMenuPane->SetItemDimmed(EStationDetailsCmd,EFalse );
		aMenuPane->SetItemDimmed(EMoveCmd,GetLastPlayedStatus() );
		if(iMarkedIndexArray.Count())
			{
			// If any element is marked then sation details option should be disabled
			// from options menu
			if(iContainer->GetLastPlayedChannel() && iContainer->GetListPointer()->CurrentItemIndex()==0)
				{
				aMenuPane->SetItemDimmed(EStationDetailsCmd,EFalse );
				}
			else
				{
				aMenuPane->SetItemDimmed(EStationDetailsCmd,ETrue );
				}
			}
		if(  iMarkedIndexArray.Count()>1 ||
		iContainer->GetFilteredFlag() ||
		(iContainer->GetListBoxCount()==2 && iContainer->GetLastPlayedChannel() ) ||
		iContainer->GetListBoxCount()==1  )
			{
			aMenuPane->SetItemDimmed(EMoveCmd,ETrue );
			}
		}
	else if(aResourceId == R_IR_MENUPANE_MARKABLE_LIST)
		{
		aMenuPane->SetItemDimmed(EMark, EFalse);
		aMenuPane->SetItemDimmed(EMarkAll, EFalse);
		aMenuPane->SetItemDimmed(EUnmark, ETrue);
		aMenuPane->SetItemDimmed(EUnmarkAll, ETrue);
		if(iMarkedIndexArray.Count())
			{
			TBool markedflag;
			TInt noOfSelectedIndexes = 0;
			TInt filteredCount = iContainer->GetFilterdModelPointer()->NumberOfItems();
			if(iContainer->GetLastPlayedChannel())
				{
				filteredCount = filteredCount-1	;
				}

			markedflag = EFalse;
			TInt CurrentItemIndex = GetCurrentIndex();
			for(TInt i=0;i<iMarkedIndexArray.Count();i++)
				{
				if(iMarkedIndexArray[i]==CurrentItemIndex)
					{
					markedflag = ETrue;
					}
				noOfSelectedIndexes++;
				}
			if((markedflag && iMarkedFlag)||(markedflag && iMarkedAllFlag))
				{
				aMenuPane->SetItemDimmed(EMark, ETrue);
				aMenuPane->SetItemDimmed(EMarkAll, EFalse);
				aMenuPane->SetItemDimmed(EUnmark, EFalse);
				aMenuPane->SetItemDimmed(EUnmarkAll, EFalse);
				}
			else
				{
				aMenuPane->SetItemDimmed(EMark, EFalse);
				aMenuPane->SetItemDimmed(EMarkAll, EFalse);
				aMenuPane->SetItemDimmed(EUnmark, ETrue);
				aMenuPane->SetItemDimmed(EUnmarkAll, EFalse);
				}
			if((markedflag && iMarkedAllFlag && noOfSelectedIndexes == filteredCount)
			||(markedflag && noOfSelectedIndexes == filteredCount))
				{
				aMenuPane->SetItemDimmed(EMark, ETrue);
				aMenuPane->SetItemDimmed(EMarkAll, ETrue);
				aMenuPane->SetItemDimmed(EUnmark, EFalse);
				aMenuPane->SetItemDimmed(EUnmarkAll, EFalse);
				}
			}
		}
	IRLOG_DEBUG( "CIRMainView::DynInitMenuPaneL - Exiting" );
	}

// ---------------------------------------------------------------------------
// Touch Toolbar
//
// ---------------------------------------------------------------------------

void CIRMainView::ConstructToolbarL()
    {
	IRLOG_DEBUG( "CIRMainView::ConstructToolbarL - Entering" );
	SetToolbarItems();
	// Update rect, as changes in toolbar visibility also affect layout.
	if(iContainer)
		{
		iContainer->SetRect( ClientRect() );
		iContainer->DrawDeferred();
		}

	IRLOG_DEBUG( "CIRMainView::ConstructToolbarL - Exiting" );
    }

// ---------------------------------------------------------------------------
// From class CAknView.
// Foreground event handling function.
// ---------------------------------------------------------------------------
//
void CIRMainView::HandleForegroundEventL( TBool aForeground )
    {
	IRLOG_DEBUG( "CIRMainView::HandleForegroundEventL - Entering" );
	CIRBaseView::HandleForegroundEventL( aForeground );

	if ( aForeground )
		{
		if ( iContainer )
			{
			MAknsSkinInstance * skin=AknsUtils::SkinInstance () ;
			TRgb color;
			TInt skinColor=AknsUtils::GetCachedColor(skin, color, KAknsIIDQsnTextColors,
							                                      EAknsCIQsnTextColorsCG6 );
			if(color.Gray2()!=0)
				{
				CreateAndSetToolbarL(R_IR_MAINVIEW_WHITE_TOOLBAR);
				}
			else
				{
				CreateAndSetToolbarL(R_IR_MAINVIEW_BLACK_TOOLBAR);
				}
			if( AknLayoutUtils::PenEnabled() && Toolbar() )
				{
				Toolbar()->SetToolbarObserver( this );
				}
			ConstructToolbarL();
			iContainer->DrawDeferred();
			}
		}
	IRLOG_DEBUG( "CIRMainView::HandleForegroundEventL---Exiting" );
    }

// ---------------------------------------------------------------------------
// From class CAknView.
// Event handler for status pane size changes.
// ---------------------------------------------------------------------------
//
void CIRMainView::HandleStatusPaneSizeChange()
    {
	IRLOG_DEBUG( "CIRMainView::HandleStatusPaneSizeChange - Entering" );
    CIRBaseView::HandleStatusPaneSizeChange();

    if ( iContainer )
        {
        iContainer->SetRect( ClientRect() );
        }
	IRLOG_DEBUG( "CIRMainView::HandleStatusPaneSizeChange - Exiting" );
    }

// ---------------------------------------------------------------------------
// CIRMainView::OfferToolbarEventL()
// catch the selection key event and perform the operation according to it...
// ---------------------------------------------------------------------------
void CIRMainView::OfferToolbarEventL( TInt aCommand )
	{
	IRLOG_DEBUG( "CIRMainView::OfferToolbarEventL - Entering" );
	HandleCommandL(aCommand);
	IRLOG_DEBUG( "CIRMainView::OfferToolbarEventL - Exiting" );
	}

// ---------------------------------------------------------------------------
// void CIRMainView::SetToolbarItems()
// Setting toolbar items when text is entered
//  ---------------------------------------------------------------------------
void CIRMainView:: SetToolbarItems()
	{
	IRLOG_DEBUG( "CIRMainView::SetToolbarItems - Entering" );
	if(Toolbar())
		{
		if(iContainer)
			{
			Toolbar()->SetToolbarVisibility(ETrue, EFalse);
			TBuf<KFindStringLength> findstring ;
			if(iContainer->GetSearchBoxPointer())
				{
				iContainer->GetSearchBoxPointer()->GetSearchText(findstring);
				}
			TInt filterModelCount = iContainer->GetFilterdModelPointer()->NumberOfItems();
			if(iContainer->GetSearchBoxPointer() && findstring.Length()!=0
			&& filterModelCount == 0)
				{
				Toolbar()->SetItemDimmed(EFindStationsCmd,ETrue,ETrue);
				Toolbar()->SetItemDimmed(EListenCmd,ETrue,ETrue);
				Toolbar()->SetItemDimmed(EHistoryCmd,ETrue,ETrue);
				}
			else
				{
				if(iMarkedItemCount||iContainer->GetListPointer()->Model()->NumberOfItems()==0 )
					{
					Toolbar()->SetItemDimmed(EListenCmd,ETrue,ETrue);
					}
				else
					{
		            Toolbar()->SetItemDimmed(EFindStationsCmd,EFalse,ETrue);
					Toolbar()->SetItemDimmed(EListenCmd,EFalse,ETrue);
					Toolbar()->SetItemDimmed(EHistoryCmd,EFalse,ETrue);
					}
	            if(iMoveMarkedFlag)
		            {
		            Toolbar()->SetItemDimmed(EFindStationsCmd,ETrue,ETrue);
					Toolbar()->SetItemDimmed(EListenCmd,ETrue,ETrue);
					Toolbar()->SetItemDimmed(EHistoryCmd,ETrue,ETrue);
		            }
				}

			Toolbar()->DrawNow();
			}
		}
	IRLOG_DEBUG( "CIRMainView::SetToolbarItems - Exiting" );
	}

// ---------------------------------------------------------------------------
// CIRMainView::HandlePresetChangedL( TInt aId, TUid aDataHandler, MPSPresetObserver::TPSReason aType )
// This is called by CIRFavoritesDb when values of presets stations has been changed
// ---------------------------------------------------------------------------
//
void CIRMainView::HandlePresetChangedL( TInt aId, TUid aDataHandler,
						 MPSPresetObserver::TPSReason aType )
	{
	IRLOG_DEBUG( "CIRMainView::HandlePresetChangedL  - Entering." );
	if(iContainer)
		{
		iContainer->HandlePresetChangedL(aId,aDataHandler,aType);
		}
	IRLOG_DEBUG( "CIRMainView::HandlePresetChangedL - Exiting." );
	}


// ---------------------------------------------------------------------------
//Notifies all observers whose network request is active
// to reissue the request
//NotifyActiveNetworkObserversL()
// ---------------------------------------------------------------------------

void CIRMainView::NotifyActiveNetworkObserversL(TIRNetworkEvent aEvent)
	{
	IRLOG_DEBUG( "CIRMainView::NotifyActiveNetworkObserversL  - Entering." );

	switch(aEvent)
		{
		case ENetworkConnectionDisconnected:
			{
			iNowPlayingWrapper->HandleNetworkDisconnected();
			}
			break;
		case ENetworkConnectionEstablished:
			{

			if(iContainer)
				{

				if(iRequestPending || iUi->iNetworkController->IsHandingOverConnection())
					{
					if(iFavListen)
						{
						DoConnectToChannelL( iContainer->GetUrlIndex() );
						}
					else if( iStationDetailsCmd )
						{
						iStationDetailsCmd = ETrue;
						AppUi()->ActivateLocalViewL(KIRStationInfoViewId);
						}
					else if(iListenRequest)
						{
						ConnectToSelectedStationL();
						}
					else if(iAddtoStationsCmd)
						{
						AddStationtoFavoritesL();
						}
					else if(iSyncReq)
						{
						HandleCommandL(EListenCmd);
						}
					}
				}

			ResetPendingRequests(EFalse);
			}
			break;
		}

	IRLOG_DEBUG( "CIRMainView::NotifyActiveNetworkObserversL  - Exiting." );
	}


// -----------------------------------------------------------------------------
// Notified by network controller when user cancels network connection, to reset
// the pending requests
// ResetPendingRequests()
// -----------------------------------------------------------------------------
void CIRMainView::ResetPendingRequests(TBool aValue)
	{
	IRLOG_DEBUG( "CIRMainView::ResetPendingRequests  - Entering." );
	iRequestPending = aValue;
	iListenRequest  = aValue;
	iStationDetailsCmd = aValue;
	iFavListen = aValue;
	iAddtoStationsCmd = aValue;
	IRLOG_DEBUG( "CIRMainView::ResetPendingRequests  - Exiting." );

	}


// ---------------------------------------------------------------------------
// CIRMainView :: GetAddDimmingStatus()
// Returns status for making the edit option dimmed or not..
// ---------------------------------------------------------------------------
//
TBool CIRMainView::GetAddDimmingStatus()
	{
	IRLOG_DEBUG( "CIRMainView::GetAddDimmingStatus  - Entering." );
	IRLOG_DEBUG( "CIRMainView::GetAddDimmingStatus  - Exiting." );
	if(iContainer->GetListPointer()->CurrentItemIndex()==0 && iContainer->GetLastPlayedChannel())
		{
		return EFalse;
		}
	else
		{
		return ETrue;
		}
	}

// ---------------------------------------------------------------------------
// CIRMainView :: GetEditDimmingStatus()
// Returns status for making the edit option dimmed or not..
// ---------------------------------------------------------------------------
//
TBool CIRMainView::GetEditDimmingStatus()
	{
	IRLOG_DEBUG( "CIRMainView::GetEditDimmingStatus  - Entering." );
	TInt favIndex = 0;
	TInt currentIndex = 0;
	currentIndex = iContainer->GetListPointer()->CurrentItemIndex();
	if(currentIndex!= -1)
		{
		// If last played station exist
		if(iContainer->GetLastPlayedChannel())
			{
		 	favIndex = GetCurrentIndex();
			}
		else
			{
			favIndex = iContainer->GetListPointer()->CurrentItemIndex();
			}
		// If focus is on last played station
		if((currentIndex == 0) && (iContainer->GetLastPlayedChannel()))
			{
			return ETrue;
			}
		if(favIndex <=iUi->iFavPresets->iFavPresetList.Count() &&
				iUi->iFavPresets->iFavPresetList[favIndex]->GetChannelType() == 0)
			{
			return EFalse;
			}
		}
	IRLOG_DEBUG( "CIRMainView::GetEditDimmingStatus  - Exiting." );
	return ETrue;
	}
// ---------------------------------------------------------------------------
// CIRMainView :: GetMainContainer()
// returns pointer to main container..
// ---------------------------------------------------------------------------
//
CIRMainContainer* CIRMainView::GetMainContainer()
	{
	IRLOG_DEBUG( "CIRMainView::GetMainContainer  - Entering." );
	return iContainer;
	}
// ---------------------------------------------------------------------------
// CIRMainView::ConnectToLastPlayedStationL()
// Connects to the last played station.
// ---------------------------------------------------------------------------
//
void CIRMainView::ConnectToLastPlayedStationL()
	{
	IRLOG_DEBUG( "CIRMainView::ConnectToLastPlayedStationL - Entering" );
	if(iNowPlayingWrapper->iNowPlayingPreset)
	*iNowPlayingWrapper->iNowPlayingPreset=*iUi->iLastPlayed->iLastPlayedStation;
	TBuf8<KUrlbufMaxLength> urlbuf;
	urlbuf.Copy(iUi->iLastPlayed->iLastPlayedStation->iUrlArray->At(0)->iURL->Des());
	iUi->iNPPrevViewId = KIRMainChoiceViewID;
	iNowPlayingWrapper->SetView(ELastplayed);
	TConnectedFrom connectedfrom = EIRPresetIsds;
	iNowPlayingWrapper->SetCurrentConnectionSource(connectedfrom);
	iNowPlayingWrapper->ConnectToChannelL(*iUi->iLastPlayed->iLastPlayedStation);
	IRLOG_DEBUG( "CIRMainView::ConnectToLastPlayedStationL - Exiting." );
	}

// ----------------------------------------------------------------------------
// CIRMainView::SetUrlFromPresetL( TInt aUrlIndex )
// Checks whether the url name contains only spaces and if the descriptor really was.
// empty replaces its content with default name
// ----------------------------------------------------------------------------
//
void CIRMainView::SetUrlFromPresetL(TInt aUrlIndex)
	{
	IRLOG_DEBUG( "CIRMainView::SetUrlFromPresetL - Entering" );
	iNowPlayingWrapper->SetView(EFav);
	//iNowPlayingWrapper->SetConnectToServerState(ETrue)
	if(aUrlIndex <= iUi->iFavPresets->iFavPresetList.Count())
	iUi->iFavPresets->iFavPresetList[aUrlIndex]->CopyPresetData(*iNowPlayingWrapper->
													iNowPlayingPreset);
	iUi->iNPPrevViewId = KIRSavedStationsViewID;
	//connected from saved preset
	iNowPlayingWrapper->SetCurrentConnectionSource(EIRPresetIsds);
	//Checking for valid url, Start
	TBool urlStatus = FALSE;
	//executed for user defined channels
	if( !iNowPlayingWrapper->iNowPlayingPreset->GetId() &&
					iNowPlayingWrapper->iNowPlayingPreset->GetUrlCount() > 0 )
		{
		//to get the only url available for user defined channel
		urlStatus = iUi->ParseAndPrepareUrl(iNowPlayingWrapper->
									iNowPlayingPreset->GetChannelUrlAtL(0));
		if( !urlStatus )
			{
			HBufC *infoBuf = StringLoader::LoadL(R_IRAPP_ERROR_ADDRESS);
			delete infoBuf;
			return;
			}
		}
	//Checking for valid url, End
	if( iContainer )
		{
		iContainer->CheckConnectionL(aUrlIndex);
		}
	IRLOG_DEBUG( "CIRMainView::SetUrlFromPresetL - Exiting." );
	}

// ---------------------------------------------------------------------------
// CIRMainView::DoConnectChannelL()
// Connects to the channel identified by the supplied index.
// ---------------------------------------------------------------------------
//
void CIRMainView::DoConnectToChannelL( TInt /*aUrlIndex*/ )
	{
	IRLOG_DEBUG( "CIRMainView::DoConnectToChannelL - Entering" );
/*d	if(aUrlIndex <= iUi->iFavPresets->iFavPresetList.Count() &&
		iUi->iFavPresets->iFavPresetList[aUrlIndex]->GetChannelType())
		{
		iIsdsWrapper->SetSyncPresetObserver(this);
		SyncPresetL(iUi->iFavPresets->iFavPresetList[aUrlIndex]->GetId(),
		iUi->iFavPresets->iFavPresetList[aUrlIndex]->GetLastModifiedTime());
		}
d*/
	iNowPlayingWrapper->ConnectToChannelL(*iNowPlayingWrapper->iNowPlayingPreset);
	IRLOG_DEBUG( "CIRMainView::DoConnectToChannelL - Exiting." );
	}
// ---------------------------------------------------------------------------
// CIRMainView::SyncPresetL()
// Connects to the channel identified by the supplied index.
// ---------------------------------------------------------------------------
//
void CIRMainView::SyncPresetL(TInt aChid,const TDesC &aModifiedTime)
	{
	IRLOG_DEBUG( "CIRMainView::SyncPresetL  - Entering." );
	//sync state set to pending
	//	iUi->SetSyncState(ESyncReqPending)
	iUi->iNowPlayingWrapper->CreateWaitDialogL();
	iUi->GetPubSubManagerInstance()->PublishBufferingState(EIRStateBufferingStart);
	iSyncReq = ETrue;
	iUi->iNowPlayingWrapper->iSyncPreset = ETrue;
	iIsdsWrapper->SetSyncPresetObserver(this,this);
	iIsdsWrapper->SetSyncReqFromMainView(ETrue);
	iIsdsWrapper->GetISDSInstance()->SyncPresetL(aChid,aModifiedTime);
	IRLOG_DEBUG( "CIRMainView::SyncPresetL - Exiting." );
	}
// ---------------------------------------------------------------------------
// CIRMainView::GetListBoxMarkedCount()
// returns the listbox count.
// ---------------------------------------------------------------------------
//
TInt CIRMainView::GetListBoxMarkedCount()
	{
	IRLOG_DEBUG( "CIRMainView::GetListBoxMarkedCount" );
	return iMarkedItemCount;
	}
// ---------------------------------------------------------
// CIRMainView::SetListBoxMarkedCount
// Sets the listbox marked count
// ---------------------------------------------------------
//
void CIRMainView::SetListBoxMarkedCount(TInt aMarkedItemCount)
	{
	IRLOG_DEBUG( "CIRMainView::SetListBoxMarkedCount - Entering." );
	iMarkedItemCount = aMarkedItemCount;
	IRLOG_DEBUG( "CIRMainView::SetListBoxMarkedCount - Exiting." );
	}
// ---------------------------------------------------------------------------
// CIRMainView::GetMarkedIndexArray()
// returns the Marked array Index.
// ---------------------------------------------------------------------------
//
RArray<TInt> CIRMainView::GetMarkedIndexArray()
	{
	IRLOG_DEBUG( "CIRMainView::GetMarkedIndexArray" );
	return iMarkedIndexArray;
	}
// ---------------------------------------------------------------------------
// CIRMainView::GetMarkedFlag()
// returns marked flag
// ---------------------------------------------------------------------------
//
TInt CIRMainView::GetMarkedFlag()
	{
	IRLOG_DEBUG( "CIRMainView::GetMarkedFlag" );
	return iMarkedFlag;
	}
// ---------------------------------------------------------
// CIRBaseView::SetMarkedFlag
// Sets the marked flag
// ---------------------------------------------------------
//
void CIRMainView::SetMarkedFlag(TInt aMarkedFlag)
	{
	IRLOG_DEBUG( "CIRMainView::SetMarkedFlag - Entering." );
	iMarkedFlag = aMarkedFlag;
	IRLOG_DEBUG( "CIRMainView::SetMarkedFlag - Exiting." );
	}
// ---------------------------------------------------------------------------
// CIRMainView::GetMarkedAllFlag()
// returns marked all flag.
// ---------------------------------------------------------------------------
//
TInt CIRMainView::GetMarkedAllFlag()
	{
	IRLOG_DEBUG( "CIRMainView::GetMarkedAllFlag" );
	return iMarkedAllFlag;
	}
// ---------------------------------------------------------
// CIRMainView::SetMarkedAllFlag
// Sets the marked all flag
// ---------------------------------------------------------
//
void CIRMainView::SetMarkedAllFlag(TInt aMarkedAllFlag)
	{
	IRLOG_DEBUG( "CIRMainView::SetMarkedAllFlag - Entering." );
	iMarkedAllFlag = aMarkedAllFlag;
	IRLOG_DEBUG( "CIRMainView::SetMarkedAllFlag - Exiting." );
	}

// ---------------------------------------------------------------------------
// CIRMainView::GetAddToFavouratesFlag()
// returns whether any station has been added to favourates or not
// ---------------------------------------------------------------------------
//
TInt CIRMainView::GetAddToFavouratesFlag()
	{
	IRLOG_DEBUG( "CIRMainView::GetAddToFavouratesFlag" );
	return iAddToFavouratesFlag;
	}

// ---------------------------------------------------------------------------
// CIRMainView :: GetLastPlayedStatus()
// Returns status for making the edit option dimmed or not..
// ---------------------------------------------------------------------------
//
TBool CIRMainView::GetLastPlayedStatus()
	{
	IRLOG_DEBUG( "CIRMainView::GetLastPlayedStatus - Entering." );
	IRLOG_DEBUG( "CIRMainView::GetLastPlayedStatus - Exiting." );
	if(iContainer && iContainer->GetListPointer()->CurrentItemIndex()==0
	   && iContainer->GetLastPlayedChannel())
		{
		return ETrue;
		}
	else
		{
		return EFalse;
		}
	}

// ---------------------------------------------------------------------------
// CIRMainView :: GetCurrentIndex()
// Returns actual index of the station depending on the lastplayed/ filtering.
// ---------------------------------------------------------------------------
//
TInt CIRMainView::GetCurrentIndex()
	{
	IRLOG_DEBUG( "CIRMainView::GetCurrentIndex - Entering." );
	TInt currentIndex ;
	SetCurrentFocus(iContainer->GetListPointer()->CurrentItemIndex());
	if(iContainer->GetSearchBoxPointer())
		{
		TBuf<KDummyLength> findString;
		iContainer->GetSearchBoxPointer()->GetSearchText(findString);
		if((findString.Length()==0))
			{
			// If current item index is other than Last played station
			// If Last played Exists then the index of Favorite preset is one less than the current item index.
			if(iUi->iLastPlayed->FileExists())
				{
				currentIndex = iContainer->GetListPointer()->CurrentItemIndex()-1;
				}
			// If Last played doesn't exists then the index of Favorite preset is the same as current item index.
			else
				{
				currentIndex = iContainer->GetListPointer()->CurrentItemIndex();
				}
			}
		else
			{
			if(findString.Length()!=0)
				{
				iContainer->Comparestrings();
				SetCurrentFocus(iFilteredIndex);
				if(iUi->iLastPlayed->FileExists())
					{
					iFilteredIndex = iFilteredIndex -1;
					}

				currentIndex = iFilteredIndex;
				}
			}
		}
	else
		{
		// If Last played Exists then the index of Favorite preset is one less than the current item index.
		if(iUi->iLastPlayed->FileExists())
			{
			currentIndex = iContainer->GetListPointer()->CurrentItemIndex()-1;
			}
		// If Last played doesn't exists then the index of Favorite preset is the same as current item index.
		else
			{
			currentIndex = iContainer->GetListPointer()->CurrentItemIndex();
			}

		}
	IRLOG_DEBUG( "CIRMainView::GetCurrentIndex - Exiting." );
	return currentIndex;
	}
// ---------------------------------------------------------------------------
// CIRMainView::GetRequestPendingStatus()
// returns whether an active request is pending.
// ---------------------------------------------------------------------------
//
TBool CIRMainView::GetRequestPendingStatus()
	{
	IRLOG_DEBUG( "CIRMainContainer::GetRequestPendingStatus" );
	return iRequestPending;
	}
// ---------------------------------------------------------
// CIRMainView::SetRequestPendingStatus ()
// Sets the request pending state
// ---------------------------------------------------------
//
void CIRMainView::SetRequestPendingStatus(TBool aRequestPending)
	{
	IRLOG_DEBUG( "CIRMainView::SetRequestPendingStatus - Entering." );
	iRequestPending = aRequestPending;
	IRLOG_DEBUG( "CIRMainView::SetRequestPendingStatus - Exiting." );
	}
// ---------------------------------------------------------------------------
// CIRMainView::GetFavListen()
// returns Checks whether request is listen to favorites.
// ---------------------------------------------------------------------------
//
TBool CIRMainView::GetFavListen()
	{
	IRLOG_DEBUG( "CIRMainContainer::GetFavListen" );
	return iFavListen;
	}
// ---------------------------------------------------------
// CIRMainView::SetFavListen ()
// Sets the listen is from favourites or not
// ---------------------------------------------------------
//
void CIRMainView::SetFavListen(TBool aFavListen)
	{
	IRLOG_DEBUG( "CIRMainView::SetFavListen - Entering." );
	iFavListen = aFavListen;
	IRLOG_DEBUG( "CIRMainView::SetFavListen - Exiting." );
	}
// ---------------------------------------------------------
// CIRMainView::SetActualFilteredItemIndex ()
// Sets the actual index of the station
// ---------------------------------------------------------
//
void CIRMainView::SetActualFilteredItemIndex(TInt aFilteredIndex)
	{
	IRLOG_DEBUG( "CIRMainView::SetActualFilteredItemIndex - Entering." );
	iFilteredIndex = aFilteredIndex;
	IRLOG_DEBUG( "CIRMainView::SetActualFilteredItemIndex - Exiting." );
	}
// ---------------------------------------------------------------------------
// CIRMainView :: GetMovingFlag()
// Gets the view is in move mode or not.
// ---------------------------------------------------------------------------
//
TBool CIRMainView::GetMovingFlag()
	{
	return iMoveMarkedFlag;
	}
// ---------------------------------------------------------------------------
// CIRMainView :: SetMovingFlag()
// Sets the view is in move mode .
// ---------------------------------------------------------------------------
//
void CIRMainView::SetMovingFlag(TBool aValue)
	{
	iMoveMarkedFlag = aValue;
	}


// ------------------------------------------------------------------------
// from MSyncPresetObserver
// SyncPresetRemovedL()
// called back when the preset that is synced was removed from isds
// @param TInt,the preset id
// -----------------------------------------------------------------------
void CIRMainView::SyncPresetRemovedL(TInt aId)
	{
	IRLOG_DEBUG( "CIRMainView::SyncPresetRemovedL - Entering." );

	iSyncReq = EFalse;

	if( !iContainer )
		return;

	if( iUi->iLastPlayed->FileExists() &&
	iUi->iLastPlayed->iLastPlayedStation->GetId() == aId)
		{
		// update the last played station
		//convert the flag to user defined and commit
		//0 for EUserDefined
		iUi->iLastPlayed->iLastPlayedStation->SetChannelType(KNull);
		//assign null id
		iUi->iLastPlayed->iLastPlayedStation->SetId(KNull);

		//make logo as null
		_LIT8(KEmpty, "");
		RBuf8 tempLogo;
		tempLogo.Create(KEmpty);
		iUi->iLastPlayed->iLastPlayedStation->SetLogoData(tempLogo);
		tempLogo.Close();
		_LIT(KEmptyString, "");
		RBuf tempString;
		tempString.Create(KEmptyString);
		//set imgurl to null
		iUi->iLastPlayed->iLastPlayedStation->SetImgUrl(tempString);
		//set genre to null
		iUi->iLastPlayed->iLastPlayedStation->SetGenreName(tempString);
		iUi->iLastPlayed->iLastPlayedStation->SetGenreId(tempString);
		//set language to null
		iUi->iLastPlayed->iLastPlayedStation->SetLang(tempString);
		//set country to null
		iUi->iLastPlayed->iLastPlayedStation->SetCountryName(tempString);
		tempString.Close();
		//set musicStoreEnabled filed to "no"
		_LIT(KNo, "no");
		RBuf tempMusicStoreEnabled;
		tempMusicStoreEnabled.Create(KNo);
		iUi->iLastPlayed->iLastPlayedStation->SetMusicStoreStatus(tempMusicStoreEnabled);
		tempMusicStoreEnabled.Close();

		//save the changes to the lastplayed file
		TRAPD(err,iUi->iLastPlayed->CommitLastPlayedChannelL());
		TInt error=KErrNone;
		if(err != KErrNone)
			{
			error = iUi->iLastPlayed->RemoveLastPlayedFile();
			User::Leave(error);
			}
		}
	iContainer->RefreshViewL();
	TInt currentIndex = GetCurrentIndex();
	if(iContainer->GetCurrentItemIndex() ==0 && iUi->iLastPlayed->FileExists())
		{
		iUi->iLastPlayed->RetriveLastPlayedChannelL();
		ConnectToLastPlayedStationL();
		}
	else
		{
		SetUrlFromPresetL(currentIndex);
		}
	IRLOG_DEBUG( "CIRMainView::SyncPresetRemovedL - Entering." );
	}

// -----------------------------------------------------------------------
// from MSyncPresetObserver
// SyncPresetChangedL()
// called back when the preset that is synced has been changed in the isds
// @param CIRIsdsPreset&,the new preset data
// ----------------------------------------------------------------------
void CIRMainView::SyncPresetChangedL(CIRIsdsPreset& aPreset)
	{
	IRLOG_DEBUG( "CIRMainView::SyncPresetChangedL - Entering." );

	iSyncReq = EFalse;

	if( !iContainer )
		return;

	if( iUi->iLastPlayed->FileExists() &&
		iUi->iLastPlayed->iLastPlayedStation->GetId() == aPreset.GetId())
		{
		aPreset.SetChannelType(KIsdsPreset);
		*iUi->iLastPlayed->iLastPlayedStation = aPreset;
		//save the changes to the lastplayed file
		TRAPD(err,iUi->iLastPlayed->CommitLastPlayedChannelL());
		TInt error=KErrNone;
		if(err != KErrNone)
			{
			error = iUi->iLastPlayed->RemoveLastPlayedFile();
			User::Leave(error);
			}
		}
	iContainer->RefreshViewL();
	TInt currentIndex = GetCurrentIndex();
	if(iContainer->GetCurrentItemIndex() ==0 && iUi->iLastPlayed->FileExists())
		{
		iUi->iLastPlayed->RetriveLastPlayedChannelL();
		ConnectToLastPlayedStationL();
		}
	else
		{
		SetUrlFromPresetL(currentIndex);
		}
	IRLOG_DEBUG( "CIRMainView::SyncPresetChangedL - Entering." );
	}

// -----------------------------------------------------------------------
// from MSyncPresetObserver
// SyncPresetNoChangeL()
// called back when the preset that is synced has not been changed
// -----------------------------------------------------------------------
void CIRMainView::SyncPresetNoChangeL()
	{
	IRLOG_DEBUG( "CIRMainView::SyncPresetNoChangeL - Entering." );

	iSyncReq = EFalse;

	if( !iContainer )
		return;
	iUi->iNowPlayingWrapper->iSyncPreset = EFalse;
	TInt currentIndex = GetCurrentIndex();
	if(iContainer->GetCurrentItemIndex() ==0 &&iUi->iLastPlayed->FileExists())
		{
		ConnectToLastPlayedStationL();
		}
	else
		{
		SetUrlFromPresetL(currentIndex);
		}
	IRLOG_DEBUG( "CIRMainView::SyncPresetNoChangeL - Entering." );
	}

// ----------------------------------------------------------------------
// CIRMainView::ConnectToSelectedStationL()
// Used to synchronize the LastPlayed preset before listen
// ----------------------------------------------------------------------
void CIRMainView::ConnectToSelectedStationL()
	{
	IRLOG_DEBUG("CIRMainView::ConnectToSelectedStationL - Entering");

	if( !iContainer )
		return;

	TInt currentIndex = GetCurrentIndex();

	// Connect to User defined / Last Played preset
	if(iContainer->GetCurrentItemIndex() == 0
		&& iUi->iLastPlayed->FileExists()
		&& iUi->iLastPlayed->iLastPlayedStation->GetChannelType() == CIRIsdsPreset::EUserDefined)
		{
		if(iNowPlayingWrapper->iPlaying)
			{
			HandleCommandL(EGotoNowPlayingViewCmd);
			}
		else
			{
			ConnectToLastPlayedStationL();
			}
		}

	// Sync ISDS / Last Played preset
	else if(iContainer->GetCurrentItemIndex() == 0
		&& iUi->iLastPlayed->FileExists()
		&& iUi->iLastPlayed->iLastPlayedStation->GetChannelType())
		{
		if(iNowPlayingWrapper->iPlaying)
			{
			HandleCommandL(EGotoNowPlayingViewCmd);
			}
		else
		{
		SyncPresetL(iUi->iLastPlayed->iLastPlayedStation->GetId(),
			iUi->iLastPlayed->iLastPlayedStation->GetLastModifiedTime());
		}
		}

	// Connect to User defined preset
	else if(currentIndex <= iUi->iFavPresets->iFavPresetList.Count()
		&& iUi->iFavPresets->iFavPresetList[currentIndex]->GetChannelType()
		   == CIRIsdsPreset::EUserDefined)
		{
		SetUrlFromPresetL(currentIndex);
		}

	// Sync ISDS preset
	else if(currentIndex <= iUi->iFavPresets->iFavPresetList.Count()
		&& iUi->iFavPresets->iFavPresetList[currentIndex]->GetChannelType())
		{
		SyncPresetL(iUi->iFavPresets->iFavPresetList[currentIndex]->GetId(),
					iUi->iFavPresets->iFavPresetList[currentIndex]->GetLastModifiedTime());
		}

	IRLOG_DEBUG("CIRMainView::ConnectToSelectedStationL - Exiting");
	}


void CIRMainView::AddStationtoFavoritesL()
	{
	IRLOG_DEBUG("CIRMainView::AddStationtoFavoritesL - Entering");
	TInt err = KErrNone;
	if(iUi->iLastPlayed->iLastPlayedStation->GetChannelType())
		{
		iUi->iFavPresets->AddPresetL(*iUi->iLastPlayed->iLastPlayedStation,err,1);
		}
	else
		{
		iUi->iFavPresets->AddPresetL(*iUi->iLastPlayed->iLastPlayedStation,err);
		}
	RBuf channel;
	HBufC* textResource = StringLoader::LoadLC( R_IRAPP_STATIONS_POPUP_SAVED,
		iUi->iLastPlayed->iLastPlayedStation->GetName() );
	channel.Create(*textResource);

	//If favourites is full then display an error message
	if(err==KErrNoMemory)
		{
		iIsdsWrapper->GetDialogLauncherInstance()->ShowErrorNoteL
						(R_IRAPP_ADDFAVOURITES_ERROR,ETrue);
		return;
		}
	else
		{
		if((err==KErrAlreadyExists )&&
		 ( iUi->iFavPresets->iFavPresetList.Count() == KPresetsArrayMaxLength) )
			{
			iIsdsWrapper->GetDialogLauncherInstance()->ShowErrorNoteL
							(R_IRAPP_ADDFAVOURITES_ERROR,ETrue);
			return;
			}
		else
			{
			iIsdsWrapper->GetDialogLauncherInstance()->
			ShowConfirmationNoteL( channel,ETrue );
			}
		}
	CleanupStack::PopAndDestroy(textResource);
	channel.Close();
	IRLOG_DEBUG("CIRMainView::AddStationtoFavoritesL - Exiting");
	}

// ---------------------------------------------------------------------------
// CIRMainView::LogoRequestL()
// called from the container
// ---------------------------------------------------------------------------
//
void CIRMainView::LogoRequestL()
	{
	IRLOG_DEBUG( "CIRMainView::LogoRequestL - Entering" );
	if(iContainer)
		{
		iUi->iIsdsWrapper->GetISDSInstance()->GetDataProvider()->GetHttpDataProvider()->
		GetLogoDownloadEngine()->SendRequestL(iContainer->GetTempPreset(),this,
									KLPReqFrom,KLPLogoSize,KLPLogoSize);
		}
	IRLOG_DEBUG( "CIRMainView::LogoRequestL - Exiting" );
	}



// ---------------------------------------------------------------------------
// CIRMainView::PresetLogoDownloadedL()
// from base class MLogoDownloadObserver
// ---------------------------------------------------------------------------
//
void CIRMainView::PresetLogoDownloadedL(CIRIsdsPreset* aPreset)
	{
	IRLOG_DEBUG( "CIRMainView::PresetLogoDownloadedL - Entering" );
	if(iContainer)
		{
		// store the last played station
		iUi->iLastPlayed->iLastPlayedStation->SetLogoData(aPreset->GetLogoData());
		TRAPD(err,iUi->iLastPlayed->CommitLastPlayedChannelL());
		TInt error=KErrNone;
		if(err != KErrNone)
			{
			error = iUi->iLastPlayed->RemoveLastPlayedFile();
			User::Leave(error);
			}
		iUi->iLastPlayed->RetriveLastPlayedChannelL();
		iContainer->PresetLogoDownloadL(aPreset);
		}
	IRLOG_DEBUG( "CIRMainView::PresetLogoDownloadedL - Exiting" );
	}


// ---------------------------------------------------------------------------
// CIRMainView::PresetLogoDownloadError()
// from base class MLogoDownloadObserver
// ---------------------------------------------------------------------------
//
void CIRMainView::PresetLogoDownloadError(CIRIsdsPreset* /*aPreset*/)
	{
	IRLOG_DEBUG( "CIRMainView::PresetLogoDownloadError " );
	//call the API of search results view container
	if(iContainer)
		{
		TRAP_IGNORE(iContainer->HandleLogoErrorL());
		}
	IRLOG_DEBUG( "CIRMainView::PresetLogoDownloadErrorL - Exiting" );
	}


// ---------------------------------------------------------------------
// ResponseL()
// From the class MViewsResponseAndErrorObserver
// ---------------------------------------------------------------------
void CIRMainView::ResponseL(CIRIsdsPreset* /*aPreset*/)
	{
	IRLOG_DEBUG( "CIRMainView::ResponseL " );
	IRLOG_DEBUG( "CIRMainView::ResponseL - Exiting" );
	}

// ---------------------------------------------------------------------
// PresetResponseL()
// From the class MViewsResponseAndErrorObserver
// ---------------------------------------------------------------------
void CIRMainView::PresetResponseL(CIRIsdsPreset* /*aPreset*/)
	{
	IRLOG_DEBUG( "CIRMainView::PresetResponseL " );
	IRLOG_DEBUG( "CIRMainView::PresetResponseL - Exiting" );
	}

// ---------------------------------------------------------------------
// ErrorL()
// From the class MViewsResponseAndErrorObserver
// ---------------------------------------------------------------------
void CIRMainView::ErrorL()
	{
	IRLOG_DEBUG( "CIRMainView::ErrorL " );
	IRRDEBUG2("CIRMAINVIEW::ERRORL",KNullDesC);
	if(iSyncReq)
		{
		iSyncReq = EFalse;
		iUi->GetPubSubManagerInstance()->PublishBufferingState(EIRStateBufferingError);

		iUi->iNowPlayingWrapper->DestroyWaitDialogL();
		}
	IRLOG_DEBUG( "CIRMainView::ErrorL - Exiting" );
	}




