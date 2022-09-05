/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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

#pragma once

#include "data/config.hpp"
#include "data/factory/new.hpp"
#include "data/Series.hpp"
#include "data/types.hpp"

#include <core/memory/BufferObject.hpp>

#include <data/factory/new.hpp>

#include <filesystem>

namespace sight::data
{

/**
 * @brief Holds DICOM series.
 */
class DATA_CLASS_API DicomSeries : public Series
{
public:

    SIGHT_DECLARE_CLASS(DicomSeries, Object, factory::New<DicomSeries>);

    typedef std::map<std::size_t, core::memory::BufferObject::sptr> DicomContainerType;

    typedef std::set<std::string> SOPClassUIDContainerType;

    typedef std::map<std::string, std::string> ComputedTagValueContainerType;

    /**
     * @brief Creates the DICOM series.
     * @param _key private construction key.
     */
    DATA_API DicomSeries(Object::Key _key);

    /// Destroys the DICOM series.
    DATA_API ~DicomSeries() override;

    /**
     * @brief Defines shallow copy.
     * @param _source the source object to copy into this one.
     */
    DATA_API void shallowCopy(const Object::csptr& _source) override;

    /**
     * @brief Defines deep copy.
     * @param _source the source object to copy into this one.
     * @param _cache contains all copied objects to avoid duplication.
     */
    DATA_API void cachedDeepCopy(const Object::csptr& _source, DeepCopyCacheType& _cache) override;

    /**
     * @brief Adds a DICOM path.
     * @param _instanceIndex index of the instance.
     * @param _path the path.
     */
    DATA_API void addDicomPath(std::size_t instanceIndex, const std::filesystem::path& _path);

    /**
     * @brief Adds a binary buffer.
     * @param _instanceIndex index of the instance.
     * @param _buffer the buffer.
     */
    DATA_API void addBinary(std::size_t _instanceIndex, const core::memory::BufferObject::sptr& _buffer);

    /**
     * @brief Gets if the instance is available on the local computer
     * @param _instanceIndex the instance to check.
     * @return True if the instance is available on the local computer
     */
    DATA_API bool isInstanceAvailable(std::size_t _instanceIndex) const;

    /**
     * @brief Adds a SOPClassUID that is used by this series.
     * @param _sopClassUID SOPClassUID that must be added.
     */
    DATA_API void addSOPClassUID(const std::string& _sopClassUID);

    /**
     * @brief Adds a computed value to the specified tag.
     * @param _tagName Name of the tag.
     * @param _value Computed value.
     */
    DATA_API void addComputedTagValue(const std::string& _tagName, const std::string& _value);

    /**
     * @brief Gets if there is a computed value for the specified tag.
     * @param _tagName Name of the tag.
     * @return True if there is a computed value for the tag.
     */
    DATA_API bool hasComputedValues(const std::string& _tagName) const;

    /**
     * @brief Gets the number of instances in the series.
     * @return The number of instances in the series.
     */
    std::size_t numInstances() const
    {
        return m_numberOfInstances;
    }

    ///f Sets the number of instances in the series.
    void setNumberOfInstances(std::size_t _val)
    {
        m_numberOfInstances = _val;
    }

    /// Gets the DICOM container.
    const DicomContainerType& getDicomContainer() const
    {
        return m_dicomContainer;
    }

    /// Sets the DICOM container.
    void setDicomContainer(const DicomContainerType& _dicomContainer)
    {
        m_dicomContainer = _dicomContainer;
    }

    /// Clears the DICOM container.
    void clearDicomContainer()
    {
        m_dicomContainer.clear();
    }

    /// Gets the SOP Class UID.
    const SOPClassUIDContainerType& getSOPClassUIDs() const
    {
        return m_SOPClassUIDs;
    }

    //------------------------------------------------------------------------------

    SOPClassUIDContainerType& getSOPClassUIDs()
    {
        return m_SOPClassUIDs;
    }

    /// Sets the SOP Class UID.
    void setSOPClassUIDs(const SOPClassUIDContainerType& _val)
    {
        m_SOPClassUIDs = _val;
    }

    /// Gets the computed tag values.
    const ComputedTagValueContainerType& getComputedTagValues() const
    {
        return m_computedTagValues;
    }

    /// Sets the computed tag values.
    void setComputedTagValues(const ComputedTagValueContainerType& _val)
    {
        m_computedTagValues = _val;
    }

    /// Gets the first instance number (0 or 1) - Used for PACS preview.
    std::size_t getFirstInstanceNumber() const
    {
        return m_firstInstanceNumber;
    }

    /// Sets the first instance number (0 or 1) - Used for PACS preview.
    void setFirstInstanceNumber(std::size_t _firstInstanceNumber)
    {
        m_firstInstanceNumber = _firstInstanceNumber;
    }

    /// Equality comparison operators
    /// @{
    DATA_API bool operator==(const DicomSeries& other) const noexcept;
    DATA_API bool operator!=(const DicomSeries& other) const noexcept;
    /// @}

protected:

    /// Defines the number of instances in the series.
    std::size_t m_numberOfInstances {0};

    /// Stores DICOM.
    DicomContainerType m_dicomContainer;

    /// Stores SOP Class UIDs.
    SOPClassUIDContainerType m_SOPClassUIDs;

    /// Stores computed tag values.
    ComputedTagValueContainerType m_computedTagValues;

    /// Defines the first instance number (0 or 1) - Used for PACS preview.
    std::size_t m_firstInstanceNumber {0};
};

} // Namespace sight::data
