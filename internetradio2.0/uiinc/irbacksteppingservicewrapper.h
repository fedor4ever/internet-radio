/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Wrapper for Back Stepping Service
*
*/


#ifndef CIRBACKSTEPPINGSERVICEWRAPPER_H
#define CIRBACKSTEPPINGSERVICEWRAPPER_H

#include <e32base.h>
#include <vwsdef.h>

class MLiwInterface;
class CLiwGenericParamList;
class CLiwServiceHandler;

/**
 *  Back Stepping (BS) Service wrapper for Internet Radio.
 */
class CIRBackSteppingServiceWrapper : public CBase
    {

public:

    /**
     * Static constructor.
     *
     * @param aUid Application UID.
     */
    static CIRBackSteppingServiceWrapper* NewL( TUid aUid );

    /**
     * Destructor.
     */
     ~CIRBackSteppingServiceWrapper();

    /**
     * Forwards view activation event to BS Service.
     *
     * @param   aViewId View id of the view where back is called.
     * @param   aEnter Indicates if it is entry or exit activation.
     */
    void HandleViewActivationEventL( const TVwsViewId aViewId, TBool aEnter );

    /**
     * Forwards back command to BS Service.
     *
     * @param   aViewId View id of the view where back is called.
     * @return  Returns Etrue if BS Service consumed the command, otherwise returns EFalse.
     */
    TBool HandleBackCommandL( const TVwsViewId aViewId );

private:

    /**
     * C++ default constructor.
     */
    CIRBackSteppingServiceWrapper();

    /**
     * 2nd phase constructor
     *
     * @param aUid Application UID.
     */
    void ConstructL( TUid aUid );

    /**
     * Initializes BS Service.
     *
     * @param aUid Application UID.
     */
    void InitializeL( TUid aUid );

    /**
     * Handles the result of a LIW command
     *
     * @return Returns ETrue if LIW command executed, otherwise returns EFalse.
     */
    TBool HandleResultL();

private: // data

    /**
     * AIW Service Handler.
     * Own.
     */
    CLiwServiceHandler* iServiceHandler;

    /**
     * BS Service interface returned by LIW.
     * Own.
     */
    MLiwInterface* iBsInterface;

    /**
     * In param list.
     * Not own.
     */
    CLiwGenericParamList* iInParamList;

    /**
     * Out param list.
     * Not own.
     */
    CLiwGenericParamList* iOutParamList;
    };

#endif // CIRBACKSTEPPINGSERVICEWRAPPER_H
