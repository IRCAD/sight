/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "fwRenderOgre/config.hpp"

#include "fwServices/IHasServices.hpp"

#include <fwData/Composite.hpp>

#include <vector>

namespace Ogre
{
class Viewport;
}

namespace fwRenderOgre
{
class SRender;

namespace compositor
{

/**
 * @brief   Manage the compositors in a layer view.
 */
class FWRENDEROGRE_CLASS_API ChainManager : ::boost::noncopyable,
                                            public ::fwServices::IHasServices
{
public:
    typedef std::unique_ptr < ChainManager > uptr;

    typedef std::string CompositorIdType;
    typedef std::pair<CompositorIdType, bool> CompositorType;
    typedef std::vector<CompositorType> CompositorChainType;

    FWRENDEROGRE_API ChainManager(::Ogre::Viewport* viewport);
    FWRENDEROGRE_API virtual ~ChainManager();

    /// Inserts the new compositor in the compositor chain vector
    FWRENDEROGRE_API void addAvailableCompositor(CompositorIdType _compositorName);
    /// Clears the compositor chain
    FWRENDEROGRE_API void clearCompositorChain(const std::string& _layerId,
                                               SPTR(::fwRenderOgre::SRender) _renderService);
    /// Enables or disables the target compositor
    FWRENDEROGRE_API void updateCompositorState(CompositorIdType _compositorName, bool _isEnabled,
                                                const std::string& _layerId,
                                                SPTR(::fwRenderOgre::SRender) _renderService);

    FWRENDEROGRE_API void setCompositorChain(const std::vector<CompositorIdType>&_compositors,
                                             const std::string& _layerId, SPTR(::fwRenderOgre::SRender) _renderService);

    FWRENDEROGRE_API CompositorChainType getCompositorChain();

    FWRENDEROGRE_API void setOgreViewport(::Ogre::Viewport* _viewport);

    /// Name of the last compositor put in the compositor chain.
    /// This compositor is used to have a blend in order to get a correct final render
    FWRENDEROGRE_API static const CompositorIdType FINAL_CHAIN_COMPOSITOR;

    /// Class used to find a compositor by its name in the compositor chain
    class FWRENDEROGRE_CLASS_API FindCompositorByName
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
                                  const std::string& _layerId, SPTR(::fwRenderOgre::SRender) _renderService);

    /// List of available compositors, the names are associated to a boolean value which indicates whether
    /// the compositor is enabled or not
    /// WARNING: It may not match the real list of compositors in the Ogre manager, as other classes may add
    /// compositors as well. So this list is only there to know which compositors are handled by this class.
    CompositorChainType m_compositorChain;

    /// The parent layer's viewport.
    /// The ogre's compositor manager needs it in order to access the right compositor chain.
    ::Ogre::Viewport* m_ogreViewport;

    /// Map allowing to keep the objects of the created adaptors alive
    ::fwData::Composite::sptr m_adaptorsObjectsOwner;

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

} // namespace fwRenderOgre
