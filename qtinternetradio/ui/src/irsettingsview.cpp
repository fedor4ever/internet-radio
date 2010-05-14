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
#include <hbdataformmodel.h>
#include <HbGroupBox>

#include "irapplication.h"
#include "irsettingsview.h"
#include "irqsettings.h"
#include "irqenums.h"

IRSettingsView::IRSettingsView(IRApplication *aApplication, TIRViewId aViewId) :
    IRBaseView(aApplication, aViewId), iForm(NULL),iModel(NULL)
{
    //this view won't be starting view, don't need lazy init
    IRBaseView::lazyInit();
    setInitCompleted(true);
    
    iPreferredQuality<<hbTrId("txt_irad_setlabel_download_quality_val_standard")<<hbTrId("txt_irad_setlabel_download_quality_val_high");
    initView();
}

IRSettingsView::~IRSettingsView()
{
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
    QGraphicsLinearLayout *layout = new QGraphicsLinearLayout(Qt::Vertical, this);
    setLayout(layout);

    HbGroupBox * titleText = new HbGroupBox();
    titleText->setHeading( hbTrId("txt_irad_subtitle_network_setting") ); 
    layout->addItem(titleText);

    iForm = new HbDataForm();
    layout->addItem(iForm);

		
    iModel = new HbDataFormModel();
    
    //set the preferred quality
    setPreferredQuality();
    iForm->setModel(iModel);
}

void IRSettingsView::setPreferredQuality()
{
    HbDataFormModelItem *preferredQualityItem = new HbDataFormModelItem(
            HbDataFormModelItem::RadioButtonListItem, hbTrId("txt_irad_setlabel_download_quality"));
    iModel->appendDataFormItem(preferredQualityItem);
	
    preferredQualityItem->setContentWidgetData(QString("items"), iPreferredQuality);
    IRQPreferredQuality settingValue = iSettings->getPreferredQuality();
    preferredQualityItem->setContentWidgetData(QString("selected"), settingValue);
	
    iForm->addConnection(preferredQualityItem,SIGNAL(itemSelected(int)),this,SLOT(preferredQualitySelected(int)));
}

