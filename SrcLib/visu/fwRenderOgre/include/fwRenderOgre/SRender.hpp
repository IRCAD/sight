/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWRENDEROGRE_SRENDER_HPP__
#define __FWRENDEROGRE_SRENDER_HPP__

#include "fwRenderOgre/config.hpp"
#include <fwRenderOgre/IRenderWindowInteractorManager.hpp>
#include <fwRenderOgre/Layer.hpp>
#include <fwRenderOgre/Utils.hpp>
#include <fwRenderOgre/picker/IPicker.hpp>

#include <fwCom/Signal.hpp>
#include <fwCom/Slot.hpp>
#include <fwCom/Slots.hpp>
#include <fwCom/helper/SigSlotConnection.hpp>

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
 * @section XML XML Configuration
 * @code{.xml}
   <service uid="generiSceneUID" type="::fwRenderOgre::SRender" autoconnect="yes">

    <in key="meshKey" uid="meshUID" />
    <in key="meshTFKey" uid="meshTFUID" />

    <scene renderMode="auto">
        <renderer id="rendererId" layer="1" compositors="Invert;Laplace;Posterize" />

        <adaptor id="meshAdaptor" class="::visuOgreAdaptor::SMesh" objectId="meshKey">
            <config dynamic="true" transform="meshTFAdaptor" texture="texLiver"/>
        </adaptor>

        <adaptor id="transformAdaptor" class="::visuOgreAdaptor::STransform" objectId="meshTFKey">
            <config transform="meshTFAdaptor"/>
        </adaptor>

        <...>

        <connect>
            <signal>adaptorUID/modified</signal>
            <slot>serviceUid/updateTM</slot>
        </connect>

        <connect waitForKey="tm3dKey">
            <signal>modified</signal><!-- signal for object "tm3dKey" -->
            <slot>serviceUid/updateTM</slot>
        </connect>

        <proxy channel="myChannel">
            <signal>adaptor2UID/modified</signal>
            <slot>service2Uid/updateTM</slot>
        </proxy>
    </scene>
   </service>
   @endcode
 * With :
 *  - \b scene
 *    - \b renderMode (optional): 'auto' (only when something has changed) or 'always' (render continuously).
 *         Default is 'auto'.
 *  - \b adaptor
 *    - \b id (mandatory): the identifier of the adaptor
 *    - \b class (mandatory): the classname of the adaptor service
 *    - \b uid (optional): the fwID to specify for the adaptor service
 *    - \b objectId (mandatory): the key of the adaptor's object in the scene's composite. The "self" key is used
 *         when the adaptor works on the scene's composite.
 *    - \b config: adaptor's configuration. It is parsed in the adaptor's configuring() method.
 *  - \b connect/proxy : not mandatory, connects signal to slot
 *    - \b waitForKey : not mandatory, defines the required object key for the signal/slot connection
 *    - \b signal : mandatory, must be signal holder UID, followed by '/', followed by signal name. To use the
 *         object (defined by waitForKey) signal, you don't have to write object uid, only the signal name.
 *    - \b slot : mandatory, must be slot holder UID, followed by '/', followed by slot name
 *  - \b layer : mandatory, defines the scene's layer
 *    - \b id (mandatory): the identifier of the layer
 *    - \b depth (mandatory): the depth of the layer, starting from 1
 *    - \b transparency (optional): the transparency technique to use: DepthPeeling, DualDepthPeeling,
 *                                  WeightedBlended, HybridTransparency or CelShadingDepthPeeling.
 *    - \b numPeels (optional): number of peels for the selected transparency technique.
 *                              Not used for WeightedBlended OIT
 *    - \b compositors (optional): defines the default compositor chain. The compositors are separated by semicolons
 *    - \b fullscreen (optional, default="no"): Show the scene in full screen.
 *    - \b stereoMode (optional, default="no"): sets the mode used for stereoscopic 3D rendering,
 *                                          available modes are "AutoStereo5", "AutoStereo8" and "no".
 */
class FWRENDEROGRE_CLASS_API SRender : public ::fwRender::IRender

{
public:
    fwCoreServiceClassDefinitionsMacro ( (SRender)(::fwRender::IRender) );

    FWRENDEROGRE_API SRender() throw();
    FWRENDEROGRE_API virtual ~SRender() throw();

    typedef std::string AdaptorIdType;
    typedef std::string OgreObjectIdType;
    typedef std::string SceneIdType;

    /// Actives layouts in the scene
    typedef std::map< SceneIdType, SPTR(::fwRenderOgre::Layer) > LayerMapType;

    FWRENDEROGRE_API static const std::string s_OGREBACKGROUNDID;

    /**
     * @name Signal API
     * @{
     */
    typedef ::fwCom::Signal< void () > SceneStartedSignalType;
    FWRENDEROGRE_API static const ::fwCom::Signals::SignalKeyType s_SCENE_STARTED_SIG;
    /** @} */

    /**
     * @name Slots API
     * @{
     */
    typedef ::fwCom::Slot< void () > StartObjectSlotType;
    /// Slot: Start each adaptor contained in this render service
    FWRENDEROGRE_API static const ::fwCom::Slots::SlotKeyType s_START_OBJECT_SLOT;

    typedef ::fwCom::Slot< void () > ComputeCameraOrigSlotType;
    /// Slot: Computes the parameters to reset the camera
    FWRENDEROGRE_API static const ::fwCom::Slots::SlotKeyType s_COMPUTE_CAMERA_ORIG_SLOT;

    typedef ::fwCom::Slot< void () > ComputeCameraClippingSlotType;
    /// Slot: Computes the parameters to reset the camera
    FWRENDEROGRE_API static const ::fwCom::Slots::SlotKeyType s_COMPUTE_CAMERA_CLIPPING_SLOT;

    typedef ::fwCom::Slot< void (int, int, int, int) > DoRayCastSlotType;
    /// Slot: Request the picker to do a ray cast according to the passed position
    FWRENDEROGRE_API static const ::fwCom::Slots::SlotKeyType s_DO_RAY_CAST_SLOT;
    /** @} */

    /// Set this render service as the current OpenGL context
    FWRENDEROGRE_API void makeCurrent();

    /// Request a render from the Ogre render engine
    FWRENDEROGRE_API void requestRender();

    /// Returns true if the scene is shown on screen
    FWRENDEROGRE_API bool isShownOnScreen();

    /// Returns the scene manager corresponding to the sceneID
    FWRENDEROGRE_API ::Ogre::SceneManager* getSceneManager(const ::std::string& sceneID);

    /// Returns the layer corresponding to the sceneID
    FWRENDEROGRE_API ::fwRenderOgre::Layer::sptr getLayer(const ::std::string& sceneID);

    /// Returns this render layers
    FWRENDEROGRE_API LayerMapType getLayers();

    /// Returns m_interactorManager
    FWRENDEROGRE_API ::fwRenderOgre::IRenderWindowInteractorManager::sptr getInteractorManager() const;

    /// Reset camera parameters with the actual global bounding box
    FWRENDEROGRE_API void resetCameraCoordinates(const std::string& _layerId);

    /// Compute camera parameters with the actual global bounding box
    FWRENDEROGRE_API void computeCameraClipping();

    /// Return true if the ogre context is ready to be used
    FWRENDEROGRE_API bool isReady() const;

    template<class T>
    std::vector<SPTR(T)> getAdaptors() const;

protected:

    /// Renders the scene.
    FWRENDEROGRE_API void render();

    /// Configures the adaptors and the connections
    FWRENDEROGRE_API virtual void starting() throw( ::fwTools::Failed);
    /// Stops all the adaptors
    FWRENDEROGRE_API virtual void stopping() throw( ::fwTools::Failed);

    ///Configures the adaptor
    FWRENDEROGRE_API virtual void configuring() throw( ::fwTools::Failed);

    /// Does nothing.
    FWRENDEROGRE_API void updating() throw( ::fwTools::Failed);

private:

    /// Wrapper class containing an adaptor
    struct SceneAdaptor
    {
        SPTR (IAdaptor) getService() const
        {
            return m_service.lock();
        }

        WPTR(IAdaptor) m_service;
        bool m_start;
    };

    /// Actives adaptors in scene
    typedef std::map< AdaptorIdType, SceneAdaptor> SceneAdaptorsMapType;

    /// Configuration element shared pointer
    typedef ::fwRuntime::ConfigurationElement::sptr ConfigurationType;

    /// Start Ogre OpenGL context
    void startContext();
    /// Stop Ogre OpenGL context
    void stopContext();

    /// Configure background layer of the scene
    void configureBackgroundLayer( ConfigurationType conf );
    /// Configure each layer of the scene
    void configureLayer( ConfigurationType conf );
    /// Start each adaptors of the scene
    void startObject();

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

    /// True is the slot startObject has been called, so the adaptors can be started in the configureObject() method.
    bool m_startAdaptor;

    /// True if the rendering is done only when requested
    bool m_renderOnDemand;

    /// True if the render window is created.
    bool m_isReady;

    /// Map containing all adaptors
    SceneAdaptorsMapType m_adaptors;

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
        SPTR(T) adaptor = T::dynamicCast(sceneAdaptor.second.getService());
        if( adaptor )
        {
            resultVector.push_back(adaptor);
        }
    }

    return resultVector;
}

//-----------------------------------------------------------------------------

inline bool SRender::isReady() const
{
    return m_isReady;
}

//-----------------------------------------------------------------------------

}
#endif // __FWRENDEROGRE_SRENDER_HPP__
