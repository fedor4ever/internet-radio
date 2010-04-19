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
#ifndef IRLINEEDITOR_H_
#define IRLINEEDITOR_H_

#include <hblineedit.h>

class IrLineEditor : public HbLineEdit
{
    Q_OBJECT

public:
    IrLineEditor(QGraphicsItem *parent = NULL);

signals:
    void pressed(void);
    
protected:     
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
};

#endif /* IRLINEEDITOR_H_ */
