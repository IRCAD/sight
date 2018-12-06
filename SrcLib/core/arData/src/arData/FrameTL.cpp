/************************************************************************
 *
 * Copyright (C) 2009-2017 IRCAD France
 * Copyright (C) 2012-2017 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#include "arData/FrameTL.hpp"

#include <fwCore/base.hpp>

#include <fwData/Exception.hpp>
#include <fwData/registry/macros.hpp>

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

    SLM_ASSERT("width or height or numberOfComponents is null", size != 0);

    m_maxElementNum = maxElementNum;
    this->allocPoolSize(size * m_maxElementNum);
}

//------------------------------------------------------------------------------

void FrameTL::initPoolSize(unsigned int maxElementNum)
{
    SLM_ERROR("This function should not be called");
}

} // namespace arData

