/*
* Copyright (c) 2007-2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Station information data container
*
*/


#include <stringloader.h>
#include <internetradio.rsg>

#include "irstationinformationdata.h"

// ======== LOCAL FUNCTIONS ========

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Static two-phased constructor.
// ---------------------------------------------------------------------------
//
CIRStationInformationData* CIRStationInformationData::NewL( CCoeEnv& aCoeEnv )
    {
    CIRStationInformationData* self = new ( ELeave ) CIRStationInformationData( aCoeEnv );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }
    
// ---------------------------------------------------------------------------
// Constructor.
// ---------------------------------------------------------------------------
//
CIRStationInformationData::CIRStationInformationData( CCoeEnv& /*aCoeEnv*/ )
    //: iCoeEnv( aCoeEnv )
    {
    }
    
// ---------------------------------------------------------------------------
// Second-phase constructor.
// ---------------------------------------------------------------------------
//
void CIRStationInformationData::ConstructL()
    {
    iName.CreateL( KNullDesC );
    iRdsPsName.CreateL( KNullDesC );
    iIndex.CreateL( KNullDesC );
    
    }
    
// ---------------------------------------------------------------------------
// Destructor.
// ---------------------------------------------------------------------------
//
CIRStationInformationData::~CIRStationInformationData()
    {
    

    iName.Close();
    iRdsPsName.Close();
    iIndex.Close();

    }

    
// ---------------------------------------------------------------------------
// Sets the name.
// ---------------------------------------------------------------------------
//
void CIRStationInformationData::SetNameL( const TDesC& aName )
    {
    iName.Close();
    iName.CreateL( aName );
    }
    
// ---------------------------------------------------------------------------
// Sets the RDS PS name.
// ---------------------------------------------------------------------------
//
void CIRStationInformationData::SetRdsPsNameL( const TDesC& aRdsPsName )
    {
    iRdsPsName.Close();
    iRdsPsName.CreateL( aRdsPsName );
    }
    
// ---------------------------------------------------------------------------
// Sets the index.
// ---------------------------------------------------------------------------
//
void CIRStationInformationData::SetIndexL( TInt aIndex )
    {
    if ( aIndex >= 0 )
        {
        HBufC* index = StringLoader::LoadLC( R_IRAPP_STATIONS_NUMBERING, aIndex + 1);
        CleanupStack::Pop( index );
        
        iIndex.Close();
        iIndex.Assign( index );
        }
    }
    

// ---------------------------------------------------------------------------
// Sets the visual service enabled or disabled.
// ---------------------------------------------------------------------------
//
void CIRStationInformationData::SetServiceEnabled( TBool aServiceEnabled )
    {
    iServiceEnabled = aServiceEnabled;
    }

// ---------------------------------------------------------------------------
// Sets the display style in use.
// ---------------------------------------------------------------------------
//
void CIRStationInformationData::SetDisplayStyle( TIRDisplayStyle aDisplayStyle )
    {
    iDisplayStyle = aDisplayStyle;
    }
    
// ---------------------------------------------------------------------------
// Sets the fade directionality.
// ---------------------------------------------------------------------------
//
void CIRStationInformationData::SetFadeDirectionality( TIRFadeStyle aFadeDirectionality )
    {
    iFadeDirectionality = aFadeDirectionality;
    }

// ---------------------------------------------------------------------------
// Returns the frequency as a descriptor.
// ---------------------------------------------------------------------------
//
const TDesC& CIRStationInformationData::Frequency() const
    {
    return iFrequency;
    }
    
// ---------------------------------------------------------------------------
// Returns the name.
// ---------------------------------------------------------------------------
//
const TDesC& CIRStationInformationData::Name() const
    {
    return iName;
    }
    
// ---------------------------------------------------------------------------
// Returns the RDS PS name.
// ---------------------------------------------------------------------------
//
const TDesC& CIRStationInformationData::RdsPsName() const
    {
    return iRdsPsName;
    }
    
// ---------------------------------------------------------------------------
// Returns the index as a descriptor.
// ---------------------------------------------------------------------------
//
const TDesC& CIRStationInformationData::Index() const
    {
    return iIndex;
    }
    
// ---------------------------------------------------------------------------
// Returns the location.
// ---------------------------------------------------------------------------
//
const TDesC& CIRStationInformationData::Location() const
    {
    return iLocation;
    }

// ---------------------------------------------------------------------------
// Returns whether or not visual service is enabled.
// ---------------------------------------------------------------------------
//
TBool CIRStationInformationData::ServiceEnabled() const
    {
    return iServiceEnabled;
    }

// ---------------------------------------------------------------------------
// Returns the display style.
// ---------------------------------------------------------------------------
//
TIRDisplayStyle CIRStationInformationData::DisplayStyle() const
    {
    return iDisplayStyle;
    }

// ---------------------------------------------------------------------------
// Returns the fade directionality.
// ---------------------------------------------------------------------------
//
TIRFadeStyle CIRStationInformationData::FadeDirectionality() const
    {
    return iFadeDirectionality;
    }

