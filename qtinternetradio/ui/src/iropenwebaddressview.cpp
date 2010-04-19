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
#include <hbapplication.h>
#include <hbmenu.h>
#include <hbtoolbar.h>
#include <hbpushbutton.h>
#include <hblineedit.h>
#include <hbinstance.h>

#include "irabstractviewmanager.h"
#include "irapplication.h"
#include "irplaycontroller.h"
#include "iropenwebaddressview.h"
#include "irqisdsdatastructure.h"
#include "irqfavoritesdb.h"
#include "irqnetworkcontroller.h"
#include "irqutility.h"
#include "irlineeditor.h"
#include "irqenums.h"
#include "iruidefines.h"

// Const strings
const char* OPEN_WEB_ADDRESS_VIEW_OBJECT_NAME = "ex-IRQOpenWebAddressView"; // object name in the XML

IROpenWebAddressView::IROpenWebAddressView(IRApplication* aApplication, TIRViewId aViewId) :
    IRBaseView(aApplication, aViewId),
    iUrl(NULL),
    iName(NULL),
    iDescription(NULL),
    iNameClicked(false),
    iPlayButton(NULL),
    iAdd2FavButton(NULL)
{
    connect( getViewManager(), SIGNAL( orientationChanged(Qt::Orientation) ),
             this, SLOT( handleOrientationChanged(Qt::Orientation) ) );
    
    // Create widget hierarchy
    setObjectName(OPEN_WEB_ADDRESS_VIEW_OBJECT_NAME);

    // List existing root elements - this allows us to refer to objects in the XML 
    // which are created outside the document.
    QObjectList roots;
    roots.append(this); // IROpenWebAddressView is referred in the XML file
    iLoader.setObjectTree(roots);

    // Load the XML file
    iLoader.load(OPEN_WEB_ADDRESS_VIEW_LAYOUT_FILENAME);

    // Find the HbLineEdit objects
    iUrl = qobject_cast<HbLineEdit *> (iLoader.findObject("streamURL"));
    iName = qobject_cast<IrLineEditor *> (iLoader.findObject("stationName"));
    iDescription = qobject_cast<HbLineEdit *> (iLoader.findObject("description"));

    initButtons();

    // This view need not to be stacked.
    setFlag(EViewFlag_UnStackable);

    connect(iName, SIGNAL(pressed()), this, SLOT(nameEditorClicked()));
    connect(iNetworkController, SIGNAL(networkRequestNotified(IRQNetworkEvent)),
    this, SLOT(networkRequestNotified(IRQNetworkEvent)));
    
    connect(iUrl, SIGNAL(textChanged(const QString&)),
    this, SLOT(urlEditorTextChanged(const QString&)));

    handleOrientationChanged(getViewManager()->orientation());
}

IROpenWebAddressView::~IROpenWebAddressView()
{
}

/*
 * Description : from base class IRBaseView.
 *               handle view commands.
 * Parameters  : aCommand : see the definition of TIRViewCommand
 * Return      : EIR_DoDefault : caller does default handling
 *               EIR_NoDefault : caller doesn't do default handling
 */
TIRHandleResult IROpenWebAddressView::handleCommand(TIRViewCommand aCommand,
        TIRViewCommandReason aReason)
{
    Q_UNUSED(aReason);

    switch (aCommand)
    {
    case EIR_ViewCommand_ACTIVATED:
        initDetails();
        return EIR_NoDefault;

    case EIR_ViewCommand_DEACTIVATE:
    default:
        break;
    }

    return EIR_DoDefault;
}

/*
 * Description : initialize the details.
 */
void IROpenWebAddressView::initDetails()
{
    // Set the initial text for line editor.
    iUrl->setText("http://");
    iName->setText(hbTrId("txt_irad_info_unnamed"));
    iDescription->setText("");
    iNameClicked = false;
}

/*
 * Description : initialize the buttons.
 */
void IROpenWebAddressView::initButtons()
{
    // Find the HbAction objects
    iPlayButton = qobject_cast<HbPushButton *> (iLoader.findObject("playButton"));
    iAdd2FavButton = qobject_cast<HbPushButton *> (iLoader.findObject("add2FavButton"));
    
    connect(iPlayButton, SIGNAL(released()), this, SLOT(play()));
    connect(iAdd2FavButton, SIGNAL(released()), this, SLOT(add2Fav()));
}

/*
 * Description : add the station to favorites.
 */
void IROpenWebAddressView::add2Fav()
{
    // Create a IRQPreset using the inputted information.
    IRQPreset preset;

    if (!initPreset(preset))
    {
        return;
    }

    // Add to favorites.
    int retValue = iFavorites->addPreset(preset);

    // Show the information from favorites.
    if (EIRQErrorAlreadyExist == retValue)
    {
        popupNote(hbTrId("txt_irad_info_favorite_updated"), HbMessageBox::MessageTypeInformation);
    }
    else if (EIRQErrorNone == retValue)
    {
        popupNote(hbTrId("txt_irad_info_added_to_favorites"), HbMessageBox::MessageTypeInformation);
    }
    else if (EIRQErrorOutOfMemory == retValue)
    {
        popupNote(hbTrId("txt_irad_info_can_not_add_more"), HbMessageBox::MessageTypeInformation);
    }
    else
    {
        Q_ASSERT(false);
    }
}

/*
 * Description : Play this station.
 */
void IROpenWebAddressView::play()
{
    // Create a IRQPreset using the inputted information
    IRQPreset preset;

    if (!initPreset(preset))
    {
        return;
    }

    setUseNetworkReason(EIR_UseNetwork_OpenWebAddress);

    // Verify the connectivity
    if (false == iApplication->verifyNetworkConnectivity())
    {
        return;
    }

    setUseNetworkReason(EIR_UseNetwork_NoReason);

    // Play this preset
    iPlayController->connectToChannel(&preset,EIRQAdhocManual);
}

/*
 * Description : Handle the network event from network controller.
 */
void IROpenWebAddressView::networkRequestNotified(IRQNetworkEvent aEvent)
{
    if (getViewManager()->currentView() != this)
    {
        return;
    }
    
    switch (aEvent)
    {
    case EIRQNetworkConnectionEstablished:
        iApplication->closeConnectingDialog();

        if (EIR_UseNetwork_OpenWebAddress == getUseNetworkReason())
        {
            play();
        }

        setUseNetworkReason(EIR_UseNetwork_NoReason);
        break;
        
    default:
        break;
    }
}

/*
 * Description : Clear the name editor.
 */
void IROpenWebAddressView::nameEditorClicked()
{    
    if(!iNameClicked)
    {
        iName->setText("");
        iNameClicked = true;
    }     
}

/*
 * Description : initialize a preset.
 * return parameter: true, initialization succeeds; vice versa.
 */
bool IROpenWebAddressView::initPreset(IRQPreset &aPreset)
{
    IRQChannelServerURL server;

    server.url = iUrl->text();
    server.url.remove(" ");

    // check the URL
    if (!IRQUtility::isValidUrl(server.url))
    {
        popupNote(hbTrId("txt_irad_info_invalid_link_please_change_it"), HbMessageBox::MessageTypeInformation);
        return false;
    }

    // Give it an initial value, supposed to be 32 kbps.
    server.bitrate = 32;
    server.serverName = iName->text();
    if (0 == server.serverName.size())
    {
        server.serverName = hbTrId("txt_irad_info_unnamed");
    }

    aPreset.insertChannelServer(server);
    aPreset.name = server.serverName;
    aPreset.description = iDescription->text();
    aPreset.shortDesc = aPreset.description;
    aPreset.type = 0;     
    aPreset.presetId = 0;
    aPreset.uniqID = 0;

    return true;
}


void IROpenWebAddressView::urlEditorTextChanged(const QString &aString)
{
    if(aString.size() < 8)
    {
        iPlayButton->setEnabled(false);
        iAdd2FavButton->setEnabled(false);
    }   
    else
    {
        iPlayButton->setEnabled(true);
        iAdd2FavButton->setEnabled(true);
    }
}

/*
 * Description : resize the container if the direction changes.
 */
void IROpenWebAddressView::handleOrientationChanged(Qt::Orientation aOrientation)
{
    // Load the XML file
    if (aOrientation == Qt::Vertical)
    {
        iLoader.load(OPEN_WEB_ADDRESS_VIEW_LAYOUT_FILENAME, "portrait");
    }
    else
    {
        iLoader.load(OPEN_WEB_ADDRESS_VIEW_LAYOUT_FILENAME, "landscape");
    }
}
