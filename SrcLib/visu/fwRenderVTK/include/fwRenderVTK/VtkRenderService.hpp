/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWRENDERVTK_VTKRENDERSERVICE_HPP__
#define __FWRENDERVTK_VTKRENDERSERVICE_HPP__

#include <fwThread/Timer.hpp>

#include <map>

#include <fwServices/helper/SigSlotConnection.hpp>

#include <fwRuntime/ConfigurationElement.hpp>

#include <fwRender/IRender.hpp>

#include <fwData/Composite.hpp>

#include "fwRenderVTK/config.hpp"
#include "fwRenderVTK/IVtkRenderWindowInteractorManager.hpp"

#include <fwThread/Timer.hpp>

class vtkRenderWindow;
class vtkRenderer;
class vtkAbstractPropPicker;
class vtkObject;
class vtkTransform;

namespace fwData
{

class TransformationMatrix3D;

}

namespace fwRenderVTK
{

class IVtkAdaptorService;

/**
 * @class VtkRenderService
 * @brief The generic scene service shows adaptors in a 3D VTK scene.
 */
class FWRENDERVTK_CLASS_API VtkRenderService : public ::fwRender::IRender
{
public:
    fwCoreServiceClassDefinitionsMacro ( (VtkRenderService)(::fwRender::IRender) );

    typedef std::string RendererIdType;
    typedef std::string PickerIdType;
    typedef std::string ObjectIdType;
    typedef std::string AdaptorIdType;
    typedef std::string VtkObjectIdType;

    FWRENDERVTK_API static const ::fwCom::Slots::SlotKeyType s_RENDER_SLOT;
    typedef ::fwCom::Slot<void ()> RenderSlotType;

    FWRENDERVTK_API VtkRenderService() throw();

    FWRENDERVTK_API virtual ~VtkRenderService() throw();

    /// Returns the vtkRenderer with the given id
    FWRENDERVTK_API vtkRenderer * getRenderer(RendererIdType rendererId);

    /// Returns true if the scene is shown on screen
    bool isShownOnScreen();

    /// Returns the picker with the given id
    FWRENDERVTK_API vtkAbstractPropPicker * getPicker(PickerIdType pickerId);

    /// Returns the vtkObject with the given id
    FWRENDERVTK_API vtkObject * getVtkObject(const VtkObjectIdType& objectId);

    /// Returns the adaptor with the given id
    FWRENDERVTK_API SPTR (IVtkAdaptorService) getAdaptor(AdaptorIdType adaptorId);

    /// Get a vtkTransform in the VtkRenderService, referenced by a key. Create it if it does not exist.
    FWRENDERVTK_API vtkTransform * getOrAddVtkTransform( const VtkObjectIdType& _id );

    bool getPendingRenderRequest()
    {
        return m_pendingRenderRequest;
    }
    void setPendingRenderRequest(bool b)
    {
        m_pendingRenderRequest = b;
    }

protected:

    /// Renders the scene.
    FWRENDERVTK_API void render();

    /// Install observations : should be implemented in IService
    FWRENDERVTK_API virtual void starting() throw( ::fwTools::Failed);
    /// Uninstall observations : should be implemented in IService
    FWRENDERVTK_API virtual void stopping() throw( ::fwTools::Failed);

    /**
     * @brief configures the adaptor
     * @verbatim
       <service uid="generiSceneUID" impl="::fwRenderVTK::VtkRenderService" type="::fwRender::IRender">
        <scene renderMode="auto|timer|none">
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
                <signal>adaptorUID/objectModified</signal>
                <slot>serviceUid/updateTM</slot>
            </connect>

            <connect waitForKey="tm3dKey">
                <signal>objectModified</signal><!-- signal for object "tm3dKey" -->
                <slot>serviceUid/updateTM</slot>
            </connect>
        </scene>
        <fps>30</fps>
       </service>
       @endverbatim
     * With :
     *  - \b renderMode (optional, "auto" by default): this attribute is forwarded to all adaptors. For each adaptor,
     *    if renderMode="auto",  the scene is automatically rendered after doStart, doUpdate, doSwap, doStop
     *    and m_vtkPipelineModified=true. If renderMode="timer" the scene is rendered at N frame per seconds (N is
     *    defined by fps tag). If renderMode="none" you should call 'render' slot to call reder the scene.
     *  - \b renderer
     *     - \b id (mandatory): the identifier of the renderer
     *     - \b layer (optional): defines the layer of the vtkRenderer. This is only used if there are layered renderers.
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
     *    - \b config: adaptor's configuration. It is parsed in the adaptor's configuring() method.
     *  - \b connect : not mandatory, connects signal to slot
     *    - \b waitForKey : not mandatory, defines the required object key for the signal/slot connection
     *    - \b signal : mandatory, must be signal holder UID, followed by '/', followed by signal name. To use the
     *         object (defined by waitForKey) signal, you don't have to write object uid, only the signal name.
     *    - \b slot : mandatory, must be slot holder UID, followed by '/', followed by slot name
     */
    FWRENDERVTK_API virtual void configuring() throw( ::fwTools::Failed);

    /// Updates the scene's adaptors with the modified objects contained in the composite.
    FWRENDERVTK_API void receiving( ::fwServices::ObjectMsg::csptr message ) throw( ::fwTools::Failed);

    /// Does nothing.
    FWRENDERVTK_API void updating() throw( ::fwTools::Failed);

    /// Add a vtk object in the VtkRenderService, referenced by a key.
    FWRENDERVTK_API void addVtkObject( const VtkObjectIdType& _id, vtkObject * _vtkObj );

    /// Slot to call render method
    RenderSlotType::sptr m_slotRender;

private:

    /// Slot called when on each timer update
    void updateTimer();

    typedef ::fwRuntime::ConfigurationElement::sptr ConfigurationType;
    ConfigurationType m_sceneConfiguration;

    /// @brief VTK Interactor window manager
    ::fwRenderVTK::IVtkRenderWindowInteractorManager::sptr m_interactorManager;


    class SceneAdaptor
    {

    public:

        SPTR (IVtkAdaptorService) getService()
        {
            return m_service.lock();
        }

        ConfigurationType m_config;
        WPTR(IVtkAdaptorService) m_service;

    };

    /// @brief scene's renderers
    typedef std::map< RendererIdType, vtkRenderer* > RenderersMapType;

    /// @brief scene's pickers
    typedef std::map< PickerIdType, vtkAbstractPropPicker * > PickersMapType;

    /// @brief scene's transforms
    typedef std::map< VtkObjectIdType, vtkObject  * > VtkObjectMapType;

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

    /// Timer used for the update
    ::fwThread::Timer::sptr m_timer;

    void startContext();
    void stopContext();

    void configurePicker   ( ConfigurationType conf );
    void configureRenderer ( ConfigurationType conf );
    void configureObject   ( ConfigurationType conf );
    void configureVtkObject( ConfigurationType conf );
    vtkTransform * createVtkTransform( ConfigurationType conf );

    /// Creates the connection if the required key is contained in the composite
    void connectAfterWait(SPTR(::fwData::Composite) composite);

    /// Creates the connection given by the configuration for obj associated with the key in the composite.
    void manageConnection(const std::string key, const ::fwTools::Object::sptr &obj,
                          ConfigurationType config);

    /// Disconnects the connection based on a object key
    void disconnect(SPTR(::fwData::Composite) composite);

    /// Signal/ Slot connection
    ::fwServices::helper::SigSlotConnection::sptr m_connections;

    typedef std::vector< ConfigurationType > ConnectConfigType;
    /// vector containing all the connections configurations
    ConnectConfigType m_connect;

    typedef std::map< std::string, ::fwServices::helper::SigSlotConnection::sptr > ObjectConnectionsMapType;
    /// map containing the object key/connection relation
    ObjectConnectionsMapType m_objectConnections;
};

}

#endif //__FWRENDERVTK_VTKRENDERSERVICE_HPP__
