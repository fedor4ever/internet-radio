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


#include "irdebug.h"
#include "irnwinfoobserver.h"
#include "irui.h"

// ---------------------------------------------------------------------------
// Function : CIRNwInfoObserver
// Function is the default constructor, sets the CActive priority
// ---------------------------------------------------------------------------
//
CIRNwInfoObserver::CIRNwInfoObserver():CActive (EPriorityLow),
				iCurrentNetworkInfoV1Pckg(iCurrentNetworkInfo),
      			iHomeOperatorInfoPckg(iHomeOperatorInfo)   
	{
	IRLOG_DEBUG( "CIRNwInfoObserver::CIRNwInfoObserver" );
	//no implementation
	}

// ---------------------------------------------------------------------------
// Function : ConstructL
// Function does all the initializations
// Two phase constructor
// ---------------------------------------------------------------------------
//
void CIRNwInfoObserver::ConstructL(CIRUi* aUi)
	{
	IRLOG_DEBUG( "CIRNwInfoObserver::ConstructL - Entering" );
	iUi = aUi;
	iNwInfoObserver = CTelephony::NewL();
	CActiveScheduler::Add(this);
	iCurrentNetwork = ETrue;
    iHomeNetworkType = EFalse;
	iFirstTime = ETrue;
	IRLOG_DEBUG( "CIRNwInfoObserver::ConstructL - Exiting." );
    }

// ---------------------------------------------------------------------------
// Function : NewL
// Function returns an instance of CIRNwInfoObserver
// Two phase constructor
// ---------------------------------------------------------------------------
//
CIRNwInfoObserver* CIRNwInfoObserver::NewL(CIRUi* aUi)
	{
	IRLOG_DEBUG( "CIRNwInfoObserver::NewL - Entering" );
	CIRNwInfoObserver* self = CIRNwInfoObserver::NewLC(aUi);
	CleanupStack::Pop(self);
	IRLOG_DEBUG( "CIRNwInfoObserver::NewL - Exiting." );
	return self;
	}

// ---------------------------------------------------------------------------
// Function : NewLC
// Function returns an instance of CIRNwInfoObserver
// Two phase constructor
// ---------------------------------------------------------------------------
//
CIRNwInfoObserver* CIRNwInfoObserver::NewLC(CIRUi* aUi)
	{
	IRLOG_DEBUG( "CIRNwInfoObserver::NewLC - Entering" );
	CIRNwInfoObserver* self = new (ELeave) CIRNwInfoObserver;
	CleanupStack::PushL(self);
	self->ConstructL(aUi);
	IRLOG_DEBUG( "CIRNwInfoObserver::NewLC - Exiting." );
	return self;
	}

// ---------------------------------------------------------------------------
// Function : ~CIRNwInfoObserver
// Default destructor calls Cancel function which cancel the active request 
// ---------------------------------------------------------------------------
//

CIRNwInfoObserver::~CIRNwInfoObserver()
	{
	IRLOG_DEBUG( "CIRNwInfoObserver::~CIRNwInfoObserver - Entering" );
	Cancel();
	delete iNwInfoObserver;
	IRLOG_DEBUG( "CIRNwInfoObserver::~CIRNwInfoObserver - Exiting" );
	}

// ---------------------------------------------------------------------------
// Function : IRStartNwMonitor
// Function to start network monitoring
// ---------------------------------------------------------------------------
//

void CIRNwInfoObserver::IRStartNwMonitor()
	{
	IRLOG_DEBUG( "CIRNwInfoObserver::IRStartNwMonitor - Entering" );
	if( !IsActive() )
		{
		//get the network status for the first time
		iNwInfoObserver->GetCurrentNetworkInfo(iStatus,
			iCurrentNetworkInfoV1Pckg);			
		SetActive(); 
		}		 
	
	IRLOG_DEBUG( "CIRNwInfoObserver::IRStartNwMonitor - Exiting." );
	}

// ---------------------------------------------------------------------------
// Function : IssueRequest
// Issue request
// ---------------------------------------------------------------------------
//

void CIRNwInfoObserver::IssueRequest()
	{
	IRLOG_DEBUG( "CIRNwInfoObserver::IssueRequest - Entering" );
	if( iCurrentNetwork )
		{
		if( !IsActive() )
			{
			//notify if current operator changes
			iNwInfoObserver->NotifyChange(iStatus,
				CTelephony::ECurrentNetworkInfoChange,
								iCurrentNetworkInfoV1Pckg);
			SetActive();
			}			
		}	
	 if( iHomeNetworkType )
		{
		if( !IsActive() )
			{
			//notify if home operator changes
			iNwInfoObserver->NotifyChange(iStatus,
				CTelephony::ENetworkRegistrationStatusChange,
									iHomeOperatorInfoPckg);
			SetActive();
			}	
		}
	IRLOG_DEBUG( "CIRNwInfoObserver::IssueRequest - Exiting." );
	}


// ---------------------------------------------------------------------------
// Function : DoCancel
// Function stops getting the battery info if it is active
// ---------------------------------------------------------------------------
//
void CIRNwInfoObserver::DoCancel()
	{
	IRLOG_DEBUG( "CIRNwInfoObserver::DoCancel - Entering" );
	if( IsActive() )
		{
		//cancel the out standing requests if any
		iNwInfoObserver->CancelAsync(CTelephony::ECurrentNetworkInfoChangeCancel);
		iNwInfoObserver->CancelAsync(CTelephony::ENetworkRegistrationStatusChangeCancel);	
		iNwInfoObserver->CancelAsync(CTelephony::EGetCurrentNetworkInfoCancel);
		iNwInfoObserver->CancelAsync(CTelephony::EGetNetworkRegistrationStatusCancel);
		}	
	IRLOG_DEBUG( "CIRNwInfoObserver::DoCancel - Exiting" );
	}

// ---------------------------------------------------------------------------
// Function : RunL
// RunL of the nwinfoobserver
// ---------------------------------------------------------------------------
//
void CIRNwInfoObserver::RunL()
	{	
	IRLOG_DEBUG( "CIRNwInfoObserver::RunL - Entering" );
	if(KErrNone == iStatus.Int())
		{		
		if( iCurrentNetwork )
			{
			//if the request was to find the current network
			//(changes) then the value is updated
			#ifdef _WINS_
				iCurrentNetworkMCC.Zero();
				iCurrentNetworkMNC.Zero();
				iUi->UpdateCurrentNetwork
					(iCurrentNetworkMCC,iCurrentNetworkMNC);		
			#else
				iCurrentNetworkMCC = iCurrentNetworkInfo.iCountryCode;
				iCurrentNetworkMNC = iCurrentNetworkInfo.iNetworkId;
				//updating mcc+mnc value
				iUi->UpdateCurrentNetwork
					(iCurrentNetworkMCC,iCurrentNetworkMNC);
			#endif
			
			iHomeNetworkType = ETrue;
			iCurrentNetwork = EFalse;
			if( iFirstTime )
				{
				//if first time we have check the 
				//current operator is home operator
				iNwInfoObserver->GetNetworkRegistrationStatus
					(iStatus,iHomeOperatorInfoPckg);			
				SetActive();
				iFirstTime = EFalse;	
				}
			else
				{
				//else issue request is called for notifying
				//home operator change
				IssueRequest();	
				}			
			}
		else if( iHomeNetworkType )
			{
			//home operator (or the change in home operator is 
			//notified. 
			#ifdef _WINS_
				iHomeNetworkMCC.Zero();
				iHomeNetworkMNC.Zero();
				iUi->UpdateHomeNetwork(iHomeNetworkMCC,iHomeNetworkMNC);
			#else
				//if operator is not same as home operator it is not updated
				if (CTelephony::ERegisteredOnHomeNetwork 
					== iHomeOperatorInfo.iRegStatus)
					{
					//if registration status shows that user is registered in 
					//home network and not in roaming status, the current operator
					//is updated
					iHomeNetworkMCC = iCurrentNetworkMCC;
					iHomeNetworkMNC = iCurrentNetworkMNC;
					iUi->UpdateHomeNetwork(iHomeNetworkMCC,iHomeNetworkMNC);
					}
			#endif
			//issue request to find the current network change
			iHomeNetworkType = EFalse;
			iCurrentNetwork = ETrue;
			IssueRequest();
			}		
		else
			{
			return;	
			}		
		}
	else
		{
		//if any error network is updated to zero
		iCurrentNetworkMCC.Zero();
		iCurrentNetworkMNC.Zero();
		iUi->UpdateCurrentNetwork(iCurrentNetworkMCC,iCurrentNetworkMNC);
		iHomeNetworkMCC.Zero();
		iHomeNetworkMNC.Zero();
		iUi->UpdateHomeNetwork(iHomeNetworkMCC,iHomeNetworkMNC);
		}
	IRLOG_DEBUG( "CIRNwInfoObserver::RunL - Exiting." );
	return;
	}
	
// ---------------------------------------------------------------------------
// Function : RunError
// Function which handles RunL error
// ---------------------------------------------------------------------------
//
TInt CIRNwInfoObserver::RunError(TInt aError)
	{
	IRLOG_DEBUG( "CIRNwInfoObserver::RunError" );
	return aError;		
	}	
	

