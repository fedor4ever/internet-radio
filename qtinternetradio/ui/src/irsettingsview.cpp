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
#include <hbmenu.h>

IRSettingsView::IRSettingsView(IRApplication *aApplication, TIRViewId aViewId) :
    IRBaseView(aApplication, aViewId), iForm(NULL),iModel(NULL)
{
    //this view won't be starting view, don't need lazy init
    IRBaseView::lazyInit();
    setInitCompleted(true);
    
    setFlag(EViewFlag_UnStackable);
    
    initView();
}

IRSettingsView::~IRSettingsView()
{
}

void IRSettingsView::initView()
{
    QGraphicsLinearLayout *layout = new QGraphicsLinearLayout(Qt::Vertical, this);
    setLayout(layout);

    HbGroupBox * titleText = new HbGroupBox();
    titleText->setHeading( hbTrId("txt_common_opt_settings") ); 
    layout->addItem(titleText);

    iForm = new HbDataForm();
    layout->addItem(iForm);

		
    iModel = new HbDataFormModel();
    
    //set the preferred quality
    setPreferredQuality();
    iForm->setModel(iModel);
    
    HbMenu *viewMenu = menu();
    viewMenu->addAction(hbTrId("txt_common_opt_exit"),iApplication, SIGNAL(quit()));    
}

void IRSettingsView::setPreferredQuality()
{
    HbDataFormModelItem *preferredQualityItem = new HbDataFormModelItem(
            HbDataFormModelItem::ToggleValueItem, hbTrId("txt_irad_setlabel_download_quality"));
    iModel->appendDataFormItem(preferredQualityItem);

    IRQPreferredQuality settingValue = iSettings->getPreferredQuality();
    if(EIRQStandardQuality == settingValue)
    {
        preferredQualityItem->setContentWidgetData("text", hbTrId("txt_irad_setlabel_download_quality_val_standard"));
        preferredQualityItem->setContentWidgetData("additionalText", hbTrId("txt_irad_setlabel_download_quality_val_high"));
    }
    else if(EIRQHighQuality == settingValue)
    {
        preferredQualityItem->setContentWidgetData("text", hbTrId("txt_irad_setlabel_download_quality_val_high"));
        preferredQualityItem->setContentWidgetData("additionalText", hbTrId("txt_irad_setlabel_download_quality_val_standard"));    
    }
    
    connect(iModel, SIGNAL(dataChanged(QModelIndex, QModelIndex)), 
        this, SLOT(toggleChange(QModelIndex, QModelIndex)));
}

void IRSettingsView::toggleChange(QModelIndex aStartIn, QModelIndex aEndIn)
{
    Q_UNUSED(aEndIn);

    HbDataFormModelItem *item = iModel->itemFromIndex(aStartIn);
    
    if(item->type() == HbDataFormModelItem::ToggleValueItem) 
    {
        if(item->data(HbDataFormModelItem::LabelRole).toString() == hbTrId("txt_irad_setlabel_download_quality")) 
        {
            QVariant data = item->contentWidgetData("text");
            QString value = data.toString();
            
            if(value == (hbTrId("txt_irad_setlabel_download_quality_val_standard")))
            {    
                // User has made it Standard.
                iSettings->setPreferredQuality(EIRQStandardQuality);
            }
            else if(value == (hbTrId("txt_irad_setlabel_download_quality_val_high")))
            {
                // User has made it High.
                iSettings->setPreferredQuality(EIRQHighQuality);
            }
        }
    }
}
