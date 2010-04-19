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
* Description:  
*
*/


#ifndef IRMESSAGEOBSERVER_H
#define IRMESSAGEOBSERVER_H

#include <e32base.h>    // CBase
#include <msvapi.h>     // MMsvSessionObserver


class CIRUi;
 
	
/**
 * This is the message observer class is used for sms observing
 * When new message comes, the MMsvSessionObserver calls HandleSessionEventL which does the message handling
 * 
 * @code
 * 
 * CIRMessageObserver* obj
 * obj = CIRMessageObserver::NewL(instance of CIRUi* aUi); //creating message observer
 * 
 * @endcode
 * 
 */

 
class CIRMessageObserver : public CBase, public MMsvSessionObserver
    {
    public:
        /**
         * 2nd phase constructor.
         * @param aUi the instance of CIRUi
         * @return A pointer to the created instance of CIRMessageObserver
         */
        static CIRMessageObserver* NewL(CIRUi* aUi);
    
        /**
         * 2nd phase constructor.
         * @param aUi the instance of CIRUi
         * @return A pointer to the created instance of CIRMessageObserver
         */
        static CIRMessageObserver* NewLC(CIRUi* aUi);
    
        /**
         * Destructor.
         */
        ~CIRMessageObserver();
 
    public:
        /**
         * From MMsvSessionObserver.
         * Called when a session event occurs.
         */
        void HandleSessionEventL(TMsvSessionEvent aEvent,
                TAny* aArg1,
                TAny* aArg2,
                TAny* aArg3);
 
    private:
        /**
         * Symbian OS default constructor.
         * @param aObserver the observer which gets notified when message
         * events occur
         */
        CIRMessageObserver(CIRUi* aUi);
 
        /**
         * 2nd phase constructor.
         * Initializes objects.
         */
        void ConstructL();
 
    private:
        // Session with the messaging server
        CMsvSession* iMsvSession;

		// instance of CIRUi
		CIRUi* iUi;
    };
 
#endif /*IRMESSAGEOBSERVER_H*/