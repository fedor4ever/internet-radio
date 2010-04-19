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

#include <e32base.h>

#include <eikenv.h>
#include <f32file.h>


#include "ircodecmanager.h"
#include "irdebug.h"

// ---------------------------------------------------------------------------
// CIRCodecManager::NewL
// ---------------------------------------------------------------------------
//
CIRCodecManager* CIRCodecManager::NewL()
    {
    CIRCodecManager* self = CIRCodecManager::NewLC();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CIRCodecManager::NewLC
// ---------------------------------------------------------------------------
//
CIRCodecManager* CIRCodecManager::NewLC()
    {
    CIRCodecManager* self = new (ELeave) CIRCodecManager();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------------------------
// CIRCodecManager::CIRCodecManager
// ---------------------------------------------------------------------------
//
CIRCodecManager::CIRCodecManager()
    {
    }

// ---------------------------------------------------------------------------
// CIRCodecManager::~CIRCodecManager
// ---------------------------------------------------------------------------
//
CIRCodecManager::~CIRCodecManager()
    {
    iSupportedDataTypes.Reset();
    }

// ---------------------------------------------------------------------------
// CIRCodecManager::ConstructL
// ---------------------------------------------------------------------------
//
void CIRCodecManager::ConstructL()
    {
    IRLOG_DEBUG( "CIRCodecManager::ConstructL" );
    LoadSupportedCodecsL();
    IRLOG_DEBUG( "CIRCodecManager::ConstructL - Exiting." );
    }

// ---------------------------------------------------------------------------
// CIRCodecManager::IsSupportedCodec
// ---------------------------------------------------------------------------
//
TBool CIRCodecManager::IsSupportedCodec( TFourCC aCodec ) const
    {
    IRLOG_DEBUG( "CIRCodecManager::::IsSupportedCodec" );
    TBool supported( EFalse );
    if ( iSupportedDataTypes.Find( aCodec ) != KErrNotFound )
        {
        supported = ETrue;
        }
    IRLOG_INFO3( "CIRCodecManager::IsSupportedCodec - Returning %d for codec %d",
                  supported, aCodec.FourCC() );
    return supported;
    }

// ---------------------------------------------------------------------------
// CIRCodecManager::InitializeComplete
// ---------------------------------------------------------------------------
//
void CIRCodecManager::InitializeComplete(TInt /* aError */)
    {
    }

// ---------------------------------------------------------------------------
// CIRCodecManager::ToneFinished
// ---------------------------------------------------------------------------
//
void CIRCodecManager::ToneFinished(TInt /* aError */)
    {
    }

// ---------------------------------------------------------------------------
// CIRCodecManager::BufferToBeFilled
// ---------------------------------------------------------------------------
//
void CIRCodecManager::BufferToBeFilled(CMMFBuffer* /* aBuffer */ )
    {
    }

// ---------------------------------------------------------------------------
// CIRCodecManager::PlayError
// ---------------------------------------------------------------------------
//
void CIRCodecManager::PlayError(TInt /* aError */ )
    {
    }

// ---------------------------------------------------------------------------
// CIRCodecManager::BufferToBeEmptied
// ---------------------------------------------------------------------------
//
void CIRCodecManager::BufferToBeEmptied(CMMFBuffer* /* aBuffer */ )
    {
    }

// ---------------------------------------------------------------------------
// CIRCodecManager::RecordError
// ---------------------------------------------------------------------------
//
void CIRCodecManager::RecordError(TInt /* aError */ )
    {
    }

// ---------------------------------------------------------------------------
// CIRCodecManager::ConvertError
// ---------------------------------------------------------------------------
//
void CIRCodecManager::ConvertError(TInt /* aError */ )
    {
    }

// ---------------------------------------------------------------------------
// CIRCodecManager::DeviceMessage
// ---------------------------------------------------------------------------
//
void CIRCodecManager::DeviceMessage(TUid /* aMessageType */, const TDesC8& /* aMsg */ )
    {
    }

// ---------------------------------------------------------------------------
// CIRCodecManager::LoadSupportedCodecsL
// ---------------------------------------------------------------------------
//
void CIRCodecManager::LoadSupportedCodecsL()
    {
    IRLOG_DEBUG( "CIRCodecManager::LoadSupportedCodecsL" );
    CMMFDevSound* devSound = CMMFDevSound::NewL();
    CleanupStack::PushL( devSound );
    TMMFPrioritySettings prioritySettings;

    prioritySettings.iPriority = 0;
    prioritySettings.iPref = EMdaPriorityPreferenceNone;
    prioritySettings.iState = EMMFStatePlaying;

    devSound->GetSupportedInputDataTypesL( iSupportedDataTypes, prioritySettings );
    CleanupStack::PopAndDestroy( devSound );

    // CMMFDevSound returns also other codecs than those supported by IR.
    TInt tempCount = iSupportedDataTypes.Count();
    IRRDEBUG2("CIRCodecManager::LoadSupportedCodecsL() - codecs count = %d",tempCount);
    for ( TInt i = iSupportedDataTypes.Count() - 1; i >= 0; i-- )
        {
        if ( !VerifySupportL( iSupportedDataTypes[i] ) )
            {
            IRLOG_DEBUG2( "CIRCodecManager::LoadSupportedCodecsL - %x not supported.",
                        iSupportedDataTypes[i].FourCC() );
            iSupportedDataTypes.Remove( i );
            }
        else
            {
            IRLOG_INFO2( "CIRCodecManager::LoadSupportedCodecsL - %x supported.",
                        iSupportedDataTypes[i].FourCC() );
            }
        }

#ifdef __WINS__
    // In emulator there is typically only 16 bit PCM supported.
    if ( iSupportedDataTypes.Count() == 0 )
        {
        TFourCC codec( KMMFFourCCCodeMP3 );
        iSupportedDataTypes.Append( codec );
        }
#endif


    IRLOG_DEBUG( "CIRCodecManager::LoadSupportedCodecsL - Exiting." );
    }

// ---------------------------------------------------------------------------
// CIRCodecManager::VerifySupportL
// ---------------------------------------------------------------------------
//
TBool CIRCodecManager::VerifySupportL( TFourCC aCodec )
    {
    IRLOG_DEBUG2( "CIRCodecManager::VefifySupportL - aCodec=%x", aCodec.FourCC() );
    TInt supported( EFalse );
    switch( aCodec.FourCC() )
        {
#ifndef __SERIES60_30__
        case KMMFFourCCCodeWMA: // fall through
#endif
        case KMMFFourCCCodeAAC: // fall through
        case KMMFFourCCCodeMP3:
            supported = ETrue;
            break;
        default:
            break;
        }
    if ( supported )
        {
        // Test initialisation to see that the codec really is supported.
        CMMFDevSound* devSound = CMMFDevSound::NewL();
        CleanupStack::PushL( devSound );

        TMMFState mmfState( EMMFStatePlaying );
        TRAPD( err, devSound->InitializeL( *this, aCodec, mmfState ) );
        if ( err != KErrNone )
            {
            supported = EFalse;
            }
        CleanupStack::PopAndDestroy( devSound );
        }
    IRLOG_DEBUG( "CIRCodecManager::VefifySupportL" );
    return supported;
    }


