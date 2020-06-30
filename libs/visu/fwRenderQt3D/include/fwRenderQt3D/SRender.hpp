/************************************************************************
 *
 * Copyright (C) 2020 IRCAD France
 * Copyright (C) 2020 IHU Strasbourg
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

#pragma once

#include "fwRenderQt3D/config.hpp"
#include "fwRenderQt3D/IRenderWindowInteractorManager.hpp"

#include <fwRender/IRender.hpp>

#include <fwRuntime/ConfigurationElement.hpp>

#include <fwServices/helper/Config.hpp>
#include <fwServices/macros.hpp>
#include <fwServices/op/Add.hpp>
#include <fwServices/registry/ObjectService.hpp>

#include <QColor>
#include <QPointer>

namespace Qt3DExtras
{
class Qt3DWindow;
}

namespace fwRenderQt3D
{

namespace core
{
class GenericScene;
}

/**
 * @brief Renders a generic scene with Qt3D API
 *
 * @section XML XML Configuration
 * @code{.xml}
     <service type="::fwRenderQt3D::SRender">
        <scene>
            <background color="...">
        </scene>
     </service>
   @endcode
 *
 * @subsection Configuration Configuration
 *  - \b scene (mandatory)
 *    - \b background (optionnal)
 *      - \b color (optional, string, default=#000000): background color. Must be defined in hexadecimal format or with
 * a string name accepted by QColor
 *    - \b adaptor (optional)
 *      - \b uid (mandatory): the identifier of the adaptor
 */
class FWRENDERQT3D_CLASS_API SRender final : public ::fwRender::IRender
{

public:

    fwCoreServiceMacro(SRender, ::fwRender::IRender)

    /// Creates the service.
    FWRENDERQT3D_API SRender();

    /// Destroys the service.
    FWRENDERQT3D_API ~SRender() override;

    /// @returns the scene instantiated by this service.
    FWRENDERQT3D_API ::fwRenderQt3D::core::GenericScene* getScene();

    /// Updates Qt container.
    FWRENDERQT3D_API void setQtContainer(::fwGuiQt::container::QtContainer::sptr _qtContainer);

    template<class T>
    std::vector<SPTR(T)> getAdaptors() const;

private:

    /// Configures the render service.
    void configuring() override;

    /// Creates a rendering context and instantiates a Qt3D generic scene.
    void starting() override;

    /// Does nothing.
    void updating() override;

    /// Destroys the service.
    void stopping() override;

    /// Contains the 3D view.
    QPointer< Qt3DExtras::Qt3DWindow > m_3dView;

    /// Contains the generic scene associated to this service.
    QPointer< ::fwRenderQt3D::core::GenericScene > m_scene;

    /// Specifies the background color.
    QColor m_backgroundColor;

};

//------------------------------------------------------------------------------

template<class T>
std::vector<SPTR(T)> SRender::getAdaptors() const
{
    auto servicesVector = ::fwServices::OSR::getServices("::fwRenderQt3D::IAdaptor");
    std::vector<SPTR(T)> resultVector;

    for(auto& sceneAdaptor : servicesVector)
    {
        SPTR(T) adaptor = T::dynamicCast(sceneAdaptor);
        if(adaptor)
        {
            if(adaptor->getRenderService() == this->getConstSptr())
            {
                resultVector.push_back(adaptor);
            }
        }
    }

    return resultVector;
}

} //namespace fwRenderQt3D
