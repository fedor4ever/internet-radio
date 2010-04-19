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

#include "irtestingaudioplayer.h"
#include "irmp3player.h"

// Constants

const TInt KTWOMILLION = 2000000 ;

CIRTestingAudioPlayer::CIRTestingAudioPlayer(CIRMP3Player& aMP3Player):
			 CActive(CActive::EPriorityStandard),iMP3Player(aMP3Player)
{
		iCounter=0;
}

CIRTestingAudioPlayer::~CIRTestingAudioPlayer()
{
	// Make sure we're cancelled
	Cancel();
	iTimer.Close();

}

CIRTestingAudioPlayer* CIRTestingAudioPlayer::NewL(CIRMP3Player& aMP3Player)
{
	CIRTestingAudioPlayer* self = new (ELeave) CIRTestingAudioPlayer(aMP3Player);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop( self );
	return self;
}
void CIRTestingAudioPlayer::ConstructL()
{
	iTimer.CreateLocal();
}

void CIRTestingAudioPlayer::IssueRequest()
{

	// Request another wait
  	iStatus = KRequestPending;
    iTimer.After( iStatus, KTWOMILLION );
    //console->Printf(_L("\n Set Active called after iTimer()"))
    SetActive();
    //console->Printf(_L("\n Leaving Issue Request"))
}

void CIRTestingAudioPlayer::DoCancel()
{
	iTimer.Cancel(); // Close the ASP
}

void CIRTestingAudioPlayer::RunL()
	{

	switch(iAudioFuncType)
		{
		case EOpen:
			{
			iMP3Player.MaoscOpenComplete(0);
			}
		break;
		case EWriteLBufferCopied:
			{
			TBuf8<2> dummy;
			iMP3Player.MaoscBufferCopied(0,dummy);
			}
		break;
		case EWriteLPlayComplete:
			{
			iMP3Player.MaoscOpenComplete(0);
			}
			break;
		default:
			{

			}
		}
	}

void CIRTestingAudioPlayer::Open()
	{
	iAudioFuncType = EOpen;
	IssueRequest();
	}
void CIRTestingAudioPlayer::Write()
	{
	iAudioFuncType = EWriteLBufferCopied;
	IssueRequest();
	}
void CIRTestingAudioPlayer::Stop()
	{
	iTimer.Cancel();
	}
void CIRTestingAudioPlayer::Close()
	{
	// do nothing
	}
