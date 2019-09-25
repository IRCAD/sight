/************************************************************************
 *
 * Copyright (C) 2018-2019 IRCAD France
 * Copyright (C) 2018-2019 IHU Strasbourg
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

#include <fwRenderOgre/IAdaptor.hpp>
#include <fwRenderOgre/Text.hpp>

#include <OGRE/OgreColourValue.h>

#include <memory>

namespace visuOgreAdaptor
{

class PostWindowRenderListener;

/**
 * @brief Adaptor displaying rendering statistics in the window overlay.
 *
 * Displays the last FPS and the triangle count.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
    <service uid="renderStatsAdaptor" type="::visuOgreAdaptor::SRenderStats">
        <config layer="default" color="#ff0000" height="0.02"/>
    </service>
   @endcode

 * @subsection Configuration Configuration:
 * - \b layer (mandatory) : not really used but it is needed to be bound to a render service.
 * - \b color (optional, default=#ffffff): stats' text color
 * - \b height (optional, default=0.03) : stats' text height, representing a fraction of the viewport's height.
 */
class VISUOGREADAPTOR_CLASS_API SRenderStats : public ::fwRenderOgre::IAdaptor
{
public:
    fwCoreServiceMacro(SRenderStats, ::fwRenderOgre::IAdaptor);

    /// Constructor.
    VISUOGREADAPTOR_API SRenderStats() noexcept;

    /// Destructor.
    VISUOGREADAPTOR_API virtual ~SRenderStats() noexcept override;

protected:

    /// Configures stats color and size.
    VISUOGREADAPTOR_API void configuring() override;

    /// Initializes the overlay text displaying the stats.
    VISUOGREADAPTOR_API void starting() override;

    /// Does nothing.
    VISUOGREADAPTOR_API void updating() override;

    /// Destroys the overlay text.
    VISUOGREADAPTOR_API void stopping() override;

private:

    friend class PostWindowRenderListener;

    /// Listens to frame update events and updates the stats text.
    std::unique_ptr< PostWindowRenderListener > m_listener;

    /// Displays stats in the overlay.
    ::fwRenderOgre::Text* m_statsText { nullptr };

    /// The text's color.
    ::Ogre::ColourValue m_textColor;

    /// The text's height, expressed as a fraction of the viewport height.
    float m_textHeight;
};

} //namespace visuOgreAdaptor
