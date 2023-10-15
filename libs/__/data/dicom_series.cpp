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

#include "data/dicom_series.hpp"

#include <core/memory/stream/in/raw.hpp>

#include <data/exception.hpp>
#include <data/registry/macros.hpp>

#include <filesystem>

SIGHT_REGISTER_DATA(sight::data::dicom_series)

namespace sight::data
{

//------------------------------------------------------------------------------

void dicom_series::shallow_copy(const object::csptr& source)
{
    const auto& other = std::dynamic_pointer_cast<const dicom_series>(source);

    SIGHT_THROW_EXCEPTION_IF(
        exception(
            "Unable to copy " + (source ? source->get_classname() : std::string("<NULL>"))
            + " to " + get_classname()
        ),
        !bool(other)
    );

    m_numberOfInstances   = other->m_numberOfInstances;
    m_dicomContainer      = other->m_dicomContainer;
    m_sop_classUIDs       = other->m_sop_classUIDs;
    m_computedTagValues   = other->m_computedTagValues;
    m_firstInstanceNumber = other->m_firstInstanceNumber;

    base_class::shallow_copy(other);
}

//------------------------------------------------------------------------------

void dicom_series::deep_copy(const object::csptr& source, const std::unique_ptr<deep_copy_cache_t>& cache)
{
    const auto& other = std::dynamic_pointer_cast<const dicom_series>(source);

    SIGHT_THROW_EXCEPTION_IF(
        exception(
            "Unable to copy " + (source ? source->get_classname() : std::string("<NULL>"))
            + " to " + get_classname()
        ),
        !bool(other)
    );

    m_numberOfInstances   = other->m_numberOfInstances;
    m_sop_classUIDs       = other->m_sop_classUIDs;
    m_computedTagValues   = other->m_computedTagValues;
    m_firstInstanceNumber = other->m_firstInstanceNumber;

    m_dicomContainer.clear();
    for(const auto& elt : other->m_dicomContainer)
    {
        const core::memory::buffer_object::sptr& bufferSrc = elt.second;
        core::memory::buffer_object::lock_t lockerSource(bufferSrc);

        if(!bufferSrc->is_empty())
        {
            auto bufferDest = std::make_shared<core::memory::buffer_object>(true);
            core::memory::buffer_object::lock_t lockerDest(bufferDest);

            bufferDest->allocate(bufferSrc->size());

            char* buffDest = static_cast<char*>(lockerDest.buffer());
            char* buffSrc  = static_cast<char*>(lockerSource.buffer());
            std::copy(buffSrc, buffSrc + bufferSrc->size(), buffDest);

            m_dicomContainer[elt.first] = bufferDest;
        }
    }

    base_class::deep_copy(other, cache);
}

//------------------------------------------------------------------------------

void dicom_series::addDicomPath(std::size_t _instanceIndex, const std::filesystem::path& _path)
{
    SIGHT_THROW_EXCEPTION_IF(
        std::filesystem::filesystem_error(
            std::string("Dicom path can not be found: "),
            _path,
            std::make_error_code(
                std::errc::no_such_file_or_directory
            )
        ),
        !std::filesystem::exists(_path)
    );

    auto buffer         = std::make_shared<core::memory::buffer_object>(true);
    const auto buffSize = std::filesystem::file_size(_path);
    buffer->set_istream_factory(
        std::make_shared<core::memory::stream::in::raw>(_path),
        static_cast<core::memory::buffer_object::size_t>(buffSize),
        _path,
        core::memory::RAW
    );
    m_dicomContainer[_instanceIndex] = buffer;
}

//------------------------------------------------------------------------------

void dicom_series::addBinary(std::size_t _instanceIndex, const core::memory::buffer_object::sptr& _buffer)
{
    m_dicomContainer[_instanceIndex] = _buffer;
}

//------------------------------------------------------------------------------

bool dicom_series::isInstanceAvailable(std::size_t _instanceIndex) const
{
    const auto& dicomContainerIter = m_dicomContainer.find(_instanceIndex);
    return dicomContainerIter != m_dicomContainer.end();
}

//------------------------------------------------------------------------------

void dicom_series::addsop_classUID(const std::string& _sopClassUID)
{
    m_sop_classUIDs.insert(_sopClassUID);
}

//------------------------------------------------------------------------------

void dicom_series::addComputedTagValue(const std::string& _tagName, const std::string& _value)
{
    m_computedTagValues[_tagName] = _value;
}

//------------------------------------------------------------------------------

bool dicom_series::hasComputedValues(const std::string& _tagName) const
{
    return m_computedTagValues.find(_tagName) != m_computedTagValues.end();
}

//------------------------------------------------------------------------------

bool dicom_series::operator==(const dicom_series& other) const noexcept
{
    if(m_numberOfInstances != other.m_numberOfInstances
       || m_sop_classUIDs != other.m_sop_classUIDs
       || m_computedTagValues != other.m_computedTagValues
       || m_firstInstanceNumber != other.m_firstInstanceNumber
       || !core::tools::is_equal(m_dicomContainer, other.m_dicomContainer))
    {
        return false;
    }

    // Super class last
    return base_class::operator==(other);
}

//------------------------------------------------------------------------------

bool dicom_series::operator!=(const dicom_series& other) const noexcept
{
    return !(*this == other);
}

} // namespace sight::data
