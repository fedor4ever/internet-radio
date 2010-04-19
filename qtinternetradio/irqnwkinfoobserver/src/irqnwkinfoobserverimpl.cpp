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
#include "irqnwkinfoobserverimpl.h"
#include "irqnwkobserverinterface.h"


// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
IRQNwkInfoObserverImpl* IRQNwkInfoObserverImpl::NewL( MIRQNwkInfoUpdate *aObserver)
{
    IRQNwkInfoObserverImpl* self = new ( ELeave ) IRQNwkInfoObserverImpl( aObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
}

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
IRQNwkInfoObserverImpl::IRQNwkInfoObserverImpl(MIRQNwkInfoUpdate *aObserver) : CActive(EPriorityLow),
                                                 iTelephony(NULL),
                                                 iCurrentNetworkInfoV1Pckg(iCurrentNetworkInfo),
                                                 iHomeOperatorInfoPckg(iHomeOperatorInfo),
                                                 iFirstTime(true),
                                                 iCurrentNetworkType(true),
                                                 iHomeNetworkType(false),
                                                 iObserver(aObserver)
{
}

// ---------------------------------------------------------------------------
// Second-phase constructor.
// ---------------------------------------------------------------------------
//
void IRQNwkInfoObserverImpl::ConstructL()
{
    iTelephony = CTelephony::NewL();
    CActiveScheduler::Add(this);
}

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
IRQNwkInfoObserverImpl::~IRQNwkInfoObserverImpl()
{
    Cancel();
    delete iTelephony;
    iTelephony = NULL;
}

// ---------------------------------------------------------------------------
// Function : IRQNwkInfoObserverImpl::startNwkInfoMonitor()
// Function to start network monitoring
// ---------------------------------------------------------------------------
//

void IRQNwkInfoObserverImpl::startNwkInfoMonitor()
{
    if(!IsActive() && iTelephony)
    {
        //get the current network info for the first time
        iCurrentNetworkType = true;
        iHomeNetworkType = false;
        iTelephony->GetCurrentNetworkInfo(iStatus, iCurrentNetworkInfoV1Pckg);      
        SetActive(); 
    }  
}

// ---------------------------------------------------------------------------
// Function : IRQNwkInfoObserverImpl::issueRequest()
// Issue Subsequent network monitoring request
// ---------------------------------------------------------------------------
//

void IRQNwkInfoObserverImpl::issueRequest()
{
    if(iCurrentNetworkType)
    {
        if(!IsActive() && iTelephony)
        {
            //notify if current operator changes
            iTelephony->NotifyChange(iStatus,
                    CTelephony::ECurrentNetworkInfoChange,
                    iCurrentNetworkInfoV1Pckg);
            SetActive();
        }      
    }
    
    if(iHomeNetworkType)
    {
        if(!IsActive() && iTelephony)
        {
            //notify if home operator changes
            iTelephony->NotifyChange(iStatus,
                      CTelephony::ENetworkRegistrationStatusChange,
                      iHomeOperatorInfoPckg);
            SetActive();
        }  
    }
}


// ---------------------------------------------------------------------------
// From CActive.
// ---------------------------------------------------------------------------
//
void IRQNwkInfoObserverImpl::DoCancel()
{
    if(IsActive() && iTelephony)
    {
        //cancel the out standing requests if any
        iTelephony->CancelAsync(CTelephony::ECurrentNetworkInfoChangeCancel);
        iTelephony->CancelAsync(CTelephony::ENetworkRegistrationStatusChangeCancel);  
        iTelephony->CancelAsync(CTelephony::EGetCurrentNetworkInfoCancel);
        iTelephony->CancelAsync(CTelephony::EGetNetworkRegistrationStatusCancel);
    }
}

// ---------------------------------------------------------------------------
// From CActive.
// ---------------------------------------------------------------------------
//
void IRQNwkInfoObserverImpl::RunL()
{  
    if(KErrNone == iStatus.Int())
    {    
        if(iCurrentNetworkType)
        {
            //if the request was to find the current network changes
            //then the value is updated
          #ifdef __WINS__
            iCurrentNwkMCC = "0";
            iCurrentNwkMNC = "0";  
          #else
            iCurrentNwkMCC = QString::fromUtf16( iCurrentNetworkInfo.iCountryCode.Ptr(),
                                            iCurrentNetworkInfo.iCountryCode.Length());
            iCurrentNwkMNC = QString::fromUtf16( iCurrentNetworkInfo.iNetworkId.Ptr(),
                                            iCurrentNetworkInfo.iNetworkId.Length());
          #endif
            //updating mcc+mnc value
            if(iObserver)
            {
                iObserver->updateCurrentNwkInfo(iCurrentNwkMCC,iCurrentNwkMNC);
            }
            
            iHomeNetworkType = ETrue;
            iCurrentNetworkType = EFalse;
            if(iFirstTime && iTelephony)
            {
                //if first time we have check the current operator is home operator
                iTelephony->GetNetworkRegistrationStatus(iStatus,iHomeOperatorInfoPckg);      
                SetActive();
                iFirstTime = EFalse;
            }
            else
            {
                //else issue request is called for notifying home operator change
                issueRequest();  
            }      
        }
        else if(iHomeNetworkType)
        {
            //home operator (or the change in home operator is notified. 
          #ifdef __WINS__
            iHomeNetworkMCC = "0";
            iHomeNetworkMNC = "0";
            if(iObserver)
            {            
                iObserver->updateHomeNwkInfo(iHomeNetworkMCC,iHomeNetworkMNC);
            }
          #else
            //if registration status shows that user is registered in home network
            //and not in roaming status, the current operator is updated
            if(CTelephony::ERegisteredOnHomeNetwork == iHomeOperatorInfo.iRegStatus)
            {
                iHomeNetworkMCC = iCurrentNwkMCC;
                iHomeNetworkMNC = iCurrentNwkMNC;
                if(iObserver)
                {                    
                    iObserver->updateHomeNwkInfo(iHomeNetworkMCC,iHomeNetworkMNC);
                }
            }
          #endif
                //issue request to find the current network change
                iHomeNetworkType = EFalse;
                iCurrentNetworkType = ETrue;
                issueRequest();

        }    
        else
        {
            return;  
        }
    }
    //if any error network is updated to zero    
    else
    {
        iCurrentNwkMCC = "0";
        iCurrentNwkMNC = "0";  
        iHomeNetworkMCC = "0";
        iHomeNetworkMNC = "0";
        if(iObserver)
        {
            iObserver->updateCurrentNwkInfo(iCurrentNwkMCC,iCurrentNwkMNC);
            iObserver->updateHomeNwkInfo(iHomeNetworkMCC,iHomeNetworkMNC);
        }
    }
}
