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

#include <fwRender/IRender.hpp>

#include <fwRuntime/ConfigurationElement.hpp>

#include <fwServices/helper/Config.hpp>
#include <fwServices/macros.hpp>
#include <fwServices/op/Add.hpp>
#include <fwServices/registry/ObjectService.hpp>

#include <QColor>

namespace fwGuiQt
{
namespace container
{
class QtContainer;
}
}

namespace Qt3DExtras
{
class Qt3DWindow;
}

namespace fwRenderQt3D
{

class GenericScene;

/**
 * @brief Renders a generic scene with Qt3D API
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
     <service type="::fwRenderQt3D::SRender">
        <scene>
            <background color="...">
        </scene>
     </service>
   @endcode

   @subsection Configuration Configuration
 *  - \b scene
 *    - \b background (optionnal)
 *      - \b color : Background color. Must be defined in hexadecimal format or with a color name accepted by QColor
 */

class FWRENDERQT3D_CLASS_API SRender : public ::fwRender::IRender
{

public:
    fwCoreServiceMacro(SRender, ::fwRender::IRender)

    FWRENDERQT3D_API SRender() noexcept;
    FWRENDERQT3D_API virtual ~SRender() noexcept;

    FWRENDERQT3D_API void setQtContainer(SPTR(::fwGuiQt::container::QtContainer) _qtContainer);
    FWRENDERQT3D_API void set3DView(Qt3DExtras::Qt3DWindow* _3dView);
    FWRENDERQT3D_API void setScene(::fwRenderQt3D::GenericScene* _scene);

    FWRENDERQT3D_API SPTR(::fwGuiQt::container::QtContainer) getQtContainer();
    FWRENDERQT3D_API Qt3DExtras::Qt3DWindow* get3DView();
    FWRENDERQT3D_API ::fwRenderQt3D::GenericScene* getScene();

protected:
    FWRENDERQT3D_API void configuring() override;

    FWRENDERQT3D_API void starting() override;

    FWRENDERQT3D_API void updating() override;

    FWRENDERQT3D_API void stopping() override;

private:
    SPTR(::fwGuiQt::container::QtContainer) m_qtContainer;
    Qt3DExtras::Qt3DWindow* m_3dView;
    ::fwRenderQt3D::GenericScene* m_scene;

    QColor m_backgroundColor;
};

} //namespace fwRenderQt3D