/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Header for CIRAddManuallyContainer
*
*/


#ifndef IRADDMANUALLYSTATIONCONTAINER_H
#define IRADDMANUALLYSTATIONCONTAINER_H

#include <aknlists.h>
#include <AknsBasicBackgroundControlContext.h> 
#include <aknsdrawutils.h>
#include <aknscontrolcontext.h>
#include <AknsSkinInstance.h>
#include <aknsutils.h>
#include <aknform.h>
#include <eikedwin.h>
#include <eikdialg.h> 
class CIRBaseView;
class CIRDialogLauncher;
//class CEikEdwin;
#include <akntoolbarobserver.h>
#include <aknbutton.h>
#include <baclipb.h>
#include <f32file.h>
class CEikEdwin;
class CGulIcon;

class CIRAddManuallyStationView;

class CIRDialogLauncher;
class CAknNavigationControlContainer;
class CAknNavigationDecorator;

class Stateind_uid : public TUid
{
  public:
   	Stateind_uid(int stateind)
  	{ 
  		iUid = stateind;
  	}
};



/**
 * Container class for Add Manually View.
 */
class CIRAddManuallyStationContainer : public CAknForm,public MCoeControlObserver
    {
    
public:  // Methods
  
    /**
	 * CIRAddManuallyStationContainer::NewL(const TRect& aRect)
	 * Two phased constructor
	 * Creates a CIRAddManuallyStationViewContainer object which
	 * will draw itself to aRect
	 * @param aRect. The rectangle to which this view will be drawn to
	 * @return. A pointer to the created instance of 
	 * CIRAddManuallyStationViewContainer.
	 */
     static CIRAddManuallyStationContainer* NewL(CIRAddManuallyStationView* aView); //(CAknToolbar* aToolbar)
       
    /**
     * Destructor.
     */
    ~CIRAddManuallyStationContainer();

	/**
     * C++ default constructor.
     *
	 * @param aView View of this container.
     */
    CIRAddManuallyStationContainer( CIRAddManuallyStationView* aView );
    

	/**
	 * From CCoeControl
	 *
	 * @see CCoeControl::GetHelpContext( TCoeHelpContext& aContext ) const
	 */
	IMPORT_C void GetHelpContext( TCoeHelpContext& aContext ) const;

  
    
 public:    // New functions
    	/**
        * To Handle the key events
        */
		void DefaultVauleL();
		
		/**
        * copies to clipboard
        */
		void CopyDataL();
		
		/**
        * paste from clipboard
        */
		void PasteDataL();
		
		/**
        * Saves the URL name nd description of the station to be saved
        */
		void SaveDataL();
		
		/**
		 * Edits the URL name and description of the station
		 */
		void EditStationL();
		
		/**	
     	* Checks whether Name Field is empty or not. If it contains empty then 
    	* replace with the default name ("Unnamed")
    	*
   		* @return NA
        */
		void ReplaceEmptyName( TDes& aUrlName );
		
		
		/**
		 * Replaces the ugly char symbols with space
		 */
		void ReplaceUglyChar(TDes& aUrlName);
		
	
        /**	
    	 * Checks if there is an URI that contains scheme (="http://")
     	 * and at some characters after that.
     	 *
     	 * @return ETrue if Name text field contains a scheme and at least one character 
     	 *         Otherwise EFalse.
     	 */
		TBool ValidateUrlL();
		
		/**
		 * Checks if there are any invalid characters in URL
		 * @return ETrue if URL contains invalid characters
		 */
		TBool HasInvalidCharsInUrl(TDes& aUrl);
		
    public: // Functions from base classes

       /**
        * To Handle the key events
        */
 		void HandlePointerEventL  (const TPointerEvent &  aPointerEvent) ;
 		void HandleControlEventL(CCoeControl* aControl, TCoeEvent aEventType);
    	void HandleResourceChange (TInt aType);
   
 		/**
        * From CAknForm, called just before options menu is displayed.
        * @param aResourceId Resource ID identifying the menu pane to initialize
        * @param aMenuPane The in-memory representation of the menu pane
        */
      	void DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane );
        
        /**
        * From CAknForm, takes care of command handling.
        * @param aCommand Command to be handled
        */        
        void ProcessCommandL( TInt aCommand );
     	void SizeChanged();
		 	
    protected:

        /**
        * From CAknForm, called by the framework if user presses a button
        * @param aButtonId The ID of the pressed button.
        * @return Should the dialog exit (EFalse or ETrue)
        */
        TBool OkToExitL( TInt aButtonId );

        /**
        * From CAknForm, called by the framework just before form is shown
        */
        void PostLayoutDynInitL();
		
    private:

       /**
        * To save station information to favourites
        */
     	void AddToFavouritesL();
     	
     	
     	void PreLayoutDynInitL();
     	     	
        
    private:    // Data
    	// True if it is edit station.
    	TBool iIsEditStation;
		TBool iIsSpaceEntered;
		TBool iIsTextEntered;
    	TBuf<10> iTextBoxValidateUrl;
    	TBool iUnNamedFlag;
    	
    	CAknNavigationControlContainer* iNaviPane;
        CAknNavigationDecorator* iNaviDecorator; 
    public:
    	CIRDialogLauncher*  iDialogLauncher;
    	CIRIsdsPreset* iPreset;
        
        TBuf<256> 			iSelText;
        TBuf<256> 			iStationURL;
        TBuf<256> 			iStationName;
        TBuf<256> 			iStationDescription;
        TBool				iIsEnable;
                          

public:

   
    TUid iAddManPrevID;
    CIRAddManuallyStationView* iView;
    
    
    //Previous ViewId
    TUid iAddManPrevId;
    
  	
    };

#endif      // IRADDMANUALLYSTATIONCONTAINER_H
