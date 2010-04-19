/*
* Copyright (c) 2009-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  interface for updating network info
*
*/


#ifndef IRQNWKOBSERVERINTERFACE_H_
#define IRQNWKOBSERVERINTERFACE_H_

#include <QString>

class MIRQNwkInfoUpdate
{
public:
    virtual void updateCurrentNwkInfo(const QString &aCurrentNwkMCC, const QString &aCurrentNwkMNC) = 0;
    virtual void updateHomeNwkInfo(const QString &aHomeNetworkMCC, const QString &aHomeNetworkMNC) = 0;
};

#endif // IRQNWKOBSERVERINTERFACE_H_
