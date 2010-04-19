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
#ifndef IRVIEWMANAGER_H
#define IRVIEWMANAGER_H

#include <QStack>
#include <hbeffect.h>

#include "irabstractviewmanager.h"

class QTimer;

class IRViewManager : public IRAbstractViewManager
{
    Q_OBJECT
public:
    IRViewManager();
    ~IRViewManager();
    
    //from base class IRAbstractViewManager
    IRBaseView* getView(TIRViewId aViewId, bool aCreateIfNotExist);
    
    //from base class IRAbstractViewManager
    bool isViewInStack(TIRViewId aViewId) const;
    
    //from base class IRAbstractViewManager
    void activateView(TIRViewId aViewId, bool aPushCurrentView);
    
    //from base class IRAbstractViewManager
    void activateView(IRBaseView *aView, bool aPushCurrentView);
    
    //from base class IRAbstractViewManager
    void pushViewById(TIRViewId aViewId);
    
    //from base class IRAbstractViewManager
    TIRViewId currentViewId() const;
    
    //from base class IRAbstractViewManager
    TIRHandleResult handleSystemEvent(TIRSystemEventType aEvent);
    
    bool isExiting() const;
       
protected:
    void mousePressEvent(QMouseEvent *aEvent);
    void mouseReleaseEvent(QMouseEvent *aEvent);

    void paintEvent(QPaintEvent *aEvent);
    
private slots:
    void backToPreviousView();
    /*we need to start the logo-down process after the effect for the process
     * may block some painting event in QT enviroment*/
    void showEffectFinished(HbEffect::EffectStatus aStatus);
    void hideEffectFinished(HbEffect::EffectStatus aStatus);
    void currentViewChanged(HbView *aView);
    
    void crossLineReset();
    void exitTimeout();
        
private:
    void backToView(TIRViewId aViewId);
    IRBaseView* createView(IRApplication* aApplication, TIRViewId aViewId);
    void clearStack();
    void updateSoftkey();
    
    bool readyToQuit();
    
    void switchToNextView(IRBaseView *aView);
        
private:
    QStack<IRBaseView*> iViewStack;
    HbAction       *iBackAction;
    HbAction       *iExitAction;
    IRBaseView     *iViewToHide;
    
    bool iCrossLineAReady;
    bool iCrossLineBReady;
    bool iCrossLineEnable;

    bool iCrossLineShowing;

    QLineF iCrossLineA;
    QLineF iCrossLineB;

    QTimer *iCrossLineTimer;
    QTimer *iExitTimer;
    
    bool iExiting;    
};

#endif
