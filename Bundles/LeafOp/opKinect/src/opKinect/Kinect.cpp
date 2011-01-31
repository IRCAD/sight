#include "opKinect/Kinect.hpp"
#include "fwData/Integer.hpp"
#include "fwData/Vector.hpp"
#include <QMessageBox>
#include <XnVSessionManager.h>
#include <XnVMultiProcessFlowClient.h>


namespace opKinect
{

fwData::Acquisition::sptr g_acquisition;
fwServices::IService::sptr g_service;


//-----------------------------------------------------------------------------
// Callbacks
//-----------------------------------------------------------------------------

// Callback for when the focus is in progress
void XN_CALLBACK_TYPE SessionProgress(const XnChar* strFocus, const XnPoint3D& ptFocusPoint, XnFloat fProgress, void* UserCxt)
{
	//printf("Session progress (%6.2f,%6.2f,%6.2f) - %6.2f [%s]\n", ptFocusPoint.X, ptFocusPoint.Y, ptFocusPoint.Z, fProgress,  strFocus);
}
// callback for session start
void XN_CALLBACK_TYPE SessionStart(const XnPoint3D& ptFocusPoint, void* UserCxt)
{
	//printf("Session started. Please wave (%6.2f,%6.2f,%6.2f)...\n", ptFocusPoint.X, ptFocusPoint.Y, ptFocusPoint.Z);
}
// Callback for session end
void XN_CALLBACK_TYPE SessionEnd(void* UserCxt)
{
	//printf("Session ended. Please perform focus gesture to start session\n");
}
// Callback for wave detection
void XN_CALLBACK_TYPE OnWaveCB(void* cxt)
{
	//printf("Wave!\n");
}
// callback for a new position of any hand
void XN_CALLBACK_TYPE Kinect::OnPointUpdate(const XnVHandPointContext* pContext, void* cxt)
{
     // data
    ::fwData::Vector::NewSptr data;
    ::fwData::Integer::NewSptr v1(pContext->ptPosition.X);
    ::fwData::Integer::NewSptr v2(pContext->ptPosition.Y);
    ::fwData::Integer::NewSptr v3(pContext->ptPosition.Z);
    ::fwData::Integer::NewSptr v4(pContext->nID);
    data->push_back(v1);
    data->push_back(v2);
    data->push_back(v3);
    data->push_back(v4);

    // Notification
    ::fwServices::ObjectMsg::NewSptr msg;
    msg->addEvent("KINECT_NEW_POSITION_HAND", data);
    ::fwServices::IEditionService::notify(g_service, g_acquisition, msg);
}

void XN_CALLBACK_TYPE Kinect::onPush(XnFloat  fVelocity,  XnFloat  fAngle, void *cxt)
{
    //printf("push\n");
}



/**
 * @brief Constructor
 *
 * @param service : pointer to the service
 * @param acq : pointer to acquisition data
 */
Kinect::Kinect(::fwServices::IService::sptr service, fwData::Acquisition::sptr acq)
{
    // init variables
    g_acquisition = acq;
    g_service = service;
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
	XnStatus rc = context.InitFromXmlFile("./Bundles/opKinect_0-1/Sample-Tracking.xml");
	if (rc != XN_STATUS_OK)
	{
		QMessageBox::critical(0, "Critical error", "Couldn't initialize: " + QString(xnGetStatusString(rc)));
	}

	// Create the Session Manager
    XnVSessionGenerator* pSessionGenerator;
	pSessionGenerator = new XnVSessionManager();
	rc = ((XnVSessionManager*)pSessionGenerator)->Initialize(&context, "Click", "RaiseHand");
	if (rc != XN_STATUS_OK)
	{
		QMessageBox::critical(0, "Critical error", "Session Manager couldn't initialize: " + QString(xnGetStatusString(rc)));
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
    while(!stopRun) {
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
