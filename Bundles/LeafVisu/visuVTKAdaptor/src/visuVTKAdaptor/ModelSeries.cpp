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

#include <fwMedData/ModelSeries.hpp>

#include <fwComEd/ModelSeriesMsg.hpp>

#include <vtkActor.h>
#include <vtkPolyDataMapper.h>

#include "visuVTKAdaptor/Reconstruction.hpp"
#include "visuVTKAdaptor/ModelSeries.hpp"



fwServicesRegisterMacro( ::fwRenderVTK::IVtkAdaptorService, ::visuVTKAdaptor::ModelSeries, ::fwMedData::ModelSeries ) ;

namespace visuVTKAdaptor
{


ModelSeries::ModelSeries() throw()
{
    m_clippingPlanes = "";
    m_autoResetCamera = true;
}

//------------------------------------------------------------------------------

ModelSeries::~ModelSeries() throw()
{}

//------------------------------------------------------------------------------

void ModelSeries::configuring() throw(fwTools::Failed)
{
    SLM_TRACE_FUNC();

    assert(m_configuration->getName() == "config");
    this->setPickerId( m_configuration->getAttributeValue("picker") );
    this->setRenderId( m_configuration->getAttributeValue("renderer") );

    this->setClippingPlanes( m_configuration->getAttributeValue("clippingplanes") );

    if(m_configuration->hasAttribute("transform") )
    {
        this->setTransformId( m_configuration->getAttributeValue("transform") );
    }

    if (m_configuration->hasAttribute("autoresetcamera") )
    {
        std::string autoresetcamera = m_configuration->getAttributeValue("autoresetcamera");
        m_autoResetCamera = (autoresetcamera == "yes");
    }
}

//------------------------------------------------------------------------------

void ModelSeries::doStart() throw(fwTools::Failed)
{
    this->doUpdate();
}

//------------------------------------------------------------------------------

void ModelSeries::doUpdate() throw(fwTools::Failed)
{
    ::fwMedData::ModelSeries::sptr modelSeries = this->getObject< ::fwMedData::ModelSeries >();

    doStop();
    bool showRec;
    showRec = modelSeries->getField("ShowReconstructions", ::fwData::Boolean::New(true))->value();

    BOOST_FOREACH( ::fwData::Reconstruction::sptr reconstruction, modelSeries->getReconstructionDB() )
    {
        ::fwRenderVTK::IVtkAdaptorService::sptr service =
                ::fwServices::add< ::fwRenderVTK::IVtkAdaptorService >
        ( reconstruction, "::visuVTKAdaptor::Reconstruction" );
        SLM_ASSERT("service not instanced", service);

        service->setTransformId( this->getTransformId() );
        service->setRenderId( this->getRenderId() );
        service->setPickerId( this->getPickerId() );
        service->setRenderService(this->getRenderService());
        ::visuVTKAdaptor::Reconstruction::sptr renconstructionAdaptor =
                ::visuVTKAdaptor::Reconstruction::dynamicCast(service);
        renconstructionAdaptor->setClippingPlanes( m_clippingPlanes );
        renconstructionAdaptor->setAutoResetCamera(m_autoResetCamera);
        service->start();
        renconstructionAdaptor->setForceHide( !showRec );

        this->registerService(service);
    }
}

//------------------------------------------------------------------------------

void ModelSeries::doSwap() throw(fwTools::Failed)
{
    this->doUpdate();
}

//------------------------------------------------------------------------------

void ModelSeries::doStop() throw(fwTools::Failed)
{
    this->unregisterServices();
}

//------------------------------------------------------------------------------

void ModelSeries::doReceive( ::fwServices::ObjectMsg::csptr msg) throw(fwTools::Failed)
{
    if ( msg->hasEvent(::fwComEd::ModelSeriesMsg::SHOW_RECONSTRUCTIONS) )
    {
        ::fwMedData::ModelSeries::sptr modelSeries = this->getObject< ::fwMedData::ModelSeries >();
        bool showRec;
        showRec = modelSeries->getField("ShowReconstructions", ::fwData::Boolean::New(true))->value();

        BOOST_FOREACH( ServiceVector::value_type service, m_subServices)
        {
            if(!service.expired())
            {
                ::visuVTKAdaptor::Reconstruction::sptr renconstructionAdaptor
                 = ::visuVTKAdaptor::Reconstruction::dynamicCast(service.lock());
                if (renconstructionAdaptor)
                {
                    renconstructionAdaptor->setForceHide( !showRec );
                }
            }
        }

        OSLM_INFO( "Receive event ShowReconstruction : " << showRec );
        this->setVtkPipelineModified();
    }
    else if ( msg->hasEvent(::fwComEd::ModelSeriesMsg::ADD_RECONSTRUCTION) )
    {
        this->doUpdate();
    }
    else if ( msg->hasEvent(::fwComEd::ModelSeriesMsg::REMOVED_RECONSTRUCTIONS) )
    {
        this->doStop();
    }

}

//------------------------------------------------------------------------------

} //namespace visuVTKAdaptor
