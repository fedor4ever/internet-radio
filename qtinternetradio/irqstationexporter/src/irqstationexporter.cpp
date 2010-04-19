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


#include <CMessageData.h>
#include <sendui.h>
#include <SendUiConsts.h>
#include <eikenv.h>
#include <txtrich.h>
#include <QTextStream>
#include <QFile>
#include <QCoreApplication>

#include "irqstationexporter.h"
#include "irqisdsdatastructure.h"

const char KPlsFilePath[]  = "c:\\data\\share.pls";
const char KPlsFileHeading[]  = "[playlist]";
const char KPlsFileEntryNum[]  = "NumberOfEntries";
const char KPlsFileEnding[]   = "Version=2";
const char KPlsEntryElementFile[]   = "File";
const char KPlsEntryElementTitle[]  = "Title";
const char KPlsEntryElementLength[] = "Length";
const char KPlsIndefiniteLength[] = "-1";

static void constructPlsElement( QTextStream &aOutput, 
                          const IRQPreset &aPreset, 
                          int &aStationIndexBase);

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
EXPORT_C IRQStationExporter::IRQStationExporter()
            : iPlsFilePath(KPlsFilePath)
{
}             
// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
EXPORT_C IRQStationExporter::~IRQStationExporter()
{
}

// ---------------------------------------------------------------------------
// sendViaMms
// NOTE : MUST ensure no changes to all the elements in the list,
//        so, can ONLY access element via const_iterator
// ---------------------------------------------------------------------------
//
EXPORT_C bool IRQStationExporter::sendViaMms(const QList<IRQPreset*> &aPresetList, const QString& aMsg)
{
    if(!constructPlsFile(aPresetList))
    {
        return false;
    }
    
    TRAPD(err,sendViaMmsL(aMsg));
    return KErrNone == err;
}

EXPORT_C bool IRQStationExporter::sendViaMms(const IRQPreset &aPreset, const QString& aMsg)
{
    if(!constructPlsFile(aPreset))
    {
        return false;
    }
    
    TRAPD(err,sendViaMmsL(aMsg));
    return KErrNone == err;
}

void IRQStationExporter::sendViaMmsL(const QString& aMsg)
{
    CMessageData* messageData = CMessageData::NewLC();

    TPtrC plsFilePath(reinterpret_cast<const TUint16*>(iPlsFilePath.utf16()));
    messageData->AppendAttachmentL(plsFilePath);
    
    CEikonEnv* eikonEnv = CEikonEnv::Static();
    CRichText* text =
    CRichText::NewL( eikonEnv->SystemParaFormatLayerL(),
                     eikonEnv->SystemCharFormatLayerL() );
    CleanupStack::PushL(text);
    
    QString shareMsgContentStr(aMsg);
    TPtrC16 shareMsgContentDesc(reinterpret_cast<const TUint16*>(shareMsgContentStr.utf16()));
    
    text->InsertL(0, shareMsgContentDesc);
    messageData->SetBodyTextL( text );
    CleanupStack::PopAndDestroy(text);

    CSendUi* sendUi = CSendUi::NewL();
    CleanupStack::PushL(sendUi);
    sendUi->CreateAndSendMessageL(KSenduiMtmUniMessageUid,messageData);
    CleanupStack::PopAndDestroy(sendUi);

    CleanupStack::PopAndDestroy( messageData );
}

// ---------------------------------------------------------------------------
// constructPlsFile
// NOTE : MUST ensure no changes to all the elements in the list,
//        so, can ONLY access element via const_iterator
// ---------------------------------------------------------------------------
//
bool IRQStationExporter::constructPlsFile(const QList<IRQPreset*> &aPresetList)
{
    if(0 == aPresetList.count())
    {
        return false;
    }
    
    QFile plsFile(iPlsFilePath);
    if( !plsFile.open(QIODevice::ReadWrite | QIODevice::Truncate) )
    {
        return false;
    }
    QTextStream outputStream( &plsFile );
    outputStream<<KPlsFileHeading<<endl;
    
    int entryNum = 0;
    for (QList<IRQPreset*>::const_iterator it = aPresetList.begin(); it != aPresetList.end(); ++it)
    {
        entryNum += (*it)->getChannelURLCount();
    }
    QString totalEntry;
    totalEntry.setNum(entryNum);
    outputStream<<KPlsFileEntryNum<<"="<<totalEntry<<endl;
    
    int stationIndexBase = 1;
    for (QList<IRQPreset*>::const_iterator it = aPresetList.begin(); it != aPresetList.end(); ++it)
    {
        constructPlsElement(outputStream,*(*it),stationIndexBase);
    }
    
    outputStream<<KPlsFileEnding;
    plsFile.close();
    return true;
}

bool IRQStationExporter::constructPlsFile(const IRQPreset &aPreset)
{       
    QFile plsFile(iPlsFilePath);
    if( !plsFile.open(QIODevice::ReadWrite | QIODevice::Truncate) )
    {
        return false;
    }
    QTextStream outputStream( &plsFile );
    outputStream<<KPlsFileHeading<<endl;
    
    QString totalEntry;
    totalEntry.setNum(aPreset.getChannelURLCount());
    outputStream<<KPlsFileEntryNum<<"="<<totalEntry<<endl;
    
    int stationIndexBase = 1;
    constructPlsElement(outputStream,aPreset,stationIndexBase);
    
    outputStream<<KPlsFileEnding;
    plsFile.close();
    return true;
}

static void constructPlsElement( QTextStream &aOutput,
                          const IRQPreset &aPreset,
                          int &aStationIndexBase)
{
    QList<int> bitrateList;
    aPreset.getAvailableBitrates(bitrateList);
    
    int urlIndex = 1;
    QString stationName;
    QList<QString> *urlList = NULL;
    
    for( int i=0; i<bitrateList.count(); i++ )
    {
        urlList = aPreset.getURLsForBitrate(bitrateList.at(i));
        for( int j=0; j<urlList->count(); j++ )
        {
            QString stationUrl = urlList->at(j);
            
            if( aPreset.getChannelURLCount() <= 1 )
            {
                stationName = aPreset.name;
            }
            else
            {
                QString stationUrlIndex;
                stationUrlIndex.setNum(urlIndex);             
                stationName = aPreset.name + " #[" + stationUrlIndex + "]";
            }

            QString stationIndex;
            stationIndex.setNum(aStationIndexBase);              
            aOutput<<KPlsEntryElementFile<<aStationIndexBase
                   <<"="<<stationUrl<<endl;
            aOutput<<KPlsEntryElementTitle<<aStationIndexBase
                   <<"="<<stationName<<endl;            
            aOutput<<KPlsEntryElementLength<<aStationIndexBase
                   <<"="<<KPlsIndefiniteLength<<endl;            
            
            urlIndex++;
            aStationIndexBase++;            
        }
        
        delete urlList;
    }
}



