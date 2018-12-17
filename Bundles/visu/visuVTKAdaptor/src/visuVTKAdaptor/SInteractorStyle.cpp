/************************************************************************
 *
 * Copyright (C) 2009-2017 IRCAD France
 * Copyright (C) 2012-2017 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

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

    const ConfigType config = this->getConfigTree().get_child("config.<xmlattr>");

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
    this->setInteractorStyle(interactor);
}

//------------------------------------------------------------------------------

void SInteractorStyle::updating()
{
}

//------------------------------------------------------------------------------

void SInteractorStyle::stopping()
{
    this->setInteractorStyle(nullptr);
}

//------------------------------------------------------------------------------

void SInteractorStyle::setInteractorStyle(vtkInteractorStyle* interactor)
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
