/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "visuOgreAdaptor/config.hpp"

#include <fwRenderOgre/IAdaptor.hpp>
#include <fwRenderOgre/Text.hpp>

#include <OGRE/OgreColourValue.h>

#include <memory>

namespace visuOgreAdaptor
{

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
    fwCoreServiceClassDefinitionsMacro((SRenderStats)(::fwRenderOgre::IAdaptor));

    /// Constructor.
    VISUOGREADAPTOR_API SRenderStats() noexcept;

    /// Destructor.
    VISUOGREADAPTOR_API virtual ~SRenderStats() noexcept;

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

    friend class PreWindowRenderListener;

    /// Listens to frame update events and updates the stats text.
    std::unique_ptr< PreWindowRenderListener > m_listener;

    /// Displays stats in the overlay.
    ::fwRenderOgre::Text* m_statsText { nullptr };

    /// The text's color.
    ::Ogre::ColourValue m_textColor;

    /// The text's height, expressed as a fraction of the viewport height.
    float m_textHeight;
};

} //namespace visuOgreAdaptor
