/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "visuVTKAdaptor/Texture.hpp"
#include "visuVTKAdaptor/Mesh.hpp"

#include <fwCom/Signal.hxx>
#include <fwCom/Slot.hxx>
#include <fwCom/Slots.hxx>

#include <fwComEd/ImageMsg.hpp>
#include <fwComEd/MaterialMsg.hpp>
#include <fwComEd/fieldHelper/MedicalImageHelpers.hpp>

#include <fwData/Image.hpp>
#include <fwData/Material.hpp>
#include <fwData/Mesh.hpp>
#include <fwData/Reconstruction.hpp>
#include <fwMedData/ModelSeries.hpp>
#include <fwData/mt/ObjectReadLock.hpp>
#include <fwData/mt/ObjectWriteLock.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/Base.hpp>
#include <fwServices/registry/ObjectService.hpp>
#include <fwServices/IEditionService.hpp>

#include <vtkTexture.h>
#include <vtkRenderWindowInteractor.h>

#include <boost/foreach.hpp>

fwServicesRegisterMacro( ::fwRenderVTK::IVtkAdaptorService, ::visuVTKAdaptor::Texture, ::fwData::Image );

namespace visuVTKAdaptor
{

const ::fwCom::Slots::SlotKeyType Texture::s_APPLY_TEXTURE_SLOT = "applyTexture";

//------------------------------------------------------------------------------

Texture::Texture() throw() :
    m_filtering("linear"),
    m_wrapping("repeat"),
    m_lighting(true)
{
    m_slotApplyTexture = ::fwCom::newSlot( &Texture::applyTexture, this );
    ::fwCom::HasSlots::m_slots( s_APPLY_TEXTURE_SLOT, m_slotApplyTexture);
    ::fwCom::HasSlots::m_slots.setWorker( m_associatedWorker );

 #ifdef COM_LOG
    ::fwCom::HasSlots::m_slots.setID();
 #endif
}

//------------------------------------------------------------------------------

Texture::~Texture() throw()
{
}

//------------------------------------------------------------------------------

void Texture::configuring() throw(fwTools::Failed)
{
    SLM_ASSERT("Missing configuration", m_configuration->getName() == "config");

    if ( m_configuration->hasAttribute( "autoRender" ) )
    {
        const std::string autoRender = m_configuration->getAttributeValue("autoRender");
        const bool autoRenderValue   = (autoRender == "true");
        this->setAutoRender(autoRenderValue);
    }

    if ( m_configuration->hasAttribute( "filtering" ) )
    {
        m_filtering = m_configuration->getAttributeValue("filtering");
    }

    if ( m_configuration->hasAttribute( "wrapping" ) )
    {
        m_wrapping = m_configuration->getAttributeValue("wrapping");
    }
    if ( m_configuration->hasAttribute( "lighting" ) )
    {
        m_lighting = (m_configuration->getAttributeValue("lighting") == "yes");
    }
}

//------------------------------------------------------------------------------

void Texture::doStart() throw(fwTools::Failed)
{
}

//------------------------------------------------------------------------------

void Texture::doUpdate() throw(fwTools::Failed)
{
    BOOST_FOREACH(::fwData::Material::sptr material, m_materialSet)
    {
        applyTexture(material);
    }
}

//------------------------------------------------------------------------------
void Texture::doSwap() throw(fwTools::Failed)
{
    this->doUpdate();
}

//------------------------------------------------------------------------------

void Texture::doStop() throw(fwTools::Failed)
{
    this->unregisterServices();
}

//------------------------------------------------------------------------------

void Texture::doReceive( ::fwServices::ObjectMsg::csptr msg) throw(fwTools::Failed)
{
    ::fwComEd::ImageMsg::csptr textureMsg = ::fwComEd::ImageMsg::dynamicConstCast(msg);
    if (textureMsg && (
            textureMsg->hasEvent( ::fwComEd::ImageMsg::MODIFIED) ||
            textureMsg->hasEvent( ::fwComEd::ImageMsg::BUFFER ) ||
            textureMsg->hasEvent( ::fwComEd::ImageMsg::NEW_IMAGE ) ) )
    {
        doUpdate();
    }
}

//------------------------------------------------------------------------------

void Texture::applyTexture( SPTR(::fwData::Material)_material )
{
    if(m_materialSet.count(_material) == 0)
    {
        m_materialSet.insert(_material);
    }

    ::fwData::Image::sptr image = this->getObject< ::fwData::Image >();

    if(!::fwComEd::fieldHelper::MedicalImageHelpers::checkImageValidity(image))
    {
        return;
    }

    _material->setDiffuseTexture(image);
    _material->setLighting(m_lighting);

    ::fwData::Material::FilteringType filtering = ::fwData::Material::LINEAR;
    ::fwData::Material::WrappingType wrapping   = ::fwData::Material::REPEAT;

    if(m_filtering == "nearest")
    {
        filtering = ::fwData::Material::NEAREST;
    }
    else if(m_filtering == "linear")
    {
        filtering = ::fwData::Material::LINEAR;
    }
    else
    {
        OSLM_WARN("Texture filtering type unknown or not supported : " << m_filtering);
    }
    _material->setDiffuseTextureFiltering(filtering);

    if(m_wrapping == "repeat")
    {
        wrapping = ::fwData::Material::REPEAT;
    }
    else if(m_wrapping == "clamp")
    {
        wrapping = ::fwData::Material::CLAMP;
    }
    else
    {
        OSLM_WARN("Texture wrapping type unknown or not supported : " << m_wrapping);
    }
    _material->setDiffuseTextureWrapping(wrapping);

    ::fwComEd::MaterialMsg::sptr msg = ::fwComEd::MaterialMsg::New();
    msg->addEvent(::fwComEd::MaterialMsg::MATERIAL_IS_MODIFIED);
    ::fwServices::IEditionService::notify(this->getSptr(), _material, msg);
}

} //namespace visuVTKAdaptor
