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


#ifndef IRAPPLICATION_H
#define IRAPPLICATION_H

#include <aknapp.h>

//========================================class declaration CIRApplication============================================

/**
 * This class Creates the document
 * 
 * @code
 *
 * Creates the document class.
 * new (ELeave) CIRDocument(*this); 
 * AppDllUid() - returns the UID for the IR application 
 *
 * @endcode
 *
 */
     
class CIRApplication : public CAknApplication
    {
private:
    
     /**
	 * Function : CreateDocumentL()
	 * Inherited from class CApaApplication
     */
    CApaDocument* CreateDocumentL();
    
     /**
	 * Function : AppDllUid().
	 * @return TUid,the UID for the IR application.
     */
    TUid AppDllUid() const;
     /**
	 * Function : PreDocConstructL().
	 */
    void PreDocConstructL();
    
public:

     /**
	 * Function : GetInstanceFlag().
	 * @return the bool value for iSecondInstance
     */
    TBool GetInstanceFlag();
    
private:    
     /**
	 * iSecondInstance
     */
    TBool iSecondInstance;
    };

#endif // IRAPPLICATION_H
