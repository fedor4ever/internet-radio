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
* Description:  class declaration for visiting nokia music shop
*
*/


#ifndef IRQMUSICSHOP_H_
#define IRQMUSICSHOP_H_

#include <QString>

class CMPXFindInMShop; //For Find in Shop


/*!
 * This class provides music shop related interface.
 *
 */
class IRQMusicShop
{
public:

    /*!
     *  Get the instance of IRQMusicShop
     *  @return IRQMusicShop*
     */
    IMPORT_C static  IRQMusicShop* openInstance();

    /*!
     *  Close the instance of IRQMusicShop
     */
    IMPORT_C void closeInstance();

private:

    /**
     *  Creates IRQMusicShop instance
     */
    static IRQMusicShop* createInstanceL();
    
    /*!
     * Constructor.
     */
    IRQMusicShop();
    
    /*!
     * Destructor.
     */    
    ~IRQMusicShop();
    
    
    void constructL();

public:
    /*!
     * Launch music shop app
     * @return bool  ture if launch successfully, otherwise return false
     */                              
    IMPORT_C static bool launchMusicShop();  

    /*!
     * Launch shazam app
     * @return bool  ture if launch successfully, otherwise return false
     */                              
    IMPORT_C static bool launchShazam(); 
                                          
    /*!
     * Find song in music shop app
     * @param   const QString& aSongName
     * @param   const QString& aArtistName
     * @param   const QString& aAlbumName
     * @return bool  ture if operation successfully, otherwise return false
     */                              
    IMPORT_C bool findInMusicShop(const QString& aSongName,
                                  const QString& aArtistName = QString(),
                                  const QString& aAlbumName = QString());

#ifdef USER_DEFINED_MUSICSHOP
private:
    void resetMusicShopUrlL();
    void restoreMusicShopUrlL();
#endif
    
private:

    CMPXFindInMShop* iMPXFindInShop;
       
    /**
     * Number of objects currently referring to the singleton object
     */
    int iSingletonInstances;
	
#ifdef USER_DEFINED_MUSICSHOP    
    QString iMusicShopUrl;
    bool iResetAlready;
#endif	
};

#endif /* IRQMUSICSHOP_H_ */

