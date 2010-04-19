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
* Description:  Listbox implementation supporting bitmap advertisement.
*
*/


#include <gulicon.h>
#include <stringloader.h>
#include <internetradio.rsg>
#include <aknbiditextutils.h> 
#include <aknlayoutscalable_avkon.cdl.h>
#include <aknutils.h> 

#include "irui.h"
#include "irdebug.h"
#include "irfavoritesdb.h"
#include "irmainview.h"
#include "irmaincontainer.h"
#include "iradvertisinglistbox.h"

const TInt KThree = 3;
const TInt KFour = 4;
const TInt KSix = 6;
const TInt KTen = 10;

// ---------------------------------------------------------------------------
// CIRAdvertisingListBoxData::NewL
// ---------------------------------------------------------------------------
//
CIRAdvertisingListBoxData* CIRAdvertisingListBoxData::NewL()
    {
    CIRAdvertisingListBoxData* self = new (ELeave) CIRAdvertisingListBoxData();
    CleanupStack::PushL( self );
    self->ConstructLD();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CIRAdvertisingListBoxData::SetAdvertisementIcon
// ---------------------------------------------------------------------------
//
void CIRAdvertisingListBoxData::SetAdvertisementIcon( CGulIcon* aIcon )
    {
    delete iIcon;
    iIcon = aIcon;
    }

// ---------------------------------------------------------------------------
// CIRAdvertisingListBoxData::AdvertisementIcon
// ---------------------------------------------------------------------------
//
CGulIcon* CIRAdvertisingListBoxData::AdvertisementIcon()
    {
    return iIcon;
    }

// ---------------------------------------------------------------------------
// CIRAdvertisingListBoxData::SetControlFlag
// ---------------------------------------------------------------------------
//
void CIRAdvertisingListBoxData::SetControlFlag(TBool aControlFlag)
    {
    iControlFlag = aControlFlag;
    }
// ---------------------------------------------------------------------------
// CIRAdvertisingListBoxData::GetControlFlag
// ---------------------------------------------------------------------------
//
TBool CIRAdvertisingListBoxData::GetControlFlag()
    {
    return iControlFlag;
    }
// ---------------------------------------------------------------------------
// CIRAdvertisingListBoxData::CIRAdvertisingListBoxData
// ---------------------------------------------------------------------------
//
CIRAdvertisingListBoxData::CIRAdvertisingListBoxData()
    {
    }
// ---------------------------------------------------------
// CIRAdvertisingListBoxData::DrawNoMatchesText
// Draws the NoMatches Text on the view
// ---------------------------------------------------------
//
void CIRAdvertisingListBoxData::DrawEmptyListL(CWindowGc &aGc,
											   TPtrC aText,
											   TRect aParentRect) const 
	{
    IRLOG_DEBUG( "CIRAdvertisingListBoxData::DrawEmptyListL - Entering" );
    
	MAknsSkinInstance* skin = AknsUtils::SkinInstance () ;


    TRgb color( KRgbBlack );
    if ( AknsUtils::AvkonSkinEnabled() )
        {
        TRgb c;
        MAknsSkinInstance *skin = AknsUtils::SkinInstance();
        TInt err = AknsUtils::GetCachedColor( skin,
                                              c,
                                              KAknsIIDQsnTextColors,
                                              EAknsCIQsnTextColorsCG6 );
        if ( !err )
            {
            color = c;
            }
        }

	
    if ( !aText.Length() )
        {
        return;
        }

    HBufC* hbuf = HBufC::New( aText.Size() + 3 * ( KAknBidiExtraSpacePerLine  +1 )
                              + KTen );  // reserve space for newlines
    if ( !hbuf )
        { // can't really do anything
        return;
        }

    TPtr buffer(hbuf->Des());
    

    /*
    * input text can be either
    * - "line1"
    * - "line1 which will be wrapped to 2 lines and truncated with..."
    * - "line1\nline2"
    * - "line1\nMany other lines which will be wrapped to several lines"
    *
    * there are 3 layouts
    * - 1 line with big font,
    * - 2 lines with big font
    * - 1 line with big font + 1..3 lines with small font (not for popup lists)
    *
    *  so first we need to check if given text has a newline,
    *  if so, then we need to check if given text fits to 2 lines or
    *  should it be split to several small font lines
    */

    TInt i, n;
    n = 0;
    for (i = 0; i < aText.Length(); i ++)
        {
        if ( aText[i] == '\n' )
            {
            n++;
            }
        }

    
    TAknTextComponentLayout layout1( AknLayoutScalable_Avkon::main_pane_empty_t1( 0 ) );
    TAknTextComponentLayout layout2( AknLayoutScalable_Avkon::main_pane_empty_t2( 0 ) );
    TAknLayoutText line1;
    TAknLayoutText line2;

    line1.LayoutText( aParentRect, layout1 );
    TInt line1length = line1.TextRect().Size().iWidth;

    line2.LayoutText( aParentRect, layout2 ); 
    TInt line2length = line2.TextRect().Size().iWidth;

    const CFont *font = line1.Font();

    if ( n == 0 )
        { // one line, or one line which will be wrapped to two
        DrawEmptyListDrawUpToTwoLinesL( aGc,aText, line1, line2,
                                                 line1length, line2length,
                                                 font, color, buffer,
                                                 aParentRect );

        delete hbuf;
        return;
        }
    
    CArrayFix<TInt>* wrapWidthArray = new( ELeave ) CArrayFixFlat<TInt>(KTen);
    CleanupStack::PushL( wrapWidthArray );

    wrapWidthArray->AppendL( line1length );
    wrapWidthArray->AppendL( line2length );
    wrapWidthArray->AppendL( line2length ); // allow wrap to 3 lines
    
    TRAPD( error,AknBidiTextUtils::ConvertToVisualAndWrapToStringL(
        aText, *wrapWidthArray, *font, buffer, ETrue ));

    CleanupStack::PopAndDestroy(wrapWidthArray); 


    n = 0;
    for ( i = 0; i < buffer.Length(); i ++)
        {
        if (buffer[i] == '\n')
            {
            n++;
            }
        }

    // wrapping adds a \n to end of each line --> n < 3
    
    if ( error != KErrNone || n < KThree)
        { // 2 lines which fit to 2 line space
        DrawEmptyListDrawUpToTwoLinesL( aGc,aText, line1, line2,
                                                 line1length, line2length,
                                                 font, color, buffer,
                                                 aParentRect );

        }
    else
        { // 1 line with big font + 1..3 lines with small font
        DrawEmptyListDrawMoreThanTwoLinesL( aGc,aParentRect, aText,
        											 color, buffer );

        }

    delete hbuf;
   
    IRLOG_DEBUG( "CIRAdvertisingListBoxData::DrawEmptyListL - Exiting" );
	}   
// ---------------------------------------------------------
// CIRAdvertisingListBoxData::DrawEmptyListDrawUpToTwoLines
// 
// ---------------------------------------------------------
//
void CIRAdvertisingListBoxData::DrawEmptyListDrawUpToTwoLinesL(CWindowGc &aGc,
											  TPtrC aText,
                                              TAknLayoutText &aLine1,
                                              TAknLayoutText &aLine2,
                                              TInt aLine1length,
                                              TInt aLine2length,
                                              const CFont* aFont,
                                              TRgb aColor,
                                              TDes& aBuffer,
                                              TRect aParentRect) const 
    {
    IRLOG_DEBUG("CIRAdvertisingListBoxData::DrawEmptyListDrawUpToTwoLinesL-Entering" );
    CArrayFix<TInt>* wrapWidthArray = new( ELeave ) CArrayFixFlat<TInt>(KTen);
    CleanupStack::PushL( wrapWidthArray );

    wrapWidthArray->AppendL( aLine1length );
    wrapWidthArray->AppendL( aLine2length );
    
    TRAPD( error,AknBidiTextUtils::ConvertToVisualAndWrapToStringL( 
        aText, *wrapWidthArray, *aFont, aBuffer, ETrue ));
    
    CleanupStack::PopAndDestroy(wrapWidthArray); 
    
    if ( error != KErrNone )
        {
        aBuffer = aText;
        }
    
    // Drawing text
    aGc.Reset();
    TBool oneline( EFalse );
    TPtrC ptr = aBuffer;
    TPtrC top = ptr;
    TInt off = ptr.Locate('\n');
    if ( off >= 0 )
        {
        top.Set(ptr.Left(off));
        ptr.Set(ptr.Mid(off+1));

        TInt off1 = ptr.Locate('\n');
        if ( off1 >= 0 )
            {
            ptr.Set(ptr.Left(off1));
            }
        else
            {
            oneline = ETrue;
            }
        }
    
    aGc.SetBrushStyle(CGraphicsContext::ENullBrush);
    aGc.SetOpaque( ETrue ); // transparency off

    // no layout exist for popup list - mainpane layout is ok for X
    // coords, center vertically.  Also need to calculate vertical
    // position for mainpane lists, since laf is broken as designed.
    // If you don't believe this, try using laf values in phonebook.
    aGc.UseFont( aFont );
    aGc.SetPenColor( aColor );
    
    // center horizontally
    CGraphicsContext::TTextAlign textAlign( CGraphicsContext::ECenter );
    
    // center vertically
    TInt h = aParentRect.Height();
    TInt lineh = aLine1.TextRect().Height();
    TRect r1( aLine1.TextRect() );
    TRect r2( aLine2.TextRect() );

    // gap between lines - must be calculated this way, since no other
    // way really exists. Could be calculated from layout data, but
    // data is wrong, as 1st line is calculated from top of parent
    // rect, and 2nd line is calculated from bottom of the parent
    // rect. iAvkonAppUi->ClientRect() as parent rect would otherwise
    // be ok and give nice results, but in phonebook ClientRect() is
    // not what it should be - it is some strange amount too large.
    // This can not be fixed unless layout data is fixed to use only
    // top marginals.
    TInt lineGap( lineh / KSix );

    TInt m;  // // middle point of texts
    m = aParentRect.iTl.iY + h / 2;// + lineh + lineGap / 2;
    if ( oneline )
        {
        m += lineh / 2;
        }
    TInt b1( m - lineGap / 2 );           // bottom of 1st line
    TInt b2( m + lineh + lineGap / 2 );   // botton of 2nd line
    
    
    // rects of texts
    r1.iTl.iY = b1 - lineh;
    r1.iBr.iY = b1;
        
    r2.iTl.iY = b2 - lineh;
    r2.iBr.iY = b2;
        
	TInt baseLineOffset = ((r1.iBr.iY - r1.iTl.iY - aFont->HeightInPixels())/2
				+ aFont->AscentInPixels());

    aGc.DrawText( top, r1, baseLineOffset, textAlign );
    if ( !oneline )
        {
        aGc.DrawText( ptr, r2, baseLineOffset, textAlign );
        }
    
    
    aGc.DiscardFont();
    aGc.SetOpaque( EFalse ); // transparency back on
    IRLOG_DEBUG("CIRAdvertisingListBoxData::DrawEmptyListDrawUpToTwoLinesL-Exiting" );
    }    
// ---------------------------------------------------------
// CIRAdvertisingListBoxData::DrawEmptyListDrawMoreThanTwoLines
// Draws the NoMatches Text on the view
// ---------------------------------------------------------
//
void CIRAdvertisingListBoxData::DrawEmptyListDrawMoreThanTwoLinesL(CWindowGc &aGc,
												  TRect &aParentRect,
                                                  TPtrC aText,
                                                  TRgb aColor,
                                                  TDes& buffer ) const

    {
    IRLOG_DEBUG("CIRAdvertisingListBoxData::DrawEmptyListDrawMoreThanTwoLinesL-Entering" );
    // fetch layouts
    TAknLayoutText line[KFour];
    
    line[0].LayoutText( aParentRect, AknLayoutScalable_Avkon::main_pane_empty_t1(2) );
    line[1].LayoutText( aParentRect, AknLayoutScalable_Avkon::main_pane_empty_t3(0) );
    line[2].LayoutText( aParentRect, AknLayoutScalable_Avkon::main_pane_empty_t4(0) );
    line[3].LayoutText( aParentRect, AknLayoutScalable_Avkon::main_pane_empty_t5(0) );

    TInt lineLength[KFour];
    TInt i;
    for ( i = 0; i < KFour; i++ )
        {
        lineLength[i] = line[i].TextRect().Width();
        }
    
    const CFont *bigFont = line[0].Font();
    const CFont *smallFont = line[1].Font(); 

    // wrap text
    TInt off = aText.Locate('\n');
    TPtrC rest( aText );
    rest.Set( aText.Right(aText.Length() - off - 1 ));

    HBufC* firstLine = NULL;
    
    TRAPD( error1, 
            {
            firstLine = HBufC::NewL(
                aText.Left(off).Length() + KAknBidiExtraSpacePerLine );
            });

    if (error1 == KErrNone)
        {
        TPtr firstLinePtr = firstLine->Des();
        AknBidiTextUtils::ConvertToVisualAndClip(
                                aText.Left(off),
                                firstLinePtr,
                                *bigFont,
                                lineLength[0],
                                lineLength[0] );
        }
    
    CArrayFix<TInt>* wrapWidthArray = new( ELeave ) CArrayFixFlat<TInt>(KTen);
    CleanupStack::PushL( wrapWidthArray );
    
    // wrap small font lines
    wrapWidthArray->Reset();
    for ( i = 1; i < KFour; i++ )
        {
		TInt temp = lineLength[i];
		if(wrapWidthArray->Count() >= 0  && wrapWidthArray->Count() < KTen)
			{
			wrapWidthArray->AppendL( temp );
			}
        }
    
    TRAPD( error2,AknBidiTextUtils::ConvertToVisualAndWrapToStringL( 
        rest, *wrapWidthArray, *smallFont, buffer, ETrue ));
    
    CleanupStack::PopAndDestroy(wrapWidthArray);
    
    
    TPtrC ptr[KFour];
    TInt n = 0;
    
    if (error1 == KErrNone)
        {
        ptr[0].Set( firstLine->Des() );
        }
    if ( error1 != KErrNone || error2 != KErrNone )
        {
        ptr[0].Set(aText.Left(off));
        }
    else
        {
        TInt newlines[KThree];
        n = 0;
        for ( i = 0; i < buffer.Length(); i++ )
            {
            if ( buffer[i] != '\n' )
                {
                continue;
                }
            newlines[n] = i;
            
            n++;
            if ( n >= KThree )
                {
                break;
                }
            }
        
        if ( n >= 1 )
            {
            ptr[1].Set( buffer.Left( newlines[0] ) );
            }
        if ( n >= 2 )
            {
            ptr[2].Set( buffer.Mid( newlines[0] + 1, newlines[1] - newlines[0] - 1 ) );
            }
        if ( n >= KThree )
            {
            ptr[3].Set( buffer.Mid( newlines[1] + 1, newlines[2] - newlines[1] - 1 ) );
            }
        }
    
    // draw texts
    aGc.SetBrushStyle(CGraphicsContext::ENullBrush);
    aGc.SetOpaque( ETrue ); // transparency off
    
    for ( i = 0; i < KFour; i++ )
        {
        line[i].DrawText( aGc,ptr[i], EFalse, aColor );
        }
 
    aGc.SetOpaque( EFalse ); // transparency back on  
    delete firstLine;
    IRLOG_DEBUG("CIRAdvertisingListBoxData::DrawEmptyListDrawMoreThanTwoLinesL-Exiting" );

    }

// ---------------------------------------------------------------------------
// CIRAdvertisingListBoxData::~CIRAdvertisingListBoxData
// ---------------------------------------------------------------------------
//
CIRAdvertisingListBoxData::~CIRAdvertisingListBoxData()
    {
    delete iIcon;
    }


// ---------------------------------------------------------------------------
// CIRAdvertisingListBoxDrawer::CIRAdvertisingListBoxDrawer
// ---------------------------------------------------------------------------
//
CIRAdvertisingListBoxDrawer::CIRAdvertisingListBoxDrawer( MTextListBoxModel* aTextListBoxModel,
                                                          const CFont* aFont,
                                                          CIRAdvertisingListBoxData* 
                                                          aFormattedCellData,
                                                          CIRAdvertisingListBox* aListBox,
                      									  CIRCategoryAdvertisingListBox* aCatListBox) 
    : CFormattedCellListBoxItemDrawer( aTextListBoxModel, aFont, aFormattedCellData ),
      iListBox(aListBox),
      iCatListBox(aCatListBox)

    {
    }
// ---------------------------------------------------------------------------
// CIRAdvertisingListBoxDrawer::DrawActualItem
// ---------------------------------------------------------------------------
//
void CIRAdvertisingListBoxDrawer::DrawActualItem( TInt aItemIndex, 
                                                  const TRect& aActualItemRect, 
                                                  TBool aItemIsCurrent, 
                                                  TBool aViewIsEmphasized, 
                                                  TBool aViewIsDimmed,
                                                  TBool aItemIsSelected) const
	{
	IRLOG_DEBUG( "CIRAdvertisingListBox::DrawActualItem - Entering" );
	CIRAdvertisingListBoxData* formattedCellData = static_cast<CIRAdvertisingListBoxData*>
														( FormattedCellData() );
	if (aItemIndex == 0)
		{
		TBool flag(EFalse);
		if (iListBox)
			{
			flag = iListBox->GetLastPlayed();
			}
		TBool selectedItem =aItemIsSelected;
		if(flag)	
			{
			selectedItem = EFalse;	
			}
		CFormattedCellListBoxItemDrawer::DrawActualItem( aItemIndex, 
		                                                 aActualItemRect, 
		                                                 aItemIsCurrent, 
		                                                 aViewIsEmphasized, 
		                                                 aViewIsDimmed,
		                                                 selectedItem);


		// For MainView the folowing code is executed for hacking to make the
		// (NoMatches) string to be visible. The icon used for drawing in main
		// view is totally transparent, which makes it invisible.
		if (aItemIndex == 0 && formattedCellData->AdvertisementIcon() )
		    {
		    const CFbsBitmap* bitmap = formattedCellData->AdvertisementIcon()->Bitmap();
		    const CFbsBitmap* mask = formattedCellData->AdvertisementIcon()->Mask();
		    TSize bitmapSize = bitmap->SizeInPixels();
		    TRect maskRect;
		    if ( mask )
		        {
		        maskRect.SetSize( mask->SizeInPixels() );
		        }
		    
		    TPoint bitmapPos( aActualItemRect.iTl.iX + 
		    				 (aActualItemRect.Width() - bitmapSize.iWidth)/2,
		                      aActualItemRect.iTl.iY + 
		                      (aActualItemRect.Height() - bitmapSize.iHeight)/2 );

		    Gc()->BitBltMasked( bitmapPos, bitmap, maskRect, mask, EFalse );
		    }

		}
	else
		{
		CFormattedCellListBoxItemDrawer::DrawActualItem( aItemIndex, 
		                                                 aActualItemRect, 
		                                                 aItemIsCurrent, 
		                                                 aViewIsEmphasized, 
		                                                 aViewIsDimmed,
		                                                 aItemIsSelected);
		}

	IRLOG_DEBUG( "CIRAdvertisingListBox::DrawActualItem - Exiting" );
	}
        
// ---------------------------------------------------------------------------
// CIRAdvertisingListBox::CIRAdvertisingListBox
// ---------------------------------------------------------------------------
//
CIRAdvertisingListBox::CIRAdvertisingListBox()
    {
    }

// ---------------------------------------------------------------------------
// CIRAdvertisingListBox::SetGC
// ---------------------------------------------------------------------------
//
void CIRAdvertisingListBox::SetGC(CWindowGc& aGc)
	{
	iGc = &aGc;	
	}
// ---------------------------------------------------------------------------
// CIRAdvertisingListBox::SetControlFlag
// ---------------------------------------------------------------------------
//
void CIRAdvertisingListBox::SetControlFlag(TBool aControlFlag)
	{
    CIRAdvertisingListBoxData* irListBoxData = 
        static_cast<CIRAdvertisingListBoxData*>( ItemDrawer()->FormattedCellData() );
	irListBoxData->SetControlFlag(aControlFlag);	
	}
// ---------------------------------------------------------------------------
// CIRAdvertisingListBox::Draw
// ---------------------------------------------------------------------------
//
void CIRAdvertisingListBox::Draw(const TRect& aRect) const 
	{
    IRLOG_DEBUG( "CIRAdvertisingListBox::Draw - Entering" );
	CAknDoubleLargeStyleListBox::Draw(aRect);
	
    TInt count = Model()->NumberOfItems();
    CIRAdvertisingListBoxData* irListBoxData = 
        static_cast<CIRAdvertisingListBoxData*>( ItemDrawer()->FormattedCellData() );
	if(irListBoxData->GetControlFlag())
		{
		if(GetActiveView())
			{	
			if(GetFavouritesCount() != 0)
				{
				if(count == 0 && !GetLastPlayed())
					{
					TRAP_IGNORE(View()->SetListEmptyTextL(*iNoMatchesTextResource));
					}
				else if(count == 1 && GetLastPlayed())
					{
					TRAP_IGNORE(irListBoxData->DrawEmptyListL(*iGc,*iNoMatchesTextResource,Rect()));	
					}
				}
			else
				{
				if(GetLastPlayed())
					{
					TRAP_IGNORE(irListBoxData->DrawEmptyListL(*iGc,*iFindStationsTextResource,Rect()));	
					}
				else
					{
					TRAP_IGNORE(View()->SetListEmptyTextL(*iFindStationsTextResource));	
					}
				}
			}
		else
			{
			if(count == 1 && irListBoxData->AdvertisementIcon())
				{
				TRAP_IGNORE(irListBoxData->DrawEmptyListL(*iGc,*iNoMatchesTextResource,Rect()));	
				}
			else
				{
				TRAP_IGNORE(View()->SetListEmptyTextL(*iNoMatchesTextResource));
				}
			}
		irListBoxData->SetControlFlag(EFalse);
		}

    IRLOG_DEBUG( "CIRAdvertisingListBox::Draw - Exiting" );
    }
// ---------------------------------------------------------------------------
// CIRAdvertisingListBox::~CIRAdvertisingListBox
// ---------------------------------------------------------------------------
//
CIRAdvertisingListBox::~CIRAdvertisingListBox()
	{
	IRLOG_DEBUG( "CIRAdvertisingListBox::~CIRAdvertisingListBox - Entering" );
	if(iFindStationsTextResource)
		{
		delete iFindStationsTextResource;
		iFindStationsTextResource = NULL;
		}
	if(iNoMatchesTextResource)
		{
		delete iNoMatchesTextResource;
		iNoMatchesTextResource = NULL;	
		}
	IRLOG_DEBUG( "CIRAdvertisingListBox::~CIRAdvertisingListBox - Exiting" );
	}

// ---------------------------------------------------------------------------
// CIRAdvertisingListBox::SetListItemFormat
// ---------------------------------------------------------------------------
//
void CIRAdvertisingListBox::SetListItemFormat(  HBufC* aItemFormat)
    {
    IRLOG_DEBUG( "CIRAdvertisingListBox::SetListItemFormat - Entering" );
	iItemFormat = aItemFormat;
    }

// ---------------------------------------------------------------------------
// CIRAdvertisingListBox::SetAdvertisementIconL
// ---------------------------------------------------------------------------
//
void CIRAdvertisingListBox::SetAdvertisementIconL( CGulIcon* aIcon,TBool aMainView )
    {
    IRLOG_DEBUG( "CIRAdvertisingListBox::SetAdvertisementIconL - Entering" );
    
    CIRAdvertisingListBoxData* irListBoxData = 
        static_cast<CIRAdvertisingListBoxData*>( ItemDrawer()->FormattedCellData() );
    
    // add listitem only, if it doesn't exist yet.
    if ( !aMainView && !irListBoxData->AdvertisementIcon() )
        {
        TInt currentItemIndex = CurrentItemIndex();
        TInt topItemIndex = TopItemIndex();

        
        CAknFilteredTextListBoxModel* listBoxModel = static_cast<CAknFilteredTextListBoxModel*>
        												( Model() );
        CDesCArraySeg* items = static_cast<CDesCArraySeg*>( listBoxModel->ItemTextArray() );
        items->InsertL( 0, *iItemFormat );
        
        HandleItemAdditionL();
        if ( currentItemIndex >= 0 )
            {
            SetCurrentItemIndex( currentItemIndex + 1 );
            }
        if ( topItemIndex > 0 )
            {
            SetTopItemIndex( topItemIndex + 1 );
            }
        }
    // always replace the existing image.
    irListBoxData->SetAdvertisementIcon( aIcon );
    
    DrawDeferred();     
    IRLOG_DEBUG( "CIRAdvertisingListBox::SetAdvertisementIconL - Exiting" );
    }

// ---------------------------------------------------------------------------
// CIRAdvertisingListBox::ClearAdvertisementIcon
// ---------------------------------------------------------------------------
//
void CIRAdvertisingListBox::ClearAdvertisementIcon()
    {
    IRLOG_DEBUG( "CIRAdvertisingListBox::ClearAdvertisementIcon - Entering" );
   CIRAdvertisingListBoxData* irListBoxData = 
        static_cast<CIRAdvertisingListBoxData*>( ItemDrawer()->FormattedCellData() );
    irListBoxData->SetAdvertisementIcon( NULL );
    IRLOG_DEBUG( "CIRAdvertisingListBox::ClearAdvertisementIcon - Exiting" );
    }

// ---------------------------------------------------------------------------
// CIRAdvertisingListBox::AdvertisementIcon
// ---------------------------------------------------------------------------
//
CGulIcon* CIRAdvertisingListBox::AdvertisementIcon()
    {
    IRLOG_DEBUG( "CIRAdvertisingListBox::AdvertisementIcon - Entering" );
  	CIRAdvertisingListBoxData* irListBoxData = 
        static_cast<CIRAdvertisingListBoxData*>( ItemDrawer()->FormattedCellData() );
    IRLOG_DEBUG( "CIRAdvertisingListBox::AdvertisementIcon - Exiting" );
    return irListBoxData->AdvertisementIcon();
    }

// ---------------------------------------------------------------------------
// CIRAdvertisingListBox::CreateItemDrawerL
// ---------------------------------------------------------------------------
//
void CIRAdvertisingListBox::CreateItemDrawerL()
    {
    IRLOG_DEBUG( "CIRAdvertisingListBox::CreateItemDrawerL - Entering" );
    CIRAdvertisingListBoxData* formattedCellListBoxData = CIRAdvertisingListBoxData::NewL();
    CleanupStack::PushL( formattedCellListBoxData );
    const CFont* font = AknLayoutUtils::FontFromId( EAknLogicalFontPrimaryFont );
    
    iItemDrawer = new (ELeave) CIRAdvertisingListBoxDrawer( Model(), font,
                                                            formattedCellListBoxData,this, NULL );
    CleanupStack::Pop( formattedCellListBoxData );
    IRLOG_DEBUG( "CIRAdvertisingListBox::CreateItemDrawerL - Exiting" );
    }
// --------------------------------------------------------------------------
// CIRAdvertisingListBox::ConstructL()
// 
// --------------------------------------------------------------------------
void CIRAdvertisingListBox::ConstructL(const CCoeControl* aParent, 
                    TInt aFlags,CAknFilteredTextListBoxModel* aFilteredmodel)
    {
    IRLOG_DEBUG( "CIRAdvertisingListBox::ConstructL - Entering" );
    iModel=aFilteredmodel;
    CreateItemDrawerL();

    EnableExtendedDrawingL();
    iItemDrawer->SetDrawMark(EFalse);
    
	if(iFindStationsTextResource)
		{
		delete iFindStationsTextResource;
		iFindStationsTextResource = NULL;
		}
	HBufC* findStationsTextResource = StringLoader::LoadLC(R_IRAPP_FAVOURITES_LIST_EMPTY);
	iFindStationsTextResource = HBufC::NewL(findStationsTextResource->Length());
	iFindStationsTextResource->Des().Append(findStationsTextResource->Des());
	CleanupStack::PopAndDestroy( findStationsTextResource );
	
	if(iNoMatchesTextResource)
		{
		delete iNoMatchesTextResource;
		iNoMatchesTextResource = NULL;	
		}
	HBufC* noMatchesTextResource = StringLoader::LoadLC(R_IRAPP_STATIONLIST_NOMATCHES);
	iNoMatchesTextResource = HBufC::NewL(noMatchesTextResource->Length());
	iNoMatchesTextResource->Des().Append(noMatchesTextResource->Des());
	CleanupStack::PopAndDestroy( noMatchesTextResource );

    CEikListBox::ConstructL(aParent,aFlags);
    IRLOG_DEBUG( "CIRAdvertisingListBox::ConstructL - Exiting" );
    }
// --------------------------------------------------------------------------
// CIRAdvertisingListBox::SetLastPlayed()
// sets the status of lastplayed
// --------------------------------------------------------------------------
void CIRAdvertisingListBox::SetLastPlayed(TBool aLastPlayed)
	{
    IRLOG_DEBUG( "CIRAdvertisingListBox::SetLastPlayed " );
	iLastPlayed = aLastPlayed;	
	}

// --------------------------------------------------------------------------
// CIRAdvertisingListBox::GetLastPlayed()
// return the status of lastplayed
// --------------------------------------------------------------------------
TBool  CIRAdvertisingListBox::GetLastPlayed() const
	{
    IRLOG_DEBUG( "CIRAdvertisingListBox::GetLastPlayed " );
	return iLastPlayed;	
	}
// --------------------------------------------------------------------------
// CIRAdvertisingListBox::GetFavouritesCount()
// returns the number of favourates present.
// --------------------------------------------------------------------------
TInt CIRAdvertisingListBox::GetFavouritesCount() const
	{
    IRLOG_DEBUG( "CIRAdvertisingListBox::GetFavouritesCount " );
	CIRUi* appUi = static_cast<CIRUi*>( iCoeEnv->AppUi() );
	TInt favCount = appUi->iFavPresets->iFavPresetList.Count();
    IRLOG_DEBUG( "CIRAdvertisingListBox::GetFavouritesCount - Exiting" );
    return favCount;
	}
// --------------------------------------------------------------------------
// CIRAdvertisingListBox::GetFavouritesCount()
// returns the number of favourates present.
// --------------------------------------------------------------------------
TBool CIRAdvertisingListBox::GetFilteredFlag() const
	{
    IRLOG_DEBUG( "CIRAdvertisingListBox::GetFavouritesCount " );
	CIRUi* appUi = static_cast<CIRUi*>( iCoeEnv->AppUi() );
	TBool flag = appUi->iMainView->GetMainContainer()->GetFilteredFlag();
    IRLOG_DEBUG( "CIRAdvertisingListBox::GetFavouritesCount - Exiting" );
    return flag;
	}
// --------------------------------------------------------------------------
// CIRAdvertisingListBox::GetActiveView()
// returns whether the active view is mainView or not.
// --------------------------------------------------------------------------
TBool CIRAdvertisingListBox::GetActiveView() const
	{
    IRLOG_DEBUG( "CIRAdvertisingListBox::GetActiveView " );
	CIRUi* appUi = static_cast<CIRUi*>( iCoeEnv->AppUi() );
    IRLOG_DEBUG( "CIRAdvertisingListBox::GetActiveView - Exiting" );
	if(appUi->iMainView->GetMainContainer())
		{
		return ETrue;	
		}
	else
		{
		return EFalse;	
		}
	}
/******************************Advertisement Listbox for Category View************/


// ---------------------------------------------------------------------------
// CIRCategoryAdvertisingListBox::CIRCategoryAdvertisingListBox
// ---------------------------------------------------------------------------
//
CIRCategoryAdvertisingListBox::CIRCategoryAdvertisingListBox()
    {
    }

// ---------------------------------------------------------------------------
// CIRCategoryAdvertisingListBox::SetGC
// ---------------------------------------------------------------------------
//
void CIRCategoryAdvertisingListBox::SetGC(CWindowGc& aGc)
	{
	iGc = &aGc;	
	}
// ---------------------------------------------------------------------------
// CIRCategoryAdvertisingListBox::SetControlFlag
// ---------------------------------------------------------------------------
//
void CIRCategoryAdvertisingListBox::SetControlFlag(TBool aControlFlag)
	{
    CIRAdvertisingListBoxData* irListBoxData = 
        static_cast<CIRAdvertisingListBoxData*>( ItemDrawer()->FormattedCellData() );
	irListBoxData->SetControlFlag(aControlFlag);	
	}
// ---------------------------------------------------------------------------
// CIRCategoryAdvertisingListBox::Draw
// ---------------------------------------------------------------------------
//
void CIRCategoryAdvertisingListBox::Draw(const TRect& aRect) const 
	{
    IRLOG_DEBUG( "CIRCategoryAdvertisingListBox::Draw - Entering" );
	CAknDoubleStyleListBox::Draw(aRect);

    TInt count = Model()->NumberOfItems();
    CIRAdvertisingListBoxData* irListBoxData = 
        static_cast<CIRAdvertisingListBoxData*>( ItemDrawer()->FormattedCellData() );
    if(irListBoxData->GetControlFlag())
	    {
	    if(count == 1 && irListBoxData->AdvertisementIcon())
		    {
			TRAP_IGNORE(irListBoxData->DrawEmptyListL(*iGc,*iTextResource,Rect()));	
			}
	    else if(count == 0)
		    {
			TRAP_IGNORE(View()->SetListEmptyTextL(*iTextResource));
		    }
		irListBoxData->SetControlFlag(EFalse);
	    }
    IRLOG_DEBUG( "CIRCategoryAdvertisingListBox::Draw - Exiting" );
    }
// ---------------------------------------------------------------------------
// CIRCategoryAdvertisingListBox::~CIRCategoryAdvertisingListBox
// ---------------------------------------------------------------------------
//
CIRCategoryAdvertisingListBox::~CIRCategoryAdvertisingListBox()
	{
	IRLOG_DEBUG( "CIRCategoryAdvertisingListBox::~CIRCategoryAdvertisingListBox - Entering" );
	if(iTextResource)
		{
		delete iTextResource;
		iTextResource = NULL;
		}
	IRLOG_DEBUG( "CIRCategoryAdvertisingListBox::~CIRCategoryAdvertisingListBox - Exiting" );
	}

// ---------------------------------------------------------------------------
// CIRCategoryAdvertisingListBox::SetListItemFormat
// ---------------------------------------------------------------------------
//
void CIRCategoryAdvertisingListBox::SetListItemFormat(  HBufC* aItemFormat )
    {
    IRLOG_DEBUG( "CIRCategoryAdvertisingListBox::SetListItemFormat - Entering" );
	__ASSERT_ALWAYS( !iItemFormat, User::Panic( KNullDesC, KErrCorrupt ) );
    iItemFormat = aItemFormat;
    }

// ---------------------------------------------------------------------------
// CIRCategoryAdvertisingListBox::SetAdvertisementIconL
// ---------------------------------------------------------------------------
//
void CIRCategoryAdvertisingListBox::SetAdvertisementIconL( CGulIcon* aIcon )
    {
    IRLOG_DEBUG( "CIRCategoryAdvertisingListBox::SetAdvertisementIconL - Entering" );
	__ASSERT_ALWAYS( iItemFormat, User::Panic( KNullDesC, KErrCorrupt  ) );
    
    CIRAdvertisingListBoxData* irListBoxData = 
        static_cast<CIRAdvertisingListBoxData*>( ItemDrawer()->FormattedCellData() );
    
    // add listitem only, if it doesn't exist yet.
    if ( !irListBoxData->AdvertisementIcon() )
        {
        TInt currentItemIndex = CurrentItemIndex();
        TInt topItemIndex = TopItemIndex();

     
        CAknFilteredTextListBoxModel* listBoxModel = static_cast<CAknFilteredTextListBoxModel*>
        											( Model() );
        CDesCArraySeg* items = static_cast<CDesCArraySeg*>( listBoxModel->ItemTextArray() );
        items->InsertL( 0, *iItemFormat );
        
        HandleItemAdditionL();
        if ( currentItemIndex >= 0 )
            {
            SetCurrentItemIndex( currentItemIndex + 1 );
            }
        if ( topItemIndex > 0 )
            {
            SetTopItemIndex( topItemIndex + 1 );
            }
        }
    // always replace the existing image.
    irListBoxData->SetAdvertisementIcon( aIcon );
    
    DrawDeferred();     
    IRLOG_DEBUG( "CIRCategoryAdvertisingListBox::SetAdvertisementIconL - Exiting" );
    }

// ---------------------------------------------------------------------------
// CIRCategoryAdvertisingListBox::ClearAdvertisementIcon
// ---------------------------------------------------------------------------
//
void CIRCategoryAdvertisingListBox::ClearAdvertisementIcon()
    {
    IRLOG_DEBUG( "CIRCategoryAdvertisingListBox::ClearAdvertisementIcon - Entering" );
   CIRAdvertisingListBoxData* irListBoxData = 
        static_cast<CIRAdvertisingListBoxData*>( ItemDrawer()->FormattedCellData() );
    irListBoxData->SetAdvertisementIcon( NULL );
    IRLOG_DEBUG( "CIRCategoryAdvertisingListBox::ClearAdvertisementIcon - Exiting" );
    }

// ---------------------------------------------------------------------------
// CIRCategoryAdvertisingListBox::AdvertisementIcon
// ---------------------------------------------------------------------------
//
CGulIcon* CIRCategoryAdvertisingListBox::AdvertisementIcon()
    {
     IRLOG_DEBUG( "CIRCategoryAdvertisingListBox::AdvertisementIcon - Entering" );
  CIRAdvertisingListBoxData* irListBoxData = 
        static_cast<CIRAdvertisingListBoxData*>( ItemDrawer()->FormattedCellData() );
    IRLOG_DEBUG( "CIRCategoryAdvertisingListBox::AdvertisementIcon - Exiting" );
    return irListBoxData->AdvertisementIcon();
    }

// ---------------------------------------------------------------------------
// CIRCategoryAdvertisingListBox::CreateItemDrawerL
// ---------------------------------------------------------------------------
//
void CIRCategoryAdvertisingListBox::CreateItemDrawerL()
    {
    IRLOG_DEBUG( "CIRCategoryAdvertisingListBox::CreateItemDrawerL - Entering" );
    CIRAdvertisingListBoxData* formattedCellListBoxData = CIRAdvertisingListBoxData::NewL();
    CleanupStack::PushL( formattedCellListBoxData );
    const CFont* font = AknLayoutUtils::FontFromId( EAknLogicalFontPrimaryFont );
    
    iItemDrawer = new (ELeave) CIRAdvertisingListBoxDrawer( Model(), font,
                                                            formattedCellListBoxData,NULL, this );
    CleanupStack::Pop( formattedCellListBoxData );
    IRLOG_DEBUG( "CIRCategoryAdvertisingListBox::CreateItemDrawerL - Exiting" );
    }
// --------------------------------------------------------------------------
// CIRCategoryAdvertisingListBox::ConstructL()
// 
// --------------------------------------------------------------------------
void CIRCategoryAdvertisingListBox::ConstructL(const CCoeControl* aParent, 
                    TInt aFlags,CAknFilteredTextListBoxModel* aFilteredmodel)
    {
    IRLOG_DEBUG( "CIRCategoryAdvertisingListBox::ConstructL - Entering" );
    iModel=aFilteredmodel;
    CreateItemDrawerL();

    EnableExtendedDrawingL();
    iItemDrawer->SetDrawMark(EFalse);

	if(iTextResource)
		{
		delete iTextResource;
		iTextResource = NULL;
		}
	HBufC* textResource = StringLoader::LoadLC(R_IRAPP_STATIONLIST_NOMATCHES);
	iTextResource = HBufC::NewL(textResource->Length());
	iTextResource->Des().Append(textResource->Des());
	CleanupStack::PopAndDestroy( textResource );

    CEikListBox::ConstructL(aParent,aFlags);
    IRLOG_DEBUG( "CIRCategoryAdvertisingListBox::ConstructL - Exiting" );
    }
