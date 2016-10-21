/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWRENDERVTK_IVTKADAPTORSERVICE_HPP__
#define __FWRENDERVTK_IVTKADAPTORSERVICE_HPP__

#include "fwRenderVTK/SRender.hpp"
#include "fwRenderVTK/config.hpp"

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

class FWRENDERVTK_CLASS_API IVtkAdaptorService : public fwServices::IService
{
friend class SRender;
public:
    fwCoreServiceClassDefinitionsMacro ( (IVtkAdaptorService)(::fwServices::IService) );

    typedef fwServices::IService SuperClass;


    /// To set a representation
    virtual void show(bool b = true)
    {
    }
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

    /// Return true if the service slots are automatically connected to the object signals
    bool getAutoConnect() const
    {
        return m_autoConnect;
    }

    /// Set if the service slots are automatically connected to the object signals
    void setAutoConnect(bool autoConnect)
    {
        m_autoConnect = autoConnect;
    }

    /**
     * @brief   Returns the starting priority of the adaptor.
     *
     * Some adaptors may have to be started before others.
     * @return priority, the lesser will be the first to be launched and the last be to stopped (default: 0)
     */
    FWRENDERVTK_API virtual int getStartPriority();

protected:

    /**
     * @brief   constructor
     */
    FWRENDERVTK_API IVtkAdaptorService() throw();

    /**
     * @brief   destructor
     */
    FWRENDERVTK_API virtual ~IVtkAdaptorService() throw();

    /**
     * @name    Standard service methods
     */
    //@{
    /// Overrides
    FWRENDERVTK_API virtual void info(std::ostream& _sstream );
    FWRENDERVTK_API void configuring() throw(fwTools::Failed);
    FWRENDERVTK_API void starting() throw(fwTools::Failed);
    FWRENDERVTK_API void stopping() throw(fwTools::Failed);
    FWRENDERVTK_API void swapping() throw(fwTools::Failed);
    FWRENDERVTK_API void updating() throw(fwTools::Failed);
    //@}


    /// priority of comChannel observing related object (specified with objectId)
    double m_comChannelPriority;

    /// state of the pipeline
    bool m_vtkPipelineModified;
    SRender::RendererIdType m_rendererId;
    SRender::PickerIdType m_pickerId;
    SRender::VtkObjectIdType m_transformId;
    SRender::wptr m_renderService;

    ::fwCom::helper::SigSlotConnection m_connections;


    typedef std::vector < ::fwRenderVTK::IVtkAdaptorService::wptr > ServiceVector;
    ServiceVector m_subServices;

    vtkPropCollection* m_propCollection;

    bool m_autoRender;
    bool m_autoConnect; ///< If true, connect the adaptor slots to its objects signals

    FWRENDERVTK_API virtual void doStart()     = 0;
    FWRENDERVTK_API virtual void doStop()      = 0;
    FWRENDERVTK_API virtual void doSwap()      = 0;
    FWRENDERVTK_API virtual void doUpdate()    = 0;
    FWRENDERVTK_API virtual void doConfigure() = 0;

    ServiceVector& getRegisteredServices()
    {
        return m_subServices;
    }
    FWRENDERVTK_API void registerService( ::fwRenderVTK::IVtkAdaptorService::sptr service );
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
};

//------------------------------------------------------------------------------

template< class DATATYPE >
CSPTR(DATATYPE) IVtkAdaptorService::getSafeInput(const std::string& key) const
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
SPTR(DATATYPE) IVtkAdaptorService::getSafeInOut(const std::string& key) const
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

#endif /*__FWRENDERVTK_IVTKADAPTORSERVICE_HPP__*/
