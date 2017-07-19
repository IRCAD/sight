/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWRENDERVTK_SRENDER_HPP__
#define __FWRENDERVTK_SRENDER_HPP__

#include "fwRenderVTK/config.hpp"

#include <fwCom/helper/SigSlotConnection.hpp>

#include <fwData/Composite.hpp>

#include <fwRender/IRender.hpp>

#include <fwRuntime/ConfigurationElement.hpp>

#include <fwServices/helper/Config.hpp>

#include <map>

class vtkRenderWindow;
class vtkRenderer;
class vtkAbstractPropPicker;
class vtkObject;
class vtkTransform;

// Forward declarations
namespace fwData
{
class TransformationMatrix3D;
}

namespace fwThread
{
class Timer;
}

namespace fwRenderVTK
{

class IVtkAdaptorService;
class IVtkRenderWindowInteractorManager;

/**
 * @brief The generic scene service shows adaptors in a 3D VTK scene.
 */
class FWRENDERVTK_CLASS_API SRender : public ::fwRender::IRender
{
public:
    fwCoreServiceClassDefinitionsMacro( (SRender)(::fwRender::IRender) );

    typedef std::string RendererIdType;
    typedef std::string PickerIdType;
    typedef std::string ObjectIdType;
    typedef std::string AdaptorIdType;
    typedef std::string VtkObjectIdType;
    typedef std::map< std::string, ::fwData::Object::csptr > ConstObjectMapType;

    FWRENDERVTK_API static const ::fwCom::Slots::SlotKeyType s_RENDER_SLOT;
    FWRENDERVTK_API static const ::fwCom::Slots::SlotKeyType s_REQUEST_RENDER_SLOT;

    FWRENDERVTK_API static const ::fwCom::Signals::SignalKeyType s_DROPPED_SIG;
    typedef ::fwCom::Signal< void (std::string)> DroppedSignalType;

    FWRENDERVTK_API SRender() noexcept;

    FWRENDERVTK_API virtual ~SRender() noexcept;

    /// Returns the vtkRenderer with the given id
    FWRENDERVTK_API vtkRenderer* getRenderer(RendererIdType rendererId);

    /// Returns true if the scene is shown on screen
    bool isShownOnScreen();

    /// Returns the picker with the given id
    FWRENDERVTK_API vtkAbstractPropPicker* getPicker(PickerIdType pickerId);

    /// Returns the vtkObject with the given id
    FWRENDERVTK_API vtkObject* getVtkObject(const VtkObjectIdType& objectId) const;

    /// Returns the adaptor with the given id
    FWRENDERVTK_API SPTR(IVtkAdaptorService) getAdaptor(const AdaptorIdType &adaptorId) const;

    /// Get a vtkTransform in the SRender, referenced by a key. Create it if it does not exist.
    FWRENDERVTK_API vtkTransform* getOrAddVtkTransform( const VtkObjectIdType& _id );

    //------------------------------------------------------------------------------

    bool getPendingRenderRequest()
    {
        return m_pendingRenderRequest;
    }
    //------------------------------------------------------------------------------

    void setPendingRenderRequest(bool b)
    {
        m_pendingRenderRequest = b;
    }

    /**
     * @brief Returns proposals to connect service slots to associated object signals,
     * this method is used for obj/srv auto connection
     *
     * Connect Composite::s_ADDED_OBJECTS_SIG to this::s_UPDATE_OBJECTS_SLOT
     * Connect Composite::s_CHANGED_OBJECTS_SIG to this::s_UPDATE_OBJECTS_SLOT
     * Connect Composite::s_REMOVED_OBJECTS_SIG to this::s_UPDATE_OBJECTS_SLOT
     */
    FWRENDERVTK_API virtual KeyConnectionsType getObjSrvConnections() const;

    /**
     * @brief Returns proposals to connect service slots to associated object signals,
     * this method is used for obj/srv auto connection
     *
     * Connect Composite::s_ADDED_OBJECTS_SIG to this::s_UPDATE_OBJECTS_SLOT
     * Connect Composite::s_CHANGED_OBJECTS_SIG to this::s_UPDATE_OBJECTS_SLOT
     * Connect Composite::s_REMOVED_OBJECTS_SIG to this::s_UPDATE_OBJECTS_SLOT
     */
    FWRENDERVTK_API ::fwServices::IService::KeyConnectionsMap getAutoConnections() const;

    /// TEMP: Function to grab the composite while we maintain appXml and appXml2
    FWRENDERVTK_API ::fwData::Composite::sptr getComposite();

protected:

    /// Renders the scene.
    FWRENDERVTK_API void render();

    /// Install observations : should be implemented in IService
    FWRENDERVTK_API virtual void starting();
    /// Uninstall observations : should be implemented in IService
    FWRENDERVTK_API virtual void stopping();

    /**
     * @brief configures the adaptor
     * @verbatim
       <service uid="generiSceneUID" impl="::fwRenderVTK::SRender" type="::fwRender::IRender">
        <scene renderMode="auto|timer|none" offScreen="imageKey" width="1920" height="1080">
            <renderer id="background" layer="0" background="0.0" />
            <vtkObject id="transform" class="vtkTransform" />
            <picker id="negatodefault" vtkclass="fwVtkCellPicker" />

            <adaptor id="tmAdaptor" class="::visuVTKAdaptor::Transform" uid="adaptorUID" objectId="tm3dKey">
                <config transform="transform" />
            </adaptor>

            <adaptor id="snapshot" class="::visuVTKAdaptor::Snapshot" objectId="self">
                <config ...... />
            </adaptor>

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
        <fps>30</fps>
       </service>
       @endverbatim
     * With :
     *  - \b renderMode (optional, "auto" by default): this attribute is forwarded to all adaptors. For each adaptor,
     *    if renderMode="auto",  the scene is automatically rendered after doStart, doUpdate, doSwap, doStop
     *    and m_vtkPipelineModified=true. If renderMode="timer" the scene is rendered at N frame per seconds (N is
     *    defined by fps tag). If renderMode="none" you should call 'render' slot to call reder the scene.
     *  - \b offScreen (optional): key of the image used for off screen render
     *  - \b width (optional, "1280" by default): width for off screen render
     *  - \b height (optional, "720" by default): height for off screen render
     *  - \b renderer
     *     - \b id (mandatory): the identifier of the renderer
     *     - \b layer (optional): defines the layer of the vtkRenderer. This is only used if there are layered
     * renderers.
     *     - \b background (optional): the background color of the rendering screen. The color value can be defines as a
     *       grey level value (ex . 1.0 for white) or as a hexadecimal value (ex : \#ffffff for white).
     *  - \b vtkObject
     *    - \b id (mandatory): the identifier of the vtkObject
     *    - \b class (mandatory): the classname of the vtkObject to create. For example vtkTransform, vtkImageBlend, ...
     *  - \b picker
     *    - \b id (mandatory): the identifier of the picker
     *    - \b vtkclass (optional, by default vtkCellPicker): the classname of the picker to create.
     *  - \b adaptor
     *    - \b id (mandatory): the identifier of the adaptor
     *    - \b class (mandatory): the classname of the adaptor service
     *    - \b uid (optional): the fwID to specify for the adaptor service
     *    - \b objectId (mandatory): the key of the adaptor's object in the scene's composite. The "self" key is used
     *     when the adaptor works on the scene's composite.
     *    - \b autoConnect (optional, "yes" by default): if "yes" the service slot are automatically connected to the
     *      object signals.
     *    - \b config: adaptor's configuration. It is parsed in the adaptor's configuring() method.
     *  - \b connect/proxy : not mandatory, connects signal to slot
     *    - \b waitForKey : not mandatory, defines the required object key for the signal/slot connection
     *    - \b signal : mandatory, must be signal holder UID, followed by '/', followed by signal name. To use the
     *         object (defined by waitForKey) signal, you don't have to write object uid, only the signal name.
     *    - \b slot : mandatory, must be slot holder UID, followed by '/', followed by slot name
     */
    FWRENDERVTK_API virtual void configuring();

    /// Does nothing.
    FWRENDERVTK_API virtual void updating();

    /// Start/stop adaptors
    FWRENDERVTK_API virtual void swapping(const KeyType& key);

    /// Add a vtk object in the SRender, referenced by a key.
    FWRENDERVTK_API void addVtkObject( const VtkObjectIdType& _id, vtkObject* _vtkObj );

private:

    /// Slot called when on each timer update
    void requestRender();

    /// Slot: add objects
    void addObjects(::fwData::Composite::ContainerType objects);

    /// Slot: change objects
    void changeObjects(::fwData::Composite::ContainerType newObjects, ::fwData::Composite::ContainerType oldObjects);

    /// Slot: remove objects
    void removeObjects(::fwData::Composite::ContainerType objects);

    /// Configure the objects
    void configureObjects(::fwData::Composite::ContainerType objects);

    typedef ::fwRuntime::ConfigurationElement::sptr ConfigurationType;
    ConfigurationType m_sceneConfiguration;

    /// @brief VTK Interactor window manager
    SPTR( ::fwRenderVTK::IVtkRenderWindowInteractorManager ) m_interactorManager;

    class SceneAdaptor
    {

    public:

        SPTR(IVtkAdaptorService) getService() const
        {
            return m_service.lock();
        }

        ConfigurationType m_config;
        WPTR(IVtkAdaptorService) m_service;

    };

    /// @brief scene's renderers
    typedef std::map< RendererIdType, vtkRenderer* > RenderersMapType;

    /// @brief scene's pickers
    typedef std::map< PickerIdType, vtkAbstractPropPicker* > PickersMapType;

    /// @brief scene's transforms
    typedef std::map< VtkObjectIdType, vtkObject* > VtkObjectMapType;

    /// @brief Actives adaptors in scene
    typedef std::map< AdaptorIdType, SceneAdaptor > SceneAdaptorsMapType;

    RenderersMapType m_renderers;
    PickersMapType m_pickers;
    SceneAdaptorsMapType m_sceneAdaptors;
    VtkObjectMapType m_vtkObjects;

    bool m_pendingRenderRequest;

    enum class RenderMode
    {
        NONE,
        AUTO,
        TIMER
    };

    /// Does the scene update automatically when something changes ?
    /// Otherwise it is updated periodically (default 30Hz)
    RenderMode m_renderMode;

    std::string m_offScreenImageKey; ///< Key of the image used for off screen render
    unsigned int m_width; ///< width for off screen render
    unsigned int m_height; ///< height for off screen render
    bool m_offScreen; ///< if true, scene is render in off screen

    /// Timer used for the update
    SPTR( ::fwThread::Timer ) m_timer;

    void startContext();
    void stopContext();

    void configurePicker   ( ConfigurationType conf );
    void configureRenderer ( ConfigurationType conf );
    void configureObject   ( ConfigurationType conf );
    void configureVtkObject( ConfigurationType conf );
    vtkTransform* createVtkTransform( ConfigurationType conf );

    /// Creates the connection if the required key is contained in the composite
    void connectAfterWait(::fwData::Composite::ContainerType objects);

    /// Version2: Creates the connection if the required key is present
    void connectAfterWait(const std::string& key);

    /// Creates the connection given by the configuration for obj associated with the key in the composite.
    void manageConnection(const std::string& key, const ::fwData::Object::csptr& obj,
                          const ConfigurationType& config);

    /// Creates the proxy given by the configuration for obj associated with the key in the composite.
    void manageProxy(const std::string& key, const ::fwData::Object::csptr& obj,
                     const ConfigurationType& config);

    /// Disconnects the connection based on a object key
    template< class ContainerType >
    void disconnect( const ContainerType& objects );

    /// Signal/ Slot connection
    ::fwCom::helper::SigSlotConnection m_connections;

    /// Map to register proxy connections
    ::fwServices::helper::Config::ProxyConnectionsMapType m_proxyMap;

    typedef std::vector< ConfigurationType > ConnectConfigType;
    /// vector containing all the connections configurations
    ConnectConfigType m_connect;

    /// vector containing all the proxy configurations
    ConnectConfigType m_proxies;

    typedef std::map< std::string, ::fwCom::helper::SigSlotConnection > ObjectConnectionsMapType;
    /// map containing the object key/connection relation
    ObjectConnectionsMapType m_objectConnections;
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
            m_objectConnections[key].disconnect();
            m_objectConnections.erase(key);
        }

        ::fwServices::helper::Config::disconnectProxies(key, m_proxyMap);
    }
}

//-----------------------------------------------------------------------------

}

#endif //__FWRENDERVTK_SRENDER_HPP__
