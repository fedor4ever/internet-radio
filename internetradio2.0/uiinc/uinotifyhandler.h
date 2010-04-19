/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Interface class to notify database changes to SongHistoryView
*
*/


#ifndef IR_SONGHISTORYUINOTIFYHANDLER
#define IR_SONGHISTORYUINOTIFYHANDLER

#include <e32cmn.h>

/**
* Observer interface for CIRSongHistoryView.
* Notifies the view when new data is received.
*/
NONSHARABLE_CLASS(MSongHistoryUINotifyHandler)
    {
		public:

			/**
			* Invoked from CIRSongHistoryDbEng when new entry is added to the database.
			* @param    aSongName
			* @param    aArtistName
			* @param    aChannelName
			* Parameters yet to be decided.
			*/
				virtual void NextSongReceivedL( const TDesC& aSongName, const TDesC& aArtistName,
							 const TDesC& aChannelName, const TDesC& aChannelUrl ) = 0;

				virtual void ChannelChangedL( const TDesC& aSongName, const TDesC& aArtistName,
						 const TDesC& aChannelName, const TDesC& aChannelUrl ) = 0;

			/**
			* Invoked from CIRSongHistoryDbEng when new entry is added to the database which *would exceed 30 entries.
			* @param    aSongName
			* @param    aArtistName
			* @param    aChannelName
			*/
				virtual TInt RemoveSongEntry( const TDesC& aSongName, const TDesC& aArtistName,
						const TDesC& aChannelName, const TDesC& aChannelUrl ) = 0;

				virtual TInt RemoveChannelEntry( const TDesC& aSongName, const TDesC& aArtistName,
							const TDesC& aChannelName, const TDesC& aChannelUrl ) = 0;

			/**
			* Invoked when an error occurred while notifying to UI.
			* @param    aError      The error code.
			*/
				virtual void NotifyDataChangeToUIError( TInt aError ) = 0;

    };

#endif //IR_SONGHISTORYUINOTIFYHANDLER
