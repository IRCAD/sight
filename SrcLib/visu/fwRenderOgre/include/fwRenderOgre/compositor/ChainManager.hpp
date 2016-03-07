/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWRENDEROGRE_COMPOSITOR_CHAINMANAGER_HPP__
#define __FWRENDEROGRE_COMPOSITOR_CHAINMANAGER_HPP__

//      Suppress when tests over
// used to view the content of the Ogre Compositor Chain
#include <OGRE/OgreCompositorChain.h>
#include <OGRE/OgreCompositor.h>



#include <fwCore/BaseObject.hpp>

#include <vector>

#include "fwRenderOgre/config.hpp"

namespace Ogre
{
class CompositorManager;
class Viewport;
}

namespace fwRenderOgre
{

namespace compositor
{

class FWRENDEROGRE_CLASS_API ChainManager : public ::fwCore::BaseObject
{
public:

    fwCoreClassDefinitionsWithFactoryMacro( (ChainManager)(::fwRenderOgre::compositor::ChainManager), (()),
                                            new ChainManager);
    fwCoreAllowSharedFromThis();

    typedef std::string CompositorIdType;
    typedef std::pair<CompositorIdType, bool> CompositorType;
    typedef std::vector<CompositorType> CompositorChainType;

    FWRENDEROGRE_API ChainManager();
    FWRENDEROGRE_API ChainManager(::Ogre::Viewport* ogreViewport);

    /// Inserts the new compositor in the compositor chain vector
    FWRENDEROGRE_API void addAvailableCompositor(CompositorIdType compositorName);
    /// Clears the compositor chain
    FWRENDEROGRE_API void clearCompositorChain();
    /// Enables or disables the target compositor
    FWRENDEROGRE_API void updateCompositorState(CompositorIdType compositorName, bool isEnabled);

    FWRENDEROGRE_API void setCompositorChain(std::vector<CompositorIdType> compositors);

    FWRENDEROGRE_API CompositorChainType getCompositorChain();

    FWRENDEROGRE_API void setOgreViewport(::Ogre::Viewport* viewport);

    /// Name of the last compositor put in the compositor chain.
    /// This compositor is used to have a blend in order to get a correct final render
    FWRENDEROGRE_API static const CompositorIdType FINAL_CHAIN_COMPOSITOR;

    /// Class used to find a compositor by its name in the compositor chain
    class FWRENDEROGRE_CLASS_API FindCompositorByName
    {
    public:
        FindCompositorByName(CompositorIdType name)
        {
            compositorName = name;
        }

        bool operator()(const CompositorType& compositor) const
        {
            return (compositor.first == compositorName);
        }

    private:
        CompositorIdType compositorName;
    };

private:

    /// Adds the final compositor to the compositor chain
    void addFinalCompositor();

    /// Getter for the Ogre CompositorManager
    ::Ogre::CompositorManager* getCompositorManager();

    /// List of available compositors, the names are associated to a boolean value which indicates whether
    /// the compositor is enabled or not
    CompositorChainType m_compositorChain;

    /// The parent layer's viewport.
    /// The ogre's compositor manager needs it in order to access the right compositor chain.
    ::Ogre::Viewport* m_ogreViewport;
};

//-----------------------------------------------------------------------------
// Inline method(s)

inline ChainManager::CompositorChainType ChainManager::getCompositorChain()
{
    return m_compositorChain;
}

//-----------------------------------------------------------------------------

inline void ChainManager::setOgreViewport(::Ogre::Viewport* viewport)
{
    m_ogreViewport = viewport;
}

//-----------------------------------------------------------------------------

} // namespace compositor

} // namespace fwRenderOgre

#endif // __FWRENDEROGRE_COMPOSITOR_CHAINMANAGER_HPP__
