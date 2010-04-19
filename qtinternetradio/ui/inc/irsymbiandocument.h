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

#ifndef IRSYMBIANDOCUMENT_H
#define IRSYMBIANDOCUMENT_H

#include <QS60MainDocument>

#include <QString>
#include <QList>

class QTextStream;

class IRPlsPlayListItem
{
public:
    IRPlsPlayListItem(const QString &aFile, const QString &aTitle, int aLength);
    
    const QString & file() const;
    const QString & title() const;
    
private:
    QString iFile;
    QString iTitle;
    int     iLength;
};

class IRSymbianDocument : public QS60MainDocument
{
public:
    static IRSymbianDocument *getInstance();
    bool isPlsAvailable() const;
    int getNumberOfEntries() const;
    IRPlsPlayListItem* getEntry(int aIndex);
    
    void parseFile(const QString &aFileName);
    
private:
    IRSymbianDocument(CEikApplication &mainApplication);
    ~IRSymbianDocument();
    
private:
    //from base class CEikDocument
    void OpenFileL(CFileStore*& aFileStore, RFile& aFile);
    CEikAppUi* CreateAppUiL();
    
    void parsePlsFile(const QString &aFileName);
    void parseM3uFile(const QString &aFileName);
    
    void clearPlsList();
    QString readValue(QTextStream &aStream, const QString &aKey);
    
private:
    static IRSymbianDocument *documentInstance;
    QList<IRPlsPlayListItem*> iPlsList;
};

#endif //IRSYMBIANDOCUMENT_H
