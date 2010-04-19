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
* Description:  Header for CIRCostWarningView
*
*/


#ifndef CIRCOSTWARNINGVIEW_H
#define CIRCOSTWARNINGVIEW_H


#include "irbaseview.h"
#include <aknview.h> 
class CIRCostWarningContainer;


class CIRCostWarningView : public CAknView
    {
    
public: 

    /**
     * Static constructor.
     */
    static CIRCostWarningView* NewLC(const TRect& aRect);
    
    /**
     * Destructor.
     */
    ~CIRCostWarningView();
    
    /**
	 * Two-phased constructor.
	 * Returns a pointer to the Cost Warning view
	 */
     static CIRCostWarningView* NewL(const TRect& aRect);

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


protected:  

    // from base class CAknView
    /**
     * From CAknView
     *
     * @see CAknView::DoActivateL(const TVwsViewId& aPrevViewId, 
     *    TUid aCustomMessageId, 
     *    const TDesC8& aCustomMessage)
     */
    void DoActivateL( const TVwsViewId& /*aPrevViewId*/, 
                      TUid /*aCustomMessageId*/, 
                      const TDesC8& /*aCustomMessage */);

	/**
     * From CAknView
     *
     * @see CAknView::DoDeactivate()
     */
	void DoDeactivate();

	/**
	 * From CAknView
	 *
	 * @see CAknView::HandleStatusPaneSizeChange()
	 */
	void HandleStatusPaneSizeChange();	

private: 

    /**
     * 2nd phase constructor
     */
    void ConstructL(const TRect& /*aRect*/);

	/**
     * C++ default constructor.
     */
    CIRCostWarningView();

private:    // Data

	/** 
	 * Container for this view. 
	 */
	CIRCostWarningContainer* iContainer;
	
	   
    };

#endif      // CIRCostWarningView_H
            

