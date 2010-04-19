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
* Description:  Header for CIRAddManuallyStationView
*
*/


#ifndef _CIRADDMANUALLYSTATIONVIEW_H
#define _CIRADDMANUALLYSTATIONVIEW_H

#include <akntoolbarobserver.h>
#include <aknbutton.h>
#include <e32base.h>
#include "irbaseview.h"


class CIRAddManuallyStationContainer;
class CIRAddStationsTimer;

/**
 * Add Manually View, adds radio channels to favorites.
 */
class CIRAddManuallyStationView : public CIRBaseView, public MAknToolbarObserver	
    {
    
public: 

    /**
     * static constructor
     */
     static CIRAddManuallyStationView* NewL(const TRect& aRect);
     
    /**
     * Static constructor.
     */
    static CIRAddManuallyStationView* NewLC(const TRect& aRect);
    
    /**
     * Destructor.
     */
    ~CIRAddManuallyStationView();

    // from base class CAknView
	/**
	 * From CAknView
	 *
	 * @see CAknView::Id() const
	 */
    TUid Id() const;

	/**
	 * From CAknView
	 *
	 * @see CAknView::HandleCommandL( TInt aCommand )
	 */
	void HandleCommandL( TInt aCommand );


	/**
     * For Touch Toolbar
     */
	void DynInitToolbarL(TInt aResourceId, CAknToolbar* aToolbar );
    void OfferToolbarEventL( TInt aCommand );
  	void ConstructToolbarL();
  	void SetToolbarItems();
 	void SetAddStationsTitleL();
 	
    /**
     * From CAknView
     *
     * @see CAknView::HandleForegroundEventL( TBool aForeground )
     */
   	void HandleForegroundEventL( TBool aForeground );
   	
   	void RefreshView();
   	
protected:  

    // from base class CAknView
    /**
     * From CAknView
     *
     * @see CAknView::DoActivateL(const TVwsViewId& aPrevViewId, 
     *    TUid aCustomMessageId, 
     *    const TDesC8& aCustomMessage)
     */
    void DoActivateL( const TVwsViewId& aPrevViewId, 
        TUid aCustomMessageId, 
        const TDesC8& aCustomMessage );

	/**
     * From CAknView
     *
     * @see CAknView::DoDeactivate()
     */
	void DoDeactivate();

private: 

    /**
     * 2nd phase constructor
     */
    void ConstructL(const TRect& aRect);

	/**
     * C++ default constructor.
     */
    CIRAddManuallyStationView();


private:    // Data

	/** 
	 * Container for this view. 
	 */
		
	/** 
	 * Moving item index. If KErrNotFound, no item is moving at the moment. 
	 */
	TInt iMovingFrom;

    /**
     * View Id of the previous view.  
     */
    TVwsViewId iPrevViewId;

    /** 
     * Is the start up of application ongoing. 
     */    
    TBool iStartUp;
    
 	CIRAddStationsTimer* iAddStationsTimer; 
 	
 	  
public:


    CIRAddManuallyStationContainer* iContainer;
    
    /**
 	 * Checks whether any char is entered in URL and 
 	 * Name fields
 	 */
 	TBool iIsTextEntered;
 	
 	/**
 	 * Checks whether any space is entered in URL and 
 	 * Name fields
 	 */
 	TBool iIsSpaceEntered;
 	
    TBool iIsCopied;
    
    };


class CIRAddStationsTimer : public CActive
	{
public:
									//Member Functions
	public:
        static CIRAddStationsTimer* NewL(CIRAddManuallyStationContainer& aContainer);
        
        void ConstructL();
        
        ~CIRAddStationsTimer();     // Destruction
        
        void IssueRequest();  // issue request
       
        void DoCancel();       // Cancel request
        
        // Defined as pure virtual by CActive
        // implementation provided by this class.
         void RunL();     
        
        // Defined as pure virtual by CActive
        // implementation provided by this class.
  		void ExecuteL();

 		void StopTimer();
      
    private:
        
        RTimer iTimer;      // Asynchronous service provider.
        
        TInt iCounter;
        
		CIRAddStationsTimer(CIRAddManuallyStationContainer& aContainer);        // Construction
		
	public:
	
		
		CIRAddManuallyStationContainer& iContainer;
	};  


#endif      // _CIRADDMANUALLYSTATIONVIEW_H
            

