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

#ifndef IRSTATIONINFOPICTURE_H_
#define IRSTATIONINFOPICTURE_H_

#include <gdi.h>

class CWindowGc;
class TSize;
class CFbsBitmap;


class CIRStationInfoPicture : public CPicture
{
	public:

	    CIRStationInfoPicture(CWindowGc& gc,CFbsBitmap& aBitmap,CFbsBitmap& aBitmapMask);

		void Draw( CGraphicsContext& aGc, const TPoint& aTopLeft, 
				   const TRect& aClipRect, MGraphicsDeviceMap* aMap ) const;
		void ExternalizeL( RWriteStream& aStream ) const;
		void GetOriginalSizeInTwips( TSize& aSize ) const;
	protected:
		TSize iSizeInTwips;
	 	CFbsBitmap* iBitmap;
	 	CFbsBitmap* iBitmapMask;
};	


#endif /*IRSTATIONINFOPICTURE_H_*/
