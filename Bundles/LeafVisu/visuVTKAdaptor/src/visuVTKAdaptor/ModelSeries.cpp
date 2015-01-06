/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/foreach.hpp>

#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>

#include <fwData/Boolean.hpp>
#include <fwData/Material.hpp>
#include <fwData/Mesh.hpp>
#include <fwData/Reconstruction.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/op/Add.hpp>
#include <fwServices/registry/ObjectService.hpp>

#include <fwMedData/ModelSeries.hpp>

#include <fwComEd/ModelSeriesMsg.hpp>

#include <vtkActor.h>
#include <vtkPolyDataMapper.h>

#include "visuVTKAdaptor/Reconstruction.hpp"
#include "visuVTKAdaptor/Mesh.hpp"
#include "visuVTKAdaptor/ModelSeries.hpp"
#include "visuVTKAdaptor/Texture.hpp"



fwServicesRegisterMacro( ::fwRenderVTK::IVtkAdaptorService, ::visuVTKAdaptor::ModelSeries, ::fwMedData::ModelSeries ) ;

namespace visuVTKAdaptor
{

const ::fwCom::Signals::SignalKeyType ModelSeries::s_TEXTURE_APPLIED_SIG = "textureApplied";

ModelSeries::ModelSeries() throw() :
    m_sigTextureApplied(TextureAppliedSignalType::New())
{
    m_clippingPlanes = "";
    m_autoResetCamera = true;

    ::fwCom::HasSignals::m_signals(s_TEXTURE_APPLIED_SIG, m_sigTextureApplied);

#ifdef COM_LOG
   ::fwCom::HasSignals::m_signals.setID();
#endif
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

    if ( m_configuration->hasAttribute("texture") )
    {
        m_textureAdaptorUID = m_configuration->getAttributeValue("texture");
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

    // doStop() disconnects everything, we have to restore connection with the data
    m_connections->connect(this->getObject(), this->getSptr(), this->getObjSrvConnections());

    bool showRec;
    showRec = modelSeries->getField("ShowReconstructions", ::fwData::Boolean::New(true))->value();

    if(!m_textureAdaptorUID.empty())
    {
        ::fwRenderVTK::VtkRenderService::sptr renderService = this->getRenderService();
        ::fwRenderVTK::IVtkAdaptorService::sptr adaptor = renderService->getAdaptor(m_textureAdaptorUID);
        ::visuVTKAdaptor::Texture::sptr textureAdaptor = ::visuVTKAdaptor::Texture::dynamicCast(adaptor);

        SLM_ASSERT("textureAdaptor is NULL", textureAdaptor);
        m_connections->connect(this->getSptr(), s_TEXTURE_APPLIED_SIG, textureAdaptor,
                               ::visuVTKAdaptor::Texture::s_APPLY_TEXTURE_SLOT);
    }

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
        service->setAutoRender( this->getAutoRender() );
        ::visuVTKAdaptor::Reconstruction::sptr renconstructionAdaptor =
                ::visuVTKAdaptor::Reconstruction::dynamicCast(service);
        renconstructionAdaptor->setClippingPlanes( m_clippingPlanes );
        renconstructionAdaptor->setAutoResetCamera(m_autoResetCamera);
        service->start();
        renconstructionAdaptor->setForceHide( !showRec );

        this->registerService(service);


        if(!m_textureAdaptorUID.empty())
        {
            ::fwData::Mesh::sptr mesh = reconstruction->getMesh();
            SLM_ASSERT("Missing mesh", mesh);

            ::fwServices::registry::ObjectService::ServiceVectorType servicesVector;
            servicesVector = ::fwServices::OSR::getServices(mesh, "::visuVTKAdaptor::Mesh");

            ::visuVTKAdaptor::Mesh::sptr meshAdaptor = ::visuVTKAdaptor::Mesh::dynamicCast(servicesVector[0]);

            ::fwData::Material::sptr material = meshAdaptor->getMaterial();
            SLM_ASSERT("Missing material", material);

            fwServicesNotifyMacro( meshAdaptor->getLightID(), m_sigTextureApplied, (material));
        }
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
    m_connections->disconnect();
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
