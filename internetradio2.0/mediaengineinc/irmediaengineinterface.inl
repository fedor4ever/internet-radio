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



// -----------------------------------------------------------------------------
// Function : CIRMediaEngineInterface
// default constructor
// @param none
// @return none
// -----------------------------------------------------------------------------
//
inline CIRMediaEngineInterface::CIRMediaEngineInterface()
	{
	//No implementation
	}

// -----------------------------------------------------------------------------
// Function : ~CIRMediaEngineInterface
// default destructor
// -----------------------------------------------------------------------------
//
inline CIRMediaEngineInterface::~CIRMediaEngineInterface()
	{
	// Destroy any instance variables and then
	// inform the framework that this specific 
	// instance of the interface has been destroyed.
	REComSession::DestroyedImplementation(iDtor_ID_Key);
	}

// -----------------------------------------------------------------------------
// Function : NewL
// function to creates an instance of CIRMediaEngineInterface
// Two phase constructor 
// @param matching string
// -----------------------------------------------------------------------------
//
inline CIRMediaEngineInterface* CIRMediaEngineInterface::NewL(const TDesC8& aMatchString)
	{
	// Set up the interface find for the default resolver.
	TEComResolverParams resolverParams;
	resolverParams.SetDataType(aMatchString);
	resolverParams.SetWildcardMatch(ETrue);		// Allow wildcard matching
	
	return reinterpret_cast<CIRMediaEngineInterface*> 
					(REComSession::CreateImplementationL(KMediaEngineInterfaceUid, 
									   _FOFF(CIRMediaEngineInterface,iDtor_ID_Key), 
										NULL,resolverParams));
	}

