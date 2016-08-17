/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "arData/FrameTL.hpp"

#include <fwCore/base.hpp>
#include <fwData/registry/macros.hpp>
#include <fwData/Exception.hpp>

#include <boost/bind.hpp>
#include <boost/pool/pool.hpp>

fwDataRegisterMacro( ::arData::FrameTL );

namespace arData
{

//------------------------------------------------------------------------------

FrameTL::FrameTL ( ::fwData::Object::Key key ) :
    GenericTL< uint8_t >(key),
    m_width(0),
    m_height(0),
    m_numberOfComponents(3),
    m_type()
{
    // Default to 100 frames since images used to eat a lot of memory...
    this->setMaximumSize(100);
}

//------------------------------------------------------------------------------

FrameTL::~FrameTL ()
{
}

//------------------------------------------------------------------------------

void FrameTL::cachedDeepCopy(const Object::csptr& _source, DeepCopyCacheType& cache)
{
    FrameTL::csptr other = FrameTL::dynamicConstCast(_source);
    FW_RAISE_EXCEPTION_IF( ::fwData::Exception(
                               "Unable to copy" + (_source ? _source->getClassname() : std::string("<NULL>"))
                               + " to " + this->getClassname()), !bool(other) );
    this->fieldDeepCopy( _source );

    this->clearTimeline();
    this->initPoolSize(other->m_width, other->m_height, other->m_type, other->m_numberOfComponents);
    ::fwCore::mt::WriteLock writeLock(m_tlMutex);
    ::fwCore::mt::WriteLock readLock(other->m_tlMutex);

    for(TimelineType::value_type elt :  other->m_timeline)
    {
        SPTR(::arData::timeline::Buffer) tlObj = this->createBuffer(elt.first);
        tlObj->deepCopy(*elt.second);
        m_timeline.insert(TimelineType::value_type(elt.first, tlObj));
    }
}

//------------------------------------------------------------------------------

void FrameTL::initPoolSize(size_t width, size_t height, const ::fwTools::Type& type, size_t numberOfComponents,
                           unsigned int maxElementNum)
{
    m_width              = width;
    m_height             = height;
    m_numberOfComponents = numberOfComponents;
    m_type               = type;

    std::size_t size = width * height * numberOfComponents * type.sizeOf();

    m_maxElementNum = maxElementNum;
    this->allocPoolSize(size * m_maxElementNum);
}

//------------------------------------------------------------------------------

void FrameTL::initPoolSize(unsigned int maxElementNum)
{
    SLM_ERROR("This function should not be called");
}

} // namespace arData


