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

#include "irdocumentloader.h"
#include "irbannerlabel.h"
#include "ircombobox.h"
#include "irlineeditor.h"

IRDocumentLoader::IRDocumentLoader(QObject *aParent) : iParent(aParent)
{
    
}

QObjectList IRDocumentLoader::load(const QString &aFileName, const QString &aSection, QObject *aParent)
{
    bool ok = false;
    QObjectList list = HbDocumentLoader::load(aFileName, aSection, &ok);
    Q_ASSERT_X(ok, "void IRDocumentLoader::load(const QString &aFileName, const QString &aSection)", "load failed");
    
    int size = list.count();
    QObject *parent = aParent ? aParent : iParent;
    for (int i = 0; i < size; i++)
    {
        QObject *obj = list[i];
        if (obj != parent)
        {
            obj->setParent(parent);
        }
    }
    
    return list;
}

QObjectList IRDocumentLoader::load(const QString &aFileName, QObject *aParent)
{
    bool ok = false;
    QObjectList list = HbDocumentLoader::load(aFileName, &ok);
    Q_ASSERT_X(ok, "void IRDocumentLoader::load(const QString &aFileName)", "load failed");
    
    int size = list.count();
    QObject *parent = aParent ? aParent : iParent;
    for (int i = 0; i < size; i++)
    {
        QObject *obj = list[i];
        if (obj != parent)
        {
            obj->setParent(parent);
        }
    }
    
    return list;
}

QObject *IRDocumentLoader::createObject(const QString& aType, const QString &aName)
{
    if (aType == IrViewBannerLabel::staticMetaObject.className())
    {
        QObject *object = new IrViewBannerLabel();
        object->setObjectName(aName);
        return object;
    }
    else if(aType == IrNowPlayingBannerLabel::staticMetaObject.className())
    {
        QObject *object = new IrNowPlayingBannerLabel();
        object->setObjectName(aName);
        return object;
    }
    else if(aType == IrComboBox::staticMetaObject.className())
    {
        QObject *object = new IrComboBox();
        object->setObjectName(aName);
        return object;
    }
    else if(aType == IrLineEditor::staticMetaObject.className())
    {
        QObject *object = new IrLineEditor();
        object->setObjectName(aName);
        return object;
    }
    else if (aType == HbTextItem::staticMetaObject.className())
    {
        QObject *object = new HbTextItem;
        object->setObjectName(aName);
        return object;
    }
    else if (aType == HbMarqueeItem::staticMetaObject.className())
    {
        QObject *object = new HbMarqueeItem;
        object->setObjectName(aName);
        return object;
    }

    return HbDocumentLoader::createObject(aType, aName);
}
