/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2011.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/foreach.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string/case_conv.hpp>

#include <fwServices/macros.hpp>

#include <fwData/Image.hpp>
#include <fwData/Float.hpp>

#include <fwComEd/ImageMsg.hpp>
#include <fwComEd/FloatMsg.hpp>

#include <fwServices/IEditionService.hpp>

#include "ctrlSelection/ImageUpdateAxis.hpp"

namespace ctrlSelection
{

//-----------------------------------------------------------------------------

REGISTER_SERVICE( ::fwServices::IController, ::ctrlSelection::ImageUpdateAxis, ::fwData::Image ) ;

//-----------------------------------------------------------------------------

ImageUpdateAxis::ImageUpdateAxis() throw()
{
    addNewHandledEvent( ::fwComEd::ImageMsg::CHANGE_SLICE_TYPE );
}

//-----------------------------------------------------------------------------

ImageUpdateAxis::~ImageUpdateAxis() throw()
{}

//-----------------------------------------------------------------------------

void ImageUpdateAxis::updating( ::fwServices::ObjectMsg::csptr message ) throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();
    ::fwComEd::ImageMsg::csptr imageMsg = ::fwComEd::ImageMsg::dynamicConstCast(message);
    if (imageMsg && imageMsg->hasEvent(::fwComEd::ImageMsg::CHANGE_SLICE_TYPE))
    {
        ::fwData::Object::csptr cObjInfo = imageMsg->getDataInfo( ::fwComEd::ImageMsg::CHANGE_SLICE_TYPE );
        ::fwData::Object::sptr objInfo = ::boost::const_pointer_cast< ::fwData::Object > ( cObjInfo );
        ::fwData::Composite::sptr info = ::fwData::Composite::dynamicCast ( objInfo );

        ::fwData::Integer::sptr fromSliceType = ::fwData::Integer::dynamicCast( info->getRefMap()["fromSliceType"] );
        ::fwData::Integer::sptr toSliceType = ::fwData::Integer::dynamicCast( info->getRefMap()["toSliceType"] );

        if( toSliceType->value() == static_cast< int > ( m_orientation ) )
        {
            m_orientation = static_cast< ::fwComEd::helper::MedicalImageAdaptor::Orientation > ( fromSliceType->value() );
        }
        else if(fromSliceType->value() == static_cast<int>(m_orientation))
        {
            m_orientation = static_cast< ::fwComEd::helper::MedicalImageAdaptor::Orientation >( toSliceType->value() );
        }

        this->updating();
    }
}

//-----------------------------------------------------------------------------

void ImageUpdateAxis::starting()  throw ( ::fwTools::Failed )
{
    this->updating();
}

//-----------------------------------------------------------------------------

void ImageUpdateAxis::stopping()  throw ( ::fwTools::Failed )
{}

//-----------------------------------------------------------------------------

void ImageUpdateAxis::swapping()  throw ( ::fwTools::Failed )
{}

//-----------------------------------------------------------------------------

void ImageUpdateAxis::configuring()  throw ( ::fwTools::Failed )
{
    ::fwRuntime::ConfigurationElement::sptr config = m_configuration->findConfigurationElement("axis");
    SLM_ASSERT("Problem with configuration for ImageUpdateAxis type, one element \"axis\" must be present", m_configuration->findAllConfigurationElement("axis").size() == 1 );

    SLM_FATAL_IF( "Sorry, attribute \"uid\" is missing", !config->hasAttribute("uid") );
    m_floatID =  config->getExistingAttributeValue("uid");

    SLM_FATAL_IF( "Sorry, attribute \"orientation\" is missing", !config->hasAttribute("orientation") );
    std::string  orientation = config->getExistingAttributeValue("orientation");
    ::boost::algorithm::trim(orientation);
    ::boost::algorithm::to_lower(orientation);

    if(orientation == "axial" )
    {
        m_orientation = ::fwComEd::helper::MedicalImageAdaptor::Z_AXIS;
    }
    else if(orientation == "frontal" )
    {
        m_orientation = ::fwComEd::helper::MedicalImageAdaptor::Y_AXIS;
    }
    else if(orientation == "sagittal" )
    {
        m_orientation = ::fwComEd::helper::MedicalImageAdaptor::X_AXIS;
    }
    else
    {
        SLM_FATAL("The value for the xml element \"value\" can only be axial, frontal or sagittal.");
    }
}

//-----------------------------------------------------------------------------

void ImageUpdateAxis::reconfiguring()  throw ( ::fwTools::Failed )
{}

//-----------------------------------------------------------------------------

void ImageUpdateAxis::updating() throw ( ::fwTools::Failed )
{
    OSLM_ASSERT("Missing object " << m_floatID, ::fwTools::fwID::exist(m_floatID));
    ::fwData::Float::sptr dataFloat = ::fwData::Float::dynamicCast(::fwTools::fwID::getObject(m_floatID));
    OSLM_ASSERT("Object " << m_floatID << " is not a valid ::fwData::Float", dataFloat);

    dataFloat->value() = (float) m_orientation;
    OSLM_TRACE(dataFloat->getID() << " new value : " << *dataFloat);
    ::fwComEd::FloatMsg::NewSptr msg;
    msg->addEvent( ::fwComEd::FloatMsg::VALUE_IS_MODIFIED );
    ::fwServices::IEditionService::notify(this->getSptr(), dataFloat, msg);
}

//-----------------------------------------------------------------------------

void ImageUpdateAxis::info( std::ostream &_sstream )
{}

//-----------------------------------------------------------------------------

} // ctrlSelection
