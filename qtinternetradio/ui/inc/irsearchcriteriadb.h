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

#ifndef IRSEARCHCRITERIADB_H
#define IRSEARCHCRITERIADB_H

#include <QObject>
#include <QEvent>
#include <QStringList>
#include "irqenums.h"

/* use this macro to limit the length of the input string */
#define CRITERIA_UPLIMIT_LENGTH 127
 
class QSqlDatabase;

class IRSearchCriteriaDB : public QObject
{
	Q_OBJECT
	
public:
		
    explicit IRSearchCriteriaDB(int aMaxCount = 100);     
	~IRSearchCriteriaDB();
	IRQError addOneCriteria(const QString& aCriteria);
    bool getAllCriteria(QStringList& aStrList) const;
    bool isAlreadyExist(const QString& aCriteria) const;

signals:
    void dataAdded();   
    
private:		
    void createDBConnection();
    bool deleteSomeCriteria();
		
private:
		
	  QSqlDatabase         *iDB;  	 
	  int          iMaxCount;					
};
#endif
