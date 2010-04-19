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
* Description:
*
*/
#include <hbaction.h>
#include <hblistview.h>

#include "irapplication.h"
#include "irabstractviewmanager.h"
#include "irmainview.h"
#include "ircategoryview.h"
#include "irstationsview.h"
#include "irqnetworkcontroller.h"
#include "irmainmodel.h"
#include "irqenums.h"

//                                          public functions

/*
 * Description : constructor
 */
IRMainView::IRMainView(IRApplication* aApplication, TIRViewId aViewId) : 
                                                      IrAbstractListViewBase(aApplication, aViewId),
                                                      iMainModel(NULL)
{
    setFlag(EViewFlag_ClearStackWhenActivate);
    
	connect(iNetworkController, SIGNAL(networkRequestNotified(IRQNetworkEvent)),
	        this, SLOT(networkRequestNotified(IRQNetworkEvent)));
	setHeadingText(tr("Collections"));
	
	iMainModel = new IRMainModel(this);
	iListView->setModel(iMainModel);
}

/*
 * Description : destructor
 */
IRMainView::~IRMainView()
{
}

/*
 * Description : virtual function from base class IRBaseView.
 *               handle view commands.
 * Parameters  : aCommand : see the definition of TIRViewCommand
 * Return      : EIR_DoDefault : caller does default handling
 *               EIR_NoDefault : caller doesn't do default handling
 */
TIRHandleResult IRMainView::handleCommand(TIRViewCommand aCommand, TIRViewCommandReason aReason)
{
    TIRHandleResult ret = IrAbstractListViewBase::handleCommand(aCommand, aReason);
    switch (aCommand)
    {
    case EIR_ViewCommand_ACTIVATED:
        updateView();
        ret = EIR_NoDefault;
        break;
        
    default:
        break;
    }
    
    return ret;
}

//                                       slots functions

/*
 * Description : slot function when an item in a list is clicked. if network is not opened yet,
 *               choose access point firstly. Afterwards, issue a request through isds client
 *               to get category list or popular stations
 * Parameters  : aItem : pointer to the clicked item
 */
void IRMainView::handleItemSelected()
{ 
    QModelIndex index = iListView->currentIndex();
    int currentRow = index.row();
    
    IRCategoryView *categoryView = NULL;
    
	if (ERecentlyPlayedStations == currentRow)
	{
        getViewManager()->activateView(EIRView_HistoryView);
	}
	else if (EBrowseByLanguage == currentRow)
	{
	    categoryView = static_cast<IRCategoryView*>(getViewManager()->getView(EIRView_CategoryView, true));
	    categoryView->loadCategory(IRQIsdsClient::ELanguages);
	}
	else if (EBrowseByCountry == currentRow)
	{
	    categoryView = static_cast<IRCategoryView*>(getViewManager()->getView(EIRView_CategoryView, true));
	    categoryView->loadCategory(IRQIsdsClient::ECountries);
	}
	else if (ESongsPlayed == currentRow)
	{
	    getViewManager()->activateView(EIRView_SongHistoryView);
	}
	else if (EPlayList == currentRow)
	{
	    getViewManager()->activateView(EIRView_PlsView);
	}
}

/*
 * Description : set the checked action in toolbar. For main view, iCollectionsAction should
 *               be checked
 */
void IRMainView::setCheckedAction()
{
    iCollectionsAction->setChecked(true);
}

/*
 * Description : slot function for active network event
 * Parameters  : aEvent, see the definition of IRQNetworkEvent
 */
void IRMainView::networkRequestNotified(IRQNetworkEvent aEvent)
{
    //simulate the item is clicked
    if (getViewManager()->currentView() != this)
    {
        return;
    }
    
    switch (aEvent)
    {
    case EIRQNetworkConnectionEstablished:
        iApplication->closeConnectingDialog();
        
        if (EIR_UseNetwork_SelectItem == getUseNetworkReason())
        {
            handleItemSelected();
        }
        setUseNetworkReason(EIR_UseNetwork_NoReason);
        
        break;
        
    default:
        setCheckedAction();
        break;
    }
}

/*
 * Description : check wether the content we need is already cached
 *  
 */
void IRMainView::itemAboutToBeSelected(bool& aNeedNetwork)
{
    QModelIndex index = iListView->currentIndex();
    int currentRow = index.row();    
    
    if (EBrowseByLanguage == currentRow)
    {
        aNeedNetwork = !iIsdsClient->isdsIsCategoryCached(IRQIsdsClient::ELanguages);
    }
    else if (EBrowseByCountry == currentRow)
    {        
        aNeedNetwork = !iIsdsClient->isdsIsCategoryCached(IRQIsdsClient::ECountries);
    }     
    else if( ESongsPlayed == currentRow )
    {
        aNeedNetwork = false;
    }
    else if( ERecentlyPlayedStations == currentRow)
    {
        aNeedNetwork = false;
    }
    else if (EPlayList == currentRow)
    {
        aNeedNetwork = false;
    }
    else  
    {
        aNeedNetwork = true;         
    }         
}

void IRMainView::updateView()
{
    IrAbstractListViewBase::updateView();
    iMainModel->checkUpdate();
}
