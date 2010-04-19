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


#ifndef IRNWINFOOBSERVER_H
#define IRNWINFOOBSERVER_H

#include <etel3rdparty.h>

class CIRUi;

class CIRNwInfoObserver : public CActive
	{
public:  // Constructors and destructor

	/**
	* Function : NewL
	* Symbian OS two-phased constructor
	* @param instance of ui
	*/
	static CIRNwInfoObserver* NewL(CIRUi* aUi);
	
	/**
	* Function : NewLC
	* Symbian OS two-phased constructor
	* @param instance of ui
	*/
	static CIRNwInfoObserver* NewLC(CIRUi* aUi);

	/**
	* Function : ~CIRNwInfoObserver
	* Destructor.
	*/
	~CIRNwInfoObserver();

public:
	/**
	 * Function : IRStartNwMonitor
	 * starts network monitoring
	 */
	void IRStartNwMonitor();

private:
	/**
	* Function : ConstructL
	* Function does all the initializations
	* Two phase constructor
	* @param CIRUi instance
	*/
	void ConstructL(CIRUi* aUi);

	/**
	* Function : CIRNwInfoObserver
	* By default, prohibit copy constructor
	*/
	CIRNwInfoObserver();
		
	/**
	 * Function : IssueRequest
	 * Issue Subsequent network monitoring request
	 */
	void IssueRequest();

	/**
	 * Function : DoCancel
	 * From CActive. Cancels the active object from observing System
	 * Agent events. .
	 */
	void DoCancel();

	/**
	 * Function : RunL
	 * From CActive. Gets called by active object when a System Agent
	 * event occurs.
	 */
	void RunL();

	/**
	* Function : RunError
	* Function which handles RunL error
	* @param Error value 
	*/
	TInt RunError(TInt aError);

private:  

	CIRUi* iUi;
	CTelephony* iNwInfoObserver;

	TBool iFirstTime;
	TBool iCurrentNetwork;
	TBool iHomeNetworkType;
	
	TBuf<10> iCurrentNetworkMCC;
	TBuf<10> iCurrentNetworkMNC;
	TBuf<10> iHomeNetworkMCC;
	TBuf<10> iHomeNetworkMNC;

	//Current Network
	CTelephony::TNetworkInfoV1 iCurrentNetworkInfo;
	CTelephony::TNetworkInfoV1Pckg iCurrentNetworkInfoV1Pckg;

	//Home Operator
	CTelephony::TNetworkRegistrationV1 iHomeOperatorInfo;
	CTelephony::TNetworkRegistrationV1Pckg iHomeOperatorInfoPckg;	
	};

#endif  // IRNWINFOOBSERVER_H

