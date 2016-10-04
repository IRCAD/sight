/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "videoCalibration/SFixedPointRegistration.hpp"

#include <arData/CalibrationInfo.hpp>

#include <fwCom/Signal.hxx>
#include <fwCom/Slots.hxx>

#include <fwData/Array.hpp>
#include <fwData/Composite.hpp>

#include <fwDataTools/helper/Array.hpp>

#include <fwGui/dialog/MessageDialog.hpp>

#include <fwServices/IService.hpp>
#include <fwServices/macros.hpp>

#include <arlcore/PointsList.h>


namespace videoCalibration
{
fwServicesRegisterMacro(::fwServices::IController, ::videoCalibration::SFixedPointRegistration,
                        ::fwData::TransformationMatrix3D);


// ----------------------------------------------------------------------------

SFixedPointRegistration::SFixedPointRegistration() throw ()
{
}

// ----------------------------------------------------------------------------

SFixedPointRegistration::~SFixedPointRegistration() throw ()
{
}

// ----------------------------------------------------------------------------

void SFixedPointRegistration::configuring() throw (fwTools::Failed)
{
}

// ----------------------------------------------------------------------------

void SFixedPointRegistration::starting() throw (fwTools::Failed)
{
}

// ----------------------------------------------------------------------------

void SFixedPointRegistration::swapping() throw (fwTools::Failed)
{
}

// ----------------------------------------------------------------------------

void SFixedPointRegistration::updating() throw (fwTools::Failed)
{
    auto pointList         = this->getInput< ::fwData::PointList >("pointList");
    auto calibrationMatrix = this->getInOut< ::fwData::TransformationMatrix3D >("matrix");

    ::arlCore::PointList::sptr arlPL = ::arlCore::PointList::New();

    ::fwData::PointList::PointListContainer plcontainer = pointList->getPoints();

    if (plcontainer.empty())
    {
        SLM_WARN("No marker points are defined");
        ::fwGui::dialog::MessageDialog::showMessageDialog("Empty List", "No marker points are defined.",
                                                          ::fwGui::dialog::IMessageDialog::WARNING);
        return;
    }

    //for all points in pointlist
    for(unsigned int i = 0; i< plcontainer.size(); ++i)
    {
        double x = plcontainer[i]->getCoord()[0]; //x
        double y = plcontainer[i]->getCoord()[1]; //y
        double z = plcontainer[i]->getCoord()[2]; //z
        //put in arlcore
        arlPL->push_back(x,y,z);
    }

    //Since all points of the pointlist sould be on a aproximate half sphere we can estimate the center of the sphere
    ::arlCore::Point::sptr centerEstimation = ::arlCore::Point::New();
    double radius;
    std::vector<double> params;
    std::vector<double> log;
    arlPL->sphereCenterEstimation(centerEstimation,radius,
                                  arlCore::ARLCORE_SCE_OPTIMIZATION,params, log);


    //TODO: estimation of the rotation between marker and tool tip

    //put the distance into matrix
    calibrationMatrix->setCoefficient(0,3,centerEstimation->x());
    calibrationMatrix->setCoefficient(1,3,centerEstimation->y());
    calibrationMatrix->setCoefficient(2,3,centerEstimation->z());


    OSLM_DEBUG("Sphere center : "<<centerEstimation->x()<<" ; "<<centerEstimation->y()<<" ; "<<centerEstimation->z());

    auto sig = calibrationMatrix->signal< ::fwData::TransformationMatrix3D::ModifiedSignalType >
                   (::fwData::TransformationMatrix3D::s_MODIFIED_SIG);
    sig->asyncEmit();
}

// ----------------------------------------------------------------------------

void SFixedPointRegistration::stopping() throw (fwTools::Failed)
{
}

// ----------------------------------------------------------------------------

} //namespace videoCalibration

