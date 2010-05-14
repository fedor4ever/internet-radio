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
#include <hbaction.h>
#include <QApplication>
#include <QTimer>

#include "irviewmanager.h"
#include "irapplication.h"
#include "irmainview.h"
#include "ircategoryview.h"
#include "irstationsview.h"
#include "irnowplayingview.h"
#include "irsearchchannelsview.h"
#include "irfavoritesview.h"
#include "irhistoryview.h"
#include "irsettingsview.h"
#include "iropenwebaddressview.h"
#include "irsonghistoryview.h"
#include "irtermsconsview.h"
#include "irplsview.h"
#include "irstationdetailsview.h"

const int KCrossLineWidth = 30; // pixel
const double KCrossLineMinLenth = 180.0; // pixel

const int KCrossLineTimeInterval = 1500; // ms
const int KExitTimeInterval = 800; // ms, used for showing cross Line

const int KCrossLineMinAngle = 15; // degree
const int KCrossLineMaxAngle = 75; // degree

static bool crossLineReady(const QLineF &aLine);
static bool crossLineIntersected(const QLineF &aLineA, const QLineF &aLineB);

enum CrossLineAngleType
{
    EPositiveAngle = 0,  // Line within 1,3 quadrant
    ENegativeAngle       // Line within 2,4 quadrant
};
static CrossLineAngleType crossLineAngleType(const QLineF &aLine);

/*
 * Description : constructor.
 *               add a softkey action to it in order to know that back buttion is touched.
 */
IRViewManager::IRViewManager() : iApplication(NULL),
                                 iCrossLineAReady(false),
                                 iCrossLineBReady(false),
                                 iCrossLineEnable(true),
                                 iCrossLineShowing(false),
                                 iCrossLineTimer(NULL),
                                 iExitTimer(NULL),
                                 iExiting(false)
{
    iBackAction = new HbAction(Hb::BackNaviAction, this);
    connect(iBackAction, SIGNAL(triggered()), this, SLOT(backToPreviousView()));
    
    iExitAction = new HbAction(Hb::QuitNaviAction, this);
    connect(iExitAction, SIGNAL(triggered()), this, SLOT(lower()));
    
    connect(this, SIGNAL(viewReady()), this, SLOT(handleViewReady()));
    connect(this, SIGNAL(currentViewChanged(HbView*)), this, SLOT(handleCurrentViewChanged(HbView*)));
    
    iCrossLineTimer = new QTimer(this);
    iExitTimer = new QTimer(this);
    iCrossLineTimer->setSingleShot(true);
    iExitTimer->setSingleShot(true);
    connect(iCrossLineTimer,SIGNAL(timeout()),this,SLOT(crossLineReset()));
    connect(iExitTimer,SIGNAL(timeout()),this,SLOT(exitTimeout()));     
}

/*
 * Description : destructor
 */
IRViewManager::~IRViewManager()
{
}

void IRViewManager::setApplication(IRApplication *aApplication)
{
    iApplication = aApplication;
}

/*
 * Description : get a pointer to a specified view. If the view is not created yet,
 *               view manager can create it and then return pointer to it.
 * Parameters  : aViewId : the view's id
 *               aCreateIfNotExist : whether or not create a view if it doesn't exist
 * Return      : pointer to the specified view.
 */
IRBaseView* IRViewManager::getView(TIRViewId aViewId, bool aCreateIfNotExist)
{
    int viewNumber = views().count();
    for (int i = 0; i < viewNumber; ++i)
    {
        IRBaseView* addedView = static_cast<IRBaseView*>(views().at(i));
        if (addedView && addedView->id() == aViewId)
        {
            return addedView;
        }
    }
    
    if (aCreateIfNotExist)
    {
        IRBaseView* newView = createView(iApplication, aViewId);
        addView(newView);
        return newView;
    }
    
    return NULL;
}

/*
 * Description : Judge if a view is in the view stack.
 * Parameters  : aViewId : the view's id
 * Return      : true  : the view is in view stack
 *               false : the view is not in view stack
 */
bool IRViewManager::isViewInStack(TIRViewId aViewId) const
{
    int numberOfViewsInStack = iViewStack.count();

    for (int i = numberOfViewsInStack-1; i >=0 ; i--)
    {
        IRBaseView* view = iViewStack[i];
        if (view)
        {
            if (view->id() == aViewId)
            {
               return true;
            }
        }
    }
    
    return false;
}

/*
 * Description : Activate a view specified aViewId. The new view will be current view.
 *               Old current view will be deactivated and pushed into view stack if 
 *               aStackCurrent is true.
 * Parameters  : aViewId : the view's id.
 *               aStackCurrent : whether or not push current view into view stack
 * Return      : None
 */
void IRViewManager::activateView(TIRViewId aViewId, bool aPushCurrentView)
{    
    if (isViewInStack(aViewId))
    {
        backToView(aViewId);
        return;
    }
    
    IRBaseView *baseView = static_cast<IRBaseView*>(currentView());
    if (baseView && baseView->id() == aViewId)
    {
        baseView->updateView();
        return;
    }
    
    IRBaseView *view = getView(aViewId, true);
    
    if (view)
    {
        if (EViewFlag_ClearStackWhenActivate == view->flag())
        {
            clearStack();
        }
        else
        {    if (aPushCurrentView)
             {
                 if (baseView && EViewFlag_UnStackable != baseView->flag())
                 {
                     iViewStack.push(baseView);
                 }
             }
        
             //deactivate current view
             if (baseView)
             {
                 baseView->handleCommand(EIR_ViewCommand_DEACTIVATE, EIR_ViewCommandReason_Hide);
             }
        }      
        switchToNextView(view);        
    }
}

/*
 * Description : Activate a view specified by aView. The new view will be current view.
 *               Old current view will be deactivated and pushed into view stack if 
 *               aStackCurrent is true.
 * Parameters  : aView : pointer to the view to be activated.
 *               aStackCurrent : whether or not push current view into view stack
 */
void IRViewManager::activateView(IRBaseView *aView, bool aPushCurrentView)
{
    if (aView == NULL)
    {
        return;
    }
    
    activateView(aView->id(), aPushCurrentView);
}

/*
 * Description : back view stack until a view whose id is aViewId.
 *               The view will become current view.
 * Parameters  : aViewId : the view's id
 */
void IRViewManager::backToView(TIRViewId aViewId)
{
    if (aViewId == currentViewId())
    {
        return;
    }
    
    //step 1 : back current view
    IRBaseView *topView = static_cast<IRBaseView*>(currentView());
    if (topView)
    {
        topView->handleCommand(EIR_ViewCommand_DEACTIVATE, EIR_ViewCommandReason_Back);
    }
    
    //step 2 : back the views in view stack
    while (!iViewStack.isEmpty())
    {
        topView = iViewStack.top();
        if (topView->id() == aViewId)
        {
            break;
        }
        
        topView->handleCommand(EIR_ViewCommand_DEACTIVATE, EIR_ViewCommandReason_Back);
        iViewStack.pop();
    }
    
    //step 3 : we back to the view or the view is not in view stack. Activate it.
    if (!iViewStack.isEmpty())
    {
        topView = iViewStack.pop();
        Q_ASSERT(topView->id() == aViewId);   
        switchToNextView(topView);
    }
    else
    {
        //backToView(id) is called when view is in stack, it's impossible to get here
        Q_ASSERT(false);
    }
}

void IRViewManager::switchToNextView(IRBaseView *aView)
{
    aView->handleCommand(EIR_ViewCommand_TOBEACTIVATED, EIR_ViewCommandReason_Show);
    IRBaseView *viewToHide = static_cast<IRBaseView*>(currentView());
    if( viewToHide )
    {
        if( (EIRView_StationDetailsView == aView->id() && EIRView_PlayingView == viewToHide->id())
          ||(EIRView_StationDetailsView == viewToHide->id() && EIRView_PlayingView == aView->id())
          )
        {
            setCurrentView(aView,true,Hb::ViewSwitchUseAltEvent); 
        }
        else
        {
            setCurrentView(aView,true,Hb::ViewSwitchUseNormalAnim);
        }        
    }
    else
    {
        setCurrentView(aView,true,Hb::ViewSwitchUseNormalAnim);
    }        
}

/*
 * Description : return the current view's id.
 * Return      : current view's id. 
 */
TIRViewId IRViewManager::currentViewId() const
{
    IRBaseView *topView = static_cast<IRBaseView*>(currentView());
    if (topView)
    {
        return topView->id();
    }
    
    return EIRView_InvalidId;
}

/*
 * Description : push a view into the view stack by id *               
 * Parameters  : aEvent : see the definition of TIRSystemEventType
 * Return      : void :  there is no return value for the function
 *                
 */
void IRViewManager::pushViewById(TIRViewId aViewId)
{
    if (isViewInStack(aViewId))
    {     
        return;
    }
    
    IRBaseView *curView = getView(aViewId, true);
    Q_ASSERT(curView);
    iViewStack.push(curView);
}
 

//                                     slot functions

/*
 * Description : slot function for softkey action.
 *               Bring user to a previous view. If view stack is empty, quit application.
 */
void IRViewManager::backToPreviousView()
{
    if(iViewStack.isEmpty())
    {
        return;
    }

    IRBaseView *topView = static_cast<IRBaseView*>(currentView());
    IRBaseView *viewToShow = iViewStack.pop();
    
    if(viewToShow)
    {
        if(topView)
        {
            topView->handleCommand(EIR_ViewCommand_DEACTIVATE, EIR_ViewCommandReason_Back);
        }
        switchToNextView(viewToShow);       
    }
}

void IRViewManager::handleViewReady()
{    
    IRBaseView *topView = static_cast<IRBaseView*>(currentView());
    if (topView)
    {
        topView->handleCommand(EIR_ViewCommand_ACTIVATED, EIR_ViewCommandReason_Show); 
    }
    
    updateSoftkey();
}

/*
 * this function is needed because signal viewReady() has a bug. will be deleted once the bug is fixed
 */
void IRViewManager::handleCurrentViewChanged(HbView * /*aView*/)
{
    updateSoftkey();
}

/*
 * Description : create a view specified by aViewId. A view is created only when it's 
 *               first time requested
 * Parameters  : aApplication : pointer to ir application object
 *               aViewId      : the view's id
 * Return      : pointer to the created view
 */
IRBaseView* IRViewManager::createView(IRApplication* aApplication, TIRViewId aViewId)
{
    switch (aViewId)
    {
        case EIRView_MainView:
            return new IRMainView(aApplication, aViewId);
        
        case EIRView_CategoryView:
            return new IRCategoryView(aApplication, aViewId);
        
        case EIRView_StationsView:        
            return new IRStationsView(aApplication, aViewId);
        
        case EIRView_PlayingView:
            return new IRNowPlayingView(aApplication, aViewId);
            
        case EIRView_StationDetailsView:
            return new IRStationDetailsView(aApplication, aViewId);      
			
		case EIRView_SearchView:
            return new IRSearchChannelsView(aApplication, aViewId);
        
        case EIRView_FavoritesView:
            return new IRFavoritesView(aApplication, aViewId);
            
        case EIRView_HistoryView:
            return new IRHistoryView(aApplication, aViewId);
            
        case EIRView_SettingsView:
            return new IRSettingsView(aApplication, aViewId);
            
        case EIRView_OpenWebAddressView:
            return new IROpenWebAddressView(aApplication, aViewId);
        
        case EIRView_SongHistoryView:                        
            return new IRSongHistoryView(aApplication, aViewId);
            
        case EIRView_TermsConsView:                        
            return new IRTermsConsView(aApplication, aViewId);            
            
        case EIRView_PlsView:
            return new IRPlsView(aApplication, aViewId);
            
        default:
            break;
    }
    
    return NULL;
}

void IRViewManager::clearStack()
{
    IRBaseView *topView = NULL;
    
    //deactivate and back current view if it exists
    topView = static_cast<IRBaseView*>(currentView());
    if (topView)
    {
        topView->handleCommand(EIR_ViewCommand_DEACTIVATE, EIR_ViewCommandReason_Back);
    }
    
    while (!iViewStack.isEmpty())
    {
        topView = iViewStack.top();
        if (topView)
        {
            topView->handleCommand(EIR_ViewCommand_DEACTIVATE, EIR_ViewCommandReason_Back);
        }
        iViewStack.pop();
    }
}

void IRViewManager::updateSoftkey()
{
    IRBaseView *topView = static_cast<IRBaseView*>(currentView());
    if (topView)
    {
        if (EViewFlag_ClearStackWhenActivate == topView->flag())
        {
            topView->setNavigationAction(iExitAction);
        }
        else
        {
            topView->setNavigationAction(iBackAction);
        }
    }
}

void IRViewManager::mousePressEvent(QMouseEvent *aEvent)
{
    if(iCrossLineEnable)
    {
        if(iCrossLineAReady)
        {
            iCrossLineB.setP1(aEvent->posF());
        }
        else
        {
            iCrossLineA.setP1(aEvent->posF());
        }
    }

    HbMainWindow::mousePressEvent(aEvent);
}

void IRViewManager::mouseReleaseEvent(QMouseEvent *aEvent)
{
    if(iCrossLineEnable)
    {    
        if(iCrossLineAReady)
        {
            iCrossLineTimer->stop();

            iCrossLineB.setP2(aEvent->posF());
            iCrossLineBReady = crossLineReady(iCrossLineB);
    
            if(iCrossLineBReady && readyToQuit())
            {   
                iCrossLineEnable = false;
                iCrossLineShowing = true;              
                viewport()->repaint(); 
                iExitTimer->start(KExitTimeInterval);       
            }
            else
            {
                crossLineReset();
            }
        }
        else
        {
            iCrossLineA.setP2(aEvent->posF());
            iCrossLineAReady = crossLineReady(iCrossLineA);
    
            if(iCrossLineAReady)
            {
                iCrossLineTimer->stop();
                iCrossLineTimer->start(KCrossLineTimeInterval);
            }
        }
    }
    
    HbMainWindow::mouseReleaseEvent(aEvent);
}

bool IRViewManager::readyToQuit()
{
    if(iCrossLineAReady && iCrossLineBReady)
    {
        return crossLineIntersected(iCrossLineA,iCrossLineB);
    }

    return false;
}

bool crossLineIntersected(const QLineF &aLineA, const QLineF &aLineB)
{
    if(crossLineAngleType(aLineA) != crossLineAngleType(aLineB))
    {
        return QLineF::BoundedIntersection == aLineA.intersect(aLineB,NULL);
    }

    return false;
}

void IRViewManager::paintEvent(QPaintEvent *aEvent)
{
    HbMainWindow::paintEvent(aEvent);

    if(iCrossLineShowing)
    {
        QPainter painter(viewport());
        painter.setPen(QPen(QColor(225,225,225,200),KCrossLineWidth));	
        painter.drawLine(iCrossLineA);
		painter.drawLine(iCrossLineB);
    }
}

void IRViewManager::crossLineReset()
{
    iCrossLineAReady = false;
    iCrossLineBReady = false;
    iCrossLineShowing = false;
}

void IRViewManager::exitTimeout()
{
    crossLineReset();
    viewport()->repaint();
    HbMessageBox::information(hbTrId("txt_common_info_exiting"), (QObject*)NULL, NULL);
    qApp->quit();
    iExiting = true;
}

bool IRViewManager::isExiting() const
{
    return iExiting;
}



CrossLineAngleType crossLineAngleType(const QLineF &aLine)
{
    int linePos = aLine.angle() / 90;
    if( 0==linePos || 2==linePos )
    {
        return EPositiveAngle;
    }
    else
    {
        return ENegativeAngle;
    }
}

bool crossLineReady(const QLineF &aLine)
{
    if(aLine.length()> KCrossLineMinLenth)
    {
        int lineDegree = qRound(aLine.angle()) % 90;
        return (lineDegree<=KCrossLineMaxAngle) && (lineDegree>=KCrossLineMinAngle) ? true : false;
    }
    return false;
}


