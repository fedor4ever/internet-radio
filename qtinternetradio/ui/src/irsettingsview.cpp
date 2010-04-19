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
#include <QGraphicsLinearLayout>
#include <hbdataformviewitem.h>
#include <hbdataformmodel.h>
#include <hbradiobuttonlist.h>

#include "irapplication.h"
#include "irsettingsview.h"
#include "irqsettings.h"
#include "irqenums.h"
#include "irlastplayedstationinfo.h"

const QString KStartingViewGroupLabelKeyWord = QObject::tr("Choose Starting View");
const QString KPreferredQualityGroupLabelKeyWord = QObject::tr(
        "Choose Preferred Quality");

IRSettingsView::IRSettingsView(IRApplication *aApplication, TIRViewId aViewId) :
    IRBaseView(aApplication, aViewId), iForm(NULL),iModel(NULL)
{
    iStartingViews << tr("Browse by Genre") << tr("Browse by Language") << tr(
            "Browse by Country") << tr("Collections view") << tr(
            "Favorite stations") << tr("Last played station") << tr(
            "Last exit view");
    iPreferredQuality<<tr("Normal")<<tr("High");
    initView();
}

IRSettingsView::~IRSettingsView()
{
}

void IRSettingsView::startingViewSelected(int aIndex)
{
    if (iStartingViews.indexOf(tr("Last exit view")) != aIndex)
    {
        iSettings->setStickyViewFlag(false);
    }

    switch (aIndex)
    {
    case 0:
        iSettings->setStartingViewId(EIRView_CategoryView);
        iSettings->setStartingViewParameter(EIRViewPara_Genre);
        break;
    case 1:
        iSettings->setStartingViewId(EIRView_CategoryView);
        iSettings->setStartingViewParameter(EIRViewPara_Language);
        break;
    case 2:
        iSettings->setStartingViewId(EIRView_CategoryView);
        iSettings->setStartingViewParameter(EIRViewPara_Country);
        break;
    case 3:
        iSettings->setStartingViewId(EIRView_MainView);
        break;
    case 4:
        iSettings->setStartingViewId(EIRView_FavoritesView);
        break;
    case 5:
        if (iApplication->getLastPlayedStationInfo()->getLastPlayedStation())
        {
            iSettings->setStartingViewId(EIRView_PlayingView);
        }
        else
        {
            iSettings->setStickyViewFlag(true);
        }
        break;
    case 6:
        iSettings->setStickyViewFlag(true);
        break;
    default:
        iSettings->setStartingViewId(EIRView_CategoryView);
        iSettings->setStartingViewParameter(EIRViewPara_Genre);
        break;
    }
}


void IRSettingsView::preferredQualitySelected(int aIndex)
{
    if (0 == aIndex)
    {
        iSettings->setPreferredQuality(EIRQStandardQuality);
    }
    else
    {
        iSettings->setPreferredQuality(EIRQHighQuality);
    }
}

void IRSettingsView::initView()
{
    QGraphicsLinearLayout *layout = new QGraphicsLinearLayout(Qt::Vertical,
            this);
    setLayout(layout);
    iForm = new HbDataForm();
    layout->addItem(iForm);
    iModel = new HbDataFormModel();
    //set the starting view list
    setStartingViewList();
    //set the preferred quality
    setPreferredQuality();
    iForm->setModel(iModel);
}

void IRSettingsView::setStartingViewList()
{
    if (NULL == iApplication->getLastPlayedStationInfo()->getLastPlayedStation())
    {
        iStartingViews.removeAt(iStartingViews.count() - 2);
    }
    HbDataFormModelItem *startViewItemGroup = new HbDataFormModelItem(
            HbDataFormModelItem::GroupItem, KStartingViewGroupLabelKeyWord);
    iModel->appendDataFormItem(startViewItemGroup);
    HbDataFormModelItem *startViewModelItem = iModel->appendDataFormItem(
            HbDataFormModelItem::RadioButtonListItem, tr("StartView"),
            startViewItemGroup);
    startViewModelItem->setContentWidgetData(QString("items"), iStartingViews);
    startViewModelItem->setData(HbDataFormModelItem::KeyRole, tr("Choose Startview"));
    startViewModelItem->setContentWidgetData(QString("selected"),getStartingViewSelection());
    iForm->addConnection(startViewModelItem,SIGNAL(itemSelected(int)),this,SLOT(startingViewSelected(int)));
}

void IRSettingsView::setPreferredQuality()
{
    HbDataFormModelItem *preferredQualityItemGroup = new HbDataFormModelItem(
            HbDataFormModelItem::GroupItem, KPreferredQualityGroupLabelKeyWord);
    iModel->appendDataFormItem(preferredQualityItemGroup);
    HbDataFormModelItem *preferredQualityItem = iModel->appendDataFormItem(
            HbDataFormModelItem::RadioButtonListItem, tr("Network Quality"),
            preferredQualityItemGroup);
    preferredQualityItem->setContentWidgetData(QString("items"), iPreferredQuality);
    preferredQualityItem->setData(HbDataFormModelItem::KeyRole, tr("Choose Preferred Quality"));
    IRQPreferredQuality settingValue = iSettings->getPreferredQuality();
    preferredQualityItem->setContentWidgetData(QString("selected"), settingValue);
    iForm->addConnection(preferredQualityItem,SIGNAL(itemSelected(int)),this,SLOT(preferredQualitySelected(int)));
}

int IRSettingsView::getStartingViewSelection()
{
    bool flag = false;
    iSettings->getStickyViewFlag(flag);
    if (flag)
    {
        return iStartingViews.indexOf(tr("Last exit view"));
    }

    TIRViewId startingViewId = EIRView_InvalidId;
    iSettings->getStartingViewId(startingViewId);
    int selected = 0;
    switch (startingViewId)
    {
    case EIRView_CategoryView:
        TIRViewParameter parameter;
        iSettings->getStartingViewParameter(parameter);
        switch (parameter)
        {
        case EIRViewPara_Genre:
            selected = 0;
            break;
        case EIRViewPara_Language:
            selected = 1;
            break;
        case EIRViewPara_Country:
            selected = 2;
            break;
        default:
            selected = 0;
            break;
        }
        break;
    case EIRView_MainView:
        selected = 3;
        break;
    case EIRView_FavoritesView:
        selected = 4;
        break;
    case EIRView_PlayingView:
        selected = 5;
        break;
    default:
        selected = 0;
        break;
    }
    return selected;
}

