/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Implementation for a class showing dialogs and notes.
*
*/


#include <aknglobalnote.h>
#include <aknlists.h>
#include <aknnotewrappers.h>
#include <aknsddata.h>
#include <bldvariant.hrh>
#include <etelpckt.h>
#include <gsmerror.h>
#include <stringloader.h>
#include <textresolver.h>
#include <aknquerydialog.h>
#include <eikdialg.h>
#include "irdialoglauncher.h"
#include <internetradio.rsg>
#include "irdebug.h"

// ---------------------------------------------------------------------------
// C++ default constructor can NOT contain any code, that might leave.
// ---------------------------------------------------------------------------
//
CIRDialogLauncher::CIRDialogLauncher()
    {
    IRLOG_DEBUG("CIRDialogLauncher::CIRDialogLauncher" );
    }


// ---------------------------------------------------------------------------
// EPOC default constructor can leave.
// ---------------------------------------------------------------------------
//
void CIRDialogLauncher::ConstructL()
    {
    IRLOG_DEBUG("CIRDialogLauncher::ConstructL" );
	return;
    }

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CIRDialogLauncher* CIRDialogLauncher::NewL()
    {
    IRLOG_DEBUG("CIRDialogLauncher::NewL - Entering" );
    CIRDialogLauncher* self = new( ELeave ) CIRDialogLauncher();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    IRLOG_DEBUG("CIRDialogLauncher::NewL - Exiting" );
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//            
CIRDialogLauncher::~CIRDialogLauncher()
    { 
    IRLOG_DEBUG("CIRDialogLauncher::~CIRDialogLauncher" );
    }
            


// ---------------------------------------------------------------------------
// CIRDialogLauncher::ShowConfirmationNoteL
// ---------------------------------------------------------------------------
//
void CIRDialogLauncher::ShowConfirmationNoteL( TInt aTextResourceId, TBool aWaiting ) const
    {
    IRLOG_DEBUG("CIRDialogLauncher::ShowConfirmationNoteL - Entering" );
    HBufC* noteTxt = StringLoader::LoadLC( aTextResourceId );
    ShowConfirmationNoteL( *noteTxt, aWaiting );
    CleanupStack::PopAndDestroy( noteTxt );
    IRLOG_DEBUG("CIRDialogLauncher::ShowConfirmationNoteL - Exiting" );
    }

// ---------------------------------------------------------------------------
// CIRDialogLauncher::ShowConfirmationNoteL
// ---------------------------------------------------------------------------
//
void CIRDialogLauncher::ShowConfirmationNoteL( const TDesC& aText, TBool aWaiting ) const
    {
    IRLOG_DEBUG("CIRDialogLauncher::ShowConfirmationNoteL - Entering" );
    CAknConfirmationNote* note = new( ELeave ) CAknConfirmationNote( aWaiting );
    note->ExecuteLD( aText );
    IRLOG_DEBUG("CIRDialogLauncher::ShowConfirmationNoteL - Exiting" );
    }

// ---------------------------------------------------------------------------
// CIRDialogLauncher::ShowInformationNoteL
// ---------------------------------------------------------------------------
//
void CIRDialogLauncher::ShowInformationNoteL( TInt aTextResourceId,
                                              TBool aWaiting, TInt aDialogId ) const
    {
    IRLOG_DEBUG("CIRDialogLauncher::ShowInformationNoteL - Entering" );
    HBufC* noteTxt = StringLoader::LoadLC( aTextResourceId );
    CAknInformationNote* note = new( ELeave ) CAknInformationNote( aWaiting );
    CleanupStack::PushL(note);
    if ( aDialogId != KErrNotFound )
        {
        note->PublishDialogL( aDialogId );
        }
    note->ExecuteLD( *noteTxt );
    CleanupStack::Pop(note);
    CleanupStack::PopAndDestroy( noteTxt );
    IRLOG_DEBUG("CIRDialogLauncher::ShowInformationNoteL - Exiting" );
    }

// ---------------------------------------------------------------------------
// CIRDialogLauncher::ShowInformationNoteL
// ---------------------------------------------------------------------------
//
void CIRDialogLauncher::ShowInformationNoteL( TInt /*aErrorCode*/ ) const
    {
    IRLOG_DEBUG("CIRDialogLauncher::ShowInformationNoteL - Entering" );
/*    HBufC* noteTxt= ResolveDefaultErrorTextLC( aErrorCode );
    
    CAknInformationNote* note = new( ELeave ) CAknInformationNote( ETrue );
    note->ExecuteLD( *noteTxt );
    CleanupStack::PopAndDestroy( noteTxt );
*/    
    IRLOG_DEBUG("CIRDialogLauncher::ShowInformationNoteL - Exiting" );
	return;
    }

// ---------------------------------------------------------------------------
// CIRDialogLauncher::ShowErrorNoteL
// ---------------------------------------------------------------------------
//
void CIRDialogLauncher::ShowErrorNoteL( TInt aTextResourceId, TBool aWaiting ) const
    {
    IRLOG_DEBUG("CIRDialogLauncher::ShowErrorNoteL - Entering" );
    HBufC* noteTxt = StringLoader::LoadLC( aTextResourceId );
    CAknErrorNote* note = new( ELeave ) CAknErrorNote( aWaiting );
    note->ExecuteLD( *noteTxt );
    CleanupStack::PopAndDestroy( noteTxt );
    IRLOG_DEBUG("CIRDialogLauncher::ShowErrorNoteL - Exiting" );
    }

// ---------------------------------------------------------------------------
// CIRDialogLauncher::ShowGlobalNoteL
// ---------------------------------------------------------------------------
//
void CIRDialogLauncher::ShowGlobalNoteL( TAknGlobalNoteType aType, TInt aTextResourceId ) const
    {
    IRLOG_DEBUG("CIRDialogLauncher::ShowGlobalNoteL - Entering" );
	HBufC* noteTxt = StringLoader::LoadLC( aTextResourceId );
	CAknGlobalNote* note = CAknGlobalNote::NewLC();
	note->ShowNoteL( aType, *noteTxt );
	CleanupStack::PopAndDestroy( 2, noteTxt );	// note, noteTxt
	IRLOG_DEBUG("CIRDialogLauncher::ShowGlobalNoteL - Exiting" );
    }
// ---------------------------------------------------------------------------
// CIRDialogLauncher::ShowQueryDialogDeletionL
// ---------------------------------------------------------------------------
//

void CIRDialogLauncher::ShowQueryDialogDeletionL(const TDesC& aMessage,TInt& aValue)
	{
	IRLOG_DEBUG("CIRDialogLauncher::ShowQueryDialogDeletionL - Entering" );
	CAknQueryDialog* dlg = CAknQueryDialog::NewL();
	aValue = dlg->ExecuteLD(R_QUERY_DELETION,aMessage);	
	IRLOG_DEBUG("CIRDialogLauncher::ShowQueryDialogDeletionL - Exiting" );
	}
// ---------------------------------------------------------------------------
// CIRDialogLauncher::ResolveDefaultErrorTextL
// ---------------------------------------------------------------------------
//

#ifdef __NOTREQUIRED__
HBufC* CIRDialogLauncher::ResolveDefaultErrorTextLC( TInt aErrorCode ) const
    {
    VRDEBUG2( " *** S60VisualRadio -- CIRDialogLauncher::ResolveDefaultErrorTextL: error=%d", 
        aErrorCode );
   
    CTextResolver* textResolver = CTextResolver::NewLC();

#ifdef RD_COMMON_ENGINE_API_CHANGE
    TPtrC ptr = textResolver->ResolveErrorString( aErrorCode, 
                                                  CTextResolver::ECtxNoCtxNoSeparator );
#else
    TPtrC ptr = textResolver->ResolveError( aErrorCode, CTextResolver::ECtxNoCtxNoSeparator );
#endif

    if( !(ptr.Length()) )    // Text resolver returned hidden
        {
        VRDEBUG( " *** S60VisualRadio -- CIRDialogLauncher::ResolveDefaultErrorTextL: TextResolver returned hidden" );
        // -4135 -> -4174
        if ( ( aErrorCode <= KErrGprsServicesNotAllowed ) && 
             ( aErrorCode >= KErrGprsPDPContextWithoutTFTAlreadyActivated ) )
            {
            VRDEBUG3( " *** S60VisualRadio -- Error note: %d - %d", 
                      KErrGprsServicesNotAllowed, 
                      KErrGprsPDPContextWithoutTFTAlreadyActivated );
#ifdef RD_COMMON_ENGINE_API_CHANGE
            ptr.Set( textResolver->ResolveErrorString( KErrGprsMissingorUnknownAPN ) );
#else
            ptr.Set( textResolver->ResolveError( KErrGprsMissingorUnknownAPN ) );
#endif
            }
        // -8000 -> -8268 & -4000 -> -5124
        else if ( ( ( aErrorCode <= KErrPhoneSpecificGsmBase ) && 
                ( aErrorCode >= KErrGsmOfflineOpNotAllowed ) )  ||
                ( ( aErrorCode <= KErrEtelGsmBase ) &&
                ( aErrorCode >= KErrGsm0707UnknownError ) ) )
            {
            VRDEBUG3( " *** S60VisualRadio -- Error note: %d - %d", 
                      KErrPhoneSpecificGsmBase, KErrGsmOfflineOpNotAllowed );
            VRDEBUG3( " *** S60VisualRadio -- Error note: %d - %d", 
                      KErrEtelGsmBase, KErrGsm0707UnknownError );
#ifdef RD_COMMON_ENGINE_API_CHANGE
            ptr.Set( textResolver->ResolveErrorString( KErrNetUnreach ) );
#else
            ptr.Set( textResolver->ResolveError( KErrNetUnreach ) );
#endif
            }
        else
            {
            }
                    
        if( !(ptr.Length()) )    // If still no error text
            {
            VRDEBUG( " *** S60VisualRadio -- Error note, no text found" );
#ifdef RD_COMMON_ENGINE_API_CHANGE
            ptr.Set( textResolver->ResolveErrorString( KErrGeneral ) );
#else
            ptr.Set( textResolver->ResolveError( KErrGeneral ) );
#endif
            }
        }
    // copy to errorText before deleting textResolver
    HBufC* errorText = ptr.AllocL();    
    CleanupStack::PopAndDestroy( textResolver );
    CleanupStack::PushL( errorText );
    
    return errorText;
    }
#endif



// -----------------------------------------------------------------------
// CIRDialogLauncher::ShowQueryDialogL
// -----------------------------------------------------------------------
//

void CIRDialogLauncher::ShowQueryDialogL( TInt aResourceId,TInt& aValue,
									 TInt aDialogId ) const
    {
    IRLOG_DEBUG("CIRDialogLauncher::ShowQueryDialogL " );
    TInt dlgValue;
    ShowQueryDialogL( aResourceId, KNullDesC,dlgValue,aDialogId);
    aValue = dlgValue;
    }
    
  
    
// -----------------------------------------------------------------------
// CIRDialogLauncher::ShowQueryDialogL
// -----------------------------------------------------------------------
//

void CIRDialogLauncher::ShowQueryDialogL( 
        TInt aResourceId, const TDesC& aPrompt, TInt& aValue ,TInt aDialogId ) const
    {
    IRLOG_DEBUG("CIRDialogLauncher::ShowQueryDialogL - Entering" );
    CAknQueryDialog* dlg = CAknQueryDialog::NewL();
    CleanupStack::PushL(dlg);
    dlg->PrepareLC( aResourceId );
    if ( aPrompt.Length() > 0 )
        {
        dlg->SetPromptL( aPrompt );
        }
    if ( aDialogId != KErrNotFound )
        {
        dlg->PublishDialogL( aDialogId );
        }
    aValue = dlg->RunLD();
    CleanupStack::Pop(dlg);
    IRLOG_DEBUG("CIRDialogLauncher::ShowQueryDialogL - Exiting" );
    }
 
    

  

   
