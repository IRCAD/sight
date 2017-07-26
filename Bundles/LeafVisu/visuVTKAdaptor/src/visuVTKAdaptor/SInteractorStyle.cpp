/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "visuVTKAdaptor/SInteractorStyle.hpp"

#include <fwRenderVTK/IInteractorStyle.hpp>

#include <fwServices/macros.hpp>

#include <vtkInstantiator.h>
#include <vtkInteractorStyleImage.h>
#include <vtkRenderWindowInteractor.h>

fwServicesRegisterMacro( ::fwRenderVTK::IAdaptor, ::visuVTKAdaptor::SInteractorStyle);

namespace visuVTKAdaptor
{

//------------------------------------------------------------------------------

SInteractorStyle::SInteractorStyle() noexcept :
    m_interactorStyle(nullptr)
{
}

//------------------------------------------------------------------------------

SInteractorStyle::~SInteractorStyle() noexcept
{
    SLM_ASSERT("SInteractorStyle should be NULL", !m_interactorStyle);
}

//------------------------------------------------------------------------------

void SInteractorStyle::configuring()
{
    this->configureParams();

    const ConfigType config = this->getConfigTree().get_child("service.config.<xmlattr>");

    SLM_ASSERT("Missing attribute 'style'", config.count("style"));
    m_configuredStyle = config.get<std::string>("style");
}

//------------------------------------------------------------------------------

void SInteractorStyle::starting()
{
    this->initialize();

    vtkObject* objectStyle         = vtkInstantiator::CreateInstance(m_configuredStyle.c_str());
    vtkInteractorStyle* interactor = vtkInteractorStyle::SafeDownCast(objectStyle);
    SLM_ASSERT("InsteractorStyle adaptor is waiting for a vtkInteractorStyle object, but '"
               + m_configuredStyle + "' has been given.", interactor);
    this->setSInteractorStyle(interactor);
}

//------------------------------------------------------------------------------

void SInteractorStyle::updating()
{
}

//------------------------------------------------------------------------------

void SInteractorStyle::stopping()
{
    this->setSInteractorStyle(nullptr);
}

//------------------------------------------------------------------------------

void SInteractorStyle::setSInteractorStyle(vtkInteractorStyle* interactor)
{
    if ( m_interactorStyle != nullptr )
    {
        m_interactorStyle->Delete();
        m_interactorStyle = nullptr;
    }

    ::fwRenderVTK::IInteractorStyle* fwInteractor = dynamic_cast< ::fwRenderVTK::IInteractorStyle* >(interactor);
    if(fwInteractor)
    {
        fwInteractor->setAutoRender(this->getAutoRender());
    }

    m_interactorStyle = interactor;

    this->getInteractor()->SetInteractorStyle(nullptr);
    this->getInteractor()->SetInteractorStyle(m_interactorStyle);
    this->setVtkPipelineModified();
}

//------------------------------------------------------------------------------

} //namespace visuVTKAdaptor
