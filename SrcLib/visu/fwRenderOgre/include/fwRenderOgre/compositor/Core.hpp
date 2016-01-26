/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWRENDEROGRE_COMPOSITOR_CORE_HPP__
#define __FWRENDEROGRE_COMPOSITOR_CORE_HPP__

#include <fwCore/BaseObject.hpp>

#include <OGRE/OgreAxisAlignedBox.h>
#include <OGRE/OgreSceneManager.h>
#include <OGRE/OgreRenderWindow.h>
#include <OGRE/OgreViewport.h>
//#include <OGRE/OgreRenderTargetListener.h>
//#include <OGRE/OgreRenderObjectListener.h>

#include "fwRenderOgre/config.hpp"

#define NB_OF_TECH 5

/// OIT (Ordrer Independent Transparency) techniques supported
enum transparencyTechnique { DEFAULT, DEPTHPEELING, DUALDEPTHPEELING, WEIGHTEDBLENDEDOIT, HYBRIDTRANSPARENCY};

namespace fwRenderOgre
{

namespace compositor
{

/**
 * @class Core
 * Manages principal compositor for a layer's 3D scene
 */
class FWRENDEROGRE_CLASS_API Core : public ::fwCore::BaseObject
                                    //,
                                    //TODO : Manage occlusion query
                                    //    The current commented ligns in this class are standing for occlusion query purposes
                                    //    An example of working occlusion query is used in the experimental branch flavien_sg rev 76
                                    //public ::Ogre::FrameListener,
                                    //public ::Ogre::RenderTargetListener,
                                    //public ::Ogre::RenderObjectListener

{
public:

    fwCoreClassDefinitionsWithFactoryMacro( (Core)(::fwRenderOgre::compositor::Core), (()),
                                            new Core);
    fwCoreAllowSharedFromThis();
    //fwCoreServiceClassDefinitionsMacro( (Core)(::fwCore::BaseObject) )

    /// Default Compositor, one per "default" layer
    FWRENDEROGRE_API Core();

    /// Destructor of default compositor
    FWRENDEROGRE_API ~Core();

    /// Return the OIT selected
    FWRENDEROGRE_API transparencyTechnique getTransparencyTechnique();

    /// Return the number of peels computed by Depth Peeling or x2 Dual Depth Peeling
    FWRENDEROGRE_API int getTransparencyDepth();

    /// Set the viewport linked with default compositor
    FWRENDEROGRE_API void setViewport(::Ogre::Viewport* viewport);

    /// Set the OIT desired
    /// Deactivate OIT compositor
    FWRENDEROGRE_API bool setTransparencyTechnique(transparencyTechnique technique);

    // FIXME : this function must be private (needing occlusion queries)
    /// Set the number of peels computed by Depth Peeling or x2 Dual Depth Peeling
    /// Deactivate OIT compositor
    FWRENDEROGRE_API void setTransparencyDepth(int depth);

    //FWRENDEROGRE_API void setTransparencyMaxDepth(int depth);

    /// Allow cel shading
    /// Deactivate OIT compositor
    FWRENDEROGRE_API bool setCelShadingActivated(bool celShadingActivated);

    /// Return if cel shading is used
    FWRENDEROGRE_API bool isCelShadingActivated();

    /// Allow or disallow OIT technique depending of materials support
    FWRENDEROGRE_API void updateTechniquesSupported(::Ogre::String materialName,
                                                    std::vector< Ogre::String > schemesSupported);

    /// Re/Activate OIT compositor
    FWRENDEROGRE_API void update();

private:

    //virtual void notifyRenderSingleObject(::Ogre::Renderable* rend, const ::Ogre::Pass* pass,
    //                                      const ::Ogre::AutoParamDataSource* source,
    //                                      const ::Ogre::LightList* pLightList, bool suppressRenderStateChanges);

    /// Set number of ping pong peels for Depth Peeling compositor
    FWRENDEROGRE_API void setTransparencyDepthOfDepthPeeling(int depth);

    /// Set number of ping pong peels for Dual Depth Peeling compositor
    FWRENDEROGRE_API void setTransparencyDepthOfDualDepthPeeling(int depth);

    /// Set number of Depth Peeling ping pong peels for Hybrid Transparency compositor
    /// - other peels computed with Weighted Blended OIT
    FWRENDEROGRE_API void setTransparencyDepthOfHybridTransparency(int depth);

    /// Setup Default compositor (without OIT)
    FWRENDEROGRE_API void setupDefaultTransparency();

    /// Setup OIT current compositor
    FWRENDEROGRE_API void setupTransparency();

    /// Update list of OIT supported
    FWRENDEROGRE_API void updateTechniqueSupported();

    /// Return if an input OIT is currently supported
    FWRENDEROGRE_API bool isOITTechniqueSupported(transparencyTechnique technique);

    /// Return if cel shading is supported
    FWRENDEROGRE_API bool isCelShadingSupported();

    //FWRENDEROGRE_API void setupQueries();

    //FWRENDEROGRE_API virtual bool frameRenderingQueued(const Ogre::FrameEvent& evt);

    //int m_transparencyTechniqueMaxDepth;

    /// OIT used
    transparencyTechnique m_transparencyTechnique;

    /// OIT used - string name
    ::Ogre::String m_transparencyTechniqueName;

    /// OIT compositor instance used
    ::Ogre::CompositorInstance* m_compositorInstance;

    /// Is cel shading activated
    bool m_useCelShading;

    /// Cel shading activated
    ::Ogre::String m_celShadingName;

    //bool m_useOcclusionQuery;

    //bool m_doOcclusionQuery;

    //::Ogre::HardwareOcclusionQuery* m_OITQuery;

    //::Ogre::HardwareOcclusionQuery* m_activeQuery;

    /// Number of peels computed for Depth Peeling or 2x Dual Depth Peeling
    int m_numPass;

    //int m_currNumPass;

    //bool m_isPing;

    //bool m_isPong;

    /// Viewport linked to compositor
    ::Ogre::Viewport* m_viewport;

    /// Map of OIT techniques + cel shading supported per material
    /// DepthPeeling - DualDepthPeeling - WeightedBlended - HybridTransparency
    std::map< Ogre::String, std::array< std::pair< bool, bool >, NB_OF_TECH > > m_OITTechniquesSupportedPerMaterial;

    /// OIT techniques supported + Cel Shading supported :
    /// DepthPeeling - DualDepthPeeling - WeightedBlended - HybridTransparency
    std::array< std::pair< bool, bool >, NB_OF_TECH > m_OITTechniquesSupported;

    /// Name of the last compositor put in the compositor chain.
    /// This compositor is used to have a blend in order to get a correct final render
    static const std::string FINAL_CHAIN_COMPOSITOR;
};

} // namespace compositor

} // namespace fwRenderOgre

#endif // __FWRENDEROGRE_COMPOSITOR_CORE_HPP__
