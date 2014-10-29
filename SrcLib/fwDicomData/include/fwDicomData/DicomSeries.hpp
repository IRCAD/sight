/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWDICOMDATA_DICOMSERIES_HPP__
#define __FWDICOMDATA_DICOMSERIES_HPP__

#include <boost/filesystem/path.hpp>

#include <fwData/Object.hpp>
#include <fwData/factory/new.hpp>
#include <fwData/macros.hpp>

#include "fwMedData/types.hpp"
#include "fwMedData/Series.hpp"
#include "fwDicomData/config.hpp"

fwCampAutoDeclareDataMacro((fwDicomData)(DicomSeries), FWDICOMDATA_API);

namespace fwData
{
    class Array;
}

namespace fwDicomData
{


/**
 * @class DicomSeries
 * @brief Holds DICOM data
 * @date  2013.
 */
class FWDICOMDATA_CLASS_API DicomSeries : public ::fwMedData::Series
{

public:
    fwCoreClassDefinitionsWithFactoryMacro( (DicomSeries)(::fwData::Object), (()), ::fwData::factory::New< DicomSeries >) ;

    fwCampMakeFriendDataMacro((fwDicomData)(DicomSeries));

    typedef std::map < unsigned int, ::boost::filesystem::path > DicomPathContainerType;

    typedef std::map < std::string, SPTR(::fwData::Array) > DicomBinaryContainerType;

    typedef std::set < std::string > SOPClassUIDContainerType;

    typedef std::map < std::string, std::string > ComputedTagValueContainerType;

    /**
     * @brief Constructor
     * @param key Private construction key
     */
    FWDICOMDATA_API DicomSeries(::fwData::Object::Key key);

    /// Destructor
    FWDICOMDATA_API virtual ~DicomSeries();

    /// Defines shallow copy
    FWDICOMDATA_API void shallowCopy( const ::fwData::Object::csptr &_source );

    /// Defines deep copy
    FWDICOMDATA_API void cachedDeepCopy( const ::fwData::Object::csptr &_source, DeepCopyCacheType &cache );

    /// Add dicom path
    FWDICOMDATA_API void addDicomPath(unsigned int instanceIndex, ::boost::filesystem::path path);

    /// Add binary array
    FWDICOMDATA_API void addBinary(const std::string &filename, SPTR(::fwData::Array) binary);

    /**
     * @brief Return true if the instance is available on the local computer
     * @todo This method could be constifiable but function std::map::at doesn't work on windows
     */
    FWDICOMDATA_API bool isInstanceAvailable(unsigned int instanceIndex);

    /**
     * @brief Add a SOPClassUID that is used by this series.
     * @param[in] sopClassUID SOPClassUID that must be added.
     */
    FWDICOMDATA_API void addSOPClassUID(const std::string& sopClassUID);

    /**
     * @brief Add a computed value to the specified tag.
     * @param[in] tagName Name of the tag
     * @param[in] value Computed value
     */
    FWDICOMDATA_API void addComputedTagValue(const std::string& tagName, const std::string& value);

    /**
     * @brief Return true if there is a computed value for the specified tag
     * @param[in] tagName Name of the tag
     * @return True if there is a computed value for the tag
     */
    FWDICOMDATA_API bool hasComputedValues(const std::string& tagName) const;

    /**
     * @brief Availability
     */
    typedef enum
    {
        NONE = 1,       /*! The DICOM files are not available on the local machine but may be available on the pacs. */
        PATHS  = 2,     /*! The paths to the DICOM files are saved in this DicomSeries. */
        BINARIES = 3    /*! The binaries of the DICOM files are saved in this DicomSeries. */
    } DICOM_AVAILABILITY;

    /**
     * @name Getters / Setters
     * @{ */

    /**
     * @brief Dicom files availability
     * @{ */
    fwDataGetSetMacro(DicomAvailability, DICOM_AVAILABILITY);
    /**  @} */

    /**
     * @brief Local paths of Dicom files
     * @{ */
    fwDataGetSetCRefMacro(LocalDicomPaths, DicomPathContainerType);
    /**  @} */

    /**
     * @brief Number of instances in the series (0020,1009)
     * @{ */
    fwDataGetSetMacro(NumberOfInstances, unsigned int);
    /**  @} */

    /**
     * @brief Dicom binaries
     * @{ */
    fwDataGetSetCRefMacro(DicomBinaries, DicomBinaryContainerType);
    /**  @} */

    /**
     * @brief SOP Class UID
     * @{ */
    fwDataGetSetCRefMacro(SOPClassUIDs, SOPClassUIDContainerType);
    /**  @} */

    /**
     * @brief Computed Tag Values
     * @{ */
    fwDataGetSetCRefMacro(ComputedTagValues, ComputedTagValueContainerType);
    /**  @} */

    /**  @} */


protected:

    /// Dicom Availability
    DICOM_AVAILABILITY m_attrDicomAvailability;

    /// Number of instances in the series (0020,1209)
    unsigned int m_attrNumberOfInstances;

    /// Local paths of Dicom files
    DicomPathContainerType m_attrLocalDicomPaths;

    /// Dicom binaries
    DicomBinaryContainerType m_attrDicomBinaries;

    /// SOP Class UIDs
    SOPClassUIDContainerType m_attrSOPClassUIDs;

    /// Computed tag values
    ComputedTagValueContainerType m_attrComputedTagValues;
};

}   //end namespace fwDicomData

#endif // __FWDICOMDATA_DICOMSERIES_HPP__


