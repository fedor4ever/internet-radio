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
#ifndef IRHISTORYVIEW_H
#define IRHISTORYVIEW_H

#include "irabstractlistviewbase.h" 
#include "irqevent.h"

class IRQPreset;
class IRHistoryModel;
class IRQSongHistoryInfo; 

class IRHistoryView : public IrAbstractListViewBase
{
    Q_OBJECT

public:
    ~IRHistoryView();
    
protected:
    IRHistoryView(IRApplication *aApplication, TIRViewId aViewId);
    
    //from base class IRBaseView
    TIRHandleResult handleCommand(TIRViewCommand aCommand, TIRViewCommandReason aReason);
   
private slots:
    void clearAllList(HbAction *aAction);
    void networkRequestNotified(IRQNetworkEvent aEvent);
    //to start the convertion asynchronously
    void convertAnother();
    void presetLogoDownload(IRQPreset* aPreset);
    void presetLogoDownloadError();
    void modelChanged();
    
    void actionClicked(HbAction *aAction);
    void popupClearHistoryConfirmMessageBox();    
private:
    void showHistory();
    void handleItemSelected();
    void listViewLongPressed(HbAbstractViewItem *aItem, const QPointF& aCoords);
    
    //from IrAbstractListViewBase
    void prepareMenu();
#ifdef HS_WIDGET_ENABLED	
    void itemAboutToBeSelected(bool &aNeedNetwork);
#endif	
    void startConvert(int aIndex); 
    void convertStationHistory2Preset(const IRQSongHistoryInfo& aHistoryInfo, IRQPreset& aPreset);
    void addContextAction();
    void deleteContextAction();
    void detailsContextAction();

private:  
    IRHistoryModel *iModel;
    HbAction       *iClearHistoryAction;
    HbAction       *iSongHistoryAction;
    

    //the following are used to support the img
    QList<int> iIconIndexArray;
    IRQPreset *iLogoPreset;
     
    
    friend class IRViewManager;
};

#endif //IRHISTORYVIEW_H
