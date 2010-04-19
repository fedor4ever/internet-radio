/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Maintains the set of supported codecs 
*
*/


#ifndef C_IRCODECMANAGER_H
#define C_IRCODECMANAGER_H

#include <sounddevice.h>

/**
 *  Maintains the list of supported codecs in a terminal.
 * 
 *
 */
NONSHARABLE_CLASS( CIRCodecManager ): public CBase, public MDevSoundObserver
    {
public:
    /**
     * Two-phased constructor.
     * @return Instance of CIRCodecManager
     */
    static CIRCodecManager* NewL();

    /**
     * Two-phased constructor.
     * @return Instance of CIRCodecManager
     */
    static CIRCodecManager* NewLC();
    
    /**
     * Destructor.
     */
    ~CIRCodecManager();

    /**
     * Tells if the codec provided as argument is supported by the terminal 
     * and by Internet Radio.
     * 
     * @param aCodec The codec that's support is tested.
     * @return ETrue if the codec is supported, EFalse if not.
     */
    TBool IsSupportedCodec( TFourCC aCodec ) const;
    
private:
    /**
     * C++ Default constructor.
     */
    CIRCodecManager();

    /**
     * Second phase constructor
     */
    void ConstructL();

private: // From MDevSoundObserver
    /**
     * @see MDevSoundObserver::InitializeComplete
     */
    void InitializeComplete(TInt aError);
    
    /**
     * @see MDevSoundObserver::ToneFinished
     */
    void ToneFinished(TInt aError);
    
    /**
     * @see MDevSoundObserver::BufferToBeFilled
     */
    void BufferToBeFilled(CMMFBuffer* aBuffer);

    /**
     * @see MDevSoundObserver::PlayError
     */
    void PlayError(TInt aError);

    /**
     * @see MDevSoundObserver::BufferToBeEmptied
     */
    void BufferToBeEmptied(CMMFBuffer* aBuffer);

    /**
     * @see MDevSoundObserver::RecordError
     */
    void RecordError(TInt aError);

    /**
     * @see MDevSoundObserver::ConvertError
     */
    void ConvertError(TInt aError);

    /**
     * @see MDevSoundObserver::DeviceMessage
     */
    void DeviceMessage(TUid aMessageType, const TDesC8& aMsg);

private:
    /**
     * Initialises the set of supported codecs.
     */
    void LoadSupportedCodecsL();

    /**
     * Checks that the codec is in the set of Internet Radio's supported codecs
     * and verifies that DevSound can be initialised with the codec.
     * 
     * @param aCodec The codec to be tested.
     * @return ETrue if the codec is really supported, EFalse otherwise.
     */
    TBool VerifySupportL( TFourCC aCodec );

private: 
    /**
     * The set of codecs that are supported.
     * Owned.
     */
    RArray<TFourCC> iSupportedDataTypes;
    
    };
#endif // C_IRCODECMANAGER_H
