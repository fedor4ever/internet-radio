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

#ifndef IRDOCUMENT_H
#define IRDOCUMENT_H


#include <akndoc.h>

class CEikApplication;
class CPeriodic;

//========================================class declaration CIRDocument============================================

/**
 * This class creates the document class
 * This class creates the IRUi class.
 * @code
 * CIRDocument* self = new (ELeave) CIRDocument(aApp) creates the document class.
 * appui->ParsePlsDataL(aFileStore,aFile) //parses the pls data.
 * @endcode
 *
 */
 
class CIRDocument : public CAknDocument
    {
public:

     /**
	 * Function : NewL
	 * function to return an instance of CIRDocument
	 * Two phase constructor
     */
	static CIRDocument* NewL(CEikApplication& aApp);

	 /**
	 * Function : ConstructL()
	 * Two phase constructor
     */	
	void ConstructL();

	/**
	 * Destructor.
	 */
	~CIRDocument();

	 /**
	 * Function : OpenFileL(TBool aDoOpen,const TDesC& aFilename,RFs& aFs).
	 * @param aDoOpen,to check whether DB is already open or not.
	 * @param aFilename,filename.
	 * @param aFs,Handle to Filesession.
     */
    CFileStore* OpenFileL(TBool aDoOpen,const TDesC& aFilename,RFs& aFs);    
    
   	 /**
	 * Function : OpenFileL(TBool aDoOpen,const TDesC& aFilename,RFs& aFs).
	 * @param aFilename,filename.
	 * @param aFile,RFile instance.
     */
    void OpenFileL(CFileStore*& aFileStore, RFile& aFile);	

protected:

    // from CEikDocument
    void UpdateTaskNameL(CApaWindowGroupName* aWgName);

private:

    /**
     * CIRDocument.
     * C++ default constructor.
     * @param instance of CEikApplication.
     **/     
  	 CIRDocument(CEikApplication& aApp);
  	 
   	/**
     * CreateAppUiL.
     * creates UI's instance..
     * @return  CEikAppUi,instance of CEikApplication.
     **/
    CEikAppUi* CreateAppUiL();

    /**
     * Callback for memory check timer.
     *
     * @param aSelf pointer to CIRDocument instance.
     * @return Always KErrNone
     */
    static TInt MemCheckTimerCallback(TAny* aSelf);

private:

    // Memory check timer.
    CPeriodic*      iMemCheckTimer;
    };


#endif // IR_DOCUMENT_H
