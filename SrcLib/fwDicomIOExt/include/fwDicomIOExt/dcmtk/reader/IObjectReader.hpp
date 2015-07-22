/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWDICOMIOEXT_DCMTK_READER_IOBJECTREADER_HPP__
#define __FWDICOMIOEXT_DCMTK_READER_IOBJECTREADER_HPP__

#include <fwDicomData/DicomSeries.hpp>
#include "fwDicomIOExt/config.hpp"

namespace fwDicomIOExt
{
namespace dcmtk
{
namespace reader
{

/**
 * @class IObjectReader
 * @brief Base class for Dicom instance reader.
 * @date  2013.
 */
class FWDICOMIOEXT_CLASS_API IObjectReader
{
public:
    /// Constructor
    FWDICOMIOEXT_API IObjectReader();

    /// Destructor
    FWDICOMIOEXT_API virtual ~IObjectReader();

    /**
     * @brief Read the series from the group of instances.
     * @return Returns the created Series.
     */
    FWDICOMIOEXT_API virtual ::fwMedData::Series::sptr read(::fwDicomData::DicomSeries::sptr series) = 0;

};

} //dcmtk
} //reader
} //fwDicomIOExt


#endif /* __FWDICOMIOEXT_DCMTK_READER_IOBJECTREADER_HPP__ */
