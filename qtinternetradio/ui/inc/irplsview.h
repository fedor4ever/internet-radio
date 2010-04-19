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

#ifndef IRPLSVIEW_H
#define IRPLSVIEW_H

#include "irabstractlistviewbase.h"
#include "irqevent.h"

class IRPlsModel;

class IRPlsView : public IrAbstractListViewBase
{
    Q_OBJECT
public:
    ~IRPlsView();
    
protected:
    //only friend class IRViewManager can create this view
    IRPlsView(IRApplication *aApplication, TIRViewId aViewId);
    
private slots:
    void networkRequestNotified(IRQNetworkEvent aEvent);
    
private:
    //from base class IrAbstractListViewBase
    void handleItemSelected();    
    
    //from base class IRBaseView
    void updateView();
    
private:
    IRPlsModel *iPlsModel;
    
    friend class IRViewManager;
};

#endif
