/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef OPKINECT_KINECT_H
#define OPKINECT_KINECT_H

#include <fwServices/IEditionService.hpp>
#include <fwData/Acquisition.hpp>
#include <QThread>
#include <XnVWaveDetector.h>
#include <XnVPushDetector.h>

namespace opKinect
{


/**
 * @brief Manage Kinect
 */
class Kinect : public QThread
{
    Q_OBJECT

public:
    Kinect(::fwServices::IService::sptr service, fwData::Acquisition::sptr acq);
    void stop();
    void static XN_CALLBACK_TYPE onPush(XnFloat  fVelocity,  XnFloat  fAngle, void *cxt);
    void static XN_CALLBACK_TYPE OnPointUpdate(const XnVHandPointContext* pContext, void* cxt);

private:
    void run();

    /**
     * @brief Flag to request the thread to stop
     */
    bool stopRun; 
};

}

#endif
