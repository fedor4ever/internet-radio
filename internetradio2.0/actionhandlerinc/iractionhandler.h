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
* Description:  Action handler providing LIW based API for external use.
*
*/


#ifndef IRACTIONHANDLER_H
#define IRACTIONHANDLER_H

#include <ahplugin.h>

/**
 * Action handler provides LIW -based interface for external use (e.g. Homescreen) 
 * 
 * The action handler communicates with IR via IR's Publish & Subscribe API.
 * 
 */
NONSHARABLE_CLASS( CIRActionHandler ) : public CAHPlugin
    {
public:
    /**
     * Two-phased constructor. 
     */
    static CIRActionHandler* NewL();

    /**
     * Destructor
     */
    ~CIRActionHandler();
    
    // From base class CAHPlugin
    TInt ExecuteActionL( const CLiwMap* aMap );
    
private:
    /**
     * C++ constructor.
     */
    CIRActionHandler();
    
    /**
     * Second phase constructor.
     */
    void ConstructL();
    
private:
     
    /**
     * Mutes VRA.
     */
    void Stop();
    
    /**
     * Unmutes VRA.
     */
    void Play();
    
     /**
     * Invokes VRA to go to now playing view.
     */
    void StartToNowPlayingL();
    
    /**
     * Extracts the value of the given mapname to given string.
     * Reserves memory to argument aString.
     * 
     * @param aMap The map containing the mapname-string pairs.
     * @param aString Descriptor where the value is extracted.
     * @param aMapName The searched map name. 
     */
    TInt ExtractStringL( const CLiwMap* aMap, RBuf& aString, const TDesC8& aMapName );
    };

#endif /* IRACTIONHANDLER_H */
