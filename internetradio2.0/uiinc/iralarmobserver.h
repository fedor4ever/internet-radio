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


#ifndef IRALARMOBSERVER_H
#define IRALARMOBSERVER_H

#include <asclisession.h>
//d #include <e32property.h>

class CIRUi;
 
	
/**
 * This is the alarm observer class is used for alarm observing
 * When alarm is comes or made it calls HandleAlarmStart which does the call handling
 * When call is disconnected it calls HandleAlarmEnd to handle the scenerio
 *
 * @code
 * 
 * CIRAlarmObserver* obj
 * obj = CIRAlarmObserver::NewL(instance of CIRUi* aUi); //creating call observer
 * obj->Start();//starting the observer
 *
 * @endcode
 * 
 */

NONSHARABLE_CLASS(CIRAlarmObserver) : public CActive
	{
								//member functions
public:

	/**
	 * Function : NewL
	 * Function returns an instance of CIRAlarmObserver
	 * Two phase constructor
	 * @param CIRUi instance
	 * @return instance of CIRAlarmObserver
	 */
	static CIRAlarmObserver* NewL(CIRUi* aUi);
	
	/**
	 * Function : NewLC
	 * Function returns an instance of CIRAlarmObserver
	 * Two phase constructor
	 * @param CIRUi instance
	 * @return instance of CIRAlarmObserver
	 */
	static CIRAlarmObserver* NewLC(CIRUi* aUi);

	/**
	 * Function : ~CIRAlarmObserver
	 * Default destructor calls Cancel function which cancel the active request 
	 */
	~CIRAlarmObserver();
	
	/**
	 * Function : Start
	 * Function starts the call observer
	 */
	void Start();
	
	/**
	 * Function : RunL
	 * This is the RunL function if a call is recieved or disconnected when the player 
	 *	is active, is handled here
	 */	
	void RunL();
	
	/**
	 * Function : DoCancel
	 * Function stops the call observer if it is active
	 */
	void DoCancel();
	
	/**
	 * Function : RunError
	 * Function which handles RunL error
	 * @param Error value 
	 */
	TInt RunError(TInt aError);

private:	
	/**
	 * Function : ConstructL
	 * Function does all the initializations
	 * Two phase constructor
	 * @param CIRNowPlayingView instance
	 */
	void ConstructL(CIRUi* aUi);
	
	/**
	 * Function : CIRAlarmObserver
	 * Function is the default constructor, sets the CActive priority, Line status
	 */
	CIRAlarmObserver();

							//data member declaration
private:
	
	//The client-side interface to the Symbian OS alarm server
	RASCliSession iAlarmSession;
	
	//Alarm id
	TAlarmId iAlarmId;
	
	//instance of Now Playing View 
	CIRUi* iUi;
	
	//Checks whether alarm is on or off
	TBool iAlarmOn;
	};
	
#endif	//IRALARMOBSERVER_H

