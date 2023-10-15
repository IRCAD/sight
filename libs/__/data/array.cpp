/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include "data/array.hpp"

#include "data/exception.hpp"
#include "data/registry/macros.hpp"

#include <algorithm>
#include <cstdlib>
#include <functional>
#include <numeric>

namespace sight::data
{

SIGHT_REGISTER_DATA(sight::data::array);

//------------------------------------------------------------------------------

inline static std::size_t computeSize(
    std::size_t elementSize,
    const data::array::SizeType& size
)
{
    std::size_t total = 0;
    if(!size.empty())
    {
        total  = elementSize;
        total *=
            std::accumulate(
                size.begin(),
                size.end(),
                static_cast<std::size_t>(1),
                std::multiplies<>()
            );
    }

    return total;
}

//------------------------------------------------------------------------------

data::array::OffsetType array::computeStrides(SizeType size, std::size_t sizeOfType)
{
    data::array::OffsetType strides;
    strides.reserve(size.size());

    std::size_t currentStride = sizeOfType;
    for(const SizeType::value_type& s : size)
    {
        strides.push_back(currentStride);
        currentStride *= s;
    }

    return strides;
}

//------------------------------------------------------------------------------

array::array() :
    m_bufferObject(std::make_shared<core::memory::buffer_object>())
{
}

//------------------------------------------------------------------------------

array::~array()
{
    this->clear();
}

//------------------------------------------------------------------------------

void array::swap(array::sptr _source) noexcept
{
    m_fields.swap(_source->m_fields);
    m_strides.swap(_source->m_strides);
    m_size.swap(_source->m_size);
    m_bufferObject->swap(_source->m_bufferObject);

    std::swap(m_type, _source->m_type);
    std::swap(m_isBufferOwner, _source->m_isBufferOwner);
}

//------------------------------------------------------------------------------

void array::shallow_copy(const object::csptr& /*unused*/)
{
    SIGHT_FATAL("shallow_copy not implemented for : " + this->get_classname());
}

//------------------------------------------------------------------------------

void array::deep_copy(const object::csptr& source, const std::unique_ptr<deep_copy_cache_t>& cache)
{
    const auto& other = std::dynamic_pointer_cast<const array>(source);

    SIGHT_THROW_EXCEPTION_IF(
        exception(
            "Unable to copy " + (source ? source->get_classname() : std::string("<NULL>"))
            + " to " + get_classname()
        ),
        !bool(other)
    );

    if(!other->m_bufferObject->is_empty())
    {
        resize(other->m_size, other->m_type, true);
        std::memcpy(m_bufferObject->buffer(), other->m_bufferObject->buffer(), other->getSizeInBytes());
    }
    else
    {
        this->clear();

        m_strides = other->m_strides;
        m_type    = other->m_type;
        m_size    = other->m_size;
    }

    base_class::deep_copy(other, cache);
}

//------------------------------------------------------------------------------

std::size_t array::resize(
    const SizeType& size,
    const core::type& type,
    bool reallocate
)
{
    const std::size_t bufSize = computeSize(type.size(), size);

    if(reallocate && bufSize != m_bufferObject->size())
    {
        if(m_bufferObject->is_empty())
        {
            m_isBufferOwner = true;
            m_bufferObject->allocate(bufSize);
        }
        else if(m_isBufferOwner)
        {
            m_bufferObject->reallocate(bufSize);
        }
        else
        {
            SIGHT_THROW_EXCEPTION_MSG(
                data::exception,
                "Tried to reallocate a not-owned Buffer."
            );
        }
    }

    m_strides = computeStrides(size, type.size());
    m_type    = type;
    m_size    = size;

    return bufSize;
}

//------------------------------------------------------------------------------

std::size_t array::resize(const SizeType& size, bool reallocate)
{
    return this->resize(size, m_type, reallocate);
}

//------------------------------------------------------------------------------

void array::clear()
{
    if(!this->m_bufferObject->is_empty())
    {
        if(m_isBufferOwner)
        {
            this->m_bufferObject->destroy();
        }

        m_strides.clear();
        m_type = core::type::NONE;
        m_size.clear();
    }
}

//------------------------------------------------------------------------------

bool array::empty() const
{
    return m_size.empty();
}

//------------------------------------------------------------------------------

std::size_t array::getElementSizeInBytes() const
{
    return m_type.size();
}

//------------------------------------------------------------------------------

std::size_t array::numElements() const
{
    return computeSize(1, m_size);
}

//------------------------------------------------------------------------------

std::size_t array::getSizeInBytes() const
{
    return computeSize(m_type.size(), m_size);
}

//------------------------------------------------------------------------------

const data::array::SizeType& array::size() const
{
    return m_size;
}

//------------------------------------------------------------------------------

const data::array::OffsetType& array::getStrides() const
{
    return m_strides;
}

//------------------------------------------------------------------------------

std::size_t array::numDimensions() const
{
    return m_size.size();
}

//------------------------------------------------------------------------------

void array::setIsBufferOwner(const bool own)
{
    m_isBufferOwner = own;
}

//------------------------------------------------------------------------------

bool array::getIsBufferOwner() const
{
    return m_isBufferOwner;
}

//------------------------------------------------------------------------------

core::type array::getType() const
{
    return m_type;
}

//------------------------------------------------------------------------------

std::size_t array::getBufferOffset(const data::array::IndexType& id) const
{
    SIGHT_THROW_EXCEPTION_IF(
        data::exception(
            "Given index has " + std::to_string(id.size()) + " dimensions, but array has "
            + std::to_string(m_size.size()) + " dimensions."
        ),
        id.size() != m_size.size()
    );

    OffsetType offsets(id.size());

    std::transform(
        id.begin(),
        id.end(),
        m_strides.begin(),
        offsets.begin(),
        std::multiplies<>()
    );

    const std::size_t offset = std::accumulate(offsets.begin(), offsets.end(), static_cast<std::size_t>(0));

    return offset;
}

//------------------------------------------------------------------------------

void* array::buffer()
{
    SIGHT_THROW_EXCEPTION_IF(
        data::exception(
            "The buffer cannot be accessed if the array is not locked for dump "
            "(see lock())"
        ),
        !m_bufferObject->is_locked()
    );
    return m_bufferObject->buffer();
}

//-----------------------------------------------------------------------------

const void* array::buffer() const
{
    SIGHT_THROW_EXCEPTION_IF(
        data::exception("The buffer cannot be accessed if the array is not locked"),
        !m_bufferObject->is_locked()
    );
    return m_bufferObject->buffer();
}

//------------------------------------------------------------------------------

void array::setBuffer(void* buf, bool takeOwnership, core::memory::buffer_allocation_policy::sptr policy)
{
    if(m_bufferObject)
    {
        if(!m_bufferObject->is_empty())
        {
            m_bufferObject->destroy();
        }
    }
    else
    {
        core::memory::buffer_object::sptr newBufferObject = std::make_shared<core::memory::buffer_object>();
        m_bufferObject->swap(newBufferObject);
    }

    m_bufferObject->set_buffer(buf, (buf == nullptr) ? 0 : this->getSizeInBytes(), policy);
    this->setIsBufferOwner(takeOwnership);
}

//------------------------------------------------------------------------------

void array::setBuffer(
    void* buf,
    bool takeOwnership,
    const data::array::SizeType& size,
    const core::type& type,
    core::memory::buffer_allocation_policy::sptr policy
)
{
    this->resize(size, type, false);
    this->setBuffer(buf, takeOwnership, policy);
}

//-----------------------------------------------------------------------------

char* array::getBufferPtr(const data::array::IndexType& id)
{
    const std::size_t offset = this->getBufferOffset(id);
    char* item               = static_cast<char*>(this->buffer()) + offset;
    return item;
}

//------------------------------------------------------------------------------

const char* array::getBufferPtr(const data::array::IndexType& id) const
{
    const std::size_t offset = this->getBufferOffset(id);
    const char* item         = static_cast<const char*>(this->buffer()) + offset;
    return item;
}

//------------------------------------------------------------------------------

void array::dump_lock_impl(std::vector<core::memory::buffer_object::lock_t>& locks) const
{
    locks.push_back(m_bufferObject->lock());
}

//------------------------------------------------------------------------------

array::iterator<char> array::begin()
{
    return {static_cast<char*>(buffer())};
}

//------------------------------------------------------------------------------

array::iterator<char> array::end()
{
    auto itr = iterator<char>(static_cast<char*>(buffer()));
    itr += static_cast<std::ptrdiff_t>(this->getSizeInBytes());
    return itr;
}

//------------------------------------------------------------------------------

array::const_iterator<char> array::begin() const
{
    return {static_cast<const char*>(buffer())};
}

//------------------------------------------------------------------------------

array::const_iterator<char> array::end() const
{
    auto itr = const_iterator<char>(static_cast<const char*>(buffer()));
    itr += static_cast<std::ptrdiff_t>(this->getSizeInBytes());
    return itr;
}

//------------------------------------------------------------------------------

bool array::operator==(const array& other) const noexcept
{
    if(m_strides != other.m_strides
       || m_type != other.m_type
       || m_size != other.m_size
       || !core::tools::is_equal(m_bufferObject, other.m_bufferObject))
    {
        return false;
    }

    // Super class last
    return base_class::operator==(other);
}

//------------------------------------------------------------------------------

bool array::operator!=(const array& other) const noexcept
{
    return !(*this == other);
}

} //namespace sight::data
