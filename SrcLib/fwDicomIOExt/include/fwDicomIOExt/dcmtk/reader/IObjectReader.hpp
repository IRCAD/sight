/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWDICOMIOEXT_DCMTK_READER_IOBJECTREADER_HPP__
#define __FWDICOMIOEXT_DCMTK_READER_IOBJECTREADER_HPP__

#include <dcmtk/config/osconfig.h>
#include <dcmtk/dcmnet/diutil.h>

#include <fwCore/BaseObject.hpp>

#include "fwDicomIOExt/config.hpp"

namespace fwMedData
{
    class Series;
}

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
class FWDICOMIOEXT_CLASS_API IObjectReader : public ::fwCore::BaseObject
{
public:
    fwCoreNonInstanciableClassDefinitionsMacro( (IObjectReader) );
    fwCoreAllowSharedFromThis();

    /// Constructor
    FWDICOMIOEXT_API IObjectReader(SPTR(::fwMedData::Series) series);

    /// Destructor
    FWDICOMIOEXT_API virtual ~IObjectReader();

    /**
     * @brief Add the dataset of an instance belonging to the series.
     * @param[in] dataset Instance's dataset
     * @param[in] filename Instance's filename
     */
    FWDICOMIOEXT_API virtual void addInstance(DcmDataset* dataset, const std::string& filename)=0;

    /**
     * @brief Create the series from the datasets.
     * @return Returns the created Series.
     */
    FWDICOMIOEXT_API virtual SPTR(::fwMedData::Series) create()=0;

protected:
    /// Series that will be read by the reader
    SPTR(::fwMedData::Series) m_series;
};

} //dcmtk
} //reader
} //fwDicomIOExt


#endif /* __FWDICOMIOEXT_DCMTK_READER_IOBJECTREADER_HPP__ */
