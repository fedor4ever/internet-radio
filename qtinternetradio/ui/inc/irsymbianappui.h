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

#ifndef IRSYMBIANAPPUI_H
#define IRSYMBIANAPPUI_H

#include <QS60MainAppUi>

class IRSymbianAppui : public QS60MainAppUi
{
public:
    //from base class CEikAppUi
    void ConstructL();
};

#endif //IRSYMBIANAPPUI_H
