/************************************************************************
 *
 * Copyright (C) 2009-2018 IRCAD France
 * Copyright (C) 2012-2018 IHU Strasbourg
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

#include <boost/filesystem/path.hpp>

fwCampAutoDeclareDataMacro((fwMedData)(DicomSeries), FWMEDDATA_API);

namespace fwMedData
{

/**
 * @brief Holds DICOM data
 */
class FWMEDDATA_CLASS_API DicomSeries : public ::fwMedData::Series
{

public:

    fwCoreClassDefinitionsWithFactoryMacro( (DicomSeries)(::fwData::Object), (()),
                                            ::fwData::factory::New< DicomSeries >);

    fwCampMakeFriendDataMacro((fwMedData)(DicomSeries));

    typedef std::map < std::size_t, ::fwMemory::BufferObject::sptr > DicomContainerType;

    typedef std::set < std::string > SOPClassUIDContainerType;

    typedef std::map < std::string, std::string > ComputedTagValueContainerType;

    /**
     * @brief Constructor
     * @param key Private construction key
     */
    FWMEDDATA_API DicomSeries(::fwData::Object::Key key);

    /// Destructor
    FWMEDDATA_API virtual ~DicomSeries() override;

    /// Defines shallow copy
    FWMEDDATA_API void shallowCopy( const ::fwData::Object::csptr& _source ) override;

    /// Defines deep copy
    FWMEDDATA_API void cachedDeepCopy( const ::fwData::Object::csptr& _source, DeepCopyCacheType& cache ) override;

    /// Add dicom path
    FWMEDDATA_API void addDicomPath(std::size_t instanceIndex, const ::boost::filesystem::path& path);

    /// Add binary buffer
    FWMEDDATA_API void addBinary(std::size_t instanceIndex, const ::fwMemory::BufferObject::sptr& buffer);

    /**
     * @brief Return true if the instance is available on the local computer
     */
    FWMEDDATA_API bool isInstanceAvailable(std::size_t instanceIndex) const;

    /**
     * @brief Add a SOPClassUID that is used by this series.
     * @param[in] sopClassUID SOPClassUID that must be added.
     */
    FWMEDDATA_API void addSOPClassUID(const std::string& sopClassUID);

    /**
     * @brief Add a computed value to the specified tag.
     * @param[in] tagName Name of the tag
     * @param[in] value Computed value
     */
    FWMEDDATA_API void addComputedTagValue(const std::string& tagName, const std::string& value);

    /**
     * @brief Return true if there is a computed value for the specified tag
     * @param[in] tagName Name of the tag
     * @return True if there is a computed value for the tag
     */
    FWMEDDATA_API bool hasComputedValues(const std::string& tagName) const;

    /**
     * @brief Number of instances in the series (0020,1009)
     * @{ */
    size_t  getNumberOfInstances () const
    {
        return m_numberOfInstances;
    }
    //------------------------------------------------------------------------------

    void setNumberOfInstances (std::size_t val)
    {
        m_numberOfInstances = val;
    }
    /**  @} */

    /**
     * @brief Dicom container
     * @{ */
    const DicomContainerType& getDicomContainer () const
    {
        return m_dicomContainer;
    }
    //------------------------------------------------------------------------------

    void setDicomContainer (const DicomContainerType& dicomContainer)
    {
        m_dicomContainer = dicomContainer;
    }

    //------------------------------------------------------------------------------

    void clearDicomContainer()
    {
        m_dicomContainer.clear();
    }
    /**  @} */

    /**
     * @brief SOP Class UID
     * @{ */
    const SOPClassUIDContainerType& getSOPClassUIDs () const
    {
        return m_SOPClassUIDs;
    }
    //------------------------------------------------------------------------------

    void setSOPClassUIDs (const SOPClassUIDContainerType& val)
    {
        m_SOPClassUIDs = val;
    }
    /**  @} */

    /**
     * @brief Computed Tag Values
     * @{ */
    const ComputedTagValueContainerType& getComputedTagValues () const
    {
        return m_computedTagValues;
    }
    //------------------------------------------------------------------------------

    void setComputedTagValues (const ComputedTagValueContainerType& val)
    {
        m_computedTagValues = val;
    }
    /**  @} */

    /**
     * @brief First Instance Number (0 or 1) - Used for PACS preview
     * @{ */
    std::size_t getFirstInstanceNumber() const
    {
        return m_firstInstanceNumber;
    }

    //------------------------------------------------------------------------------

    void setFirstInstanceNumber(std::size_t firstInstanceNumber)
    {
        m_firstInstanceNumber = firstInstanceNumber;
    }
    /**  @} */

    /**  @} */

protected:

    /// Number of instances in the series (0020,1209)
    size_t m_numberOfInstances;

    /// Dicom container
    DicomContainerType m_dicomContainer;

    /// SOP Class UIDs
    SOPClassUIDContainerType m_SOPClassUIDs;

    /// Computed tag values
    ComputedTagValueContainerType m_computedTagValues;

    /// First instance number (0 or 1) - Used for PACS preview
    std::size_t m_firstInstanceNumber;
};

}   //end namespace fwMedData
