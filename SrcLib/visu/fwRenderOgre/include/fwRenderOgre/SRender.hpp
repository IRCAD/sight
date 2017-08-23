/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWRENDEROGRE_SRENDER_HPP__
#define __FWRENDEROGRE_SRENDER_HPP__

#include "fwRenderOgre/config.hpp"
#include "fwRenderOgre/IRenderWindowInteractorManager.hpp"
#include "fwRenderOgre/Layer.hpp"
#include "fwRenderOgre/picker/IPicker.hpp"
#include "fwRenderOgre/Utils.hpp"

#include <fwCom/helper/SigSlotConnection.hpp>
#include <fwCom/Signal.hpp>
#include <fwCom/Slot.hpp>
#include <fwCom/Slots.hpp>

#include <fwData/Composite.hpp>

#include <fwRender/IRender.hpp>

#include <fwRuntime/ConfigurationElement.hpp>

#include <fwServices/helper/Config.hpp>

#include <OGRE/OgreAxisAlignedBox.h>

#include <map>

namespace fwRenderOgre
{

class IAdaptor;
class Layer;

/**
 * @brief The generic scene service shows adaptors in a 3D Ogre scene.
 *
 * @section XML XML Configuration
 * @code{.xml}
    <service uid="..." type="::fwRenderOgre::SRender" autoconnect="yes">
        <scene renderMode="auto">
            <layer id="rendererId" depth="1" compositors="Invert;Laplace;Posterize" defaultLight="no" />

            <adaptor uid="meshAdaptor"/>
            <adaptor uid="transformAdaptor"/>
        </scene>
    </service>
   @endcode
 * With :
 *  - \b scene
 *    - \b renderMode (optional): 'auto' (only when something has changed) or 'always' (render continuously).
 *         Default is 'auto'.
 *  - \b layer : mandatory, defines the scene's layer
 *    - \b id (mandatory): the identifier of the layer
 *    - \b depth (mandatory): the depth of the layer, starting from 1
 *    - \b transparency (optional): the transparency technique to use: DepthPeeling, DualDepthPeeling,
 *                                  WeightedBlended, HybridTransparency or CelShadingDepthPeeling.
 *    - \b numPeels (optional): number of peels for the selected transparency technique.
 *                              Not used for WeightedBlended OIT
 *    - \b compositors (optional): defines the default compositor chain. The compositors are separated by semicolons
 *    - \b fullscreen (optional, yes/no, default="no"): Show the scene in full screen.
 *    - \b stereoMode (optional, yes/no, default="no"): Sets the mode used for stereoscopic 3D rendering,
 *                                          available modes are "AutoStereo5", "AutoStereo8" and "no".
 *    - \b defaultLight (optional, yes/no, default="yes"): Sets if a default light is created in the layer.
 *  - \b adaptor
 *    - \b uid (mandatory): the identifier of the adaptor
 */
class FWRENDEROGRE_CLASS_API SRender : public ::fwRender::IRender

{
public:
    fwCoreServiceClassDefinitionsMacro( (SRender)(::fwRender::IRender) );

    FWRENDEROGRE_API SRender() noexcept;
    FWRENDEROGRE_API virtual ~SRender() noexcept;

    typedef std::string AdaptorIdType;
    typedef std::string OgreObjectIdType;
    typedef std::string SceneIdType;

    /// Actives layouts in the scene.
    typedef std::map< SceneIdType, SPTR(::fwRenderOgre::Layer) > LayerMapType;

    FWRENDEROGRE_API static const std::string s_OGREBACKGROUNDID;

    /**
     * @name Slots API
     * @{
     */
    typedef ::fwCom::Slot< void () > ComputeCameraOrigSlotType;
    /// Slot: Computes the parameters to reset the camera.
    FWRENDEROGRE_API static const ::fwCom::Slots::SlotKeyType s_COMPUTE_CAMERA_ORIG_SLOT;

    typedef ::fwCom::Slot< void () > ComputeCameraClippingSlotType;
    /// Slot: Computes the parameters to reset the camera.
    FWRENDEROGRE_API static const ::fwCom::Slots::SlotKeyType s_COMPUTE_CAMERA_CLIPPING_SLOT;

    typedef ::fwCom::Slot< void (int, int, int, int) > DoRayCastSlotType;
    /// Slot: Request the picker to do a ray cast according to the passed position.
    FWRENDEROGRE_API static const ::fwCom::Slots::SlotKeyType s_DO_RAY_CAST_SLOT;

    typedef ::fwCom::Slot< void () > RequestRenderSlotType;
    /// Slot: Request a rendering.
    FWRENDEROGRE_API static const ::fwCom::Slots::SlotKeyType s_REQUEST_RENDER_SLOT;
    /** @} */

    /// Sets this render service as the current OpenGL context.
    FWRENDEROGRE_API void makeCurrent();

    /// Requests a render from the Ogre render engine.
    FWRENDEROGRE_API void requestRender();

    /// Returns true if the scene is shown on screen.
    FWRENDEROGRE_API bool isShownOnScreen();

    /// Returns the scene manager corresponding to the sceneID.
    FWRENDEROGRE_API ::Ogre::SceneManager* getSceneManager(const ::std::string& sceneID);

    /// Returns the layer corresponding to the sceneID.
    FWRENDEROGRE_API ::fwRenderOgre::Layer::sptr getLayer(const ::std::string& sceneID);

    /// Returns this render layers.
    FWRENDEROGRE_API LayerMapType getLayers();

    /// Returns m_interactorManager.
    FWRENDEROGRE_API ::fwRenderOgre::IRenderWindowInteractorManager::sptr getInteractorManager() const;

    /// Resets camera parameters with the actual global bounding box.
    FWRENDEROGRE_API void resetCameraCoordinates(const std::string& _layerId);

    /// Computes camera parameters with the actual global bounding box.
    FWRENDEROGRE_API void computeCameraClipping();

    template<class T>
    std::vector<SPTR(T)> getAdaptors() const;

protected:

    /// Renders the scene.
    FWRENDEROGRE_API void render();

    /// Configures the adaptors and the connections
    FWRENDEROGRE_API virtual void starting();
    /// Stops all the adaptors
    FWRENDEROGRE_API virtual void stopping();

    ///Configures the adaptor
    FWRENDEROGRE_API virtual void configuring();

    /// Does nothing.
    FWRENDEROGRE_API void updating();

private:

    /// Configuration element shared pointer
    typedef ::fwRuntime::ConfigurationElement::sptr ConfigurationType;

    /// Configure background layer of the scene
    void configureBackgroundLayer( const ConfigType& _cfg );
    /// Configure each layer of the scene
    void configureLayer(const ConfigType& _cfg );

    /// Execute a ray cast with a ray built from (x,y) point, which is the mouse position
    void doRayCast(int x, int y, int width, int height);

    /// Contains the scene configuration which is the scene xml node
    ConfigurationType m_sceneConfiguration;

    /// Contains all the layers of the scene
    LayerMapType m_layers;

    /// Signal/ Slot connection
    ::fwCom::helper::SigSlotConnection m_connections;

    /// Ogre window interactor manager
    ::fwRenderOgre::IRenderWindowInteractorManager::sptr m_interactorManager;

    /// Ogre root
    ::Ogre::Root* m_ogreRoot;

    /// Show or not ogre overlay
    bool m_showOverlay;

    /// True if the rendering is done only when requested
    bool m_renderOnDemand;

    /// Contain all adaptors
    std::map< AdaptorIdType, WPTR(IAdaptor)> m_adaptors;

    /// True if the render window is in fullscreen.
    bool m_fullscreen;
};

//-----------------------------------------------------------------------------

template<class T>
std::vector<SPTR(T)> SRender::getAdaptors() const
{
    std::vector<SPTR(T)> resultVector;

    for(auto& sceneAdaptor : m_adaptors)
    {
        SPTR(T) adaptor = T::dynamicCast(sceneAdaptor.second.lock());
        if( adaptor )
        {
            resultVector.push_back(adaptor);
        }
    }

    return resultVector;
}

//-----------------------------------------------------------------------------

}
#endif // __FWRENDEROGRE_SRENDER_HPP__
