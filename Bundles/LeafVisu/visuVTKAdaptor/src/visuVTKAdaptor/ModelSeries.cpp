/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "visuVTKAdaptor/ModelSeries.hpp"

#include "visuVTKAdaptor/Mesh.hpp"
#include "visuVTKAdaptor/Reconstruction.hpp"
#include "visuVTKAdaptor/Texture.hpp"

#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>

#include <fwData/Boolean.hpp>
#include <fwData/Material.hpp>
#include <fwData/Mesh.hpp>
#include <fwData/Reconstruction.hpp>

#include <fwMedData/ModelSeries.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/op/Add.hpp>

#include <vtkActor.h>
#include <vtkPolyDataMapper.h>

fwServicesRegisterMacro( ::fwRenderVTK::IAdaptor, ::visuVTKAdaptor::ModelSeries, ::fwMedData::ModelSeries );

namespace visuVTKAdaptor
{

const ::fwCom::Signals::SignalKeyType ModelSeries::s_TEXTURE_APPLIED_SIG = "textureApplied";

const ::fwCom::Slots::SlotKeyType ModelSeries::s_UPDATE_NORMAL_MODE_SLOT   = "updateNormalMode";
const ::fwCom::Slots::SlotKeyType ModelSeries::s_SHOW_RECONSTRUCTIONS_SLOT = "showReconstructions";

//------------------------------------------------------------------------------

ModelSeries::ModelSeries() noexcept :
    m_sigTextureApplied(TextureAppliedSignalType::New()),
    m_autoResetCamera(true)
{
    m_clippingPlanes = "";

    ::fwCom::HasSignals::m_signals(s_TEXTURE_APPLIED_SIG, m_sigTextureApplied);

    m_slotUpdateNormalMode    = ::fwCom::newSlot(&ModelSeries::updateNormalMode, this);
    m_slotShowReconstructions = ::fwCom::newSlot(&ModelSeries::showReconstructions, this);
    ::fwCom::HasSlots::m_slots(s_UPDATE_NORMAL_MODE_SLOT, m_slotUpdateNormalMode)
        (s_SHOW_RECONSTRUCTIONS_SLOT, m_slotShowReconstructions);

    ::fwCom::HasSlots::m_slots.setWorker( m_associatedWorker );
}

//------------------------------------------------------------------------------

ModelSeries::~ModelSeries() noexcept
{
}

//------------------------------------------------------------------------------

void ModelSeries::doConfigure()
{
    SLM_TRACE_FUNC();

    SLM_ASSERT("Configuration must begin with <config>", m_configuration->getName() == "config");

    this->setClippingPlanes( m_configuration->getAttributeValue("clippingplanes") );

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

void ModelSeries::doStart()
{
    this->doUpdate();
}

//------------------------------------------------------------------------------

void ModelSeries::doUpdate()
{
    ::fwMedData::ModelSeries::sptr modelSeries = this->getObject< ::fwMedData::ModelSeries >();

    doStop();

    // doStop() disconnects everything, we have to restore connection with the data
    m_connections.connect(this->getObject(), this->getSptr(), this->getObjSrvConnections());

    bool showRec;
    showRec = modelSeries->getField("ShowReconstructions", ::fwData::Boolean::New(true))->value();

    if(!m_textureAdaptorUID.empty())
    {
        ::fwRenderVTK::SRender::sptr renderService     = this->getRenderService();
        ::fwRenderVTK::IAdaptor::sptr adaptor          = renderService->getAdaptor(m_textureAdaptorUID);
        ::visuVTKAdaptor::Texture::sptr textureAdaptor = ::visuVTKAdaptor::Texture::dynamicCast(adaptor);

        SLM_ASSERT("textureAdaptor is NULL", textureAdaptor);
        m_connections.connect(this->getSptr(), s_TEXTURE_APPLIED_SIG, textureAdaptor,
                              ::visuVTKAdaptor::Texture::s_APPLY_TEXTURE_SLOT);
    }

    for( ::fwData::Reconstruction::sptr reconstruction :  modelSeries->getReconstructionDB() )
    {
        ::fwRenderVTK::IAdaptor::sptr service =
            ::fwServices::add< ::fwRenderVTK::IAdaptor >
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
            SLM_ASSERT("Missing material adaptor", !servicesVector.empty());

            ::visuVTKAdaptor::Mesh::sptr meshAdaptor = ::visuVTKAdaptor::Mesh::dynamicCast(*servicesVector.begin());

            ::fwData::Material::sptr material = meshAdaptor->getMaterial();
            SLM_ASSERT("Missing material", material);

            m_sigTextureApplied->asyncEmit(material);
        }
    }
}

//------------------------------------------------------------------------------

void ModelSeries::doSwap()
{
    this->doUpdate();
}

//------------------------------------------------------------------------------

void ModelSeries::doStop()
{
    m_connections.disconnect();
    this->unregisterServices();
}

//------------------------------------------------------------------------------

void ModelSeries::updateNormalMode(std::uint8_t mode, std::string recID)
{
    for( ServiceVector::value_type service :  m_subServices)
    {
        if(!service.expired())
        {
            ::visuVTKAdaptor::Reconstruction::sptr renconstructionAdaptor
                = ::visuVTKAdaptor::Reconstruction::dynamicCast(service.lock());
            if (renconstructionAdaptor && renconstructionAdaptor->getObject()->getID() == recID)
            {
                renconstructionAdaptor->updateNormalMode(mode);
                break;
            }
        }
    }
}

//------------------------------------------------------------------------------

void ModelSeries::showReconstructions(bool show)
{
    ::fwMedData::ModelSeries::sptr modelSeries = this->getObject< ::fwMedData::ModelSeries >();

    for( ServiceVector::value_type service :  m_subServices)
    {
        if(!service.expired())
        {
            ::visuVTKAdaptor::Reconstruction::sptr renconstructionAdaptor
                = ::visuVTKAdaptor::Reconstruction::dynamicCast(service.lock());
            if (renconstructionAdaptor)
            {
                renconstructionAdaptor->setForceHide( !show );
            }
        }
    }
    this->setVtkPipelineModified();
    this->requestRender();
}

//------------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsType ModelSeries::getObjSrvConnections() const
{
    KeyConnectionsType connections;
    connections.push_back( std::make_pair( ::fwMedData::ModelSeries::s_MODIFIED_SIG, s_UPDATE_SLOT ) );
    connections.push_back( std::make_pair( ::fwMedData::ModelSeries::s_RECONSTRUCTIONS_ADDED_SIG, s_UPDATE_SLOT ) );
    connections.push_back( std::make_pair( ::fwMedData::ModelSeries::s_RECONSTRUCTIONS_REMOVED_SIG, s_UPDATE_SLOT ) );

    return connections;
}

//------------------------------------------------------------------------------

} //namespace visuVTKAdaptor
