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

#ifndef IRDOCUMENTLOADER_H_
#define IRDOCUMENTLOADER_H_

#include <hbdocumentloader.h>

class IRDocumentLoader : public HbDocumentLoader
{
public:
    IRDocumentLoader(QObject *aParent);
    
    QObjectList load(const QString &aFileName, const QString &aSection, QObject *aParent = NULL);
    QObjectList load(const QString &aFileName, QObject *aParent = NULL);
    
public:
    virtual QObject *createObject(const QString& aType, const QString &aName);
    
private:
    QObject *iParent;
};

#endif /* IRDOCUMENTLOADER_H_ */
