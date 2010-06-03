/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWRENDERVTK_VTKRENDERSERVICE_HPP_
#define _FWRENDERVTK_VTKRENDERSERVICE_HPP_


#include <map>

#include <fwRuntime/ConfigurationElement.hpp>

#include <fwRender/IRender.hpp>

#include <fwData/Composite.hpp>

//#include "fwRenderVTK/IVtkAdaptorService.hpp"

#include "fwRenderVTK/config.hpp"


class wxAuiManager;
class vtkRenderWindow;
class vtkRenderer;
class vtkAbstractPropPicker;
class vtkObject;

namespace fwData
{

class TransformationMatrix3D;

}


namespace fwRenderVTK
{

class fwWxVTKRenderWindowInteractor;
class IVtkAdaptorService;

class FWRENDERVTK_CLASS_API VtkRenderService : public ::fwRender::IRender
{
public :
    fwCoreServiceClassDefinitionsMacro ( (VtkRenderService)(::fwRender::IRender) ) ;

    typedef std::string RendererIdType;
    typedef std::string PickerIdType;
    typedef std::string ObjectIdType;
    typedef std::string AdaptorIdType;
    typedef std::string VtkObjectIdType;

    FWRENDERVTK_API VtkRenderService() throw() ;

    FWRENDERVTK_API virtual ~VtkRenderService() throw() ;

    FWRENDERVTK_API vtkRenderer * getRenderer(RendererIdType rendererId);

    FWRENDERVTK_API void render();
    bool isShownOnScreen();

    FWRENDERVTK_API vtkAbstractPropPicker * getPicker(PickerIdType pickerId);

    FWRENDERVTK_API vtkObject * getVtkObject(VtkObjectIdType objectId);

    // For temporary NegatoAdaptor use
    FWRENDERVTK_API wxAuiManager* getWxManager(){return m_wxmanager;};

    FWRENDERVTK_API bool getPendingRenderRequest(){return m_pendingRenderRequest;}
    FWRENDERVTK_API void setPendingRenderRequest(bool b){m_pendingRenderRequest=b;}

protected:

    /// Install observations : should be implemented in IService
    FWRENDERVTK_API virtual void starting() throw( ::fwTools::Failed);
    /// Uninstall observations : should be implemented in IService
    FWRENDERVTK_API virtual void stopping() throw( ::fwTools::Failed);

    FWRENDERVTK_API virtual void configuring() throw( ::fwTools::Failed) ;

    FWRENDERVTK_API void updating( ::fwServices::ObjectMsg::csptr message ) throw( ::fwTools::Failed);

    FWRENDERVTK_API void updating() throw( ::fwTools::Failed);

    /// Add a vtk object in the VtkRenderService, referenced by a key.
    void addVtkObject( VtkObjectIdType _id, vtkObject * _vtkObj );

private :

    typedef ::fwRuntime::ConfigurationElement::sptr ConfigurationType;
    ConfigurationType m_sceneConfiguration;
    class SceneAdaptor {

        public:

        SceneAdaptor()
        { }

        ~SceneAdaptor()
        { }

        SPTR (IVtkAdaptorService) getService()
        { return m_service.lock(); }

        ConfigurationType m_config;
        WPTR(IVtkAdaptorService) m_service;

    };



    /// @brief required to facilitate resize of an empty vtk rendering window : why ?
    wxAuiManager* m_wxmanager;

    /// @brief VTK Interactor window
    ::fwRenderVTK::fwWxVTKRenderWindowInteractor* m_interactor;

    /// @brief scene's renderers
    typedef std::map< RendererIdType , vtkRenderer* > RenderersMapType ;

    /// @brief scene's pickers
    typedef std::map< PickerIdType , vtkAbstractPropPicker * > PickersMapType ;

    /// @brief scene's transforms
    typedef std::map< VtkObjectIdType , vtkObject  * > VtkObjectMapType ;

    /// @brief Actives adaptors in scene
    typedef std::map< AdaptorIdType, SceneAdaptor > SceneAdaptorsMapType ;

    RenderersMapType     m_renderers;
    PickersMapType       m_pickers;
    SceneAdaptorsMapType m_sceneAdaptors;
    VtkObjectMapType     m_vtkObjects;

    bool m_pendingRenderRequest;

    void startContext();
    void stopContext();

    void configurePicker   ( ConfigurationType conf );
    void configureRenderer ( ConfigurationType conf );
    void configureObject   ( ConfigurationType conf );
    void configureVtkObject( ConfigurationType conf );
};

}

#endif //_FWRENDERVTK_VTKRENDERSERVICE_HPP_
