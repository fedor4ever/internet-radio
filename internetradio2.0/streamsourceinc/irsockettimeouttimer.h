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
* Description:  A Timer.
*
*/


#ifndef IRSOCKETTIMEOUTTIMER_H
#define IRSOCKETTIMEOUTTIMER_H

#include <e32base.h>

/**
 * A class for notifying of a timeout event.
 */
class MIRSocketTimeoutNotifier
	{
public:
	/*
	 * Called when the timer raises an event.
	 */
	virtual void TimerExpired() = 0;
	};

/**
 * A class that raises timeout events.
 */

NONSHARABLE_CLASS ( CIRSocketTimeOutTimer ) : public CTimer
	{
public:
	/**
	 * Creates an object of CIRSocketTimeOutTimer 
	 * @param aPriority Priority of the active object.
	 * @param aNotify Timer observer.
	 * @return Pointer to CIRSocketTimeOutTimer object
	 */
	static CIRSocketTimeOutTimer* NewL( const TInt aPriority,
	MIRSocketTimeoutNotifier& aNotify );

	/**
	 * Default C++ Destructor
	 */
	virtual ~CIRSocketTimeOutTimer();

protected: 

	/**
	 * Derived from CTimer
	 */

	/**
	 * Active object post-request handling.
	 */
	void RunL();

	/**
	 * Handles a leave occurring in the request completion event handler RunL()
	 */
	TInt RunError( TInt aError );

private:

	/**
	 * Creates an object of CIRSocketTimeOutTimer
	 * @param aPriority Priority of the active object.
	 * @param aNotify A handle to the class to be notified of a timeout event.
	 */
	static CIRSocketTimeOutTimer* NewLC( const TInt aPriority,
	MIRSocketTimeoutNotifier& aNotify );

	/**
	 * @param aPriority Priority of the active object.
	 * @param aNotify A handle to the class to be notified of a timeout event.
	 */
	CIRSocketTimeOutTimer( const TInt aPriority,
		MIRSocketTimeoutNotifier& aNotify);

	/**
	 * Standard Symbian second-phase construction.
	 */
	void ConstructL();

private:

	/**
	 * Handle to the class to be notified of a timeout event.
	 */
	MIRSocketTimeoutNotifier& iNotify;
	
};

#endif // IRSOCKETTIMEOUTTIMER_H

