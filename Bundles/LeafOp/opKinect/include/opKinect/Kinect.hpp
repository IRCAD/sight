/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __OPKINECT_KINECT_HPP__
#define __OPKINECT_KINECT_HPP__

#include <fwServices/IEditionService.hpp>
#include <QThread>

#include <XnVWaveDetector.h>
#include <XnVPushDetector.h>

namespace fwMedData
{
class ModelSeries;
}

namespace fwServices
{
class IService;
}

namespace opKinect
{

/**
 * @brief Manage Kinect
 */
class Kinect : public QThread
{
Q_OBJECT

public:
    Kinect(SPTR(::fwServices::IService)service, SPTR(::fwMedData::ModelSeries)ms);
    void stop();
    void static XN_CALLBACK_TYPE onPush(XnFloat fVelocity,  XnFloat fAngle, void *cxt);
    void static XN_CALLBACK_TYPE OnPointUpdate(const XnVHandPointContext* pContext, void* cxt);

private:
    void run();

    /**
     * @brief Flag to request the thread to stop
     */
    bool stopRun;
};

}

#endif //__OPKINECT_KINECT_HPP__
