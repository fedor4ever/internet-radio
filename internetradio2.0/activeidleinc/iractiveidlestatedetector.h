/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Header file of CIRActiveIdleStateDetector.
*
*/


#ifndef CIRACTIVEIDLESTATEDETECTOR_H
#define CIRACTIVEIDLESTATEDETECTOR_H

#include <e32base.h>

class MIRActiveIdleEngNotifyHandler;

/**
 *  Class for detecting application running state.
 */
NONSHARABLE_CLASS(CIRActiveIdleStateDetector) : public CActive
    {

public:
    
    /**
     * C++ default constructor.
	 * @param sStateObserver    Observer for running state change
     */
    CIRActiveIdleStateDetector( MIRActiveIdleEngNotifyHandler& aStateObserver );

    /**
     * Destructor.
     */
    ~CIRActiveIdleStateDetector();

    /**
     * Requests notification for termination of Internet Radio
     */
    void StartListeningL();

protected: 

// from base class CActive
    void RunL();
    void DoCancel();

private: // data

	/** Observer for running state change */
	MIRActiveIdleEngNotifyHandler&  iStateObserver;
	/** The process to be observed */
	RProcess                        iProcess;
    
    };

#endif // CIRACTIVEIDLESTATEDETECTOR_H
