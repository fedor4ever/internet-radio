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
#ifndef IROPENWEBADDRESSVIEW_H_
#define IROPENWEBADDRESSVIEW_H_

#include "irbaseview.h"
#include "irqevent.h"

class IRQPreset;
class HbLineEdit;
class IrLineEditor;
class HbPushButton;

/**
 * This class is for users to input a station manually.
 */

class IROpenWebAddressView : public IRBaseView
{
Q_OBJECT

public:
    ~IROpenWebAddressView();

    // from base class IRBaseView
    TIRHandleResult handleCommand(TIRViewCommand aCommand, TIRViewCommandReason aReason);

protected:
    IROpenWebAddressView(IRApplication* aApplication, TIRViewId aViewId);
   
private:
    void initDetails();
    void initButtons();
    bool initPreset(IRQPreset &aPreset);

private slots:
    // add the station to favorites
    void add2Fav();

    // play this station
    void play();

    // get network controller event.
    void networkRequestNotified(IRQNetworkEvent aEvent);
    
    // used to check the url editor status
    void urlEditorTextChanged(const QString &aString);

    // clear name editor
    void nameEditorClicked();
    
    void handleOrientationChanged(Qt::Orientation aOrientation);

private:
    // station information
    HbLineEdit*   iUrl;
    IrLineEditor* iName;
    HbLineEdit*   iDescription;
    bool          iNameClicked;
    
    HbPushButton* iPlayButton;
    HbPushButton* iAdd2FavButton;
    friend class IRViewManager;
};

#endif /* IROPENWEBADDRESSVIEW_H_ */
