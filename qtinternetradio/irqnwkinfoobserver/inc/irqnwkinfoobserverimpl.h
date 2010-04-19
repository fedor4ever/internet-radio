/*
* Copyright (c) 2009-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  class declaration for monitoring current network info change
*
*/


#ifndef IRQNWKINFOOBSERVERIMPL_H_
#define IRQNWKINFOOBSERVERIMPL_H_


#include <e32base.h> // CActive
#include <etel3rdparty.h> // CTelephony
#include <QString>

class MIRQNwkInfoUpdate;

class IRQNwkInfoObserverImpl : public CActive
{
public:
    static IRQNwkInfoObserverImpl* NewL(MIRQNwkInfoUpdate *aObserver);

    /*!
     * Destructor.
     */
    ~IRQNwkInfoObserverImpl();
    
    /*!
     * starts network info monitoring
     */
    void startNwkInfoMonitor();    
        
private:
    /*!
     * Constructor
     */
    explicit IRQNwkInfoObserverImpl(MIRQNwkInfoUpdate *aObserver);
    
    /**
     * Second-phase constructor.
     */
    void ConstructL();

private:
  
    /*!
     * Issue Subsequent network monitoring request
     */
    void issueRequest();

    /*!
     * From CActive.
     */
    void DoCancel();
    void RunL();
    
private:  

    CTelephony* iTelephony;
    
    //Current Network
    CTelephony::TNetworkInfoV1 iCurrentNetworkInfo;
    CTelephony::TNetworkInfoV1Pckg iCurrentNetworkInfoV1Pckg;
  
    //Home Operator
    CTelephony::TNetworkRegistrationV1 iHomeOperatorInfo;
    CTelephony::TNetworkRegistrationV1Pckg iHomeOperatorInfoPckg;  
      
    QString iCurrentNwkMCC;
    QString iCurrentNwkMNC;
    
    QString iHomeNetworkMCC;
    QString iHomeNetworkMNC;    

    bool iFirstTime;
    bool iCurrentNetworkType;
    bool iHomeNetworkType;
  
    MIRQNwkInfoUpdate *iObserver;
};

#endif // IRQNWKINFOOBSERVERIMPL_H_