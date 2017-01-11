/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <QMessageBox>

#include <XnVSessionManager.h>
#include <XnVMultiProcessFlowClient.h>

#include <fwMedData/ModelSeries.hpp>

#include <fwData/Integer.hpp>
#include <fwData/Vector.hpp>

#include <fwServices/IService.hpp>

#include "opKinect/Kinect.hpp"

namespace opKinect
{

::fwMedData::ModelSeries::sptr g_modelSeries;
::fwServices::IService::sptr g_service;


//-----------------------------------------------------------------------------
// Callbacks
//-----------------------------------------------------------------------------

// Callback for when the focus is in progress
void XN_CALLBACK_TYPE SessionProgress(const XnChar* strFocus, const XnPoint3D& ptFocusPoint, XnFloat fProgress,
                                      void* UserCxt)
{
    OSLM_TRACE("Session progress ("
               << ptFocusPoint.X << ", "
               << ptFocusPoint.Y << ", "
               << ptFocusPoint.Z << ") - "
               << fProgress << "[" <<strFocus << "]");
}

// callback for session start
void XN_CALLBACK_TYPE SessionStart(const XnPoint3D& ptFocusPoint, void* UserCxt)
{
    OSLM_TRACE("Session started. Please wave ("
               << ptFocusPoint.X << ", "
               << ptFocusPoint.Y << ", "
               << ptFocusPoint.Z << ")");
}

// Callback for session end
void XN_CALLBACK_TYPE SessionEnd(void* UserCxt)
{
    SLM_TRACE("Session ended. Please perform focus gesture to start session");
}

// Callback for wave detection
void XN_CALLBACK_TYPE OnWaveCB(void* cxt)
{
}

// callback for a new position of any hand
void XN_CALLBACK_TYPE Kinect::OnPointUpdate(const XnVHandPointContext* pContext, void* cxt)
{
    // data
    ::fwData::Vector::sptr data = ::fwData::Vector::New();
    ::fwData::Integer::sptr v1  = ::fwData::Integer::New(pContext->ptPosition.X);
    ::fwData::Integer::sptr v2  = ::fwData::Integer::New(pContext->ptPosition.Y);
    ::fwData::Integer::sptr v3  = ::fwData::Integer::New(pContext->ptPosition.Z);
    ::fwData::Integer::sptr v4  = ::fwData::Integer::New(pContext->nID);

    std::vector< ::fwData::Integer::sptr > points;
    points.push_back(v1);
    points.push_back(v2);
    points.push_back(v3);
    points.push_back(v4);

    data->setDataContainer(points);

    // Notification
    ::fwServices::ObjectMsg::sptr msg = ::fwServices::ObjectMsg::New();
    msg->addEvent("KINECT_NEW_POSITION_HAND", data);
    msg->setSource(g_service);
    msg->setSubject( g_modelSeries);
    ::fwData::Object::ObjectModifiedSignalType::sptr sig;
    sig = g_modelSeries->signal< ::fwData::Object::ObjectModifiedSignalType >(::fwData::Object::s_OBJECT_MODIFIED_SIG);
    {
        ::fwCom::Connection::Blocker block(sig->getConnection(m_slotReceive));
        sig->asyncEmit( msg);
    }
}

void XN_CALLBACK_TYPE Kinect::onPush(XnFloat fVelocity,  XnFloat fAngle, void *cxt)
{
}



/**
 * @brief Constructor
 *
 * @param service : pointer to the service
 * @param ms : pointer to model series data
 */
Kinect::Kinect(::fwServices::IService::sptr service, ::fwMedData::ModelSeries::sptr ms)
{
    // init variables
    g_modelSeries = ms;
    g_service     = service;
}

/**
 * @brief Containing the separated thread loop for the Kinect processing.
 *
 */
void Kinect::run()
{
    stopRun = false;

    // Create context
    xn::Context context;
    XnStatus rc = context.InitFromXmlFile(std::string(BUNDLE_PREFIX) + "/opKinect_0-1/Sample-Tracking.xml");
    if (rc != XN_STATUS_OK)
    {
        QMessageBox::critical(0, "Critical error", "Couldn't initialize: " + QString(xnGetStatusString(rc)));
    }

    // Create the Session Manager
    XnVSessionGenerator* pSessionGenerator;
    pSessionGenerator = new XnVSessionManager();
    rc                = ((XnVSessionManager*)pSessionGenerator)->Initialize(&context, "Click", "RaiseHand");
    if (rc != XN_STATUS_OK)
    {
        QMessageBox::critical(0, "Critical error",
                              "Session Manager couldn't initialize: " + QString(xnGetStatusString(rc)));
        delete pSessionGenerator;
    }

    // Initialization done. Start generating
    context.StartGeneratingAll();

    // Register session callbacks
    pSessionGenerator->RegisterSession(NULL, &SessionStart, &SessionEnd, &SessionProgress);

    // init & register wave control
    XnVWaveDetector wc;
    wc.RegisterWave(NULL, OnWaveCB);
    wc.RegisterPointUpdate(NULL, Kinect::OnPointUpdate);
    pSessionGenerator->AddListener(&wc);

    // init & register push control
    XnVPushDetector g_pushDetector;
    g_pushDetector.RegisterPush(NULL, Kinect::onPush);
    pSessionGenerator->AddListener(&g_pushDetector);


    // start loop
    while(!stopRun)
    {
        context.WaitAndUpdateAll();
        ((XnVSessionManager*)pSessionGenerator)->Update(&context);

        // sleep the thread 1ms
        this->msleep(1);
    }

    // Shutdown Kinect
    delete pSessionGenerator;
    context.Shutdown();
}

/**
 * @brief Stop the thread
 */
void Kinect::stop()
{
    stopRun = true;
    this->wait(100);
}

}
