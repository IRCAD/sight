/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "uiMeasurement/action/AddDistance.hpp"

#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>
#include <fwCom/Signals.hpp>

#include <fwCore/base.hpp>

#include <fwData/Boolean.hpp>
#include <fwData/Image.hpp>
#include <fwData/Point.hpp>
#include <fwData/PointList.hpp>
#include <fwData/Vector.hpp>

#include <fwDataTools/fieldHelper/Image.hpp>

#include <fwServices/macros.hpp>

#include <exception>


namespace uiMeasurement
{
namespace action
{

fwServicesRegisterMacro( ::fwGui::IActionSrv, ::uiMeasurement::action::AddDistance, ::fwData::Image );


//------------------------------------------------------------------------------


AddDistance::AddDistance( ) throw() :
    m_actionCheckId(-1)
{
}

//------------------------------------------------------------------------------

AddDistance::~AddDistance() throw()
{
}

//------------------------------------------------------------------------------

void AddDistance::info(std::ostream& _sstream )
{
    _sstream << "Action for remove distance" << std::endl;
}

//------------------------------------------------------------------------------

void AddDistance::updating() throw(::fwTools::Failed)
{
    SLM_TRACE("AddDistance::updating");
    ::fwData::Image::sptr image = this->getObject< ::fwData::Image >();
    if (!image)
    {
        return;
    }

    ::fwData::Point::sptr pt1 = ::fwData::Point::New();
    std::copy( image->getOrigin().begin(),  image->getOrigin().begin() +3, pt1->getRefCoord().begin() );

    ::fwData::Point::sptr pt2 = ::fwData::Point::New();
    std::copy( image->getSize().begin(),  image->getSize().begin() +3, pt2->getRefCoord().begin() );

    std::transform( pt2->getRefCoord().begin(),pt2->getRefCoord().end(),
                    image->getSpacing().begin(),
                    pt2->getRefCoord().begin(),
                    std::multiplies<double>() );
    std::transform( pt2->getRefCoord().begin(),pt2->getRefCoord().end(),
                    image->getOrigin().begin(),
                    pt2->getRefCoord().begin(),
                    std::plus<double>() );

    ::fwData::PointList::sptr pl = ::fwData::PointList::New();

    pl->getRefPoints().push_back( pt1 );
    pl->getRefPoints().push_back( pt2 );

    ::fwData::Vector::sptr vectDist;
    vectDist = image->setDefaultField(::fwDataTools::fieldHelper::Image::m_imageDistancesId, ::fwData::Vector::New());

    vectDist->getContainer().push_back(pl);

    // force distance to be shown
    image->setField("ShowDistances",  ::fwData::Boolean::New(true));

    auto sig = image->signal< ::fwData::Image::DistanceAddedSignalType >(::fwData::Image::s_DISTANCE_ADDED_SIG);
    sig->asyncEmit(pl);
}

//------------------------------------------------------------------------------

void AddDistance::configuring() throw (::fwTools::Failed)
{
    this->::fwGui::IActionSrv::initialize();
}

//------------------------------------------------------------------------------

void AddDistance::starting() throw (::fwTools::Failed)
{
    this->::fwGui::IActionSrv::actionServiceStarting();
}

//------------------------------------------------------------------------------

void AddDistance::stopping() throw (::fwTools::Failed)
{
    this->::fwGui::IActionSrv::actionServiceStopping();
}

//------------------------------------------------------------------------------

} // namespace action

} // namespace uiMeasurement
