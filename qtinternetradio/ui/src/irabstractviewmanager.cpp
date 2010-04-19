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
#include "irabstractviewmanager.h"


/*
 * Description : constructor
 * Parameters  : None
 * Return      : None
 */
IRAbstractViewManager::IRAbstractViewManager() : iApplication(NULL)
{
    
}

/*
 * Description : set the application to view manager. View manager then use it to pass to 
 *               a view during view creation
 * Parameters  : aApplication : pointer to internet radio application
 * Return      : None   
 */
void IRAbstractViewManager::setApplication(IRApplication *aApplication)
{
    iApplication = aApplication;
}
