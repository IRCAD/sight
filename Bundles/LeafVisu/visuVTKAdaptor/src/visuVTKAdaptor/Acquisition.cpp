/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */



#include <boost/foreach.hpp>

#include <fwData/Acquisition.hpp>
#include <fwData/Reconstruction.hpp>
#include <fwData/Material.hpp>
#include <fwData/Boolean.hpp>

#include <fwComEd/AcquisitionMsg.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/Factory.hpp>

#include <fwServices/registry/ObjectService.hpp>

#include <vtkActor.h>
#include <vtkPolyDataMapper.h>

#include "visuVTKAdaptor/Reconstruction.hpp"
#include "visuVTKAdaptor/Acquisition.hpp"



REGISTER_SERVICE( ::fwRenderVTK::IVtkAdaptorService, ::visuVTKAdaptor::Acquisition, ::fwData::Acquisition ) ;

namespace visuVTKAdaptor
{


Acquisition::Acquisition() throw()
{
    m_clippingPlanes = "";
    m_autoResetCamera = true;
    addNewHandledEvent("ShowReconstructions");
    addNewHandledEvent(::fwComEd::AcquisitionMsg::ADD_RECONSTRUCTION);
}

//------------------------------------------------------------------------------

Acquisition::~Acquisition() throw()
{}

//------------------------------------------------------------------------------

void Acquisition::configuring() throw(fwTools::Failed)
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

void Acquisition::doStart() throw(fwTools::Failed)
{
    this->doUpdate();
    addNewHandledEvent("ShowReconstructions");
}

//------------------------------------------------------------------------------

void Acquisition::doUpdate() throw(fwTools::Failed)
{
    ::fwData::Acquisition::sptr acq = this->getObject< ::fwData::Acquisition >();

    doStop();
    bool showRec = true;
    if (acq->getFieldSize("ShowReconstructions"))
    {
        showRec = acq->getFieldSingleElement< ::fwData::Boolean >("ShowReconstructions")->value();
    }
    ::fwData::Acquisition::ReconstructionIterator iter;

    for (iter = acq->getReconstructions().first; iter != acq->getReconstructions().second; ++iter)
    {
        ::fwRenderVTK::IVtkAdaptorService::sptr service =
                ::fwServices::add< ::fwRenderVTK::IVtkAdaptorService >
        ( *iter, "::visuVTKAdaptor::Reconstruction" );
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

void Acquisition::doSwap() throw(fwTools::Failed)
{
    this->doUpdate();
}

//------------------------------------------------------------------------------

void Acquisition::doStop() throw(fwTools::Failed)
{
    this->unregisterServices();
}

//------------------------------------------------------------------------------

void Acquisition::doUpdate( ::fwServices::ObjectMsg::csptr msg) throw(fwTools::Failed)
{
    ::fwComEd::AcquisitionMsg::csptr acquisitionMsg = ::fwComEd::AcquisitionMsg::dynamicConstCast( msg ) ;
    if ( acquisitionMsg )
    {
        if ( acquisitionMsg->hasEvent("ShowReconstructions") )
        {
            ::fwData::Acquisition::sptr acq = this->getObject< ::fwData::Acquisition >();
            bool showRec = true;
            if (acq->getFieldSize("ShowReconstructions"))
            {
                showRec = acq->getFieldSingleElement< ::fwData::Boolean >("ShowReconstructions")->value();
            }

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
        else if ( acquisitionMsg->hasEvent(::fwComEd::AcquisitionMsg::ADD_RECONSTRUCTION) )
        {
            this->doUpdate();
        }
    }
}

//------------------------------------------------------------------------------

} //namespace visuVTKAdaptor
