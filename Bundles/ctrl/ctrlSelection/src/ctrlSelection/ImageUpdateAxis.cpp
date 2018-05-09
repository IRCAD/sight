/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "ctrlSelection/ImageUpdateAxis.hpp"

#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>

#include <fwData/Float.hpp>
#include <fwData/Image.hpp>

#include <fwServices/macros.hpp>

#include <boost/algorithm/string/case_conv.hpp>
#include <boost/algorithm/string/trim.hpp>

namespace ctrlSelection
{

static const ::fwCom::Slots::SlotKeyType s_UPDATE_SLICE_TYPE_SLOT = "updateSliceType";

//-----------------------------------------------------------------------------

fwServicesRegisterMacro( ::fwServices::IController, ::ctrlSelection::ImageUpdateAxis, ::fwData::Image );

//-----------------------------------------------------------------------------

ImageUpdateAxis::ImageUpdateAxis() noexcept
{
}

//-----------------------------------------------------------------------------

ImageUpdateAxis::~ImageUpdateAxis() noexcept
{
}

//-----------------------------------------------------------------------------

void ImageUpdateAxis::updateSliceType(int from, int to)
{
    if( to == static_cast< int > ( m_orientation ) )
    {
        m_orientation =
            static_cast< ::fwDataTools::helper::MedicalImageAdaptor::Orientation > ( from );
    }
    else if(from == static_cast<int>(m_orientation))
    {
        m_orientation = static_cast< ::fwDataTools::helper::MedicalImageAdaptor::Orientation >( to );
    }
    this->updating();
}

//-----------------------------------------------------------------------------

void ImageUpdateAxis::starting()
{
    FW_DEPRECATED_MSG("This service is deprecated and will be removed");
    this->updating();
}

//-----------------------------------------------------------------------------

void ImageUpdateAxis::stopping()
{
}

//-----------------------------------------------------------------------------

void ImageUpdateAxis::swapping()
{
}

//-----------------------------------------------------------------------------

void ImageUpdateAxis::configuring()
{
    ::fwRuntime::ConfigurationElement::sptr config = m_configuration->findConfigurationElement("axis");
    SLM_ASSERT("Problem with configuration for ImageUpdateAxis type, one element \"axis\" must be present", m_configuration->findAllConfigurationElement(
                   "axis").size() == 1 );

    SLM_FATAL_IF( "The attribute \"uid\" is missing, it represents the fwID of the ::fwData::Float to update",
                  !config->hasAttribute("uid") );
    m_floatID = config->getExistingAttributeValue("uid");

    SLM_FATAL_IF( "The attribute \"orientation\" is missing, it represents the image orientation "
                  "(axial, frontal or sagittal)", !config->hasAttribute("orientation") );
    std::string orientation = config->getExistingAttributeValue("orientation");
    ::boost::algorithm::trim(orientation);
    ::boost::algorithm::to_lower(orientation);

    if(orientation == "axial" )
    {
        m_orientation = ::fwDataTools::helper::MedicalImageAdaptor::Z_AXIS;
    }
    else if(orientation == "frontal" )
    {
        m_orientation = ::fwDataTools::helper::MedicalImageAdaptor::Y_AXIS;
    }
    else if(orientation == "sagittal" )
    {
        m_orientation = ::fwDataTools::helper::MedicalImageAdaptor::X_AXIS;
    }
    else
    {
        SLM_FATAL("The value for the xml element \"value\" can only be axial, frontal or sagittal.");
    }
}

//-----------------------------------------------------------------------------

void ImageUpdateAxis::reconfiguring()
{
}

//-----------------------------------------------------------------------------

void ImageUpdateAxis::updating()
{
    OSLM_ASSERT("Missing object " << m_floatID, ::fwTools::fwID::exist(m_floatID));
    ::fwData::Float::sptr dataFloat = ::fwData::Float::dynamicCast(::fwTools::fwID::getObject(m_floatID));
    OSLM_ASSERT("Object " << m_floatID << " is not a valid ::fwData::Float", dataFloat);

    dataFloat->value() = (float) m_orientation;
    OSLM_TRACE(dataFloat->getID() << " new value : " << *dataFloat);

    auto sig = dataFloat->signal< ::fwData::Object::ModifiedSignalType >(::fwData::Object::s_MODIFIED_SIG);
    sig->asyncEmit();
}

//-----------------------------------------------------------------------------

void ImageUpdateAxis::info( std::ostream& _sstream )
{
}

//------------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsType ImageUpdateAxis::getObjSrvConnections() const
{
    KeyConnectionsType connections;
    connections.push_back( std::make_pair( ::fwData::Image::s_SLICE_TYPE_MODIFIED_SIG, s_UPDATE_SLICE_TYPE_SLOT ) );

    return connections;
}

//-----------------------------------------------------------------------------

} // ctrlSelection
