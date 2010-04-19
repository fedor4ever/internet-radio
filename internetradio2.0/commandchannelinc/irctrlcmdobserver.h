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
* Description:  ?Description
*
*/


#ifndef IRCTRLCMDOBSERVER_H
#define IRCTRLCMDOBSERVER_H

#include <e32base.h>

#include "ircontrolparams.h"

class MIRCtrlCmdObserver;
class MIRStreamFill;

/**
 * This is the class is implemented to provide completed independence to 
 * mediaengine from client as well as network.It is through this class media 
 * engine interacts with client and network. This also provides additional 
 * features like audio fade in, audio fade out effect. This design is has to 
 * provide flexiblilty to irmediaengine by avoiding any other dll interaction
 * other than ircommandchannel.dll
 *
 * @code
 *
 * Observer* observer; //Observer class should be derived from MIRCtrlCmdObserver
 * StreamFill* stream; //Streamfill class should be derived from MIRStreamFill
 * CIRCtrlCmdObserver* channel = CIRCtrlCmdObserver::NewL(observer,stream);   
 * //creates instance of ircommandchannel
 * TIRControlCommmand command = EError;
 * //senting an error value and is defined in IRControlParams.h
 * TInt some_integer_value;
 * TPtr ptrbuffer;
 * TUint8* ptr = new (ELeave) TUint8[100];
 * ptrbuffer.Set(ptr,100,100);
 * channel->SentRequest(command,some_integer_value); 
 * //sending a command to channel
 * channel->FilltheBuffer(ptrbuffer); //call for filling the buffer
 * TInt volume = channel->FetchVolume(); 
 * //gets current volume (which can stored in database)
 * command = EBufferFadeIn; //calling for a fade in effect
 * channel->AudioFadeInOut(command,5);
 * channel->DoAudioFadeOut(); //function for fade out effect
 * delete channel;
 *
 * @endcode
 * 
 * @lib ircommandchannel.lib
 * @since S_60 3.0
 *
 */
 
NONSHARABLE_CLASS(CIRCtrlCmdObserver) :  public CBase
    {
public:                                //member function
    /**
     * Function : NewL
     * Function returns an instance of CIRCtrlCmdObserver
     * Two phase constructor
     * @param MIRCtrlCmdObserver observers instance,pointer to MIRStreamFill
     * @return instance of CIRCtrlCmdObserver
     */
    IMPORT_C static CIRCtrlCmdObserver* NewL(MIRCtrlCmdObserver* aObserver,
		MIRStreamFill* aStream);    

    /**
     * Function : NewLC
     * Function returns an instance of CIRCtrlCmdObserver
     * Two phase constructor
     * @param MIRCtrlCmdObserver observers instance,pointer to MIRStreamFill
     * @return instance of CIRCtrlCmdObserver
     */
    IMPORT_C static CIRCtrlCmdObserver* NewLC(MIRCtrlCmdObserver* aObserver,
		MIRStreamFill* aStream);
    
    /**
     * Function : ~CIRCtrlCmdObserver
     * Default destructor calls Cancel function which cancel the active request 
     */
    ~CIRCtrlCmdObserver();

   /**
     * Function : SentRequest
     * Function sends request to handle events represented by 
     * TIRControlCommmand
     * @param TIRControlCommmand represents what action to be taken
     * @param aValue represents any integer value if any, like buffering 
     * percentage
     */
    IMPORT_C void SentRequest(TIRControlCommmand aCommand,TInt aValue);

    /**
     * Function : FilltheBuffer
     * Function sends request to fill the buffer
     * @param buffer to be filled with data
     */
    IMPORT_C void FilltheBuffer(TDes8& aInputBuffer);

    /**
     * Function : FetchVolume
     * Function sends request to get the current volume index
     * @return volume index
     */
    IMPORT_C TInt FetchVolume();

        /**
     * Function : AudioFadeInOut
     * Function is called for audio Fade In Fade Out Effects
     * @param TIRControlCommmand represents what action to be taken Fade 
     * In or Fade Out
     * @param aValue represents any integer value of cuurent volume level
     */
    IMPORT_C void AudioFadeInOut(TIRControlCommmand& aCommand,TInt aValue);
    
    /**
     * Function : DoAudioFadeOut
     * Function is used to sent request start audio Fade Out
     */
    IMPORT_C void DoAudioFadeOut();
    
private:
    /**
     * Function : ConstructL
     * Function does all the initializations
     * Two phase constructor
     * @param MIRCtrlCmdObserver observers instance,pointer to MIRStreamFill
     */
    void ConstructL(MIRCtrlCmdObserver* aObserver,MIRStreamFill* aStream);

    /**
     * Function : CIRCtrlCmdObserver
     * Function is the default constructor, sets the CActive priority, 
     * Line status
     */
    CIRCtrlCmdObserver();
    
    /**
     * Function : StartAudioFadeIn
     * Function Starts audio Fade In effects
     * @param take a parameter of CIRCtrlCmdObserver into a TAny pointer
     * @return KErrNone upon completion
     */
    static TInt StartAudioFadeIn(TAny* aPtr);
    
    /**
     * Function : StartAudioFadeOut
     * Function Starts audio Fade Out effects
     * @param take a parameter of CIRCtrlCmdObserver into a TAny pointer
     * @return KErrNone upon completion
     */
    static TInt StartAudioFadeOut(TAny* aPtr);

    /**
     * Function : UpdateVolumeInc
     * Function does the Fade In effect
     */
    void UpdateVolumeInc();
    
    /**
     * Function : UpdateVolumeDec
     * Function does the Fade In effect 
     */
    void UpdateVolumeDec();


private:                                //data members

    //This is instance of MIRCtrlCmdObserver 
    MIRCtrlCmdObserver* iObserver;
    
    //Access to fill data
    MIRStreamFill* iStream;

    //Volume timer
    CPeriodic* iVolumeTimer;

    //current volume level
    TInt iVolumeLevel;

    //actual volume level
    TInt iPersistentVolume;
    
    //instance of the command
    TIRControlCommmand iCommand;
    };
    
#endif//IRCTRLCMDOBSERVER_H

