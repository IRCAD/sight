/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/foreach.hpp>

#include <fwData/Reconstruction.hpp>
#include <fwData/Material.hpp>
#include <fwData/Boolean.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/op/Add.hpp>
#include <fwServices/registry/ObjectService.hpp>

#include <fwMedData/ImageSeries.hpp>

#include <fwComEd/AcquisitionMsg.hpp>

#include <vtkActor.h>
#include <vtkPolyDataMapper.h>

#include "visuVTKAdaptor/Reconstruction.hpp"
#include "visuVTKAdaptor/ImageSeries.hpp"



fwServicesRegisterMacro( ::fwRenderVTK::IVtkAdaptorService, ::visuVTKAdaptor::ImageSeries, ::fwMedData::ImageSeries ) ;

namespace visuVTKAdaptor
{


ImageSeries::ImageSeries() throw()
{
    m_clippingPlanes = "";
    m_autoResetCamera = true;
    //addNewHandledEvent(::fwComEd::ImageSeriesMsg::SHOW_RECONSTRUCTIONS);
    //addNewHandledEvent(::fwComEd::ImageSeriesMsg::ADD_RECONSTRUCTION);
    //addNewHandledEvent(::fwComEd::ImageSeriesMsg::REMOVED_RECONSTRUCTIONS);
}

//------------------------------------------------------------------------------

ImageSeries::~ImageSeries() throw()
{}

//------------------------------------------------------------------------------

void ImageSeries::configuring() throw(fwTools::Failed)
{
    SLM_TRACE_FUNC();

    assert(m_configuration->getName() == "config");
    this->setPickerId( m_configuration->getAttributeValue("picker") );
    this->setRenderId( m_configuration->getAttributeValue("renderer") );

    if(m_configuration->hasAttribute("transform") )
    {
        this->setTransformId( m_configuration->getAttributeValue("transform") );
    }
}

//------------------------------------------------------------------------------

void ImageSeries::doStart() throw(fwTools::Failed)
{
    this->doUpdate();
}

//------------------------------------------------------------------------------

void ImageSeries::doUpdate() throw(fwTools::Failed)
{
    ::fwMedData::ImageSeries::sptr series = this->getObject< ::fwMedData::ImageSeries >();

    doStop();

    ::fwData::Image::sptr image = series->getImage();

    ::fwRenderVTK::IVtkAdaptorService::sptr service =
            ::fwServices::add< ::fwRenderVTK::IVtkAdaptorService >( image, "::visuVTKAdaptor::NegatoMPR" );
    SLM_ASSERT("service not instanced", service);

    service->setTransformId( this->getTransformId() );
    service->setRenderId( this->getRenderId() );
    service->setPickerId( this->getPickerId() );
    service->setRenderService(this->getRenderService());
    service->start();

    this->registerService(service);
}

//------------------------------------------------------------------------------

void ImageSeries::doSwap() throw(fwTools::Failed)
{
    this->doUpdate();
}

//------------------------------------------------------------------------------

void ImageSeries::doStop() throw(fwTools::Failed)
{
    this->unregisterServices();
}

//------------------------------------------------------------------------------

void ImageSeries::doReceive( ::fwServices::ObjectMsg::csptr msg) throw(fwTools::Failed)
{

}

//------------------------------------------------------------------------------

} //namespace visuVTKAdaptor
