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
// System includes
#include <QGraphicsLinearLayout>
#include <QSettings>
#include <QPixmap>
#include <HbLabel>
#include <HbColorScheme>
#include <HbStyleLoader>

// User includes
#include "irhswidgettitlerow.h"
#include "irservicedef.h"
#include "irhswidgetuiloader.h"

// Constants
static const QString KIrHsWidgetTitleRowDocML       = ":/resource/irhswidgettitlerow.docml";
static const QString KIrHsWidgetTitleRowContainer   = "titlerow_layout";
static const QString KIrHsWidgetStationLogo         = "station_logo";
static const QString KNmHsWidgetStationName         = "station_name";

static const QString KStationNameColor      = "qtc_hs_list_item_title";
static const QString KDefaultStationLogo    = "qtg_large_internet_radio";

static const int KIrHsWidgetContentsMargin  = 0;
static const int KIrHsWidgetLogoSize        = 50; // hs widget logo size

IrHsWidgetTitleRow::IrHsWidgetTitleRow(QGraphicsItem *aParent, Qt::WindowFlags aFlags) :
    HbWidget(aParent, aFlags), 
    mStationLogo(NULL), 
    mStationName(NULL)
{
    loadUi();
}

IrHsWidgetTitleRow::~IrHsWidgetTitleRow()
{
}

void IrHsWidgetTitleRow::loadUi()
{
    IRHsWidgetUiLoader loader;
    loader.load(KIrHsWidgetTitleRowDocML);

    //Create layout
    QGraphicsLinearLayout *layout = new QGraphicsLinearLayout(Qt::Vertical);

    layout->setContentsMargins(KIrHsWidgetContentsMargin, KIrHsWidgetContentsMargin,
        KIrHsWidgetContentsMargin, KIrHsWidgetContentsMargin);
    layout->setSpacing(KIrHsWidgetContentsMargin);
    setLayout(layout);
    
    QGraphicsWidget *container = loader.findWidget(KIrHsWidgetTitleRowContainer);
    layout->addItem(container);

    mStationLogo = static_cast<HbLabel*> (loader.findWidget(KIrHsWidgetStationLogo));
    mStationName = static_cast<HbLabel*> (loader.findWidget(KNmHsWidgetStationName));
    mStationName->setTextColor(HbColorScheme::color(KStationNameColor));         
}

void IrHsWidgetTitleRow::setDefaultTitle()
{
#ifdef SUBTITLE_STR_BY_LOCID 
    mStationName->setPlainText(hbTrId("txt_irad_title_internet_radio"));
#else
    mStationName->setPlainText(hbTrId("Internet radio"));    
#endif
}

void IrHsWidgetTitleRow::setStationName(const QString& aStationName)
{
    if (mStationName->plainText() != aStationName)
    {
        mStationName->setPlainText(aStationName);
    }   
}
    
void IrHsWidgetTitleRow::loadStationLogo()
{
    QSettings settings(KIrSettingOrganization, KIrSettingApplication);
    if (settings.value(KIrSettingStationLogo).canConvert<QPixmap>())
    {
        QPixmap logoPixmap = settings.value(KIrSettingStationLogo).value<QPixmap>();
        QPixmap newLogoPixmap = 
             logoPixmap.scaled(QSize(KIrHsWidgetLogoSize,KIrHsWidgetLogoSize),Qt::KeepAspectRatio);
        QIcon logoQIcon(newLogoPixmap);
        HbIcon logoHbIcon(logoQIcon);            
        mStationLogo->setIcon(logoHbIcon);
    }  
}

void IrHsWidgetTitleRow::setDefaultLogo()
{
    mStationLogo->setIcon(KDefaultStationLogo);
}


void IrHsWidgetTitleRow::mousePressEvent(QGraphicsSceneMouseEvent *aEvent)
{
    Q_UNUSED(aEvent); 
	emit titleRowClicked();
}
