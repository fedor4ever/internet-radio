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
#ifndef IRBASEVIEW_H
#define IRBASEVIEW_H

#include <hbview.h>
#include <hbmessagebox.h>

#include "irdocumentloader.h"
#include "irviewdefinitions.h"
 

class IRApplication;
class IRViewManager;
class IRPlayController;
class IRQIsdsClient;
class IRQNetworkController;
class IRQFavoritesDB;
class IRQSettings;

enum TViewFlag
{
    EViewFlag_None = 0,
    EViewFlag_UnStackable = 0x01,
    EViewFlag_ClearStackWhenActivate = 0x02
};

class IRBaseView : public HbView
{
public:
    TIRViewId id() const;
    //use a enum to instead in future
    virtual TIRHandleResult handleSystemEvent(int aEvent);
    virtual TIRHandleResult handleCommand(TIRViewCommand aCommand, TIRViewCommandReason aReason);
    virtual void launchAction();
    virtual void updateView();
    
    void setFlag(int aFlag);
    int flag() const;
    
    void setUseNetworkReason(TIRUseNetworkReason aReason);
        
    TIRUseNetworkReason getUseNetworkReason() const;
        
    virtual void lazyInit();
    
protected:
    IRBaseView(IRApplication *aApplication, TIRViewId aViewId);

    IRViewManager* getViewManager() const;

    void popupNote(const QString &aNote, const HbMessageBox::MessageBoxType &aNoteType) const;
    
    void setInitCompleted(bool aFlag);
    
    bool initCompleted() const;
    
protected:
    IRApplication* iApplication;
    
    //reference, owned by IRApplication
    IRPlayController *iPlayController;
    IRQIsdsClient    *iIsdsClient;
    IRQNetworkController *iNetworkController;
    IRQFavoritesDB       *iFavorites;
    IRQSettings          *iSettings;
    IRDocumentLoader iLoader;
    
private:
    TIRViewId      iViewId;
    int            iFlag;
    TIRUseNetworkReason iUseNetworkReason;
    bool iInitCompleted;
};

#endif
