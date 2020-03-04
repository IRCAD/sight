/************************************************************************
 *
 * Copyright (C) 2009-2020 IRCAD France
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

#include "fwMedData/config.hpp"
#include "fwMedData/Series.hpp"
#include "fwMedData/types.hpp"

#include <fwData/factory/new.hpp>

#include <fwMemory/BufferObject.hpp>

#include <filesystem>

fwCampAutoDeclareDataMacro((fwMedData)(DicomSeries), FWMEDDATA_API)

namespace fwMedData
{

/**
 * @brief Holds DICOM series.
 */
class FWMEDDATA_CLASS_API DicomSeries : public ::fwMedData::Series
{

public:

    fwCoreClassMacro(DicomSeries, ::fwData::Object, ::fwData::factory::New< DicomSeries >)

    fwCampMakeFriendDataMacro((fwMedData)(DicomSeries))

    typedef std::map < std::size_t, ::fwMemory::BufferObject::sptr > DicomContainerType;

    typedef std::set < std::string > SOPClassUIDContainerType;

    typedef std::map < std::string, std::string > ComputedTagValueContainerType;

    /**
     * @brief Creates the DICOM series.
     * @param _key private construction key.
     */
    FWMEDDATA_API DicomSeries(::fwData::Object::Key _key);

    /// Destroys the DICOM series.
    FWMEDDATA_API virtual ~DicomSeries() override;

    /**
     * @brief Defines shallow copy.
     * @param _source the source object to copy into this one.
     */
    FWMEDDATA_API void shallowCopy( const ::fwData::Object::csptr& _source ) override;

    /**
     * @brief Defines deep copy.
     * @param _source the source object to copy into this one.
     * @param _cache
     */
    FWMEDDATA_API void cachedDeepCopy( const ::fwData::Object::csptr& _source, DeepCopyCacheType& cache ) override;

    /**
     * @brief Adds a DICOM path.
     * @param _instanceIndex index of the instance.
     * @param _path the path.
     */
    FWMEDDATA_API void addDicomPath(std::size_t instanceIndex, const std::filesystem::path& _path);

    /**
     * @brief Adds a binary buffer.
     * @param _instanceIndex index of the instance.
     * @param _buffer the buffer.
     */
    FWMEDDATA_API void addBinary(std::size_t _instanceIndex, const ::fwMemory::BufferObject::sptr& _buffer);

    /**
     * @brief Gets if the instance is available on the local computer
     * @param _instanceIndex
     * @return True if the instance is available on the local computer
     */
    FWMEDDATA_API bool isInstanceAvailable(std::size_t _instanceIndex) const;

    /**
     * @brief Adds a SOPClassUID that is used by this series.
     * @param _sopClassUID SOPClassUID that must be added.
     */
    FWMEDDATA_API void addSOPClassUID(const std::string& _sopClassUID);

    /**
     * @brief Adds a computed value to the specified tag.
     * @param _tagName Name of the tag.
     * @param _value Computed value.
     */
    FWMEDDATA_API void addComputedTagValue(const std::string& _tagName, const std::string& _value);

    /**
     * @brief Gets if there is a computed value for the specified tag.
     * @param _tagName Name of the tag.
     * @return True if there is a computed value for the tag.
     */
    FWMEDDATA_API bool hasComputedValues(const std::string& _tagName) const;

    /**
     * @brief Gets the number of instances in the series.
     * @return The number of instances in the series.
     */
    size_t  getNumberOfInstances() const
    {
        return m_numberOfInstances;
    }

    /**
     * @brief Sets the number of instances in the series.
     * @param _val the number of instances in the series.
     */
    void setNumberOfInstances(std::size_t _val)
    {
        m_numberOfInstances = _val;
    }

    /**
     * @brief Gets the DICOM container.
     * @return The DICOM container.
     */
    const DicomContainerType& getDicomContainer () const
    {
        return m_dicomContainer;
    }

    /**
     * @brief Sets the DICOM container.
     * @param _dicomContainer the DICOM container.
     */
    void setDicomContainer(const DicomContainerType& _dicomContainer)
    {
        m_dicomContainer = _dicomContainer;
    }

    /// Clears the DICOM container.
    void clearDicomContainer()
    {
        m_dicomContainer.clear();
    }

    /**
     * @brief Gets the SOP Class UID.
     * @return The SOP Class UID.
     */
    const SOPClassUIDContainerType& getSOPClassUIDs() const
    {
        return m_SOPClassUIDs;
    }

    /**
     * @brief Sets the SOP Class UID.
     * @param _val the SOP Class UID.
     */
    void setSOPClassUIDs(const SOPClassUIDContainerType& _val)
    {
        m_SOPClassUIDs = _val;
    }

    /**
     * @brief Gets the computed tag values.
     * @return The computed tag values.
     */
    const ComputedTagValueContainerType& getComputedTagValues() const
    {
        return m_computedTagValues;
    }

    /**
     * @brief Sets the computed tag values.
     * @param _val the computed tag values.
     */
    void setComputedTagValues(const ComputedTagValueContainerType& _val)
    {
        m_computedTagValues = _val;
    }

    /**
     * @brief Gets the first instance number (0 or 1) - Used for PACS preview.
     * @return The first instance number.
     */
    std::size_t getFirstInstanceNumber() const
    {
        return m_firstInstanceNumber;
    }

    /**
     * @brief Sets the first instance number (0 or 1) - Used for PACS preview.
     * @param _firstInstanceNumber the first instance number.
     */
    void setFirstInstanceNumber(std::size_t _firstInstanceNumber)
    {
        m_firstInstanceNumber = _firstInstanceNumber;
    }

protected:

    /// Defines the number of instances in the series.
    size_t m_numberOfInstances;

    /// Stores DICOM.
    DicomContainerType m_dicomContainer;

    /// Stores SOP Class UIDs.
    SOPClassUIDContainerType m_SOPClassUIDs;

    /// Stores computed tag values.
    ComputedTagValueContainerType m_computedTagValues;

    /// Defines the first instance number (0 or 1) - Used for PACS preview.
    std::size_t m_firstInstanceNumber;
};

} // Namespace fwMedData.
