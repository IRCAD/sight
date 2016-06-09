/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWRENDEROGRE_SRENDER_HPP__
#define __FWRENDEROGRE_SRENDER_HPP__

#include <fwCom/Slot.hpp>
#include <fwCom/Slots.hpp>
#include <fwCom/Signal.hpp>

#include <fwData/Composite.hpp>

#include <fwRender/IRender.hpp>

#include <fwRenderOgre/picker/IPicker.hpp>
#include <fwRenderOgre/IRenderWindowInteractorManager.hpp>
#include <fwRenderOgre/Layer.hpp>
#include <fwRenderOgre/Utils.hpp>

#include <fwRuntime/ConfigurationElement.hpp>

#include <fwServices/helper/Config.hpp>
#include <fwServices/helper/SigSlotConnection.hpp>

#include <OGRE/OgreAxisAlignedBox.h>

#include <map>

#include "fwRenderOgre/config.hpp"

namespace fwRenderOgre
{

class IAdaptor;

/**
 * @class SRender
 * @brief The generic scene service shows adaptors in a 3D Ogre scene.
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
    typedef std::map< std::string, ::fwData::Object::csptr > ConstObjectMapType;

    /// Actives layouts in the scene
    typedef std::map< SceneIdType, ::fwRenderOgre::Layer::sptr > LayerMapType;

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

    /// Returns the adaptor corresponding to the given id
    FWRENDEROGRE_API SPTR (IAdaptor) getAdaptor(AdaptorIdType adaptorId);

    /// Returns the scene manager corresponding to the sceneID
    FWRENDEROGRE_API ::Ogre::SceneManager* getSceneManager(::std::string sceneID = "default");

    /// Returns the layer corresponding to the sceneID
    FWRENDEROGRE_API ::fwRenderOgre::Layer::sptr getLayer(::std::string sceneID = "default");

    /// Returns this render layers
    FWRENDEROGRE_API LayerMapType getLayers();

    /// Returns m_interactorManager
    FWRENDEROGRE_API ::fwRenderOgre::IRenderWindowInteractorManager::sptr getInteractorManager();

    /// Reset camera parameters with the actual global bounding box
    FWRENDEROGRE_API void resetCameraCoordinates(const std::string& _layerId);

    /// Compute camera parameters with the actual global bounding box
    FWRENDEROGRE_API void computeCameraClipping();

    /**
     * @brief Returns proposals to connect service slots to associated object signals,
     * this method is used for obj/srv auto connection
     *
     * Connects Composite::s_ADDED_OBJECTS_SIG to this::s_ADD_OBJECTS_SLOT
     * Connects Composite::s_CHANGED_OBJECTS_SIG to this::s_CHANGE_OBJECTS_SLOT
     * Connects Composite::s_REMOVED_OBJECTS_SIG to this::s_REMOVE_OBJECTS_SLOT
     */
    FWRENDEROGRE_API virtual KeyConnectionsType getObjSrvConnections() const;

    /**
     * @brief Returns proposals to connect service slots to associated object signals,
     * this method is used for obj/srv auto connection
     *
     * Connect Composite::s_ADDED_OBJECTS_SIG to this::s_UPDATE_OBJECTS_SLOT
     * Connect Composite::s_CHANGED_OBJECTS_SIG to this::s_UPDATE_OBJECTS_SLOT
     * Connect Composite::s_REMOVED_OBJECTS_SIG to this::s_UPDATE_OBJECTS_SLOT
     */
    FWRENDEROGRE_API ::fwServices::IService::KeyConnectionsMap getAutoConnections() const;

    /// TEMP: Function to grab the composite while we maintain appXml and appXml2
    FWRENDEROGRE_API ::fwData::Composite::sptr getComposite();

protected:

    /// Renders the scene.
    FWRENDEROGRE_API void render();

    /// Configures the adaptors and the connections
    FWRENDEROGRE_API virtual void starting() throw( ::fwTools::Failed);
    /// Stops all the adaptors
    FWRENDEROGRE_API virtual void stopping() throw( ::fwTools::Failed);

    /**
     * @brief Configures the adaptor
     * @code{.xml}
       <service uid="generiSceneUID" impl="::fwRenderOgre::SRender" type="::fwRender::IRender" autoconnect="yes">
        <scene renderMode="auto">
            <renderer id="rendererId" layer="1" compositors="Invert;Laplace;Posterize" />

            <adaptor id="meshAdaptor" class="::visuOgreAdaptor::SMesh" objectId="meshKey">
                <config dynamic="true" transform="meshTFAdaptor" texture="texLiver"/>
            </adaptor>

            <adaptor id="transformAdaptor" class="::visuOgreAdaptor::STransform" objectId="meshTF">
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
     *  - \b renderer : mandatory, defines the scene's layer
     *    - \b id (mandatory): the identifier of the layer
     *    - \b layer (mandatory): the depth of the layer, starting from 1
     *    - \b transparency (optional): the transparency technique to use: DepthPeeling, DualDepthPeeling,
     *                                  WeightedBlended, HybridTransparency or CelShadingDepthPeeling.
     *    - \b numPeels (optional): number of peels for the selected transparency technique.
     *                              Not used for WeightedBlended OIT
     *    - \b compositors (optional): defines the default compositor chain. The compositors are separated by semicolons
     */
    FWRENDEROGRE_API virtual void configuring() throw( ::fwTools::Failed);

    /// Does nothing.
    FWRENDEROGRE_API void updating() throw( ::fwTools::Failed);

private:

    /// Wrapper class containing an adaptor
    class SceneAdaptor
    {
    public:

        SPTR (IAdaptor) getService() const
        {
            return m_service.lock();
        }

        ConfigurationType m_config;
        WPTR(IAdaptor) m_service;

    };

    /// Actives adaptors in scene
    typedef std::map< AdaptorIdType, SceneAdaptor > SceneAdaptorsMapType;
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
    /// Configure each adaptors of the scene
    void configureObject( ConfigurationType conf );
    /// Start each adaptors of the scene
    void startObject();

    /// Creates the connection if the required key is contained in the composite
    void connectAfterWait(::fwData::Composite::ContainerType objects);

    /// Creates the connection given by the configuration for obj associated with the key in the composite.
    void manageConnection(const std::string &key, const ::fwData::Object::csptr &obj,
                          const ConfigurationType &config);

    /// Creates the proxy given by the configuration for obj associated with the key in the composite.
    void manageProxy(const std::string &key, const ::fwData::Object::csptr &obj,
                     const ConfigurationType &config);

    /// Disconnects the connection based on a object key
    template< class ContainerType >
    void disconnect( const ContainerType& objects );

    /// Execute a ray cast with a ray built from (x,y) point, which is the mouse position
    void doRayCast(int x, int y, int width, int height);

    /// Slot: add objects
    void addObjects(::fwData::Composite::ContainerType objects);

    /// Slot: change objects
    void changeObjects(::fwData::Composite::ContainerType newObjects, ::fwData::Composite::ContainerType oldObjects);

    /// Slot: remove objects
    void removeObjects(::fwData::Composite::ContainerType objects);

    /// Configure the objects
    void configureObjects(::fwData::Composite::ContainerType objects);

    /// Contains all the adaptors of the scene
    SceneAdaptorsMapType m_sceneAdaptors;

    /// Contains the scene configuration which is the scene xml node
    ConfigurationType m_sceneConfiguration;

    /// Contains all the layers of the scene
    LayerMapType m_layers;

    /// Signal/ Slot connection
    ::fwServices::helper::SigSlotConnection::sptr m_connections;

    /// Map to register proxy connections
    ::fwServices::helper::Config::ProxyConnectionsMapType m_proxyMap;

    typedef std::vector< ConfigurationType > ConnectConfigType;
    /// vector containing all the connections configurations
    ConnectConfigType m_connect;

    /// vector containing all the proxy configurations
    ConnectConfigType m_proxies;

    typedef std::map< std::string, ::fwServices::helper::SigSlotConnection::sptr > ObjectConnectionsMapType;
    /// map containing the object key/connection relation
    ObjectConnectionsMapType m_objectConnections;

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
};

//-----------------------------------------------------------------------------

template< class ContainerType >
void SRender::disconnect(const ContainerType& objects)
{
    for(auto element :  objects)
    {
        std::string key = element.first;
        if(m_objectConnections.find(key) != m_objectConnections.end())
        {
            m_objectConnections[key]->disconnect();
            m_objectConnections.erase(key);
        }

        ::fwServices::helper::Config::disconnectProxies(key, m_proxyMap);
    }
}

//-----------------------------------------------------------------------------

}
#endif // __FWRENDEROGRE_SRENDER_HPP__
