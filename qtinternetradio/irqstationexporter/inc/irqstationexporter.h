/*
* Copyright (c) 2009-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  class declaration for sharing stations
*
*/


#ifndef IRQSTATIONEXPORTER_H_
#define IRQSTATIONEXPORTER_H_

#include <QString>
#include <QList>

class IRQPreset;

/*!
 * This class provides station exporter related interface.
 *
 */
class IRQStationExporter
{
public:
    /*!
     * Constructor.
     */
    IMPORT_C IRQStationExporter();
    
    /*!
     * Destructor.
     */    
    IMPORT_C ~IRQStationExporter();
        
public:
    /*!
     * Launch MMS editor to send station list 
     * @return bool  ture if launch successfully, otherwise return false
     */                              
    IMPORT_C bool sendViaMms(const QList<IRQPreset*> &aPresetList, const QString& aMsg);
    IMPORT_C bool sendViaMms(const IRQPreset &aPreset, const QString& aMsg);

private:
    void sendViaMmsL(const QString& aMsg);
    bool constructPlsFile(const QList<IRQPreset*> &aPresetList);
    bool constructPlsFile(const IRQPreset &aPreset);
        
private:
    QString  iPlsFilePath;
};

#endif // IRQSTATIONEXPORTER_H_

