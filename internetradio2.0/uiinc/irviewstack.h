/*
* Copyright (c) 2008-2008 Nokia Corporation and/or its subsidiary(-ies). 
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


#ifndef C_CIRVIEWSTACK_H
#define C_CIRVIEWSTACK_H

#include <e32base.h>

class CIRViewStack : public CBase
{

public:

    /**
     * Two-phased constructor.
     *
     * @return Instance of CVRViewStack
     */
    static CIRViewStack* NewL();

    /**
     * ~CVRViewStack
     * Destructor.
     */
    ~CIRViewStack();
    
    /**
     * Returns UID of the previously active view.
     * @return UID of previous view.
     */
    TUid Peek() const;

    /**
     * Pushes view UID to view stack.
     * View stack holds all previous views, not the current view.
     * So the top item of this stack should usually be the previous view.
     * @param aView UID of the view to push to view stack.
     */
    void PushL( TUid aView );

    /**
     * Returns number of views in to the stack.
     * @return UID of the view popped from view stack.
     */
    TInt Count() const;
    
    /**
     * Pops and returns view UID from top of view stack.
     * View stack holds all previous views, not the current view.
     * So the top item of this stack should usually be the previous view.
     * @return UID of the view popped from view stack.
     */
    TUid Pop();

    /**
     * Pops to given Id and returns view UID from top of view stack.
     * View stack holds all previous views, not the current view.
     * So the top item of this stack should usually be the previous view.
     * @return UID of the view popped from view stack.
     */ 
    TUid CIRViewStack::PopTo( TUid aId );

private:

    /**
     * CVRViewStack.
     * C++ default constructor.
     */
    CIRViewStack();
    
    /**
     * Second phase constructor.
     */
    void ConstructL();

private: // Owned
    /**
     * View stack holds all previous views, not the current view.
     * So the top item of this stack should usually be the previous view.
     */  
    RArray<TUid> iViewStack;

};

#endif // C_CIRVIEWSTACKk_H
