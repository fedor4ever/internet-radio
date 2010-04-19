/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Source file for IRControlEventObserver.
*
*/


#include "ircontroleventobserver.h"
#include "irdebug.h"
#include "irpubsubkeys.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CIRControlEventObserver::CIRControlEventObserver(CIRUi& aObserver, const TUid& aCategory,
 const TUint aKey, const RProperty::TType aPropertyType)
    : CActive( CActive::EPriorityStandard ),
    iObserver( aObserver ),
    iCategory( aCategory ),
    iKey( aKey ),
    iPropertyType( aPropertyType )
	{
	}

// -----------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CIRControlEventObserver::ConstructL()
	{
    User::LeaveIfError( iProperty.Attach( iCategory, iKey ) );
    CActiveScheduler::Add( this );
   
	}

// -----------------------------------------------------------------------------
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CIRControlEventObserver* CIRControlEventObserver::NewL(CIRUi& aObserver,
	 const TUid& aCategory,
 	const TUint aKey, const RProperty::TType aPropertyType)
	{
    CIRControlEventObserver* self = new( ELeave )CIRControlEventObserver( aObserver, aCategory, 
    									aKey, aPropertyType );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
	}

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
CIRControlEventObserver::~CIRControlEventObserver()
	{
	Cancel();
    iProperty.Close();
   	}

// ---------------------------------------------------------------------------
// Subscribes to a property and reads the value, if not already active.
// ---------------------------------------------------------------------------
//
EXPORT_C void CIRControlEventObserver::SecondConstructL()
	{
    if ( !IsActive() )
        {
         iProperty.Subscribe( iStatus );
         SetActive();
        }
	}

// -----------------------------------------------------------------------------
// CIRControlEventObserver::RunL
// -----------------------------------------------------------------------------
//
void CIRControlEventObserver::RunL()
	{
	IRRDEBUG2("CIRControlEventObserver:RunL entering",KNullDesC);
    iProperty.Subscribe( iStatus );
    SetActive();
    TInt err(KErrNone);
    
	IRRDEBUG2("CIRControlEventObserver:RunL iKey = %d",iKey);
	
	if(iKey == KIRPSControlSetPlayState)
	{
	IRRDEBUG2("CIRControlEventObserver:RunL KIRPSControlSetPlayState = %d",iKey);
	
	}

    switch (iPropertyType)
        {
        // Subscribes the property of interger type 
        case RProperty::EInt:
	        {
	        err = iProperty.Get( iValue );
	        IRRDEBUG2("CIRControlEventObserver:RunL Error in get of property = %d",err);

	        if (!err)
            	{
            	if(iKey == KIRPSControlSetPlayState)
	            	{
	            	iObserver.PlayerStateChangedL(static_cast<TIRStopPlayState> (iValue));
		           	}
                
             	}
	        break;
	        }
	    // Subscribes the property of text type 
	    case RProperty::ELargeText:
	        {
	        break;
	        }
	    default:
	        {
	        break;
	        }
        }
    
    if (err)
    	{
        //iObserver.HandlePropertyChangeErrorL(iCategory, iKey, err)
    	}
	IRRDEBUG2("CIRControlEventObserver:RunL exiting",KNullDesC);
	
    }

// -----------------------------------------------------------------------------
// Cancels an outstanding active request
// -----------------------------------------------------------------------------
//
void CIRControlEventObserver::DoCancel()
	{
    iProperty.Cancel();
	}


