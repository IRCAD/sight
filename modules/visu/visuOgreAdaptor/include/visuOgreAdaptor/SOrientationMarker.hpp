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

#include "visuOgreAdaptor/config.hpp"

#include <fwData/Mesh.hpp>

#include <fwRenderOgre/IAdaptor.hpp>

namespace visuOgreAdaptor
{

/**
 * @brief TODO
 *
 * @section Slots Slots
 *
 * @section XML XML Configuration
 * @code{.xml}
    <service uid="..." type="::visuOgreAdaptor::SOrientationMarker">
        <config layer="default" transform="transformUID" length="30" label="true" />
    </service>
   @endcode
 *
 * @subsection Configuration Configuration:
 * - \b layer (mandatory, string): defines the mesh's layer.
 */
class VISUOGREADAPTOR_CLASS_API SOrientationMarker final : public ::fwRenderOgre::IAdaptor
{

public:

    /// Generates default methods as New, dynamicCast, ...
    fwCoreServiceMacro(SOrientationMarker, ::fwRenderOgre::IAdaptor)

    /// TODO
    VISUOGREADAPTOR_API SOrientationMarker() noexcept;

    /// TODO
    VISUOGREADAPTOR_API ~SOrientationMarker() noexcept override;

protected:

    /// TODO
    VISUOGREADAPTOR_API void configuring() override;

    /// TODO
    VISUOGREADAPTOR_API void starting() override;

    /// TODO
    VISUOGREADAPTOR_API void updating() override;

    /// TODO
    VISUOGREADAPTOR_API void stopping() override;

    /**
     * @brief TODO
     * @param TODO
     */
    VISUOGREADAPTOR_API void setVisible(bool _visible) override;

private:

    ::fwData::Mesh::sptr m_mesh { nullptr };

};

} // namespace visuOgreAdaptor.
