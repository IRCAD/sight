/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWDICOMIOEXT_DCMTK_READER_CTIMAGESTORAGEREADER_HPP__
#define __FWDICOMIOEXT_DCMTK_READER_CTIMAGESTORAGEREADER_HPP__

#include <dcmtk/config/osconfig.h>
#include <dcmtk/dcmnet/diutil.h>

#include <fwCore/BaseObject.hpp>
#include <fwMedData/Series.hpp>

#include "fwDicomIOExt/config.hpp"
#include "fwDicomIOExt/dcmtk/reader/IObjectReader.hpp"

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
class FWDICOMIOEXT_CLASS_API CTImageStorageReader : public ::fwDicomIOExt::dcmtk::reader::IObjectReader
{
public:
    fwCoreClassDefinitionsWithFactoryMacro( (CTImageStorageReader)(::fwDicomIOExt::dcmtk::reader::IObjectReader),
            ( ((::fwMedData::Series::sptr)(::fwMedData::Series::sptr())) ), new CTImageStorageReader );
    fwCoreAllowSharedFromThis();

    typedef std::vector< std::string > InstanceFileContainerType;

    /// Constructor
    FWDICOMIOEXT_API CTImageStorageReader(SPTR(::fwMedData::Series) series);

    /// Destructor
    FWDICOMIOEXT_API virtual ~CTImageStorageReader();

    /// Override
    FWDICOMIOEXT_API virtual void addInstance(DcmDataset* dataset, const std::string& filename);

    /// Override
    FWDICOMIOEXT_API virtual SPTR(::fwMedData::Series) create();

protected:

    virtual void sortInstances();

    /// Instance datasets
    InstanceFileContainerType m_instanceFiles;

};

} //dcmtk
} //reader
} //fwDicomIOExt


#endif /* __FWDICOMIOEXT_DCMTK_READER_CTIMAGESTORAGEREADER_HPP__ */
