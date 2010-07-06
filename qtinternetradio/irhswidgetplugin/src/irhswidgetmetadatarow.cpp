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
#include <HbLabel>
#include <HbMarqueeItem>
#include <HbIconAnimationManager>
#include <HbIconAnimationDefinition>
#include <HbColorScheme>
#include <HbStyleLoader>

// User includes
#include "irhswidgetmetadatarow.h"
#include "irhswidgetuiloader.h"

// Constants
static const QString KIrHsWidgetMetaDataRowDocML       = ":/resource/irhswidgetmetadatarow.docml";
static const QString KIrHsWidgetMetaDataRowContainer   = "metadatarow_layout";
static const QString KIrHsWidgetMetaDataLabel          = "metadata_label";
static const QString KNmHsWidgetControlLabel           = "control_label";

static const QString KPlayButtonIcon     = "qtg_mono_play";
static const QString KStopButtonIcon     = "qtg_mono_stop";
static const QString KMetaDataColor      = "qtc_hs_list_item_title";

static const QString KLoadingAnimationPrefix  = "qtg_anim_loading_";
static const QString KLoadingIconName         = "LoadingAnimation";

static const int KIrHsWidgetContentsMargin  = 0;
static const int KLoadingFramePostfixMin    = 1;  // loading animation frame name postfix (min value) defined in layout guide.
static const int KLoadingFramePostfixMax    = 10; // loading animation frame name postfix (max value) defined in layout guide.
static const int KLoadingAnimationDuration  = 100; // duration for frames to construct a animation, unit : ms

IrHsWidgetMetaDataRow::IrHsWidgetMetaDataRow(QGraphicsItem *aParent, Qt::WindowFlags aFlags) :
    HbWidget(aParent, aFlags), 
    mMetaData(NULL), 
    mControlLabel(NULL)
{
    loadUi();
    createLoadingIcon();
}


IrHsWidgetMetaDataRow::~IrHsWidgetMetaDataRow()
{
}

void IrHsWidgetMetaDataRow::loadUi()
{
    IRHsWidgetUiLoader loader;
    loader.load(KIrHsWidgetMetaDataRowDocML);

    QGraphicsLinearLayout *layout = new QGraphicsLinearLayout(Qt::Vertical);
    layout->setContentsMargins(KIrHsWidgetContentsMargin, KIrHsWidgetContentsMargin,
        KIrHsWidgetContentsMargin, KIrHsWidgetContentsMargin);
    layout->setSpacing(KIrHsWidgetContentsMargin);
    setLayout(layout);

    QGraphicsWidget *container = loader.findWidget(KIrHsWidgetMetaDataRowContainer);
    layout->addItem(container);   
                      
    mMetaData = static_cast<HbMarqueeItem*> (loader.findWidget(KIrHsWidgetMetaDataLabel));
    mControlLabel  = static_cast<HbLabel*> (loader.findWidget(KNmHsWidgetControlLabel));
    mMetaData->setTextColor(HbColorScheme::color(KMetaDataColor));  
    mMetaData->setLoopCount(-1);
                    
    mMetaData->installEventFilter(this);
    mControlLabel->installEventFilter(this);        
}

void IrHsWidgetMetaDataRow::setMetaData(const QString &aMetaData)
{      
    if (mMetaData->text() != aMetaData)
    {
        mMetaData->setText(aMetaData);
        if (aMetaData.isEmpty())
        {
            mMetaData->stopAnimation();        
        }
        else
        {
            mMetaData->startAnimation();
        }
    }    
}

void IrHsWidgetMetaDataRow::startMetaDataMarquee()
{    
    if (!mMetaData->text().isEmpty())
    {
        mMetaData->startAnimation();        
    }      
}
    
void IrHsWidgetMetaDataRow::stopMetaDataMarquee()
{
    mMetaData->stopAnimation(); 
}   

void IrHsWidgetMetaDataRow::setPlayIcon()
{
    mControlLabel->setIcon(KPlayButtonIcon);
}
    
void IrHsWidgetMetaDataRow::setStopIcon()
{
    mControlLabel->setIcon(KStopButtonIcon);
}
    
void IrHsWidgetMetaDataRow::setLoadingIcon()
{
    mControlLabel->setIcon(mLoadingIcon);
}    
    
bool IrHsWidgetMetaDataRow::eventFilter(QObject *aObject, QEvent *aEvent)
{
    QString objectName     = aObject->objectName();
    
    if (QEvent::GraphicsSceneMousePress == aEvent->type())
    {
        
        if (KIrHsWidgetMetaDataLabel == objectName)
        {
            emit metaDataAreaClicked();
        }
        else if (KNmHsWidgetControlLabel == objectName)
        {
            emit controlAreaClicked();
        }
    }
        
    return false;
}

void IrHsWidgetMetaDataRow::createLoadingIcon()
{
    HbIconAnimationManager *animationManager = HbIconAnimationManager::global();
    HbIconAnimationDefinition animationDefinition;
    HbIconAnimationDefinition::AnimationFrame animationFrame;
    QList<HbIconAnimationDefinition::AnimationFrame> animationFrameList;
    
    QString animationFrameIconName;
    for (int i = KLoadingFramePostfixMin; i <= KLoadingFramePostfixMax; i++)
    {
        animationFrame.duration = KLoadingAnimationDuration;
        animationFrameIconName.clear();
        animationFrameIconName.append(KLoadingAnimationPrefix);
        animationFrameIconName.append(animationFrameIconName.number(i));
        animationFrame.iconName = animationFrameIconName;
        animationFrameList.append(animationFrame);
    }
    animationDefinition.setPlayMode(HbIconAnimationDefinition::Loop);
    animationDefinition.setFrameList(animationFrameList);
    animationManager->addDefinition(KLoadingIconName, animationDefinition);

    mLoadingIcon.setIconName(KLoadingIconName);        
}        
