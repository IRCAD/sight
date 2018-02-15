/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "visuVTKAdaptor/SResection.hpp"

#include "visuVTKAdaptor/SReconstruction.hpp"

#include <fwData/Reconstruction.hpp>
#include <fwData/Resection.hpp>

#include <fwServices/macros.hpp>

fwServicesRegisterMacro( ::fwRenderVTK::IAdaptor, ::visuVTKAdaptor::SResection);

namespace visuVTKAdaptor
{

const ::fwServices::IService::KeyType SResection::s_RESECTION_INPUT = "resection";

SResection::SResection() noexcept :
    m_autoResetCamera(true)
{
}

//------------------------------------------------------------------------------

SResection::~SResection() noexcept
{
}

//------------------------------------------------------------------------------

void SResection::configuring()
{
    this->configureParams();

    const ConfigType config = this->getConfigTree().get_child("config.<xmlattr>");

    this->setClippingPlanes(config.get("clippingplanes", ""));

    const std::string autoresetcamera = config.get<std::string>("autoresetcamera", "yes");
    SLM_ASSERT("'autoresetcamera' value must be 'yes' or 'no', actual: " + autoresetcamera,
               autoresetcamera == "yes" || autoresetcamera == "no");
    m_autoResetCamera = (autoresetcamera == "yes");
}

//------------------------------------------------------------------------------

void SResection::starting()
{
    this->initialize();
    this->updating();
}

//------------------------------------------------------------------------------

void SResection::updating()
{
    this->stopping();

    ::fwData::Resection::csptr resec = this->getInput< ::fwData::Resection >(s_RESECTION_INPUT);

    if (resec->getIsVisible())
    {
        std::vector< ::fwData::Reconstruction::sptr > vReconst;
        bool resectionIsValid = resec->getIsValid();
        if(resec->getIsSafePart() || resectionIsValid)
        {
            vReconst = resec->getOutputs();
        }
        else
        {
            vReconst = resec->getInputs();
        }
        for (const ::fwData::Reconstruction::sptr& resec: vReconst)
        {
            // create the srv configuration for objects auto-connection
            auto reconstAdaptor =
                this->registerService< ::visuVTKAdaptor::SReconstruction>("::visuVTKAdaptor::SReconstruction");
            reconstAdaptor->registerInput(resec, SReconstruction::s_RECONSTRUCTION_INPUT, true);

            reconstAdaptor->setTransformId( this->getTransformId() );
            reconstAdaptor->setRendererId( this->getRendererId() );
            reconstAdaptor->setPickerId( this->getPickerId() );
            reconstAdaptor->setRenderService(this->getRenderService());
            if(!resectionIsValid)
            {
                reconstAdaptor->setClippingPlanes( m_clippingPlanes );
            }
            reconstAdaptor->setAutoResetCamera(m_autoResetCamera);
            reconstAdaptor->start();
        }
    }
    this->setVtkPipelineModified();
}

//------------------------------------------------------------------------------

void SResection::stopping()
{
    this->unregisterServices();
}

//------------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsMap SResection::getAutoConnections() const
{
    KeyConnectionsMap connections;
    connections.push(s_RESECTION_INPUT, ::fwData::Resection::s_MODIFIED_SIG, s_UPDATE_SLOT);
    connections.push(s_RESECTION_INPUT, ::fwData::Resection::s_VISIBILITY_MODIFIED_SIG, s_UPDATE_SLOT);
    connections.push(s_RESECTION_INPUT, ::fwData::Resection::s_RECONSTRUCTION_ADDED_SIG, s_UPDATE_SLOT);

    return connections;
}

//------------------------------------------------------------------------------

} //namespace visuVTKAdaptor
