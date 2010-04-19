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

#include <fbs.h>
#include <w32std.h>
#include <gdi.h>
#include "irstationinfopicture.h"
#include "irdebug.h"

CIRStationInfoPicture::CIRStationInfoPicture(CWindowGc& gc,CFbsBitmap& aBitmap, 
											 CFbsBitmap& aBitmapMask)
											 :iBitmap(&aBitmap),iBitmapMask(&aBitmapMask)
	
	{
	IRLOG_DEBUG( "CIRStationInfoPicture::CIRStationInfoPicture - Entering" );
	TSize sizeInPixels = gc.Device()->SizeInPixels();
	TSize sizeInTwips = gc.Device()->SizeInTwips();
	TSize bitmapSizeInPixels = aBitmap.SizeInPixels();
	iSizeInTwips = TSize(sizeInTwips.iWidth * bitmapSizeInPixels.iWidth / sizeInPixels.iWidth,
		sizeInTwips.iHeight * bitmapSizeInPixels.iHeight / sizeInPixels.iHeight);
	IRLOG_DEBUG( "CIRStationInfoPicture::CIRStationInfoPicture - Exiting" );
	}


void CIRStationInfoPicture::ExternalizeL(RWriteStream& ) const
	{
	IRLOG_DEBUG( "CIRStationInfoPicture::ExternalizeL" );
	return;
	}

void CIRStationInfoPicture::GetOriginalSizeInTwips(TSize& aSize) const 
    { 
    aSize = iSizeInTwips; 
    }

// The actual drawing code
void CIRStationInfoPicture::Draw(CGraphicsContext&   aGc, 
		                      	 const TPoint& 	     aTopLeft,
		                      	 const TRect& 	     /*aClipRect*/,
		                         MGraphicsDeviceMap* /*aMap*/) const
    {	
	IRLOG_DEBUG( "CIRStationInfoPicture::Draw - Entering" );
	CBitmapContext& gc = static_cast<CBitmapContext &>(aGc);
	TRect bmpPieceRect(TPoint(0,0), iBitmap->SizeInPixels());
	//the picture draw itself
	gc.BitBltMasked(aTopLeft,iBitmap, bmpPieceRect,iBitmapMask, ETrue);
	IRLOG_DEBUG( "CIRStationInfoPicture::ExternalizeL - Exiting" );
    }



