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


#ifndef	IRBAT_H
#define	IRBAT_H

#include <e32def.h>

/**
 * Callback for battery Status indicator 
 * To display the battery level
 */
class MBatteryObserver
	{
public:
   /**
	* DisplayBatteryValueL() 
	* Loads the image icon based on battery level
	*/
	virtual void DisplayBatteryValueL(TInt)=0;
	};

#endif //IRBAT_H
