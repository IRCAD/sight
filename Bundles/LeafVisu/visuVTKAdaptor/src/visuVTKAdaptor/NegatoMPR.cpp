/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/foreach.hpp>

#include <fwTools/fwID.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/macros.hpp>
#include <fwServices/Factory.hpp>
#include <fwServices/registry/ObjectService.hpp>

#include <fwData/Image.hpp>
#include <fwData/Integer.hpp>
#include <fwData/String.hpp>
#include <fwData/Boolean.hpp>

#include <fwComEd/fieldHelper/MedicalImageHelpers.hpp>
#include <fwComEd/Dictionary.hpp>
#include <fwComEd/ImageMsg.hpp>

#include "visuVTKAdaptor/NegatoWindowingInteractor.hpp"
#include "visuVTKAdaptor/NegatoOneSlice.hpp"
#include "visuVTKAdaptor/NegatoMPR.hpp"


REGISTER_SERVICE( ::fwRenderVTK::IVtkAdaptorService, ::visuVTKAdaptor::NegatoMPR, ::fwData::Image ) ;

namespace visuVTKAdaptor
{

//------------------------------------------------------------------------------

NegatoMPR::NegatoMPR() throw() :
        m_3dModeEnabled ( ::boost::logic::indeterminate ),
        m_sliceMode(THREE_SLICES),
        m_backupedSliceMode(THREE_SLICES)
{
    m_allowAlphaInTF = false;
    m_interpolation  = true;

    addNewHandledEvent("SLICE_MODE");
    addNewHandledEvent("SCAN_SHOW");
    addNewHandledEvent( ::fwComEd::ImageMsg::CHANGE_SLICE_TYPE );
}

//------------------------------------------------------------------------------

NegatoMPR::~NegatoMPR() throw()
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void NegatoMPR::doStart() throw(fwTools::Failed)
{
    this->doUpdate();
}

//------------------------------------------------------------------------------

void NegatoMPR::doStop() throw(fwTools::Failed)
{
    this->unregisterServices();
}

//------------------------------------------------------------------------------

void NegatoMPR::doSwap() throw(fwTools::Failed)
{
    ::fwData::Image::sptr image = this->getObject< ::fwData::Image >();
    bool imageIsValid = ::fwComEd::fieldHelper::MedicalImageHelpers::checkImageValidity( image );

    if ( imageIsValid)
    {
        if(m_subServices.empty())
        {
            doStart();
        }
        else
        {
            BOOST_FOREACH( ServiceVector::value_type service, m_subServices)
            {
                OSLM_ASSERT("sub services expired in service : " << this->getSptr()->getID(), !service.expired());
                service.lock()->swap(image);
                service.lock()->update();
            }
        }
    }
    else
    {
        doStop();
    }
}

//------------------------------------------------------------------------------

void NegatoMPR::doUpdate() throw(::fwTools::Failed)
{
    this->doStop();
    if(!this->getSliceMode() == NO_SLICE)
    {
        if(this->getSliceMode() == ONE_SLICE)
        {
            this->addAdaptor("::visuVTKAdaptor::NegatoOneSlice", m_orientation);
        }
        else if(this->getSliceMode() == THREE_SLICES)
        {
            this->addAdaptor("::visuVTKAdaptor::NegatoOneSlice", X_AXIS);
            this->addAdaptor("::visuVTKAdaptor::NegatoOneSlice", Y_AXIS);
            this->addAdaptor("::visuVTKAdaptor::NegatoOneSlice", Z_AXIS);
        }

        this->addAdaptor("::visuVTKAdaptor::NegatoWindowingInteractor");
        this->addAdaptor("::visuVTKAdaptor::NegatoSlicingInteractor", m_orientation);
        this->addAdaptor("::visuVTKAdaptor::ProbeCursor", m_orientation);
        this->addAdaptor("::visuVTKAdaptor::SlicesCursor", m_orientation);
    }
    if(this->is3dModeEnabled())
    {
        this->addAdaptor("::visuVTKAdaptor::Medical3DCamera", m_orientation);
    }
    else if(!this->is3dModeEnabled())
    {
        this->addAdaptor("::visuVTKAdaptor::SliceFollowerCamera", m_orientation);
    }
    else
    {
        SLM_TRACE("No 2D/3D mode specified.");
    }
    this->setVtkPipelineModified();
}

//------------------------------------------------------------------------------

void NegatoMPR::doUpdate(::fwServices::ObjectMsg::csptr msg) throw(::fwTools::Failed)
{
    ::fwComEd::ImageMsg::csptr imageMsg = ::fwComEd::ImageMsg::dynamicConstCast( msg );

    if ( imageMsg && imageMsg->hasEvent( "SLICE_MODE"))
    {
        ::fwData::Object::csptr dataInfo = imageMsg->getDataInfo("SLICE_MODE");
        SLM_ASSERT("dataInfo is missing", dataInfo);
        SLM_ASSERT("m_relatedServiceId is missing", dataInfo->getField( ::fwComEd::Dictionary::m_relatedServiceId ) );
        std::string servId = dataInfo->getField< ::fwData::String >(::fwComEd::Dictionary::m_relatedServiceId)->value();
        if( servId ==   this->getSptr()->getID() )
        {
            ::fwData::Integer::csptr integer = ::fwData::Integer::dynamicConstCast(dataInfo);
            SLM_ASSERT("dataInfo is missing", integer);
            if(integer->value()== 0)
            {
                this->setSliceMode(NO_SLICE);
            }
            else if(integer->value()== 1)
            {
                this->setSliceMode(ONE_SLICE);
            }
            else if(integer->value()==3)
            {
                this->setSliceMode(THREE_SLICES);
            }
            else if(integer->value()==-1)
            {
                SLM_FATAL("not yet implemented :(");
            }
            else
            {
                OSLM_FATAL("Unknown slice mode "<< integer->value());
            }
            m_backupedSliceMode = this->getSliceMode();
            this->doUpdate();
        }
    }
    else if( imageMsg && imageMsg->hasEvent( "SCAN_SHOW"))
    {
        ::fwData::Object::csptr dataInfo = imageMsg->getDataInfo("SCAN_SHOW");
        SLM_ASSERT("dataInfo is missing", dataInfo);
        SLM_ASSERT("m_relatedServiceId is missing", dataInfo->getField( ::fwComEd::Dictionary::m_relatedServiceId ) );
        std::string servId = dataInfo->getField< ::fwData::String >(::fwComEd::Dictionary::m_relatedServiceId)->value();
        if( servId ==   this->getSptr()->getID() )
        {
            ::fwData::Boolean::csptr integer = ::fwData::Boolean::dynamicConstCast(dataInfo);
            if(integer->value())
            {
                this->setSliceMode(m_backupedSliceMode);
            }
            else
            {
                m_backupedSliceMode = this->getSliceMode();
                this->setSliceMode(NO_SLICE);
            }
            this->doUpdate();
        }
    }
    else if (imageMsg && imageMsg->hasEvent( ::fwComEd::ImageMsg::CHANGE_SLICE_TYPE ))
    {
        ::fwData::Object::csptr cObjInfo = imageMsg->getDataInfo( ::fwComEd::ImageMsg::CHANGE_SLICE_TYPE );
        ::fwData::Object::sptr objInfo = ::boost::const_pointer_cast< ::fwData::Object > ( cObjInfo );
        ::fwData::Composite::sptr info = ::fwData::Composite::dynamicCast ( objInfo );

        int fromSliceType = ::fwData::Integer::dynamicCast( info->getRefMap()["fromSliceType"] )->value();
        int toSliceType =   ::fwData::Integer::dynamicCast( info->getRefMap()["toSliceType"] )->value();

        if( toSliceType == static_cast<int>(m_orientation) )
        {
            setOrientation( static_cast< Orientation >( fromSliceType ));
        }
        else if(fromSliceType == static_cast<int>(m_orientation))
        {
            setOrientation( static_cast< Orientation >( toSliceType ));
        }
    }
}

//------------------------------------------------------------------------------

void NegatoMPR::configuring() throw(fwTools::Failed)
{
    assert(m_configuration->getName() == "config");
    this->setRenderId( m_configuration->getAttributeValue("renderer") );
    this->setPickerId( m_configuration->getAttributeValue("picker") );

    if (m_configuration->hasAttribute("mode"))
    {
        std::string value(m_configuration->getAttributeValue("mode"));
        std::transform(value.begin(), value.end(), value.begin(), tolower);
        OSLM_ASSERT("Sorry, bad value "<<value<<" for attribute mode.",
                value == "3d" || value == "2d");
        this->set3dMode(value == "3d");
    }
    if (m_configuration->hasAttribute("slices"))
    {
        std::string value(m_configuration->getAttributeValue("slices"));
        std::transform(value.begin(), value.end(), value.begin(), tolower);

        if(value == "0")
        {
            this->setSliceMode(NO_SLICE);
        }
        else if(value == "1")
        {
            this->setSliceMode(ONE_SLICE);
        }
        else if(value == "3")
        {
            this->setSliceMode(THREE_SLICES);
        }
        m_backupedSliceMode = this->getSliceMode();
    }
    if(m_configuration->hasAttribute("sliceIndex"))
    {
         std::string  orientation = m_configuration->getAttributeValue("sliceIndex");
         if(orientation == "axial" )
         {
             m_orientation = Z_AXIS;
         }
         else if(orientation == "frontal" )
         {
             m_orientation = Y_AXIS;
         }
         else if(orientation == "sagittal" )
         {
             m_orientation = X_AXIS;
         }
    }
    if(m_configuration->hasAttribute("transform") )
    {
        this->setTransformId( m_configuration->getAttributeValue("transform") );
    }
    if(m_configuration->hasAttribute("tfalpha") )
    {
        this->setAllowAlphaInTF(m_configuration->getAttributeValue("tfalpha") == "yes");
    }
    if (m_configuration->hasAttribute("interpolation"))
    {
        this->setInterpolation(!(m_configuration->getAttributeValue("interpolation") == "off"));
    }
    if (m_configuration->hasAttribute("vtkimagesource"))
    {
        this->setVtkImageSourceId( m_configuration->getAttributeValue("vtkimagesource") );
    }
    this->parseTFConfig( m_configuration );
}

//------------------------------------------------------------------------------

void  NegatoMPR::setSliceMode(SliceMode sliceMode)
{
    if(m_sliceMode != sliceMode)
    {
        m_sliceMode = sliceMode;
    }
}

//------------------------------------------------------------------------------

NegatoMPR::SliceMode NegatoMPR::getSliceMode()
{
    return m_sliceMode;
}

//------------------------------------------------------------------------------

::boost::logic::tribool NegatoMPR::is3dModeEnabled()
{
    return m_3dModeEnabled;
}

//------------------------------------------------------------------------------

void NegatoMPR::set3dMode( bool enabled )
{
    m_3dModeEnabled = enabled;
}

//------------------------------------------------------------------------------

void NegatoMPR::addAdaptor(std::string adaptor, int axis)
{
    ::fwData::Image::sptr image = this->getObject< ::fwData::Image >();
    ::fwRenderVTK::IVtkAdaptorService::sptr service;

    if(axis >= 0)
    {
        service = ::fwServices::add< ::fwRenderVTK::IVtkAdaptorService >( image, adaptor );
        SLM_ASSERT("service not instanced", service);
        ::fwComEd::helper::MedicalImageAdaptor::sptr adaptorSrv =
                ::fwComEd::helper::MedicalImageAdaptor::dynamicCast(service);
        SLM_ASSERT("adaptorSrv not instanced", adaptorSrv);
        adaptorSrv->setOrientation((Orientation) axis);
    }
    else
    {
        service = ::fwServices::add< ::fwRenderVTK::IVtkAdaptorService >( image, adaptor );
        SLM_ASSERT("service not instanced", service);
    }


    ::visuVTKAdaptor::NegatoOneSlice::sptr negatoAdaptor;
    negatoAdaptor = ::visuVTKAdaptor::NegatoOneSlice::dynamicCast(service);

    ::visuVTKAdaptor::NegatoWindowingInteractor::sptr negatoWindowingAdaptor;
    negatoWindowingAdaptor = ::visuVTKAdaptor::NegatoWindowingInteractor::dynamicCast(service);

    if (negatoAdaptor)
    {
        negatoAdaptor->setAllowAlphaInTF(m_allowAlphaInTF);
        negatoAdaptor->setInterpolation(m_interpolation);
        if (!m_imageSourceId.empty())
        {
            negatoAdaptor->setVtkImageSourceId(m_imageSourceId);
        }
        negatoAdaptor->setSelectedTFKey( this->getSelectedTFKey() );
        negatoAdaptor->setTFSelectionFwID( this->getTFSelectionFwID() );
    }
    else if (negatoWindowingAdaptor)
    {
        negatoWindowingAdaptor->setSelectedTFKey( this->getSelectedTFKey() );
        negatoWindowingAdaptor->setTFSelectionFwID( this->getTFSelectionFwID() );
    }

    service->setRenderService(this->getRenderService());
    service->setRenderId( this->getRenderId() );
    service->setPickerId( this->getPickerId() );
    service->setTransformId( this->getTransformId() );

    service->start();
    service->update();
    this->registerService(service);
}

//------------------------------------------------------------------------------

} //namespace visuVTKAdaptor
