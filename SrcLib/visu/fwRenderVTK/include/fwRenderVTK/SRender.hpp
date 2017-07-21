/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWRENDERVTK_SRENDER_HPP__
#define __FWRENDERVTK_SRENDER_HPP__

#include "fwRenderVTK/config.hpp"

#include <fwCom/helper/SigSlotConnection.hpp>

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

class IAdaptor;
class IVtkRenderWindowInteractorManager;

/**
 * @brief The generic scene service shows adaptors in a 3D VTK scene.
 *
 * @section Signals Signals
 * - \b dropped(std::string) : emitted when an object is dropped in the scene (contains the uid of the object)
 *
 * @section Slots Slots
 * - \b render() : render the scene
 * - \b requestRender() : request a render of the scene (call to render only if the scene is displayed)
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
    <service uid="generiSceneUID" impl="::fwRenderVTK::SRender" type="::fwRender::IRender">
        <scene renderMode="auto|timer|none" offScreen="imageKey" width="1920" height="1080">
            <renderer id="background" layer="0" background="0.0" />
            <vtkObject id="transform" class="vtkTransform" />
            <picker id="negatodefault" vtkclass="fwVtkCellPicker" />

            <adaptor uid="tmAdaptor" start="yes" />
            <adaptor uid="snapshot" start="yes" />
        </scene>
        <fps>30</fps>
    </service>
   @endcode
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
 *    - \b uid (mandatory): the uid of the adaptor
 *    - \b start (optional, default: "no") defines if the adaptor is started by the scene
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
    FWRENDERVTK_API SPTR(IAdaptor) getAdaptor(const AdaptorIdType &adaptorId) const;

    /// Get a vtkTransform in the SRender, referenced by a key. Create it if it does not exist.
    FWRENDERVTK_API vtkTransform* getOrAddVtkTransform( const VtkObjectIdType& _id );

    bool getPendingRenderRequest() const;

    void setPendingRenderRequest(bool b);

    enum class RenderMode
    {
        NONE,
        AUTO,
        TIMER
    };

    /// Get the render mode
    RenderMode getRenderMode() const;

protected:

    /// Renders the scene.
    FWRENDERVTK_API void render();

    /// Starts the render context, start the adaptors with start="yes"
    FWRENDERVTK_API virtual void starting();

    /// Stops the context, stops the started adaptors
    FWRENDERVTK_API virtual void stopping();

    /// configures the scene
    FWRENDERVTK_API virtual void configuring();

    /// Does nothing.
    FWRENDERVTK_API virtual void updating();

    /// Add a vtk object in the SRender, referenced by a key.
    FWRENDERVTK_API void addVtkObject( const VtkObjectIdType& _id, vtkObject* _vtkObj );

private:

    class SceneAdaptor
    {

    public:

        SPTR(IAdaptor) getService() const
        {
            return m_service.lock();
        }

        WPTR(IAdaptor) m_service;
        bool m_start;
    };

    /// Slot called when on each timer update
    void requestRender();

    /// Installs scene interactors
    void startContext();

    /// Remove scene interactors
    void stopContext();

    /// Parse the 'picker' configuration and create the associated vtk picker
    void configurePicker   ( ConfigurationType conf );

    /// Parse the 'renderer' configuration and create the associated vtk renderer
    void configureRenderer ( ConfigurationType conf );

    /// Parse the 'vtkObject' configuration and create the associated vtk objects
    void configureVtkObject( ConfigurationType conf );

    /// Parse the transform configuration to create vtk transform and manage concatenate
    vtkTransform* createVtkTransform( ConfigurationType conf );

    typedef ::fwRuntime::ConfigurationElement::sptr ConfigurationType;
    ConfigurationType m_sceneConfiguration;

    /// @brief VTK Interactor window manager
    SPTR( ::fwRenderVTK::IVtkRenderWindowInteractorManager ) m_interactorManager;

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

    /// Does the scene update automatically when something changes ?
    /// Otherwise it is updated periodically (default 30Hz)
    RenderMode m_renderMode;

    std::string m_offScreenImageKey; ///< Key of the image used for off screen render
    unsigned int m_width; ///< width for off screen render
    unsigned int m_height; ///< height for off screen render
    bool m_offScreen; ///< if true, scene is render in off screen

    /// Timer used for the update
    SPTR( ::fwThread::Timer ) m_timer;

    /// Signal/ Slot connection
    ::fwCom::helper::SigSlotConnection m_connections;

    /// Map to register proxy connections
    ::fwServices::helper::Config::ProxyConnectionsMapType m_proxyMap;

    typedef std::vector< ConfigurationType > ConnectConfigType;
    /// vector containing all the connections configurations
    ConnectConfigType m_connect;

    /// vector containing all the proxy configurations
    ConnectConfigType m_proxies;
};

//-----------------------------------------------------------------------------

inline bool SRender::getPendingRenderRequest() const
{
    return m_pendingRenderRequest;
}

//------------------------------------------------------------------------------

inline void SRender::setPendingRenderRequest(bool b)
{
    m_pendingRenderRequest = b;
}

//-----------------------------------------------------------------------------

inline SRender::RenderMode SRender::getRenderMode() const
{
    return m_renderMode;
}

}

#endif //__FWRENDERVTK_SRENDER_HPP__
