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
* Description:  Header for CIRStationInfoView
*
*/


/* ---------------------------------------------------------------------------
*  Version history:
*  Template version:
*  <ccm_history>
*
*  Version: 2, Thu Jul 3 16:30:00 2008 by Rohit
*  Ref:
*  Codescanner fixes
*
*  Version: 1, Wed Apr 30 16:15:00 2008 by Rohit
*  Ref:
*  Created. Implemented StationInfo potrait view
*
*  </ccm_history>
* ============================================================================
*/

#ifndef CIRSTATIONINFOVIEW_H
#define CIRSTATIONINFOVIEW_H

#include <AknProgressDialog.h>
#include <aknview.h>

#include "irbaseview.h"
#include "MLogoDownloadObserver.h"

class CIRStationInfoContainer;
class CIRIsdsPreset;
class CIRPreset;
class CAknWaitDialog;

/**
 * Channel List View, displays a list of preset FM / visual radio channels.
 */
class CIRStationInfoView : public CIRBaseView,
						   public MLogoDownloadObserver
    {

public:

    /**
     * Static constructor.
     */
    static CIRStationInfoView* NewLC(const TRect& aRect);

    /**
     * Destructor.
     */
    ~CIRStationInfoView();

    static CIRStationInfoView* NewL(const TRect& aRect);

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
	 * From MLogoDownloadObserver
	 *
	 * Invoked after requested logo is downloaded
	 */
	void PresetLogoDownloadedL(CIRIsdsPreset* aPreset);

	/**
	 * From MLogoDownloadObserver
	 *
	 * Invoked if an error has occured while logo is downloading or no logo data available
	 */
	void PresetLogoDownloadError(CIRIsdsPreset* aPreset);

	/**
	* SetStationPresetL
	* Copies the given preset to member iStationPreset for display
	* @param aPreset Station Preset given by invoking view
	*/
	void SetStationPresetL( CIRIsdsPreset* aPreset );

	/**
	* SetStationPresetL
	* Copies the given preset to member iStationPreset for display
	* @param aPreset Station Preset given by invoking view
	*/
	void SetStationPresetL( CIRPreset* aPreset );

	/**
	* GetStationPresetL
	* 
	* 
	*/
	CIRIsdsPreset* GetStationPresetL( );

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
    CIRStationInfoView();

private:    // Data

	/**
	 * Container for this view.
	 */
	CIRStationInfoContainer* iContainer;

public:
	/**
	 * Station Preset to display Station Info
	 */
	CIRIsdsPreset* iStationPreset;

    };

#endif      // CIRStationInfoView_H


