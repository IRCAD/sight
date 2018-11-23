/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "uiVisuOgre/config.hpp"

#include <fwGui/IActionSrv.hpp>

#include <fwRenderOgre/Layer.hpp>

namespace uiVisuOgre
{

/**
 * @brief Action to enable/disable stereo in an ogre scene layer.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
    <service uid="..." type="::uiVisuOgre::SStereoToggler" >
        <config layer="default" stereoMode="interlaced" />
    </service>
 *
 * @subsection Configuration Configuration
 * - \b layer : layer in which to activate stereo rendering.
 * - \b stereoMode (values=interlaced|AutoStereo5|AutoStereo8): mode to activate.
 *
 */
class UIVISUOGRE_CLASS_API SStereoToggler : public ::fwGui::IActionSrv
{
public:

    fwCoreServiceClassDefinitionsMacro( (SStereoToggler)(::fwGui::IActionSrv) )

    /// Constructor.
    UIVISUOGRE_API SStereoToggler();

    /// Destructor.
    UIVISUOGRE_API virtual ~SStereoToggler() override;

protected:

    /// Configures the service.
    UIVISUOGRE_API virtual void configuring() override;

    /// Starts the action service.
    UIVISUOGRE_API virtual void starting() override;

    /// Enables stereo if active, disables it otherwise.
    UIVISUOGRE_API virtual void updating() override;

    /// Stops the action service.
    UIVISUOGRE_API virtual void stopping() override;

private:

    /// Layer in which we enable/disable stereo.
    std::string m_layerId;

    /// Mode that is toggled.
    ::fwRenderOgre::Layer::StereoModeType m_stereoMode { ::fwRenderOgre::Layer::StereoModeType::NONE };

};

} // namespace uiVisuOgre
