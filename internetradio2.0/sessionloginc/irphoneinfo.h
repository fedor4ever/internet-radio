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
* Description:  ?Description
*
*/


#ifndef IRPHONEINFO_H
#define IRPHONEINFO_H

#include <e32base.h>
#include <etel3rdparty.h>

class CTelephony;
class MIRPhoneInfoObserver;

NONSHARABLE_CLASS(CPhoneInfo) : public CActive
    {
public:
    static CPhoneInfo* NewL(MIRPhoneInfoObserver* aObserver);
    ~CPhoneInfo();

    void StartUpdating();

private:
    CPhoneInfo(MIRPhoneInfoObserver* aObserver);
    void ConstructL();

    void RunL();
    void DoCancel();

private:
    CTelephony::TPhoneIdV1 iPhoneIdV1;
    CTelephony::TPhoneIdV1Pckg iPhoneIdV1Pckg;

    CTelephony* iTelephony;
    MIRPhoneInfoObserver* iObserver;
    };

#endif // IRPHONEINFO_H
