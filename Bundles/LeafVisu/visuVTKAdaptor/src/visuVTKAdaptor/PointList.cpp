/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */



#include <boost/foreach.hpp>

#include <fwData/PointList.hpp>
#include <fwData/Reconstruction.hpp>
#include <fwData/Material.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/Factory.hpp>

#include <fwServices/ObjectServiceRegistry.hpp>

#include <vtkCubeSource.h>
#include <vtkActor.h>
#include <vtkPolyDataMapper.h>

#include "visuVTKAdaptor/PointList.hpp"



REGISTER_SERVICE( ::fwRenderVTK::IVtkAdaptorService, ::visuVTKAdaptor::PointList, ::fwData::PointList ) ;

namespace visuVTKAdaptor
{


PointList::PointList() throw()
{
    handlingEventOff();
}

PointList::~PointList() throw()
{

}


void PointList::configuring() throw(fwTools::Failed)
{

    SLM_TRACE_FUNC();

    assert(m_configuration->getName() == "config");
    this->setPickerId( m_configuration->getAttributeValue("picker") );
    this->setRenderId( m_configuration->getAttributeValue("renderer") );
}

void PointList::doStart() throw(fwTools::Failed)
{
    this->doUpdate();
}

void PointList::doUpdate() throw(fwTools::Failed)
{
    this->doStop();

    ::fwData::PointList::sptr ptList = this->getObject< ::fwData::PointList >();

    BOOST_FOREACH( ::fwData::Point::sptr pt, ptList->getPoints() )
    {
        ::fwRenderVTK::IVtkAdaptorService::sptr service =
            ::fwServices::add< ::fwRenderVTK::IVtkAdaptorService >
                ( pt, "::visuVTKAdaptor::Point" );
        assert(service);

        service->setRenderService(this->getRenderService());
        service->setRenderId( this->getRenderId() );
        service->setPickerId( this->getPickerId() );
        service->start();

        this->registerService(service);
    }
}

void PointList::doSwap() throw(fwTools::Failed)
{
    this->doUpdate();
}

void PointList::doStop() throw(fwTools::Failed)
{
    this->unregisterServices();
}



} //namespace visuVTKAdaptor
