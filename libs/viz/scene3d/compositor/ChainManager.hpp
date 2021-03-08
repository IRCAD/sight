/************************************************************************
 *
 * Copyright (C) 2014-2021 IRCAD France
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

#include "service/IHasServices.hpp"

#include "viz/scene3d/config.hpp"

#include <data/Composite.hpp>

#include <vector>

namespace Ogre
{
class Viewport;
}

namespace sight::viz::scene3d
{
class SRender;

namespace compositor
{

/**
 * @brief   Manage the compositors in a layer view.
 */
class VIZ_SCENE3D_CLASS_API ChainManager : ::boost::noncopyable,
                                           public service::IHasServices
{
public:
    typedef std::unique_ptr < ChainManager > uptr;

    typedef std::string CompositorIdType;
    typedef std::pair<CompositorIdType, bool> CompositorType;
    typedef std::vector<CompositorType> CompositorChainType;

    VIZ_SCENE3D_API ChainManager(::Ogre::Viewport* viewport);
    VIZ_SCENE3D_API virtual ~ChainManager();

    /// Inserts the new compositor in the compositor chain vector
    VIZ_SCENE3D_API void addAvailableCompositor(CompositorIdType _compositorName);
    /// Clears the compositor chain
    VIZ_SCENE3D_API void clearCompositorChain(const std::string& _layerId,
                                              SPTR(viz::scene3d::SRender) _renderService);
    /// Enables or disables the target compositor
    VIZ_SCENE3D_API void updateCompositorState(CompositorIdType _compositorName, bool _isEnabled,
                                               const std::string& _layerId,
                                               SPTR(viz::scene3d::SRender) _renderService);

    VIZ_SCENE3D_API void setCompositorChain(const std::vector<CompositorIdType>&_compositors,
                                            const std::string& _layerId, SPTR(viz::scene3d::SRender) _renderService);

    VIZ_SCENE3D_API CompositorChainType getCompositorChain();

    VIZ_SCENE3D_API void setOgreViewport(::Ogre::Viewport* _viewport);

    /// Name of the last compositor put in the compositor chain.
    /// This compositor is used to have a blend in order to get a correct final render
    VIZ_SCENE3D_API static const CompositorIdType FINAL_CHAIN_COMPOSITOR;

    /// Class used to find a compositor by its name in the compositor chain
    class VIZ_SCENE3D_CLASS_API FindCompositorByName
    {
    public:
        FindCompositorByName(const CompositorIdType& _name) :
            compositorName(_name)
        {
        }

        //------------------------------------------------------------------------------

        bool operator()(const CompositorType& _compositor) const
        {
            return (_compositor.first == compositorName);
        }

    private:
        const CompositorIdType& compositorName;
    };

private:

    /// Adds the final compositor to the compositor chain
    void addFinalCompositor();

    void updateCompositorAdaptors(CompositorIdType _compositorName, bool _isEnabled,
                                  const std::string& _layerId, SPTR(viz::scene3d::SRender) _renderService);

    /// List of available compositors, the names are associated to a boolean value which indicates whether
    /// the compositor is enabled or not
    /// WARNING: It may not match the real list of compositors in the Ogre manager, as other classes may add
    /// compositors as well. So this list is only there to know which compositors are handled by this class.
    CompositorChainType m_compositorChain;

    /// The parent layer's viewport.
    /// The ogre's compositor manager needs it in order to access the right compositor chain.
    ::Ogre::Viewport* m_ogreViewport;

    /// Map allowing to keep the objects of the created adaptors alive
    data::Composite::sptr m_adaptorsObjectsOwner;

};

//-----------------------------------------------------------------------------
// Inline method(s)

inline ChainManager::CompositorChainType ChainManager::getCompositorChain()
{
    return m_compositorChain;
}

//-----------------------------------------------------------------------------

inline void ChainManager::setOgreViewport(::Ogre::Viewport* _viewport)
{
    m_ogreViewport = _viewport;
}

//-----------------------------------------------------------------------------

} // namespace compositor

} // namespace sight::viz::scene3d
