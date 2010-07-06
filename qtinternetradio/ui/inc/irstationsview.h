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
#ifndef IRSTATIONSVIEW_H
#define IRSTATIONSVIEW_H

 
#include "irabstractlistviewbase.h"
#include "irqevent.h"
 
class IRQPreset;
class IrChannelModel;
class QTimer;

class IRStationsView : public IrAbstractListViewBase
{
    Q_OBJECT
    
public:
    ~IRStationsView();
    
    void loadCategoryStations(int aIndex, const QString &aHeadingText);
    
protected:
    IRStationsView(IRApplication* aApplication, TIRViewId aViewId);
    
    //from base class IRBaseView
    TIRHandleResult handleCommand(TIRViewCommand aCommand, TIRViewCommandReason aReason);
#ifdef HS_WIDGET_ENABLED	
    //from IrAbstractListViewBase
    void itemAboutToBeSelected(bool &aNeedNetwork);
#endif
    
private slots:  
    void presetResponse(IRQPreset *aPreset);
    
    void presetLogoDownload(IRQPreset* aPreset);
    
    void presetLogoDownloadError();
    
    void networkRequestNotified(IRQNetworkEvent aEvent);
    
    void dataChanged();
    
    //used for connect signal from isds client
    void operationException(IRQError aError);
    
    //to start the convertion 
    void convertAnother();
    
    void connectTimeOut(); 
  
    void cancelRequest();
    
private:
    void resetCurrentItem();
    void storeCurrentItem();
    void handleItemSelected();
    
    void startConvert(int aIndex); 
    
    void cleanupResource();   
    
    void connectToIsdsClient();
    
    void disconnectIsdsClient();
    
private:
    IRQPreset *iLogoPreset;
    IRQPreset *iPreset; //the object is created by IsdsClient, but application is responsible for free
    int iLastSelectitem;
    
    //the following are used to support the img
    QList<int> iIconIndexArray;
    IrChannelModel *iChannelModel;
    QTimer *iConnectTimer;
    
    friend class IRViewManager;
};

#endif //IRSTATIONSVIEW_H
