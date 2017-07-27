/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWRENDERVTK_IADAPTOR_HPP__
#define __FWRENDERVTK_IADAPTOR_HPP__

#include "fwRenderVTK/config.hpp"
#include "fwRenderVTK/SRender.hpp"

#include <fwCom/helper/SigSlotConnection.hpp>

#include <fwServices/IService.hpp>

#include <string>

class vtkProp;
class vtkPropCollection;
class vtkRenderer;
class vtkRenderWindowInteractor;
class vtkTransform;

namespace fwRenderVTK
{

/**
 * @brief Base class for VTK adaptors
 */
class FWRENDERVTK_CLASS_API IAdaptor : public fwServices::IService
{
friend class SRender;
public:
    fwCoreServiceClassDefinitionsMacro( (IAdaptor)(::fwServices::IService) );

    typedef fwServices::IService SuperClass;

    /// Sets the assiciated render service
    FWRENDERVTK_API void setRenderService( SRender::sptr service );

    /// Returd the associated render service
    FWRENDERVTK_API SRender::sptr getRenderService() const;

    /// Sets the renderer identifier
    FWRENDERVTK_API void setRendererId(SRender::RendererIdType newID);

    /// Returns the renderer identifier
    FWRENDERVTK_API SRender::RendererIdType getRendererId() const;

    /// Returns the renderer
    FWRENDERVTK_API vtkRenderer* getRenderer();

    /// Sets the identifier of the picker used by this adaptor
    FWRENDERVTK_API void setPickerId(SRender::PickerIdType newID);

    /// Gets the identifier of the picker used by this adaptor
    FWRENDERVTK_API SRender::PickerIdType getPickerId() const;

    /**
     * @brief Get the picker
     * @param pickerId identifier of the picker in the vtk scene (if it is empty, it returns the picker associated with
     *        this adaptor
     * @return Returns the associated picker
     */
    FWRENDERVTK_API vtkAbstractPropPicker* getPicker(std::string pickerId = "");

    /// Sets the identifier of the transform used by this adaptor
    FWRENDERVTK_API void setTransformId(SRender::VtkObjectIdType newID);

    /// Returns the identifier of the transform used by this adaptor
    FWRENDERVTK_API SRender::VtkObjectIdType getTransformId() const;

    /// Returns the transform used by this adaptor
    FWRENDERVTK_API vtkTransform* getTransform();

    /// Returns the vtk object defined by 'objectId' in the vtk scene
    FWRENDERVTK_API vtkObject* getVtkObject(const SRender::VtkObjectIdType& objectId) const;

    /// Returns the render interactor
    FWRENDERVTK_API vtkRenderWindowInteractor* getInteractor();

    /**
     * @brief Returns the object associated to the vtkProp.
     * @param prop vtkProp to find the associated Object
     * @param depth depth of reseach in sub-adaptors, if depth == 0 it only search in current prop collection
     */
    FWRENDERVTK_API virtual ::fwData::Object::sptr getAssociatedObject(vtkProp* prop, int depth = 0);

    /**
     * @brief Gets all the vtkProp associated to this adaptor
     * @param[out] propc output prop collection
     * @param depth depth of reseach in sub-adaptors, if depth == 0 it only search in current prop collection,
     * if depth == -1, it recursively searchs in all sub-adaptors
     */
    FWRENDERVTK_API void getAllSubProps(vtkPropCollection* propc, int depth = -1);

    /// End-user have to call this method when a vtk structure has been modified, thus a render request will be sent.
    FWRENDERVTK_API void setVtkPipelineModified();

    /// Returns true if the service automatically triggers the rendering.
    bool getAutoRender() const
    {
        return m_autoRender;
    }

    /// Sets if the service automatically triggers the rendering.
    void setAutoRender(bool autoRender)
    {
        m_autoRender = autoRender;
    }

    /**
     * @brief   Returns the starting priority of the adaptor.
     *
     * Some adaptors may have to be started before others.
     * @return priority, the lesser will be the first to be launched and the last be to stopped (default: 0)
     */
    FWRENDERVTK_API virtual int getStartPriority();

protected:

    typedef std::vector < ::fwRenderVTK::IAdaptor::wptr > ServiceVector;

    /**
     * @brief   constructor
     */
    FWRENDERVTK_API IAdaptor() noexcept;

    /**
     * @brief   destructor
     */
    FWRENDERVTK_API virtual ~IAdaptor() noexcept;

    /**
     * @name    Standard service methods
     */
    //@{
    // TODO remove: must be reimplemented in all adaptors
    FWRENDERVTK_API void configuring();
    FWRENDERVTK_API void starting();
    FWRENDERVTK_API void stopping();
    FWRENDERVTK_API void updating();
    //@}

    /// Parse the xml configuration for renderer, picker and transform
    FWRENDERVTK_API void configureParams();

    /// Initialize the adaptor with the associated render service. (must be call in starting).
    FWRENDERVTK_API void initialize();

    //------------------------------------------------------------------------------

    /// Return the list of the sub-adaptor managed by this adaptor
    const ServiceVector& getRegisteredServices() const
    {
        return m_subServices;
    }

    /**
     * @brief Creates a new sub adaptor
     * @param[in] type service type, must inherits of IAdaptor (ex. ::visuVTKAdaptor::Image)
     * @param[out] config struct used to configure the service's objects and auto-connection
     * @return Returns the created adaptor
     * @throw fwCore::Exception if the service cannot be created
     * @note if the service works on objects, you will need to call :registerServiceInput()  or registerServiceInOut()
     * with the objects
     */
    FWRENDERVTK_API ::fwRenderVTK::IAdaptor::sptr createSubAdaptor(const std::string& type,
                                                                   ::fwServices::IService::Config& config);

    /**
     * @brief Registers the object of the service in the OSR
     * @param[in] obj input object used by the service
     * @param[in] key key of the object in the new adaptor
     * @param[in] srv service to register with the object
     * @param[in] autoConnect if true, the service will be connected to all of its objects
     * @param[in,out] config struct used to configure the service's objects and auto-connection
     * @return
     */
    FWRENDERVTK_API void registerServiceInput(const ::fwData::Object::csptr& obj,
                                              const std::string& key,
                                              const ::fwRenderVTK::IAdaptor::sptr& srv,
                                              const bool autoConnect,
                                              ::fwServices::IService::Config& config);

    /**
     * @brief Registers the object of the service in the OSR
     * @param[in] obj in-out object used by the service
     * @param[in] key key of the object in the new adaptor
     * @param[in] srv service to register with the object
     * @param[in] autoConnect if true, the service will be connected to all of its objects
     * @param[in,out] config struct used to configure the service's objects and auto-connection
     * @return
     */
    FWRENDERVTK_API void registerServiceInOut(const ::fwData::Object::sptr& obj,
                                              const std::string& key,
                                              const ::fwRenderVTK::IAdaptor::sptr& srv,
                                              const bool autoConnect,
                                              ::fwServices::IService::Config& config);

    /// Stores the service in the subServices list
    FWRENDERVTK_API void registerService( ::fwRenderVTK::IAdaptor::sptr service );

    /// Unregiters the sub-adaptor from the OSR and the subServices list
    FWRENDERVTK_API void unregisterService(::fwRenderVTK::IAdaptor::sptr service);

    /// Unregiters all the sub-adaptor from the OSR and clear the subServices list
    FWRENDERVTK_API void unregisterServices();

    /// Registers the vtkProp
    FWRENDERVTK_API void registerProp(vtkProp* prop);

    /// Unregisters all the vtkProp
    FWRENDERVTK_API void unregisterProps();

    /// Adds the vtkProp to the renderer
    FWRENDERVTK_API void addToRenderer(vtkProp* prop);

    /// Removes all the vtkProp from the renderer
    FWRENDERVTK_API void removeAllPropFromRenderer();

    /**
     * @brief Adds the vtkProp to the picking list
     * @param prop vtkProp to be pickable
     * @param pickerId picker identifier in the scene (if empty the current adaptor picker is used)
     */
    FWRENDERVTK_API void addToPicker(vtkProp* prop, std::string pickerId = "");

    /**
     * @brief Removes the vtkProp to the picking list
     * @param prop vtkProp to be pickable
     * @param pickerId picker identifier in the scene (if empty the current adaptor picker is used)
     */
    FWRENDERVTK_API void removeFromPicker(vtkProp* prop, std::string pickerId = "");

    /**
     * @brief Get all the vtkProp associated to the inProp (actors, actors2D, volumes)
     * @param[out] outPropc collection of vtkProp
     * @param[in] inProp
     */
    FWRENDERVTK_API static void getProps(vtkPropCollection* outPropc, vtkProp* inProp);

    /// notify a render request iff vtkPipeline is modified
    FWRENDERVTK_API void requestRender();

    /// @deprecated use getInput instead
    template< class DATATYPE >
    CSPTR(DATATYPE) getSafeInput(const std::string& key) const;
    /// @deprecated use getInOut instead
    template< class DATATYPE >
    SPTR(DATATYPE) getSafeInOut(const std::string& key) const;

    /// state of the pipeline
    bool m_vtkPipelineModified;
    SRender::RendererIdType m_rendererId;
    SRender::PickerIdType m_pickerId;
    SRender::VtkObjectIdType m_transformId;
    SRender::wptr m_renderService;

    ServiceVector m_subServices;

    vtkPropCollection* m_propCollection;

    ::fwCom::helper::SigSlotConnection m_connections;

    bool m_autoRender;
};

//------------------------------------------------------------------------------

template< class DATATYPE >
CSPTR(DATATYPE) IAdaptor::getSafeInput(const std::string& key) const
{
    if( ::fwServices::IService::isVersion2() )
    {
        return this->getRenderService()->getInput<DATATYPE>(key);
    }
    else
    {
        return std::dynamic_pointer_cast<DATATYPE>( ::fwTools::fwID::getObject(key) );
    }
}

//------------------------------------------------------------------------------

template< class DATATYPE >
SPTR(DATATYPE) IAdaptor::getSafeInOut(const std::string& key) const
{
    if( ::fwServices::IService::isVersion2() )
    {
        return this->getRenderService()->getInOut<DATATYPE>(key);
    }
    else
    {
        return std::dynamic_pointer_cast<DATATYPE>( ::fwTools::fwID::getObject(key) );
    }
}

//------------------------------------------------------------------------------

}

#endif /*__FWRENDERVTK_IADAPTOR_HPP__*/
