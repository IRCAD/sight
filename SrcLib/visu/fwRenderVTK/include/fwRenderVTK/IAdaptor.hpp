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

    /// To set a representation
    virtual void show(bool b = true)
    {
    }
    //------------------------------------------------------------------------------

    virtual void hide()
    {
        this->show(false);
    }

    FWRENDERVTK_API void setRenderService( SRender::sptr service );
    FWRENDERVTK_API void setRenderId(SRender::RendererIdType newID);
    FWRENDERVTK_API SRender::sptr getRenderService() const;
    FWRENDERVTK_API SRender::RendererIdType getRenderId() const;
    FWRENDERVTK_API vtkRenderer* getRenderer();

    FWRENDERVTK_API void setPickerId(SRender::PickerIdType newID);
    FWRENDERVTK_API SRender::PickerIdType getPickerId() const;
    FWRENDERVTK_API vtkAbstractPropPicker* getPicker(std::string pickerId = "");

    FWRENDERVTK_API void setTransformId(SRender::VtkObjectIdType newID);
    FWRENDERVTK_API SRender::VtkObjectIdType getTransformId() const;
    FWRENDERVTK_API vtkTransform* getTransform();

    FWRENDERVTK_API vtkObject* getVtkObject(const SRender::VtkObjectIdType& objectId) const;

    FWRENDERVTK_API vtkRenderWindowInteractor* getInteractor();

    FWRENDERVTK_API virtual ::fwData::Object::sptr getAssociatedObject(vtkProp* prop, int depth = 0);

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
    /// Overrides
    FWRENDERVTK_API void configuring();
    FWRENDERVTK_API void starting();
    FWRENDERVTK_API void stopping();
    FWRENDERVTK_API void updating();
    //@}

    /// Initialize the adaptor with the associated render service. (must be call in starting).
    FWRENDERVTK_API void initialize();

    //------------------------------------------------------------------------------

    ServiceVector& getRegisteredServices()
    {
        return m_subServices;
    }
    FWRENDERVTK_API void registerService( ::fwRenderVTK::IAdaptor::sptr service );
    FWRENDERVTK_API void unregisterServices();

    FWRENDERVTK_API void registerProp(vtkProp* prop);
    FWRENDERVTK_API void unregisterProps();

    FWRENDERVTK_API void addToRenderer(vtkProp* prop);
    FWRENDERVTK_API void addToPicker(vtkProp* prop, std::string pickerId = "");
    FWRENDERVTK_API void removeFromPicker(vtkProp* prop, std::string pickerId = "");

    FWRENDERVTK_API void removeAllPropFromRenderer();

    FWRENDERVTK_API static void getProps(vtkPropCollection* propc, vtkProp* prop);

    /// notify a render request iff vtkPipeline is modified
    FWRENDERVTK_API void requestRender();

    template< class DATATYPE >
    CSPTR(DATATYPE) getSafeInput(const std::string& key) const;
    template< class DATATYPE >
    SPTR(DATATYPE) getSafeInOut(const std::string& key) const;

    /// priority of comChannel observing related object (specified with objectId)
    double m_comChannelPriority;

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
