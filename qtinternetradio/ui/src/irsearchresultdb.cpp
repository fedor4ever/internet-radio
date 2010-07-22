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
#include <QSqlQuery>
#include <QSqlRecord> 
#include <QSqlDatabase>
#include <QVariant>
 

#include "irsearchresultdb.h"
#include "irqisdsdatastructure.h"


IRSearchResultDB::IRSearchResultDB()
{
    createDBConnection();
}

IRSearchResultDB::~IRSearchResultDB()
{
    if (iDB->isOpen())
    {
        iDB->close();
    }        
    
    delete iDB;
    iDB = NULL;
}

 
IRQError IRSearchResultDB::cacheChannelList(QList<IRQChannelItem*> *aChannelList)
{
    IRQError ret = EIRQErrorNone;
    
    if( NULL == aChannelList )
    {
        return EIRQErrorBadParameter;
    }
    
    if( iDB->isOpen() )
    {
        clearCache();
        
        for(int i=0; i<aChannelList->count(); i++)
        {
            IRQChannelItem* insertItem = aChannelList->at(i);
            QString name = insertItem->channelName;
            int channelID = insertItem->channelID;
            QString imageURL = insertItem->imageURL;
            QString description = insertItem->shortDescription;
            
            //if some data overflows, we just skip it.note that
            //the VARCHAR is word-based, so here we use the real size 
            if( name.size()>= 256 || imageURL.size() >= 256 || description.size() >= 256 )
            {
                continue;
            }            
            
            QSqlQuery query;
            bool result;
            
            result = query.prepare("INSERT INTO searchresult (name, channelID, imageURL, description) "
                "VALUES (:name, :channelID, :imageURL, :description)");
            query.bindValue(":name", name);
            query.bindValue(":channelID",channelID);
            query.bindValue(":imageURL", imageURL);
            query.bindValue(":description", description);
            
            result = query.exec();
            if( !result )
            {
                ret = EIRQErrorServiceUnavailable;
                break;
            }
        }
    }
    else
    {
        ret = EIRQErrorGeneral;
    }
    
    return ret;
}

QList<IRQChannelItem*> *IRSearchResultDB::getCahcedChannelList()
{
    if( !iDB->isOpen() )
    {
        return NULL;
    }
    
    QList<IRQChannelItem*> *channelList = new QList<IRQChannelItem*>();
    QSqlQuery query("SELECT * FROM searchresult");
    QSqlRecord rec = query.record();
    int nameCol = rec.indexOf("name");
    int channelIDCol = rec.indexOf("channelID");
    int imageURLCol = rec.indexOf("imageURL");
    int descriptionCol = rec.indexOf("description");     
    
    while(query.next())
    {
        IRQChannelItem *oneItem = new IRQChannelItem();
        oneItem->channelName = query.value(nameCol).toString();
        oneItem->channelID = query.value(channelIDCol).toInt();
        oneItem->imageURL = query.value(imageURLCol).toString();
        oneItem->shortDescription = query.value(descriptionCol).toString();
        channelList->append(oneItem);
    }
    
    return channelList;    
}
  
IRQError IRSearchResultDB::clearCache()
{
    IRQError ret = EIRQErrorNone;
    if( !iDB->open())
    {
        ret = EIRQErrorServiceUnavailable;
    }
    else
    { 
        QSqlQuery query("DELETE FROM searchresult");         
        bool ret = query.exec();
        if( !ret )
        {
            ret = EIRQErrorServiceUnavailable;
        }
    }
    
    return ret;
}

void IRSearchResultDB::createDBConnection()
{ 
    bool created = QFile::exists("searchresult.dat");    
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");     
    iDB = new QSqlDatabase(db);

    iDB->setDatabaseName("searchresult.dat");

    if (!iDB->open())
    {         
        return;
    }
     

    if (!created)
    {
        bool dbResult = false;
        QSqlQuery query;         
        //note: the VARCHAR is word-based but not byte-based. and 255 
        //means 255 unicode words.
        dbResult = query.exec("CREATE TABLE searchresult ("
            "id INTEGER PRIMARY KEY AUTOINCREMENT, "
            "name VARCHAR(255) NOT NULL, "
            "channelID INTEGER, "
            "imageURL VARCHAR(255), "
            "description VARCHAR(255) NOT NULL)");

        if (!dbResult)
        {             
            return;
        }
    } 
} 
