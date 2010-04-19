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
#ifndef IRABSTRACTVIEWMANAGER_H
#define IRABSTRACTVIEWMANAGER_H

#include <hbmainwindow.h>

#include "irviewdefinitions.h"
#include "irsystemeventobserver.h"

class IRApplication;
class IRBaseView;

class IRAbstractViewManager : public HbMainWindow
{
public:
    IRAbstractViewManager();
    void setApplication(IRApplication *aApplication);
    
public:
    virtual IRBaseView* getView(TIRViewId aViewId, bool aCreateIfNotExist = false) = 0;
    
    virtual bool isViewInStack(TIRViewId aViewId) const = 0;
    
    virtual void activateView(TIRViewId aViewId, bool aPushCurrentView = true) = 0;
    
    virtual void activateView(IRBaseView *aView, bool aPushCurrentView = true) = 0;  
    
    virtual TIRViewId currentViewId() const = 0;
    
    virtual void pushViewById(TIRViewId aViewId) = 0;
    
    //Peter : use of TIRSystemEventType causes dependence of InternetRadio/uiinc/irsystemeventobserver.h
    virtual TIRHandleResult handleSystemEvent(TIRSystemEventType aEvent) = 0;
    
    virtual void backToPreviousView() = 0;
    
protected:
    IRApplication* iApplication;
};

#endif
