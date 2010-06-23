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
* Description:  Internet Radio home screen widget
*
*/

#ifndef IRHSWIDGET_H
#define IRHSWIDGET_H

// System includes
#include <QObject>
#include <hbwidget.h>
#include <hbicon.h>

// User includes
#include "irservicedef.h"

// Forward declarations
class HbLabel;
class IrServiceClient;

class IrHsWidget : public HbWidget
{
    Q_OBJECT

public:
    explicit IrHsWidget(QGraphicsItem *aParent = 0, Qt::WindowFlags aFlags = 0);
    virtual ~IrHsWidget();


public slots:
    // from KQTI widget model framework, must be slots
    void onInitialize();
    void onUninitialize();    
    void onShow();
    void onHide();

private:
    void setupUi();
    void setupConnection();

    enum IrHsWidgetState
    {
        EUnknown   = -1,
        EInit      = 0,
        EPlaying   = 1,
        EStopped   = 2,
        ELoading   = 3
        
    };
    void handleHsWidgetStateChange(IrHsWidgetState aNewState);
    
    void loadStationLogo();
    
    void initHomeSreenWidget();
    void loadInitLayout();
    void loadStoppedLayout();
    void loadPlayingLayout();
    void loadLoadingLayout();

    // from QObject
    bool eventFilter(QObject *aObject, QEvent *aEvent);

private slots:
    // handle user press event
    void handleLogoAreaAction();
    void handleControlAreaAction();
    void handleGoToIrAreaAction();
    void handleCentralAreaAction();
    
    void handleControlFailed();

    // for service notification
    void handleStationNameUpdated(const QString &aStationName);
    void handleStationLogoUpdated(bool aLogoAvailable);
    void handleMetaDataUpdated(const QString &aMetaData);
    void handleIrStateUpdated(IrAppState::Type aNewState);

private:   
    HbIcon   mLoadingIcon;
    
    HbLabel *mLogoLabel;
    HbLabel *mControlLabel;
    HbLabel *mGoToIrLabel;
    HbLabel *mStationNameLabel;
    HbLabel *mMetadataLabel;

    HbWidget *mInfoControlArea;
    HbWidget *mCentralArea;

    IrServiceClient *mServiceClient;

    IrAppState::Type mIrState;
    IrHsWidgetState  mIrHsWidgetState;
    
    Q_DISABLE_COPY(IrHsWidget)    
};

#endif // IRHSWIDGET_H
