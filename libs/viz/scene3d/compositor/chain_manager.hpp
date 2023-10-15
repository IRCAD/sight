/************************************************************************
 *
 * Copyright (C) 2014-2023 IRCAD France
 * Copyright (C) 2014-2018 IHU Strasbourg
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

#include "service/has_services.hpp"

#include "viz/scene3d/config.hpp"

#include <data/object.hpp>

#include <map>
#include <vector>

namespace Ogre
{

class Viewport;

} // namespace Ogre

namespace sight::viz::scene3d
{

class Layer;

namespace compositor
{

/**
 * @brief   Manages the compositors in a layer view.
 */
class VIZ_SCENE3D_CLASS_API chain_manager : boost::noncopyable,
                                            public service::has_services
{
public:

    typedef std::unique_ptr<chain_manager> uptr;

    typedef std::string CompositorIdType;
    typedef std::pair<CompositorIdType, bool> CompositorType;
    typedef std::vector<CompositorType> CompositorChainType;

    VIZ_SCENE3D_API chain_manager(const SPTR(viz::scene3d::Layer)& _layer);
    VIZ_SCENE3D_API ~chain_manager() override;

    /// Inserts the new compositor in the compositor chain vector
    VIZ_SCENE3D_API void addAvailableCompositor(CompositorIdType _compositorName);
    /// Clears the compositor chain
    VIZ_SCENE3D_API void clearCompositorChain();
    /// Enables or disables the target compositor
    VIZ_SCENE3D_API void updateCompositorState(CompositorIdType _compositorName, bool _isEnabled);

    VIZ_SCENE3D_API void setCompositorChain(const std::vector<CompositorIdType>& _compositors);

    VIZ_SCENE3D_API CompositorChainType getCompositorChain();

    /// Name of the last compositor put in the compositor chain.
    /// This compositor is used to have a blend in order to get a correct final render
    VIZ_SCENE3D_API static const CompositorIdType FINAL_CHAIN_COMPOSITOR;

private:

    /// Adds the final compositor to the compositor chain
    void addFinalCompositor();

    void updateCompositorAdaptors(CompositorIdType _compositorName, bool _isEnabled);

    /// List of available compositors, the names are associated to a boolean value which indicates whether
    /// the compositor is enabled or not
    /// WARNING: It may not match the real list of compositors in the Ogre manager, as other classes may add
    /// compositors as well. So this list is only there to know which compositors are handled by this class.
    CompositorChainType m_compositorChain;

    /// The parent layer's.
    WPTR(viz::scene3d::Layer) m_layer;

    /// Map allowing to keep the objects of the created adaptors alive
    std::map<std::string, data::object::sptr> m_adaptorsObjectsOwner;
};

//-----------------------------------------------------------------------------
// Inline method(s)

inline chain_manager::CompositorChainType chain_manager::getCompositorChain()
{
    return m_compositorChain;
}

//-----------------------------------------------------------------------------

} // namespace compositor

} // namespace sight::viz::scene3d
