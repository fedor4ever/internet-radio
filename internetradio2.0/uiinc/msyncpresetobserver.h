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


#ifndef MSYNCPRESETOBSERVER
#define MSYNCPRESETOBSERVER

#include <e32def.h>

class CIRIsdsPreset;

/**
 * MSyncPresetObserver
 * Sends the Preset synchronization callbacks to the observers
 */
class MSyncPresetObserver
    {
public:
	/**
	 * MSyncPresetObserver::SyncPresetRemovedL()
	 * called back when the preset that is synced was removed from isds
	 * @param TInt,the preset id
	 */
	virtual void SyncPresetRemovedL(TInt aId)=0;

	/**
	 * MSyncPresetObserver::SyncPresetChangedL()
	 * called back when the preset that is synced has been changed in the isds
	 * @param CIRIsdsPreset&,the new preset data
	 */
	virtual void SyncPresetChangedL(CIRIsdsPreset& aPreset)=0;
	
	/**
	 * MSyncPresetObserver::SyncPresetNoChangeL()
	 * called back when the preset that is synced has not been changed
	 */
	 virtual void SyncPresetNoChangeL()=0;
	 
	};

#endif // MSYNCPRESETOBSERVER
