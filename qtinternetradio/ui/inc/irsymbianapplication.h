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

#ifndef IRSYMBIANAPPLICATION_H
#define IRSYMBIANAPPLICATION_H

#include <QS60MainApplication>

CApaApplication *newS60Application();

class IRSymbianApplication : public QS60MainApplication
{
public:
    static IRSymbianApplication * getInstance();
    bool getInstanceFlag() const;
    
private:
    IRSymbianApplication();
    
    virtual CApaDocument *CreateDocumentL();
    virtual void PreDocConstructL();
    
private:
    static IRSymbianApplication * appInstance;
    bool iSecondInstance;
};


#endif  //IRSYMBIANAPPLICATION_H
