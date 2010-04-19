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
* Description:  Header file for a class showing dialogs and notes.
*
*/


#ifndef CIRDIALOGLAUNCHER_H
#define CIRDIALOGLAUNCHER_H

#include <e32base.h>
#include <aknnotifystd.h>

class CAknGlobalNote;
class CAknInformationNote;

/**
 * Defines dialog launcher. 
 *
 * Helper class to show dialogs, notes etc.
 */
class CIRDialogLauncher : public CBase
    {

public:

    /**
     * Static constructor.     
     */
    static CIRDialogLauncher* NewL();

    /**
     * Destructor
     */
    ~CIRDialogLauncher();
            
	/**
     * Displays a confirmation note
     *
     * @param aText Text to show.
     * @param aWaiting If true the function blocks until the note is dismissed.
     */    
    void ShowConfirmationNoteL( const TDesC& aText, TBool aWaiting ) const;

    /**
     * Displays a confirmation note
     *
     * @param aTextResourceId Resource ID for the text string.
     * @param aWaiting If true the function blocks until the note is dismissed.
     */    
    void ShowConfirmationNoteL( TInt aTextResourceId, TBool aWaiting ) const;

    /**
     * Displays an information note
     *
     * @param aTextResourceId Resource ID for the text string.
     * @param aWaiting If true the function blocks until the note is dismissed.
     * @param aDialogId The dialog ID for publishing its usage (given only if publishing is wanted).     
     */    
    void ShowInformationNoteL( TInt aTextResourceId, TBool aWaiting,
    		 TInt aDialogId = KErrNotFound ) const;

    /**
     * Displays an information note
     *
     * @param   aErrorCode  The error code whose related text is being resolved.
     */    
    void ShowInformationNoteL( TInt aErrorCode ) const;

    /**
     * Displays an error note
     *
     * @param aTextResourceId Resource ID for the text string.
     * @param aWaiting If true the function blocks until the note is dismissed.
     */    
    void ShowErrorNoteL( TInt aTextResourceId, TBool aWaiting ) const;

    /**
     * Displays a global note
     *
     * @param aType Note type.
     * @param aTextResourceId Resource ID for the text string.
     */    
    void ShowGlobalNoteL( TAknGlobalNoteType aType, TInt aTextResourceId ) const;

	
	    
	/**
     * Launches a query dialog.
     * @param aResourceId The resource ID of the dialog to load.
     * @param aDialogId The dialog ID for publishing its usage (given only if publishing is wanted).
     * @return ID of the button that closed the dialog, or zero if it was the cancel button.
     */
    void ShowQueryDialogL( TInt aResourceId, TInt& aValue,TInt aDialogId = KErrNotFound) const;

	/**
     * Launches a query dialog.
     * @param aResourceId The resource ID of the dialog to load.
     * @param aPrompt Text for prompt.
     * @param aDialogId The dialog ID for publishing its usage (given only if publishing is wanted).
     * @return ID of the button that closed the dialog, or zero if it was the cancel button.
     */
    void ShowQueryDialogL(TInt aResourceId,const TDesC& aPrompt, TInt& aValue,
    			TInt aDialogId = KErrNotFound) const;

    void ShowQueryDialogDeletionL(const TDesC& aMessage,TInt& aValue);

   

protected:

private:

    /**
     * Constructor.
     */
    CIRDialogLauncher();

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL();

	
    /**
     * Resolves the platform provided error text related to system errorcodes.
     *
     * @param   aErrorCode  The error code whose related text is being resolved.
     * @return  The error text provided by platform.
     */
    HBufC* ResolveDefaultErrorTextLC( TInt aErrorCode ) const;

private: // Data

		
    };

#endif