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
#ifndef _IRQUTILITY_H
#define _IRQUTILITY_H

#include "irqenums.h"

class IRQPreset;
class CIRPreset;
class CIRIsdsPreset; 
class CFbsBitmap;
class QPixmap;
class QString;

class IRQUtility 
{
public:
  
  /* convert the CIRIsdsPreset to the IRQPreset. 
   */
  IMPORT_C static void convertCIRIsdsPreset2IRQPrest(const CIRIsdsPreset& aCIRIsdsPreset, IRQPreset& aQIRPreset);
  /* convert the IRQPreset to the CIRIsdsPreset 
   */
  IMPORT_C static void convertIRQPreset2CIRIsdsPreset(const IRQPreset& aQIRPreset, CIRIsdsPreset& aCIRIsdsPreset);
  /* convert CIRPreset to the IRQPreset 
   */
  IMPORT_C static void convertCIRPreset2IRQPreset(const CIRPreset& aCIRPreset, IRQPreset& aIRQPreset);
  /* switch from the symbian error code to the IRQ error code 
   */
  IMPORT_C static void convertSError2QError(const int aSError, int& aQError);

  IMPORT_C static bool isValidUrl(const QString& aUrl);

  IMPORT_C static bool openAdvLink(const QString& aUrl);
  
  /**
   * this function is only used when in debug mode, for it's a time costing operation
   */
  IMPORT_C static void writeLog2File(const QString& aString, bool aDeleteOldFile = false);

private:
  
  void static appendURLL(const IRQPreset& aQIRPreset, CIRIsdsPreset& aCIRIsdsPreset);
  bool static openAdvLinkL(const QString& aUrl);
};

#endif
