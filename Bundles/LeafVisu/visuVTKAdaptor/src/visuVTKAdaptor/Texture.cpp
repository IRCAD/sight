/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "visuVTKAdaptor/Texture.hpp"

#include "visuVTKAdaptor/Mesh.hpp"

#include <fwCom/Signal.hxx>
#include <fwCom/Slot.hxx>
#include <fwCom/Slots.hxx>

#include <fwData/Image.hpp>
#include <fwData/Material.hpp>
#include <fwData/Mesh.hpp>
#include <fwData/Reconstruction.hpp>
#include <fwData/mt/ObjectReadLock.hpp>
#include <fwData/mt/ObjectWriteLock.hpp>

#include <fwDataTools/fieldHelper/MedicalImageHelpers.hpp>

#include <fwMedData/ModelSeries.hpp>

#include <fwServices/macros.hpp>

#include <vtkRenderWindowInteractor.h>
#include <vtkTexture.h>

fwServicesRegisterMacro( ::fwRenderVTK::IVtkAdaptorService, ::visuVTKAdaptor::Texture, ::fwData::Image );

namespace visuVTKAdaptor
{

const ::fwCom::Slots::SlotKeyType Texture::s_APPLY_TEXTURE_SLOT = "applyTexture";

//------------------------------------------------------------------------------

Texture::Texture() noexcept :
    m_filtering("linear"),
    m_wrapping("repeat"),
    m_lighting(true)
{
    m_slotApplyTexture = ::fwCom::newSlot( &Texture::applyTexture, this );
    ::fwCom::HasSlots::m_slots( s_APPLY_TEXTURE_SLOT, m_slotApplyTexture);
    ::fwCom::HasSlots::m_slots.setWorker( m_associatedWorker );
}

//------------------------------------------------------------------------------

Texture::~Texture() noexcept
{
}

//------------------------------------------------------------------------------

void Texture::doConfigure()
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

void Texture::doStart()
{
}

//------------------------------------------------------------------------------

void Texture::doUpdate()
{
    for(::fwData::Material::sptr material :  m_materialSet)
    {
        applyTexture(material);
    }
}

//------------------------------------------------------------------------------
void Texture::doSwap()
{
    this->doUpdate();
}

//------------------------------------------------------------------------------

void Texture::doStop()
{
    this->unregisterServices();
}

//------------------------------------------------------------------------------

void Texture::applyTexture( SPTR(::fwData::Material)_material )
{
    if(m_materialSet.count(_material) == 0)
    {
        m_materialSet.insert(_material);
    }

    ::fwData::Image::sptr image = this->getObject< ::fwData::Image >();

    if(!::fwDataTools::fieldHelper::MedicalImageHelpers::checkImageValidity(image))
    {
        return;
    }

    _material->setDiffuseTexture(image);
    if(m_lighting == false)
    {
        _material->setShadingMode(::fwData::Material::AMBIENT);
    }

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

    ::fwData::Object::ModifiedSignalType::sptr sig;
    sig = _material->signal< ::fwData::Object::ModifiedSignalType >(::fwData::Object::s_MODIFIED_SIG);
    {
        ::fwCom::Connection::Blocker block(sig->getConnection(m_slotUpdate));
        sig->asyncEmit();
    }
}

//------------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsType Texture::getObjSrvConnections() const
{
    KeyConnectionsType connections;
    connections.push_back( std::make_pair( ::fwData::Image::s_MODIFIED_SIG, s_UPDATE_SLOT ) );
    connections.push_back( std::make_pair( ::fwData::Image::s_BUFFER_MODIFIED_SIG, s_UPDATE_SLOT ) );

    return connections;
}

//------------------------------------------------------------------------------


} //namespace visuVTKAdaptor
