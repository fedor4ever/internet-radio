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
 

#include "irsearchcriteriadb.h"


IRSearchCriteriaDB::IRSearchCriteriaDB(int aMaxCount):iDB(NULL),iMaxCount(aMaxCount)
{
    createDBConnection();
}

IRSearchCriteriaDB::~IRSearchCriteriaDB()
{
    if (iDB->isOpen())
    {
        iDB->close();
    }        
    
    delete iDB;
    iDB = NULL;
}

IRQError IRSearchCriteriaDB::addOneCriteria(const QString& aCriteria)
{         
    if (iDB->isOpen())
    {
        if( aCriteria.length() >= CRITERIA_UPLIMIT_LENGTH )
        {             
            //the unicode string is too long to store
            return EIRQErrorBadParameter;
        }
        
        QSqlQuery query("SELECT * FROM criteria");
        QSqlRecord rec = query.record();         
 
        if( rec.count() >= iMaxCount )
        {
            bool ret = deleteSomeCriteria();            
            if( !ret )
            {
                return EIRQErrorGeneral;
            }
        }
         
        
        bool ret = query.prepare("INSERT INTO criteria (cricolum) "
            "VALUES (?)");
        query.addBindValue(aCriteria,QSql::In);
        ret = query.exec();        
         
        emit dataAdded();
        return EIRQErrorNone;
    }

    return EIRQErrorServiceUnavailable;
}

bool IRSearchCriteriaDB::getAllCriteria(QStringList& aStrList) const
{
    aStrList.clear();     
    if (iDB->isOpen())
    {
        QSqlQuery query("SELECT * FROM criteria");
        QSqlRecord rec = query.record();         

        int criteriaCol = rec.indexOf("cricolum");
        while (query.next())
        {
            aStrList.append(query.value(criteriaCol).toString());
        }
        return true;
    }

    return false;
}

void IRSearchCriteriaDB::createDBConnection()
{    
    bool created = QFile::exists("searchcriteria.dat");    
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");     
    iDB = new QSqlDatabase(db);

    iDB->setDatabaseName("searchcriteria.dat");

    if (!iDB->open())
    {         
        return;
    }
     

    if (!created)
    {
    	  bool dbResult = false;
        QSqlQuery query;         
        dbResult = query.exec("CREATE TABLE criteria ("
            "id INTEGER PRIMARY KEY AUTOINCREMENT, "
            "cricolum VARCHAR(255) NOT NULL)");

        if (!dbResult)
        {             
            return;
        }
    }    
}

bool IRSearchCriteriaDB::isAlreadyExist(const QString& aCriteria) const
{         
    //TODO: change to the sql statement, but now the sqlquery binding 
    //dose not work well 
    if( aCriteria.length() >= CRITERIA_UPLIMIT_LENGTH )
    {    	   
        return true;
    }
    
    QStringList strList;
    getAllCriteria(strList);
    
    for(int i=0; i<strList.count(); i++)
    {
        if( 0 == strList[i].compare(aCriteria, Qt::CaseInsensitive))
        {            
            return true;
        }
    }     
     
    return false;
}

bool IRSearchCriteriaDB::deleteSomeCriteria()
{
    //TODO: to delete some items when the db is full
    return true;   
}

 
