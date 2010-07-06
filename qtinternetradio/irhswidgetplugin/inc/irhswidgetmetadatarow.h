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


#ifndef IRHSWIDGETMETADATAROW_H_
#define IRHSWIDGETMETADATAROW_H_

// System includes
#include <HbWidget>
#include <HbIcon>

//FORWARD DECLARATIONS:
class HbLabel;
class HbMarqueeItem;

class IrHsWidgetMetaDataRow : public HbWidget
{
    Q_OBJECT
  
public:
    explicit IrHsWidgetMetaDataRow(QGraphicsItem *aParent = 0, Qt::WindowFlags aFlags = 0);
    virtual ~IrHsWidgetMetaDataRow();

    void setMetaData(const QString &aMetaData);
  
    void startMetaDataMarquee();  
    void stopMetaDataMarquee();
    
    void setPlayIcon();
    void setStopIcon();
    void setLoadingIcon(); 
    
private:
    // from QObject
    bool eventFilter(QObject *aObject, QEvent *aEvent);

private:
    void loadUi();
    void createLoadingIcon();
    
signals:
    void controlAreaClicked();
    void metaDataAreaClicked();

private:
    HbMarqueeItem *mMetaData;
    HbLabel *mControlLabel;
    HbIcon   mLoadingIcon;    
};

#endif // IRHSWIDGETMETADATAROW_H_
