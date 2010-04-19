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


#ifndef IRHISTORY_H
#define IRHISTORY_H

#include <e32base.h>

class CIRIsdsPreset;
class CIRUi;

//========================================class declaration CIRLastPlayedStationInfo============================================

/**
 *  This class Creates CIRLastPlayedStationInfo.
 *  //Please put brife about this class
 * 
 * @code
 *
 * 
 * //Please put brife about this function
 * 
 * 
 *
 * @endcode
 *
 */
	
class CIRLastPlayedStationInfo:public CBase
	{
public:
	/**
	* NewL.
	* Two-phased constructor.
	* Create a CIRLastPlayedStationInfo object 
	* 
	* @param 
	* @return 
	*/
	static CIRLastPlayedStationInfo*NewL();
	
	/**
	* NewLC.
	* Two-phased constructor.
	* Create a CIRLastPlayedStationInfo object
	* 
	* @param 
	* @return 
	*/
	static CIRLastPlayedStationInfo* NewLC();
	
	/**
	* ~CIRLastPlayedStationInfo
	* Destructor.
	*/
	~CIRLastPlayedStationInfo();
	
	/**
	* CommitLastPlayedChannelL
	* 
	*/
	void CommitLastPlayedChannelL();
	
	/**
	* RetriveLastPlayedChannelL
	* 
	*/
	void RetriveLastPlayedChannelL();
	
	/**
	* FileExists
	* 
	*/
	TBool FileExists();
	
	/**
	* RemoveLastPlayedFile
	* 
	*/
	TInt RemoveLastPlayedFile();
	
protected:
	/**
	* ConstructL
	* 2nd phase constructor.
	* Perform the second phase construction of a
	* CIRLastPlayedStationInfo object.
	* @param 
	*/
	void ConstructL();
	
public:
	CIRIsdsPreset *iLastPlayedStation;
	CIRUi* ui;	
	};
	
#endif	//IR_HISTORY_H