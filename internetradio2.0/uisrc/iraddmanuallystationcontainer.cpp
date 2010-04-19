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
* Description:  Container class for channel list view
*
*/


#include <akniconutils.h>
#include <aknindicatorcontainer.h>
#include <aknutils.h>

#include <akniconarray.h>
#include <aknlists.h>
#include <aknview.h>
#include <barsread.h>
#include <eikenv.h>
#include <eiklabel.h>
#include <eikmenub.h>
#include <eikedwin.h>
#include <gulicon.h>
#include <stringloader.h>
#include <internetradio.rsg>
#include <internetradio.mbg>
#include <baclipb.h>
#include <aknsfld.h> //CAknSearchField
#include <txtetext.h>
#include <eikdialg.h> 
#include <akntoolbar.h>
#include <aknappui.h>
#include <akntouchpane.h>
#include <aknbutton.h>
#include <f32file.h>
#include <s32file.h>
#include <eikmenub.h>                       
#include <avkon.hrh>                        
#include <avkon.rsg>  
#include <aknnotewrappers.h>                     
#include <aknpopupfieldtext.h>              
#include <mgfetch.h> 
/* the head file is really uplayer case, so we don't need to change it.  */
#include <AknsConstants.h>
#include <eikaufty.h>
#include <hlplch.h>
#include <aknnavi.h>
#include <aknnavide.h> 

#include "irui.h"
#include "ir.hrh"
#include "ircommon.h"
#include "irdebug.h"
#include "irbaseview.h"
#include "iriconloader.h"
#include "irfavoritesdb.h"
#include "irdialoglauncher.h"
#include "irpreset.h"
#include "irhistory.h"

#include "irisdspreset.h"
#include "irchannelserverurl.h"
#include "irnowplayingwrapper.h"
#include "iraddmanuallystationview.h"
#include "iraddmanuallystationcontainer.h"
#include "irapplication.h"
#include "iraap.hlp.hrh" //Added to implement HELP option SPP
#include "irsettings.h"     //Added to maintain the IfAnyFav variable in settings


const TInt KBeginning = 0;
const TInt KMaxLength = 100;
const TInt KbufMaxLength = 10;

_LIT(KUnamedStation,"Unnamed station");
_LIT(KNullStation,"");
_LIT(KSpace," ");
	
const TInt KMaxSize = 256;	
// -----------------------------------------------------------------------------
// CIRAddManuallyStationContainer::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CIRAddManuallyStationContainer* CIRAddManuallyStationContainer::NewL(
											CIRAddManuallyStationView* aView)  
    {
    IRLOG_DEBUG( "CIRAddManuallyStationContainer::NewL - Begin" );
	CIRAddManuallyStationContainer* self = new ( ELeave ) CIRAddManuallyStationContainer(aView);//(aToolbar)
	CleanupStack::PushL( self );
	CleanupStack::Pop( self );
	IRLOG_DEBUG( "CIRAddManuallyStationContainer::NewL - Exiting" );
	return self;
    } 
    

// ---------------------------------------------------------------------------
// C++ default constructor can NOT contain any code, that might leave.
// ---------------------------------------------------------------------------
//
CIRAddManuallyStationContainer::CIRAddManuallyStationContainer( CIRAddManuallyStationView* aView):
								iView(aView)
    {
    IRLOG_DEBUG( "CIRAddManuallyStationContainer::CIRAddManuallyStationContainer" );
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CIRAddManuallyStationContainer::~CIRAddManuallyStationContainer()
    {
    IRLOG_DEBUG( "CIRAddManuallyStationContainer::~CIRAddManuallyStationContainer" );
	if(iPreset)
		{
		delete iPreset;
		iPreset=NULL;	
		}
	IRLOG_DEBUG( "CIRAddManuallyStationContainer::~CIRAddManuallyStationContainer" );
    }


// ---------------------------------------------------------------------------
// From class CCoeControl.
// Opens help by context.
// ---------------------------------------------------------------------------
//
EXPORT_C void CIRAddManuallyStationContainer::GetHelpContext( TCoeHelpContext& aContext ) const
    {
    IRLOG_DEBUG( "CIRAddManuallyStationContainer::GetHelpContext Begin" );
    aContext = TCoeHelpContext( KIRMCVUid, KIR_HELP_ADD_URL );
    IRLOG_DEBUG( "CIRAddManuallyStationContainer::GetHelpContext End" );
    }


// ---------------------------------------------------------------------------
// CIRAddManuallyStationContainer::DynInitMenuPaneL()
// Dynamically initialises a menu pane
// ---------------------------------------------------------------------------
//
void CIRAddManuallyStationContainer::DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane )
    { 
    IRLOG_DEBUG( "CIRAddManuallyStationContainer::DynInitMenuPaneL - Begin " );
  	CAknForm::DynInitMenuPaneL( aResourceId, aMenuPane );
    if ( aResourceId == R_AVKON_FORM_MENUPANE )        //R_AVKON_FORM_MENUPANE
        {
        aMenuPane->SetItemDimmed( EAknFormCmdLabel, ETrue);
        aMenuPane->SetItemDimmed( EAknFormCmdAdd, ETrue );
        aMenuPane->SetItemDimmed( EAknFormCmdDelete, ETrue );
        aMenuPane->SetItemDimmed( EAknFormCmdSave, ETrue );
        aMenuPane->SetItemDimmed( EAknFormCmdEdit, ETrue );
        if(iView->iNowPlayingWrapper->iPlaying)
			{
			aMenuPane->SetItemDimmed(EGotoNowPlayingViewCmd,EFalse);
			}
		else
			{
			aMenuPane->SetItemDimmed(EGotoNowPlayingViewCmd,ETrue);
			}
	    }
    IRLOG_DEBUG( "CIRAddManuallyStationContainer::DynInitMenuPaneL - Exiting." );
    }

//-------------------------------------------------------------------------------
// CIRAddManuallyStationContainer::ProcessCommandL
//  From CAknForm, takes care of command handling.
//  @param aCommand Command to be handled
//--------------------------------------------------------------------------------
void CIRAddManuallyStationContainer::ProcessCommandL( TInt aCommand )    
    {
    IRLOG_DEBUG( "CIRAddManuallyStationContainer::ProcessCommandL - Begin " );
	CIRUi* appUi = static_cast<CIRUi*>( iCoeEnv->AppUi() );
	switch(aCommand)
		{
	   	case EGotoNowPlayingViewCmd:
	   		 {
	  	  	 iView->HandleCommandL(EGotoNowPlayingViewCmd);
	       	 break;
	      	 }
	    case EHelpCmd:
	         {
	      	 iView->HandleCommandL(EHelpCmd);
	       	 break;
	      	 }
	  	case EExitCmd:
	  	  	 {
	  		 iView->HandleCommandL(EExitCmd);
	  		 break;
	  		 }
	  	default:
		   	 iView->HandleCommandL( aCommand );
	    	 
	 	}
	CAknForm::ProcessCommandL(aCommand);
    IRLOG_DEBUG( "CIRAddManuallyStationContainer::ProcessCommandL - Exiting " );
    }
    


// -----------------------------------------------------------------------------
// CIRAddManuallyStationContainer::OkToExitL
// 
// -----------------------------------------------------------------------------
TBool CIRAddManuallyStationContainer::OkToExitL( TInt aButtonId )
    {
    IRLOG_DEBUG( "CIRAddManuallyStationContainer::OkToExitL - Begin " );
 	if ( aButtonId == EAknSoftkeyCancel)
		{
		iView->HandleCommandL(EAknSoftkeyBack);
		}
	if ( aButtonId == EAknSoftkeyOptions )
		{
		DisplayMenuL();
		}
    if( aButtonId == EAddExit )
	    {
	  	iView->HandleCommandL(EAknSoftkeyBack);
	  	return ETrue;
	    }
	TBool ret = CAknForm::OkToExitL( aButtonId );
	
    IRLOG_DEBUG( "CIRAddManuallyStationContainer::OkToExitL - Exiting " );
    return ret;
    }    

void CIRAddManuallyStationContainer::PreLayoutDynInitL()
	{
	CAknForm::PreLayoutDynInitL();
	}

// -----------------------------------------------------------------------------
// CIRAddManuallyStationContainer::PostLayoutDynInitL
// 
// -----------------------------------------------------------------------------
void CIRAddManuallyStationContainer::PostLayoutDynInitL()
	{
	IRLOG_DEBUG( "CIRAddManuallyStationContainer::PostLayoutDynInitL - Begin " );
	CIRUi* appUi = static_cast<CIRUi*>( iCoeEnv->AppUi() );
	//for enabling paste button if clipboard contains some data
	RFs& fs = iEikonEnv->FsSession();
	CClipboard* clipBoard = CClipboard::NewForReadingLC(fs);
	TBuf<KMaxLength> pasteData;
	CPlainText* plainText = CPlainText::NewL();
	CleanupStack::PushL(plainText);
	plainText->PasteFromStoreL(clipBoard->Store(), clipBoard->StreamDictionary(),KBeginning);
    plainText->Extract(pasteData,KBeginning,KMaxLength);
    TInt length=pasteData.Length() ;
    if(length)
     	{
    	iView->iIsCopied=ETrue;
    	iView->SetToolbarItems();
     	}
    CleanupStack::PopAndDestroy(plainText);
    CleanupStack::PopAndDestroy(clipBoard);
	iIsEditStation=iView->iUi->GetEditStation();
	if(iIsEditStation)
	   	{
		TInt urlCount = iView->iNowPlayingWrapper->iEditStationPreset->GetUrlCount();
	   	if (urlCount <= 0)
	  		{
	  		DefaultVauleL();
	  		iView->iIsTextEntered=EFalse;
			}
		else
			{
			SetEdwinTextL(EAddStaionURL,
						  &iView->iNowPlayingWrapper->iEditStationPreset->GetChannelUrlAtL(0) );
			TBuf<KMaxSize> unnamedStation(KUnamedStation);
			TBuf<KMaxSize> stationName =	iView->iNowPlayingWrapper->iEditStationPreset->GetName();		  	
			if(stationName == unnamedStation)
				{
				stationName.Copy(KNullStation);
				}
			SetEdwinTextL(EAddStaionName,
			              &stationName);	
			              
			SetEdwinTextL(EAddStaionDescription,
			              &iView->iNowPlayingWrapper->iEditStationPreset->GetDescription());
						              
			iView->iIsTextEntered=ETrue;
			}
		iView->SetAddStationsTitleL();
		iView->SetToolbarItems();
	   	}
	else
		{
		DefaultVauleL();
		iView->iIsTextEntered=EFalse;
		iView->SetToolbarItems();
		}

//	CEikDialog::SetTitleL(R_IRAPP_ADDMANUALLY_TITLE)
	CAknForm::PostLayoutDynInitL();
	iView->iUi->HandleResourceChangeL(KEikDynamicLayoutVariantSwitch);
	HandleResourceChange(KEikDynamicLayoutVariantSwitch);
	IRLOG_DEBUG( "CIRAddManuallyStationContainer::PostLayoutDynInitL - Exiting " );
	} 



// -----------------------------------------------------------------------------
// CIRAddManuallyStationContainer::DefaultVauleL()
// 
// -----------------------------------------------------------------------------
void CIRAddManuallyStationContainer::DefaultVauleL()
	{
	IRLOG_DEBUG( "CIRAddManuallyStationContainer::DefaultVauleL - Begin " );
	HBufC *httpstring = StringLoader::LoadLC(R_IRAPP_ADDMANUALLY_HTTP);
	SetEdwinTextL( EAddStaionURL, httpstring );
	CleanupStack::PopAndDestroy(httpstring);
		    	
	HBufC *unnamedstring = StringLoader::LoadLC(R_IRAPP_ADDMANUALLY_NONAME);
	SetEdwinTextL( EAddStaionName, unnamedstring );
	CleanupStack::PopAndDestroy(unnamedstring);
	_LIT(blank,"");
	TBuf<KbufMaxLength> buf(blank);  
	SetEdwinTextL( EAddStaionDescription,&buf );
	
	CEikDialog::SetInitialCurrentLine();
    TryChangeFocusToL(EAddStaionURL);
    
    CCoeControl* cntrl = CEikDialog::Control(EAddStaionURL);
	CEikEdwin* myEdwin = static_cast<CEikEdwin*> ( cntrl );
	TInt txtLength=myEdwin->TextLength();	
	myEdwin->SetCursorPosL (txtLength, EFalse);
    	
	iView->SetAddStationsTitleL();
	
		
	IRLOG_DEBUG( "CIRAddManuallyStationContainer::DefaultVauleL - Exiting " );
	}
	
// -----------------------------------------------------------------------------
// CIRAddManuallyStationContainer::CopyData
// 
// -----------------------------------------------------------------------------
void CIRAddManuallyStationContainer::CopyDataL()
	{
	IRLOG_DEBUG( "CIRAddManuallyStationContainer::CopyDataL - Begin " );
	RFs& fs = iEikonEnv->FsSession();
	CClipboard*	clipBoard = CClipboard::NewForWritingLC(fs);
	CPlainText*	plainText = CPlainText::NewL();
	CleanupStack::PushL(plainText);
	TInt idoffocused = CEikDialog::IdOfFocusControl();
	CCoeControl* ictrl = CEikDialog::Control(idoffocused);
	CEikEdwin* myEdwin = static_cast<CEikEdwin*> ( ictrl );
	TCursorSelection sel=myEdwin->Selection();
	GetEdwinText( iSelText, idoffocused );
	TInt txtlength=myEdwin->TextLength();
	TInt length = sel.Length();
	if(length)
		{
		HBufC* number = HBufC::NewLC( length );
		TPtr numberPtr = number->Des();
		TInt pos=sel.LowerPos();
		if (pos < iSelText.Length())
			numberPtr.Copy(&iSelText[pos],length);		
		plainText->InsertL(0, *number);
		plainText->CopyToStoreL(clipBoard->Store(), clipBoard->StreamDictionary(),KBeginning, 
		                        plainText->DocumentLength());
		clipBoard->CommitL();
		CleanupStack::PopAndDestroy(number);	
		}
	else
		{
		myEdwin->SetSelectionL(0,txtlength);
		plainText->InsertL(0, iSelText);
		plainText->CopyToStoreL(clipBoard->Store(), clipBoard->StreamDictionary(),KBeginning, 
		                        plainText->DocumentLength());
		clipBoard->CommitL();
		}
	CleanupStack::PopAndDestroy(plainText);
	CleanupStack::PopAndDestroy(clipBoard);
	IRLOG_DEBUG( "CIRAddManuallyStationContainer::CopyDataL - Exiting " );
	}

// -----------------------------------------------------------------------------
// CIRAddManuallyStationContainer::PasteData
// 
// -----------------------------------------------------------------------------
void CIRAddManuallyStationContainer::PasteDataL()
    {
    IRLOG_DEBUG( "CIRAddManuallyStationContainer::PasteDataL - Begin " );
    RFs& fs = iEikonEnv->FsSession();
	CClipboard* clipBoard = CClipboard::NewForReadingLC(fs);
	TBuf<KMaxLength> clipBoardText;
	CPlainText* plainText = CPlainText::NewL();
	CleanupStack::PushL(plainText);
	plainText->PasteFromStoreL(clipBoard->Store(), clipBoard->StreamDictionary(),KBeginning);
    TInt idOfFocused=CEikDialog::IdOfFocusControl();
    CCoeControl* ictrl=CEikDialog::Control(idOfFocused);
    CEikEdwin* myEdwin = static_cast<CEikEdwin*> ( ictrl );
    TCursorSelection sel=myEdwin->Selection();
	GetEdwinText( iSelText, idOfFocused );
	HBufC* number = HBufC::NewLC(KMaxLength);
	TPtr numberPtr = number->Des();
	TInt length = sel.Length();
	TInt txtLength=myEdwin->TextLength();
	TInt diffLength=KMaxLength-(txtLength-length);
	plainText->Extract(clipBoardText,KBeginning,diffLength);
	if (txtLength!=0)
		{
		numberPtr.Copy(&iSelText[0],txtLength);	
		if(length!=0)
			{
			TInt pos=sel.LowerPos();
	
			numberPtr.Delete(pos,length);
			numberPtr.Insert(pos,clipBoardText);
			SetEdwinTextL( idOfFocused, number ); 
			}
		else
			{
			TInt pos=myEdwin->CursorPos();
			numberPtr.Insert(pos,clipBoardText);
			SetEdwinTextL( idOfFocused, number ); 
			}
	    }
	else
		{
		numberPtr.Insert(KBeginning,clipBoardText);
		SetEdwinTextL( idOfFocused, number );	
		}
	TBool enable=ValidateUrlL();
	if(enable)
		{
		iView->iIsTextEntered=ETrue;
		iView->SetToolbarItems();			    
		}
	else
		{
		iView->iIsTextEntered=EFalse;
	   	iView->SetToolbarItems();
		}
	TInt txtLengthAfterPaste=myEdwin->TextLength();	
	myEdwin->SetSelectionL(0,0);
	myEdwin->SetCursorPosL (txtLengthAfterPaste, EFalse);

	CleanupStack::PopAndDestroy(number); 
	CleanupStack::PopAndDestroy(plainText);
	CleanupStack::PopAndDestroy(clipBoard);
	IRLOG_DEBUG( "CIRAddManuallyStationContainer::PasteDataL - Exiting " );
	}

// -----------------------------------------------------------------------------
// CIRAddManuallyStationContainer::SaveData
// 
// -----------------------------------------------------------------------------
void CIRAddManuallyStationContainer::SaveDataL()
	{
	IRLOG_DEBUG( "CIRAddManuallyStationContainer::SaveDataL - Begin " );
	CIRUi* appUi = static_cast<CIRUi*>( iCoeEnv->AppUi() );
	GetEdwinText( iStationURL, EAddStaionURL );
	GetEdwinText( iStationName, EAddStaionName );
	GetEdwinText( iStationDescription, EAddStaionDescription );
	
	ReplaceUglyChar(iStationName);
	ReplaceUglyChar(iStationDescription);
	
		
	if(iIsEditStation)
		{
		EditStationL();
		iView->iIsTextEntered=EFalse;
		}
		
    if(!iIsEditStation)
		{
		TBool urlStatus = iView->iUi->ParseAndPrepareUrl(iStationURL);
		if( !urlStatus )
			{
			iIsSpaceEntered=ETrue;
			iDialogLauncher->ShowErrorNoteL(R_IRAPP_ERROR_ADDRESS, ETrue);
			return;	
			}
		else 
			{
			AddToFavouritesL();
			iView->iIsTextEntered=EFalse;
			iView->SetToolbarItems();
			DefaultVauleL();
			}
		}
	IRLOG_DEBUG( "CIRAddManuallyStationContainer::SaveDataL - Exiting " );
	}

/**
 * void CIRAddManuallyStationContainer::AddToFavouritesL()
 *
 * Description		: This method is used to Add the stations to Favourites
 *  
 * @Algorithm		: 
 *		      Step 1: Validate the Staion name by calling 'ReplaceEmptyName'
 *			          method
 *		      Step 2: Validate the entered url by calling 'ParseAndPrepareUrl'
 *                    method.
 *            Step 3: If Url is incorrect then display an error message, else
 *					  goto step4.
 *            Step 4: 
 *            Step 5: 
 *                                 
 * @Param			: [IN]  Nil
 *					  [OUT] Nil
 *
 * @Return		    :
 *
 * Example   Usage	: 
 */
void CIRAddManuallyStationContainer::AddToFavouritesL()
	{
	IRLOG_DEBUG( "CIRAddManuallyStationContainer::AddToFavouritesL - Begin " );
	
	ReplaceEmptyName( iStationName );  
	
	TBool isInvalidChar = HasInvalidCharsInUrl(iStationURL);
	if( !isInvalidChar)
		{
		iIsSpaceEntered=ETrue;
		iDialogLauncher->ShowErrorNoteL(R_IRAPP_ERROR_ADDRESS, ETrue);
		return;	
		}
		
	/**
	 * Validate the entered URL 
	 */
	TBool urlStatus = iView->iUi->ParseAndPrepareUrl(iStationURL);
	
	/**
	 * If the entered URL is incorrect then display the error message
	 * "Incorrect URL"
	 */
	if( !urlStatus )
		{
		iDialogLauncher = CIRDialogLauncher::NewL();
		iView->iIsSpaceEntered=ETrue;
		iDialogLauncher->ShowErrorNoteL(R_IRAPP_ERROR_ADDRESS, ETrue);
		delete iDialogLauncher;
		return;	
		}
	if(iPreset)
		{
		delete iPreset;
		iPreset=NULL;	
		}
	   iPreset = CIRIsdsPreset::NewL();
	   TInt err(KErrNoMemory);
	   iPreset->SetName(iStationName);
	   iPreset->SetShortDesc(iStationDescription);
	   
	   /**
	    * only one url is saved for manually added channel
	    */
	   iPreset->SetUrlCount(1);
	   
	   _LIT(KNotAvailable,"NA");
	   
	   /**
	    * bitrate set to zero so that it is accessible to all kinds of 
	    * bitrate settings
	    */
	   iPreset->SetUrlL(KNotAvailable,iStationURL,0);
	   
	   /**
	    * Set the Description
	    */
	   iPreset->SetDescription(iStationDescription);
	   
	   /**
	    * Save into the favourites
	    */
	   iView->iUi->iFavPresets->AddPresetL(*iPreset,err);		
	   
	   /**
	    * If there is no space in favourites display error note.	
	    */
	   if (err == KErrNoMemory)
			{
			iDialogLauncher->ShowErrorNoteL(R_IRAPP_ADDFAVOURITES_ERROR, ETrue);			
			}
	   else
			{
			if(iUnNamedFlag)
				{
				iStationName.Copy(KNullStation);
				iUnNamedFlag = EFalse;
				iDialogLauncher->ShowConfirmationNoteL( R_IRAPP_STATION_SAVED,ETrue );
				}
			else
				{
				iView->iUi->DisplayInformationL(R_IRAPP_STATIONS_POPUP_SAVED, iStationName);
				}
			iView->iUi->iIRSettings->SetFlagIfAnyFavL();
			}
	IRLOG_DEBUG( "CIRAddManuallyStationContainer::AddToFavouritesL - Exiting " );
	}

/**
 * void CIRAddManuallyStationContainer::EditStationL()
 *
 * Description		: This method is used to Edit the userdefined stations
 *  
 * @Algorithm		: 
 *		      Step 1: Validate the entered Url by using 'ParseAndPrepareUrl'
 *		      Step 2: If Entered Url is incorrect then display an error 
 *					  message, else goto step3.
 *            Step3 : calculate the Index of the station to be deleted
 *            Step4 : create an instance of 'CIRIsdsPreset'
 *            Step5 : Set the new name of the station
 *			  Step6 : Set the Short description to the station
 *			  Step7 : Set the Url 
 *			  Step8 : Set the Bitrate
 *			  Step9 : Set the UniqId of the Station to edited
 *			  Step10: Update the station with modifed values in favourites
 *                    by calling 'ReplaceUserDefinedPresetL'
 *			  Step11: Display a confirmation note that station saved
 *                                 
 * @Param			: [IN]
 *					  [OUT]
 *
 * @Return		    :
 *
 * Example Usage	:
 */
void CIRAddManuallyStationContainer::EditStationL()
	{
	IRLOG_DEBUG( "CIRAddManuallyStationContainer::EditStationL - Entered" );
	
	ReplaceUglyChar(iStationName);
	
	TBool isInvalidChar = HasInvalidCharsInUrl(iStationURL);
	if( !isInvalidChar)
		{
		iIsSpaceEntered=ETrue;
		iDialogLauncher->ShowErrorNoteL(R_IRAPP_ERROR_ADDRESS, ETrue);
		return;	
		}
		 
	TBool urlStatus = iView->iUi->ParseAndPrepareUrl(iStationURL);
	if( !urlStatus )
		{
		iIsSpaceEntered=ETrue;
		iDialogLauncher->ShowErrorNoteL(R_IRAPP_ERROR_ADDRESS, ETrue);
		return;	
		}
	else
		{
		TInt deleteIndex ;
		if (iView->iUi->iLastPlayed->FileExists())
			{
			deleteIndex = iView->iUi->iEditStationIndex-1;
			}
		else
			{
			deleteIndex = iView->iUi->iEditStationIndex;
			}
		/**
		 * prepare isdsPreset with the edited values
		 */	
		CIRIsdsPreset* isdsPreset =  CIRIsdsPreset::NewL();
		
		/**
		 * Set the Station Name with new station name
		 */
		if(iStationName.Length()==0)
			{
			ReplaceEmptyName( iStationName );  
			}
		isdsPreset->SetName(iStationName);
		
		/**
		 * Set Description with the new Description
		 */
		isdsPreset->SetShortDesc(iStationDescription);
	    
	    /**
	     *only one url is saved for manually added channel
	     */
	    isdsPreset->SetUrlCount(1);
	    
	    _LIT(KNotAvailable,"NA");
	    
	    /**
	     * bitrate set to zero so that it is accessible to all kinds of 
	     * bitrate settings
	     */
	    isdsPreset->SetUrlL(KNotAvailable,iStationURL,0);
	    
	    isdsPreset->SetDescription(iStationDescription);
	    
	    /**
	     * Set the UniqId of the Station to edit
	     */
	    if (deleteIndex < iView->iUi->iFavPresets->iFavPresetList.Count())
	       isdsPreset->SetUniqId(iView->iUi->iFavPresets->
	                          iFavPresetList[deleteIndex]->Id());
	    /**
	     * Replace the preset with the new values 
	     */                      
	   	iView->iUi->iFavPresets->ReplaceUserDefinedPresetL(*isdsPreset);
		
		/**
		 * display a confirmation note
		 */
		if(iUnNamedFlag)
			{
			iStationName.Copy(KNullStation);
			iUnNamedFlag = EFalse;
			iDialogLauncher->ShowConfirmationNoteL( R_IRAPP_STATION_SAVED,ETrue );
			}
		else
			{
			iView->iUi->DisplayInformationL(R_IRAPP_STATIONS_POPUP_SAVED, 
			                                iStationName);
			}
		if (isdsPreset)
			{
			delete isdsPreset;
			isdsPreset = NULL;				
			}
		}
	IRLOG_DEBUG( "CIRAddManuallyStationContainer::EditStationL - Exited " ); 
	}

// ----------------------------------------------------------------------------
// CIRAddManuallyStationContainer::ReplaceEmptyName( TDes& aUrlName )
// Checks whether the url name contains only spaces and if the descriptor really was.
// empty replaces its content with default name
// ----------------------------------------------------------------------------
void CIRAddManuallyStationContainer::ReplaceEmptyName( TDes& aUrlName )
	{
	IRLOG_DEBUG( "CIRAddManuallyStationContainer::ReplaceEmptyName - Begin " );
	TBool isEmpty = ETrue;
	for( TInt i = 0; i < aUrlName.Length() && isEmpty; i++ )
		{
		( static_cast<TChar>( aUrlName[i] ) ).Fold();
		if ( !( static_cast<TChar>( aUrlName[i] ) ).IsSpace() )
			{
			isEmpty = EFalse;
			}
	   	}
	    if ( isEmpty )		
	  		{
	    	StringLoader::Load( aUrlName, R_IRAPP_ADDMANUALLY_NOTEXT );
	    	iUnNamedFlag = ETrue;
	  		}
	    IRLOG_DEBUG( "CIRAddManuallyStationContainer::ReplaceEmptyName - Exiting " );
	}


void CIRAddManuallyStationContainer::ReplaceUglyChar(TDes& aUrlName)
	{
	for(TInt i = 0; i < aUrlName.Length(); i++)
		{
		TChar xChar(aUrlName[i]);
		if (xChar.GetCategory() == TChar::EZpCategory)
			{
			aUrlName.Replace(i, 1, KSpace);				
			}
		}
	}

// ----------------------------------------------------------------------------
// CIRAddManuallyStationContainer::HandlePointerEventL(const TPointerEvent&  
// aPointerEvent)
// This function is used handle pen input events 
// ----------------------------------------------------------------------------
void CIRAddManuallyStationContainer::HandlePointerEventL(const TPointerEvent &aPointerEvent) 
	{
	IRLOG_DEBUG( "CIRAddManuallyStationContainer::HandlePointerEventL - Begin " );
    CAknForm::HandlePointerEventL( aPointerEvent );
    IRLOG_DEBUG( "CIRAddManuallyStationContainer::HandlePointerEventL - Exiting " );
    }

// ---------------------------------------------------------------------------
//TBool CIRAddManuallyStationContainer::ValidateUrlL() 
//Validates The entered URL Scheme 
// ---------------------------------------------------------------------------
TBool CIRAddManuallyStationContainer::ValidateUrlL()
	{
	IRLOG_DEBUG( "CIRAddManuallyStationContainer::ValidateUrlL - Begin " );
	TBool retVal = EFalse;
	_LIT(KMMSText, "mms://");
	
	TBuf<KbufMaxLength> iTextBoxValidateUrl;
	HBufC *httpstring1 = StringLoader::LoadLC(R_IRAPP_ADDMANUALLY_HTTP);
	iTextBoxValidateUrl = *httpstring1;

   	HBufC* mmsString  = HBufC::NewLC(KbufMaxLength);
	*mmsString = KMMSText;
	TBuf<KbufMaxLength> textBoxValidateMMS(*mmsString);

	
	GetEdwinText( iStationURL, EAddStaionURL );
	
	CleanupStack::PopAndDestroy(mmsString);
	CleanupStack::PopAndDestroy(httpstring1);

    if ( iTextBoxValidateUrl.CompareF( iStationURL.Left( iTextBoxValidateUrl.Length()))== 0)
		{
		if ( iStationURL.Length() <= iTextBoxValidateUrl.Length() ||
    	     iTextBoxValidateUrl.CompareF( iStationURL.Left( iTextBoxValidateUrl.Length() ))) 
			{
			retVal = EFalse;
			}
	    else 
	    	{
			retVal = ETrue;
			}
		}
	else if (textBoxValidateMMS.CompareF( iStationURL.Left( textBoxValidateMMS.Length()))==0)
		{
		if ( iStationURL.Length() <= textBoxValidateMMS.Length() ||
    	     textBoxValidateMMS.CompareF( iStationURL.Left( textBoxValidateMMS.Length() ))) 
			{
			retVal = EFalse;
			}
	    else 
	    	{
			retVal = ETrue;
			}
		}

    else
	    {
		//nothing	    	
	    }
	IRLOG_DEBUG( "CIRAddManuallyStationContainer::ValidateUrlL - Exiting " );	
	return retVal;
	}
	
	
// ---------------------------------------------------------------------------
// void CIRAddManuallyStationContainer::HandleControlEventL(CCoeControl* aControl, 
//                                                          TCoeEvent aEventType)
// To handle key events.
// 
// ---------------------------------------------------------------------------

void CIRAddManuallyStationContainer::HandleControlEventL( 
                                                    CCoeControl* aControl, 
                                                    TCoeEvent aEventType)
	{
	IRLOG_DEBUG( "CIRAddManuallyStationContainer::HandleControlEventL - Begin " );
    CAknForm::HandleControlEventL(aControl, aEventType);
    if (aEventType == EEventStateChanged)
    	{
        if (aControl == Control(EAddStaionURL))
            {
		  	TInt focus=CEikDialog::IdOfFocusControl();
	      	CCoeControl* ictrl=CEikDialog::Control(focus);
	      	TBool enable=ValidateUrlL();
	      	iIsEnable = enable;
	    	if(enable)
	  			{
	  			iView->iIsTextEntered=ETrue;
	    	 	iView->SetToolbarItems();			    
	  			}
	  		else
	  			{
	  			iView->iIsTextEntered=EFalse;
	    	 	iView->SetToolbarItems();
	  			}
            }
        if (aControl == Control(EAddStaionName))
            {
            }  
    	}
    IRLOG_DEBUG( "CIRAddManuallyStationContainer::HandleControlEventL - Exiting " );
	}
	
	
// ---------------------------------------------------------------------------
//void CIRAddManuallyStationContainer::HandleResourceChange (TInt aType)
//
// 
// ---------------------------------------------------------------------------

void CIRAddManuallyStationContainer::HandleResourceChange (TInt /*aType*/)
	{
	IRLOG_DEBUG( "CIRAddManuallyStationContainer::HandleResourceChange - Begin " );
    CAknForm::HandleResourceChange(KEikDynamicLayoutVariantSwitch);
    
    iView->RefreshView();
    
	SizeChanged();
	DrawDeferred();
    IRLOG_DEBUG( "CIRAddManuallyStationContainer::HandleResourceChange - Exiting " );   
	}

// ---------------------------------------------------------------------------
//void CIRAddManuallyStationContainer::SizeChanged ()
//
// ---------------------------------------------------------------------------

void CIRAddManuallyStationContainer::SizeChanged()
	{
	CAknForm::SizeChanged();		
	}
	
// ---------------------------------------------------------------------------
//TBool CIRAddManuallyStationContainer::HasInvalidCharsInUrl (TDes& aUrl)
//
// ---------------------------------------------------------------------------

TBool CIRAddManuallyStationContainer::HasInvalidCharsInUrl(TDes& aUrl)
	{
	TBool retVal = EFalse;
	for(TInt i = 0; i < aUrl.Length(); i++)
		{
		TChar xChar(aUrl[i]);
		switch(xChar.GetCategory())
			{
			case TChar::ESeparatorGroup: 
			case TChar::EControlGroup:
			case TChar::EZpCategory:
			case TChar::EWhitespace:
			case TChar::EZlCategory:
			case TChar::EParagraphSeparator:
			case TChar::ESegmentSeparator:
		  	case TChar::EPsCategory: 
			case TChar::EPeCategory:
				{
				retVal = EFalse;
				}
			break;
			default:
				{
				retVal = ETrue;
				}
			}
		(static_cast<TChar>( aUrl[i] ) ).Fold();
		if( ( static_cast<TChar>( aUrl[i] ) ) == '%' || 
		    ( static_cast<TChar>( aUrl[i] ) ) == '^' ||
		    ( static_cast<TChar>( aUrl[i] ) ) == '"' ||
		    ( static_cast<TChar>( aUrl[i] ) ) == '\\'||
		    ( static_cast<TChar>( aUrl[i] ) ) == '`' ||
		    ( static_cast<TChar>( aUrl[i] ) ) == '|' ||
		    ( static_cast<TChar>( aUrl[i] ) ) == '<' ||
		    ( static_cast<TChar>( aUrl[i] ) ) == '>' ||
		    ( static_cast<TChar>( aUrl[i] ) ) == '#' )
			{
			retVal = EFalse;					
			}
		
		
		if(!retVal)
			{
			i = aUrl.Length();							
			}
		}
	return retVal;		
	}
	
