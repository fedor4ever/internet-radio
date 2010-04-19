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

#ifndef IRTESTINGAUDIOPLAYER_H
#define IRTESTINGAUDIOPLAYER_H


#include <e32base.h>

enum TAudioFunctionType
	{
	EOpen=0,
	EWriteLBufferCopied,
	EWriteLPlayComplete
	};

class CIRMP3Player;


class CIRTestingAudioPlayer : public CActive
	{
public:
									//Member Functions
	public:
        static CIRTestingAudioPlayer* NewL(CIRMP3Player& aMP3Player);
        
        void ConstructL();
        
        ~CIRTestingAudioPlayer();     // Destruction
        
        void IssueRequest();  // issue request
       
        void DoCancel();       // Cancel request
        // Defined as pure virtual by CActive
        // implementation provided by this class.
       
        void RunL();     // service completed request.
        // Defined as pure virtual by CActive
        // implementation provided by this class.
  
  		void Open();
		void Write();
		void Stop();
		void Close();
        
    private:
        
        RTimer iTimer;      // Asynchronous service provider.
        
        TInt iCounter;
        
		CIRTestingAudioPlayer(CIRMP3Player& aMP3Player);        // Construction
		
	public:
	
		TAudioFunctionType iAudioFuncType;
		
		CIRMP3Player& iMP3Player;
	};  

#endif//IRTESTINGAUDIOPLAYER_H
