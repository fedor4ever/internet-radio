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

#include <QFile>
#include <QTextStream>
#include <QStringList>
#include <QLocalSocket>
#include <qcoreapplication.h>

#include "irsymbiandocument.h"
#include "irsymbianapplication.h"
#include "irsymbianappui.h"

const QString KEqual("=");
const QString KFile("File");
const QString KTitle("Title");
const QString KLength("Length");
const QString KPlsExtension(".pls");
const QString KM3uExtension(".m3u");

IRSymbianDocument * IRSymbianDocument::documentInstance = NULL;

IRPlsPlayListItem::IRPlsPlayListItem(const QString &aFile, const QString &aTitle, int aLength) :
                                     iFile(aFile), iTitle(aTitle), iLength(aLength)
{
}

const QString & IRPlsPlayListItem::file() const
{
    return iFile;
}

const QString & IRPlsPlayListItem::title() const
{
    return iTitle;
}

IRSymbianDocument * IRSymbianDocument::getInstance()
{
    if (NULL == documentInstance)
    {
        documentInstance = new IRSymbianDocument(*IRSymbianApplication::getInstance());
    }
    
    return documentInstance;
}

bool IRSymbianDocument::isPlsAvailable() const
{
    return iPlsList.count() > 0;
}

int IRSymbianDocument::getNumberOfEntries() const
{
    return iPlsList.count();
}

IRPlsPlayListItem* IRSymbianDocument::getEntry(int aIndex)
{
    if (aIndex >=0 && aIndex < iPlsList.count())
    {
        return iPlsList.at(aIndex);
    }
    
    return NULL;
}

IRSymbianDocument::IRSymbianDocument(CEikApplication &mainApplication) : QS60MainDocument(mainApplication)
{
    
}

IRSymbianDocument::~IRSymbianDocument()
{
    clearPlsList();
}

void IRSymbianDocument::OpenFileL(CFileStore*& aFileStore, RFile& aFile)
{
    aFileStore = NULL;
    TBuf<255> fileName;
    aFile.FullName(fileName);
    TParsePtr ptr(fileName);    
    TPtrC extension = ptr.Ext();
    QString qExtension = QString::fromUtf16(extension.Ptr(), extension.Length());
    
    if (0 == qExtension.compare(KPlsExtension, Qt::CaseInsensitive) ||
        0 == qExtension.compare(KM3uExtension, Qt::CaseInsensitive) ) 
    { 
        QString qFileName = QString::fromUtf16(fileName.Ptr(), fileName.Length());
        IRSymbianApplication *symbianApp = static_cast<IRSymbianApplication*>(Application());
        if (symbianApp->getInstanceFlag())
        {
            //if an instance is already running, send the file name to that instance through socket
            QLocalSocket socket;
            QString serverName = QCoreApplication::applicationName();
            socket.connectToServer(serverName);
            if (socket.waitForConnected(500)) 
            {
                QTextStream stream(&socket);
                stream << qFileName;
                stream.flush();
                socket.waitForBytesWritten();
            }
        }
        else
        {
            parseFile(qFileName);
        }
    }
}

CEikAppUi* IRSymbianDocument::CreateAppUiL()
{
    return new (ELeave) IRSymbianAppui;
}

void IRSymbianDocument::parseFile(const QString &aFileName)
{
    QString extension = aFileName.right(4);
    
    if (0 == extension.compare(KPlsExtension, Qt::CaseInsensitive))
    {
        parsePlsFile(aFileName);
    }
    else if (0 == extension.compare(KM3uExtension, Qt::CaseInsensitive))
    {
        parseM3uFile(aFileName);
    }
    else
    {
        Q_ASSERT(false);
    }
}

void IRSymbianDocument::parsePlsFile(const QString &aFileName)
{
    if (aFileName.length() == 0)
    {
        return;
    }
  
    clearPlsList();
    
    QFile file(aFileName);
    if (!file.open(QIODevice::ReadOnly))
    {
        return;
    }
    
    QTextStream stream(&file);
    QString readStr = stream.readLine().trimmed();
    if (readStr.compare("[playlist]", Qt::CaseInsensitive))
    {
        file.close();
        return;
    }
    
    readStr = stream.readLine().trimmed();
    QStringList splitStr = readStr.split(KEqual);
    if (splitStr.count() != 2)
    {
        file.close();
        return;
    }
    
    int nbEntries = 0;
    if (0 == splitStr.first().compare("NumberOfEntries", Qt::CaseInsensitive))
    {
        nbEntries = splitStr.last().toInt();
    }
    
    if (nbEntries <= 0)
    {
        file.close();
        return;
    }
    
    for (int i = 0; i < nbEntries; ++i)
    {
        //read file line : File=xxx     
        QString filePath = readValue(stream, KFile);
        if (filePath.length() == 0)
        {
            break;
        }
        
        //read title line : Title=xxx
        QString title = readValue(stream, KTitle);
        if (title.length() == 0)
        {
            break;
        }
        
        //read length line : Length=xxx
        QString strLength = readValue(stream, KLength);
        if (strLength.length() == 0)
        {
            break;
        }
        
        int length = strLength.toInt();
        
        //all information is ready
        if (length == -1)
        {
            IRPlsPlayListItem *item = new IRPlsPlayListItem(filePath, title, length);
            iPlsList.append(item);
        }
    }
    
    file.close();
}

void IRSymbianDocument::parseM3uFile(const QString &aFileName)
{
    clearPlsList();
    
    QFile file(aFileName);
    if (!file.open(QIODevice::ReadOnly))
    {
        return;
    }
    
    QTextStream stream(&file);
    QString readStr;
    int stationCount = 0;
    
    while (!stream.atEnd())
    {
        readStr = stream.readLine().trimmed();
        if (readStr.startsWith("http://", Qt::CaseInsensitive))
        {
            IRPlsPlayListItem *item = new IRPlsPlayListItem(readStr, "Station " + QString::number(++stationCount), -1);
            iPlsList.append(item);
        }
    }
    
    file.close();
}

void IRSymbianDocument::clearPlsList()
{
    while (!iPlsList.isEmpty())
    {
        IRPlsPlayListItem *firstItem = iPlsList.takeFirst();
        delete firstItem;
    }
}

QString IRSymbianDocument::readValue(QTextStream &aStream, const QString &aKey)
{
    QString readStr;
    QStringList splitStr;
    
    do
    {
        readStr = aStream.readLine().trimmed();    
    }while (readStr.length() == 0 && !aStream.atEnd());
            
    splitStr = readStr.split(KEqual);
    if (splitStr.count() != 2 || !splitStr.first().startsWith(aKey, Qt::CaseInsensitive))
    {
        return QString();
    }
    else
    {
        return splitStr.last();
    }
}
