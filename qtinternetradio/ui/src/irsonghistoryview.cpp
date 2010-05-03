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
#include <hblistview.h>
#include <hbmenu.h>
#include <hbaction.h>
#include <hbicon.h>
#include <QTimer> 

#include "irabstractviewmanager.h"
#include "irapplication.h" 
#include "irplaycontroller.h"
#include "irsonghistoryview.h" 
#include "irsonghistorymodel.h"
#include "irqsonghistoryinfo.h" 
#include "irqenums.h"
#include "irqmusicshop.h"
#include "irqstatisticsreporter.h"
#include "irqsettings.h"

 

//                                         public functions

/*
 * Description : constructor
 */
IRSongHistoryView::IRSongHistoryView(IRApplication *aApplication, TIRViewId aViewId) :
    IrAbstractListViewBase(aApplication, aViewId), iClearSongHistoryAction(NULL),
    iShowPrompt(false)
    
{         
    iModel = new IRSongHistoryModel(this);
    iListView->setModel(iModel);
    iListView->setCurrentIndex(iModel->index(0));
    
    iClearSongHistoryAction = new HbAction(hbTrId("txt_irad_opt_clear_song_history"), this);
    iMusicShop = iApplication->getMusicShop();
    iStatisticsReporter = iApplication->getStatisticsReporter();
    
    iStationHistoryAction = new HbAction(hbTrId("txt_irad_opt_recently_played_stations"), this);	 
    menu()->insertAction(iOpenWebAddressAction, iStationHistoryAction);
    connect(iStationHistoryAction, SIGNAL(triggered()), this, SLOT(gotoStationHistory()));
    
    iShowPrompt = iSettings->getSongHistoryShow();    
    if( iShowPrompt )
    {
        iSettings->setSongHistoryShow(0);
    }  
    
    connect(iClearSongHistoryAction, SIGNAL(triggered()), this, SLOT(clearHisotrySongDB()));    
    connect(iModel, SIGNAL(modelChanged()), this, SLOT(modelChanged()));        
}

/* 
 * Description : destructor
 */
IRSongHistoryView::~IRSongHistoryView()
{
 
}

/*
 * Description : virtual functions from base class IRBaseView. 
 *               handle view commands
 * 
 * see also    : IRBaseView::handleCommand
 */
TIRHandleResult IRSongHistoryView::handleCommand(TIRViewCommand aCommand,
        TIRViewCommandReason aReason)
{     
    Q_UNUSED(aReason);
    TIRHandleResult ret = IrAbstractListViewBase::handleCommand(aCommand, aReason);
     

    switch (aCommand)
    {
    case EIR_ViewCommand_ACTIVATED:        
        connect(iPlayController, SIGNAL(metaDataAvailable(IRQMetaData*)), this, SLOT(newMetadataAdded(IRQMetaData*)));
        showSongHistory();
        
        if( iShowPrompt )
        {             
            QTimer::singleShot(5, this, SLOT(showPrompt()));
            iShowPrompt = false;
        }         
        ret = EIR_NoDefault;       
        break;

    case EIR_ViewCommand_DEACTIVATE:        
        disconnect(iPlayController, SIGNAL(metaDataAvailable(IRQMetaData*)), this, SLOT(newMetadataAdded(IRQMetaData*)));         
        ret = EIR_NoDefault;        
        break;

    default:
        break;
    }

    return ret;
}

//                                 slots functions 

/*
 * Description : slot function when an item in a list is clicked.
 *               issue a listen request to isds client
 * Parameters  : aItem : pointer to the clicked item.
 */
void IRSongHistoryView::handleItemSelected()
{     
    int index = iListView->currentIndex().row();
    IRQSongInfo *hisInfo = iModel->getSongHistoryInfo(index);
     

    if(NULL == iMusicShop)
    {
        popupNote(hbTrId("txt_irad_info_music_shop_not_available"), HbMessageBox::MessageTypeInformation);
        return;
    }   
    
    if( (NULL == hisInfo) ||    
        ( hisInfo->getSongName().isEmpty() &&  
          hisInfo->getArtistName().isEmpty()
        )
      )
    {
        popupNote(hbTrId("txt_irad_info_no_song_info"), HbMessageBox::MessageTypeInformation);
        return;
    }


    if( iMusicShop->findInMusicShop(hisInfo->getSongName(), hisInfo->getArtistName()))
    {
        if(iStatisticsReporter)
        {
            //we will add the report in future. Add the channel id in the song info db
            //iStatisticsReporter->logNmsEvents(EIRQFind,channelId);
        }
    }
    else
    {
        popupNote(hbTrId("txt_irad_info_music_shop_not_available"), HbMessageBox::MessageTypeInformation);
    }     
}
   

// ---------------------------------------------------------------------------
// IRSongHistoryView::showSongHistory()
// gets the List which was stored earlier
//---------------------------------------------------------------------------
void IRSongHistoryView::showSongHistory()
{
    if (iModel->checkSongHistoryUpdate())
    {
        iListView->reset();
        iListView->setCurrentIndex(iModel->index(0));        
    }
}

// ---------------------------------------------------------------------------
// IRSongHistoryView::clearAllList()
// gets the List which was stored earlier
//---------------------------------------------------------------------------
void IRSongHistoryView::clearAllList()
{        
    iModel->clearAllList();
    iListView->reset();
    updateView();     
}

void IRSongHistoryView::prepareMenu()
{
    HbMenu *viewMenu = menu();

    viewMenu->removeAction(iClearSongHistoryAction);
    if (iModel->rowCount() > 0)
    {
        viewMenu->insertAction(iOpenWebAddressAction, iClearSongHistoryAction);
    }
}
    
void IRSongHistoryView::modelChanged()
{    
    iListView->reset();
    QString headingStr = hbTrId("txt_irad_list_recently_played_songs") + " (" + QString::number(iModel->rowCount()) + ")";    
    setHeadingText(headingStr);
 
    //the case is that, we active the song history view with no items
    //but immediately, we get the metadata and show it on the list,
    //then we need to remove the "no content" label
    if( 1 == iListView->model()->rowCount())
    {
        updateView();
    }     
}

void IRSongHistoryView::newMetadataAdded(IRQMetaData *aMetadata)
{
    Q_UNUSED(aMetadata);     
    iModel->checkSongHistoryUpdate();
     
}

void IRSongHistoryView::clearHisotrySongDB()
{
    iModel->clearHisotrySongDB(); 
    updateView();    
}

void IRSongHistoryView::showPrompt()
{
    QString str = hbTrId("txt_irad_info_click_the_song_and_find_it_in_nokia_music_store");
    popupNote(str, HbMessageBox::MessageTypeInformation);
}

void IRSongHistoryView::itemAboutToBeSelected(bool& needNetwork)
{
    /* for in song history view, the data will retrived from the web browser*/
    needNetwork = false;
}
 
void IRSongHistoryView::gotoStationHistory()
{
	  getViewManager()->activateView(EIRView_HistoryView);
}
