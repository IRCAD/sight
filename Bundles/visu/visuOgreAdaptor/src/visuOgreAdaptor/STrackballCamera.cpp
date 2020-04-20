/************************************************************************
 *
 * Copyright (C) 2019-2020 IRCAD France
 * Copyright (C) 2019-2020 IHU Strasbourg
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

#include "visuOgreAdaptor/STrackballCamera.hpp"

namespace visuOgreAdaptor
{

static const std::string s_PRIORITY_CONFIG = "priority";

//-----------------------------------------------------------------------------

STrackballCamera::STrackballCamera() noexcept
{

}

//-----------------------------------------------------------------------------

STrackballCamera::~STrackballCamera() noexcept
{

}

//-----------------------------------------------------------------------------

void STrackballCamera::configuring()
{
    this->configureParams();

    const ConfigType configType = this->getConfigTree();
    const ConfigType config     = configType.get_child("config.<xmlattr>");

    m_priority = config.get<int>(s_PRIORITY_CONFIG, m_priority);
}

//-----------------------------------------------------------------------------

void STrackballCamera::starting()
{
    this->initialize();

    const auto layer = this->getLayer();
    m_trackball = std::make_shared< ::fwRenderOgre::interactor::TrackballInteractor >(layer);

    layer->addInteractor(m_trackball, m_priority);
}

//-----------------------------------------------------------------------------

void STrackballCamera::updating() noexcept
{

}

//-----------------------------------------------------------------------------

void STrackballCamera::stopping()
{
    const auto layer = this->getLayer();
    layer->removeInteractor(m_trackball);
}

//-----------------------------------------------------------------------------

} // namespace visuOgreAdaptor.
