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

#include <hbdataform.h>
#include <hbdataformmodelitem.h>
#include <hbdataformviewitem.h>
#include <hbdataformmodel.h>
#include <hbpushbutton.h>
#include <hblineedit.h>
#include <hbstyleloader.h>
#include <hbaction.h>

#include "irviewmanager.h"
#include "irapplication.h"
#include "irplaycontroller.h"
#include "iropenwebaddressview.h"
#include "irqisdsdatastructure.h"
#include "irqnetworkcontroller.h"
#include "irqutility.h"
#include "irqenums.h"
#include "iruidefines.h"
#include "irqsettings.h"

IROpenWebAddressView::IROpenWebAddressView(IRApplication* aApplication, TIRViewId aViewId) :
    IRBaseView(aApplication, aViewId),
    iForm(NULL),
    iModel(NULL),
    iUrl(NULL),
    iName(NULL),
    iNameEditorPtr(NULL),
    iPlayButton(NULL)
{
    //this view won't be starting view, don't need lazy init
    IRBaseView::lazyInit();
    setInitCompleted(true);
    
    // TODO: te be deleted if there's no difference between the landscape and portrait.
//    connect( getViewManager(), SIGNAL( orientationChanged(Qt::Orientation) ),
//             this, SLOT( handleOrientationChanged(Qt::Orientation) ) );
    
    // Create widget hierarchy
    setObjectName(OPEN_WEB_ADDRESS_VIEW_OBJECT_VIEW);

    // List existing root elements - this allows us to refer to objects in the XML 
    // which are created outside the document.
    QObjectList roots;
    roots.append(this); // IROpenWebAddressView is referred in the XML file
    iLoader.setObjectTree(roots);

    // Load the XML file
    iLoader.load(OPEN_WEB_ADDRESS_VIEW_LAYOUT_FILENAME);

    // Find the HbDataForm objects
    iForm = qobject_cast<HbDataForm *> (iLoader.findObject(OPEN_WEB_ADDRESS_VIEW_OBJECT_DATA_FORM));
    iModel = new HbDataFormModel();
    initDataForm();
    initDetails();
    iForm->setModel(iModel);
    initMenu();

    // get a new instance different from the base class. Because it's used in destructor and 
    // the viewmanager is destructed after application, the base class iSettings is NULL before
    // destruct this class.
    iSettings = IRQSettings::openInstance();

    // Install event filter
    QModelIndex index = iModel->indexFromItem(iName);
    HbAbstractViewItem* viewItem = iForm->itemByIndex(index);
    HbDataFormViewItem* dataformviewitem = static_cast<HbDataFormViewItem *>(viewItem);
    HbWidget *widget = dataformviewitem->dataItemContentWidget();
    iNameEditorPtr = static_cast<HbLineEdit *>(widget);
    iNameEditorPtr->installEventFilter(this);

    // Find the play button objects
    iPlayButton = qobject_cast<HbPushButton *>(iLoader.findObject(OPEN_WEB_ADDRESS_VIEW_OBJECT_PLAY_BUTTON));
    connect(iPlayButton, SIGNAL(released()), this, SLOT(play()));
    HbStyleLoader::registerFilePath(OPEN_WEB_ADDRESS_VIEW_PUSH_BUTTON_CSS);
    HbStyleLoader::registerFilePath(OPEN_WEB_ADDRESS_VIEW_PUSH_BUTTON_WIDGETML);

    connect(iNetworkController, SIGNAL(networkRequestNotified(IRQNetworkEvent)),
    this, SLOT(networkRequestNotified(IRQNetworkEvent)));

    // TODO: te be deleted if there's no difference between the landscape and portrait.
//    handleOrientationChanged(getViewManager()->orientation());
}

IROpenWebAddressView::~IROpenWebAddressView()
{
    if (iSettings)
    {
        iSettings->setManuallyInputtedStationUrl(iUrl->contentWidgetData(QString("text")).toString());
        iSettings->setManuallyInputtedStationName(iName->contentWidgetData(QString("text")).toString());    
        iSettings->closeInstance();
    }
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
        initUrlAndName();
    case EIR_ViewCommand_DEACTIVATE:
    default:
        break;
    }

    return EIR_DoDefault;
}

/*
 * Description : create menu
 */
void IROpenWebAddressView::initMenu()
{
    HbAction *settings = qobject_cast<HbAction *> (iLoader.findObject(SETTINGS_ACTION));
    HbAction *exitAction = qobject_cast<HbAction *> (iLoader.findObject(EXIT_ACTION));

    connect(settings, SIGNAL(triggered()), this, SLOT(handleSettingAction()));
    connect(exitAction, SIGNAL(triggered()), iApplication, SIGNAL(quit()));
}


/*
 * Description : initialize the details in constructor.
 */
void IROpenWebAddressView::initDetails()
{
    // Set the initial text for line editor.
    QString stationUrl, stationName;
    iSettings->getManuallyInputtedStationUrl(stationUrl);
    iSettings->getManuallyInputtedStationName(stationName);

    if (0 == stationUrl.size())
    {
        iUrl->setContentWidgetData(QString("text"), QString("http://"));
    }
    else
    {
        iUrl->setContentWidgetData(QString("text"), stationUrl);
    }

    if (0 == stationName.size())
    {
        iName->setContentWidgetData(QString("text"), hbTrId("txt_irad_info_unnamed"));
    }
    else
    {
        iName->setContentWidgetData(QString("text"), stationName);
    }
}

/*
 * Description : initialize the details when activated.
 */
void IROpenWebAddressView::initUrlAndName()
{
    QString temp = iUrl->contentWidgetData(QString("text")).toString();
    if (0 == temp.size())
    {
        iUrl->setContentWidgetData(QString("text"), QString("http://"));
    }

    temp = iName->contentWidgetData(QString("text")).toString();
    if (0 == temp.size())
    {
        iName->setContentWidgetData(QString("text"), hbTrId("txt_irad_info_unnamed"));
    }
}

/*
 * Description : initialize the details.
 */
void IROpenWebAddressView::initDataForm()
{
    iUrl = new HbDataFormModelItem(
            HbDataFormModelItem::TextItem, hbTrId("txt_irad_formlabel_station_url"));
    iModel->appendDataFormItem(iUrl);

    iName = new HbDataFormModelItem(
            HbDataFormModelItem::TextItem, hbTrId("txt_irad_formlabel_station_name"));
    iModel->appendDataFormItem(iName);

    iForm->addConnection(iUrl, SIGNAL(textChanged(const QString&)),
    this, SLOT(urlEditorTextChanged(const QString&)));
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
        if (EIR_UseNetwork_OpenWebAddress == getUseNetworkReason())
        {
            play();
        }
        break;
        
    default:
        break;
    }
    
    setUseNetworkReason(EIR_UseNetwork_NoReason);
}

/*
 * Description : initialize a preset.
 * return parameter: true, initialization succeeds; vice versa.
 */
bool IROpenWebAddressView::initPreset(IRQPreset &aPreset)
{
    IRQChannelServerURL server;

    server.url = iUrl->contentWidgetData(QString("text")).toString();
    server.url.remove(" ");

    // check the URL
    if (!IRQUtility::isValidUrl(server.url))
    {
        popupNote(hbTrId("txt_irad_info_invalid_link_please_change_it"), HbMessageBox::MessageTypeInformation);
        return false;
    }

    // Give it an initial value, supposed to be 32 kbps.
    server.bitrate = 32;
    server.serverName = iName->contentWidgetData(QString("text")).toString();
    if (0 == server.serverName.size())
    {
        server.serverName = hbTrId("txt_irad_info_unnamed");
    }

    aPreset.insertChannelServer(server);
    aPreset.name = server.serverName;
    aPreset.description = hbTrId("txt_irad_info_added_station_description");
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
    }   
    else
    {
        iPlayButton->setEnabled(true);
    }
}

/*
 * Description : resize the container if the direction changes.
 * TODO: te be deleted if there's no difference between the landscape and portrait.
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

void IROpenWebAddressView::handleSettingAction()
{
    getViewManager()->activateView(EIRView_SettingsView);
}

bool IROpenWebAddressView::eventFilter(QObject *object, QEvent *event)
{
    if( object == iNameEditorPtr
        && event->type() == QEvent::FocusIn )
    {
        if(hbTrId("txt_irad_info_unnamed") == iName->contentWidgetData(QString("text")).toString())
        {
            iName->setContentWidgetData(QString("text"), QString(""));
        }
    }
    return false;
}
