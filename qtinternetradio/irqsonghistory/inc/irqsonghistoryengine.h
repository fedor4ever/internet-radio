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
#ifndef IRQSONGHISTORYENGINE_H_
#define IRQSONGHISTORYENGINE_H_

#include <QObject>

class IRQMetaData;
class IRQPreset;
class IRQSongHistoryInfo;
class CIRSongHistoryDb;
class IRQSongInfo;

/**
* Engine component for Internet Radio SongHistory.
*/
class IRQSongHistoryEngine : public QObject
{
    Q_OBJECT

public:

    IMPORT_C static IRQSongHistoryEngine* openInstance();
    IMPORT_C void closeInstance();

    /**
    * Get all history from daba base
    * @param    QList<IRQSongHistoryInfo *>
    */
    IMPORT_C void getAllHistory(QList<IRQSongHistoryInfo *>& aSongHistoryArr);
    
    /**
     * this is songs' get, the above will be changed to the stations only
     */
    IMPORT_C void getAllSongHistory(QList<IRQSongInfo *>& aSongHistoryArr);

    /**
    * Clear all history in data base
    */
    IMPORT_C int clearAllHistory();
    
    IMPORT_C int clearAllSongHistory();

    /**
    * Called by the UI when a particular channel has been removed from the ISDS.
    */
    IMPORT_C void syncHistory(int aChannelId);

    /**
    * Called by the UI when a particular channel has been changed in the ISDS.
    */

    IMPORT_C void updateSongHistoryDb(int aChannelId,
                                      const QString& aChannelName,
                                      const QString& aChannelUrl,
                                      const QString& aImageUrl,
                                      const QString& aMusicFlag);

    /**
    * Connected with play controller with the song name changed.
    * @param    aMetaData   The meta data contains song name, artist and bitrate.
    * @param    aPreset     The preset contains other information.
    */
    IMPORT_C void handleMetaDataReceived(const IRQMetaData& aMetaData, const IRQPreset& aPreset);
    /**
    * Connected with play controller with the song name changed.
    * @param    aMetaData   The meta data contains song name, artist and bitrate.
    * @param    aMusicshopStatus     The music shop status of the song
    */
    IMPORT_C void handleSongMetaDataReceived(const IRQMetaData& aMetaData, QString& aMusicshopStatus);
    
    /*
     * delete one item from the station history db
     * @param   aIndex    the index of the item need deleted
     */
    IMPORT_C bool deleteOneItem(int aIndex);

private:

    enum TSongHistoryItemChange
    {
        ENOCHANGE = 0,
        ECHANNELCHANGED,
        EMETADATACHANGED,
        EBLANKENTRIES
    };

private:

    /**
     *  Creates IRQSongHistoryEngine instance
     */
    static IRQSongHistoryEngine* createInstanceL();
    
    void constructL();

    /**
    * Constructor.
    */
    IRQSongHistoryEngine();

    /**
    * Destructor.
    */
    ~IRQSongHistoryEngine();
    
    TSongHistoryItemChange detectSongHistoryItemChangesL(const IRQMetaData& aMetaData, const QString& aChannelName);
    
    void handleMetaDataReceivedL(const IRQMetaData& aMetaData, const IRQPreset& aPreset);
	void getAllHistoryL(QList<IRQSongHistoryInfo *>& aSongHistoryArr);
	void handleSongMetaDataReceivedL(const IRQMetaData& aMetaData, QString& aMusicshopStatus);
    void getAllSongHistoryL(QList<IRQSongInfo *>& aSongHistoryArr);

private:

    /** An instance of songhistory database. */
    CIRSongHistoryDb* iSongHistoryDb;

    /**
     * Number of objects currently referring to the singleton object IRQSongHistoryEngine
     */
    int iSingletonInstances;
};

#endif //IRQSONGHISTORYENGINE_H_
