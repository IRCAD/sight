/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __IODICOMEXT_GDCM_SDICOMSERIESWRITER_HPP__
#define __IODICOMEXT_GDCM_SDICOMSERIESWRITER_HPP__

#include <string>
#include <boost/filesystem/path.hpp>

#include <io/IWriter.hpp>

#include "ioDicomExt/config.hpp"

namespace fwDicomData
{
    class DicomSeries;
}


namespace ioDicomExt
{

namespace gdcm
{

/**
 * @brief Services to write an DicomSeries in DICOM format.
 * @class SDicomSeriesWriter
 * @date  2013.
 */
class IODICOMEXT_CLASS_API SDicomSeriesWriter : public ::io::IWriter
{

public :
    fwCoreServiceClassDefinitionsMacro ( (SDicomSeriesWriter)( ::io::IWriter) ) ;

    /**
     * @brief   constructor
     */
    IODICOMEXT_API SDicomSeriesWriter() throw();

    /**
     * @brief   destructor
     */
    IODICOMEXT_API virtual ~SDicomSeriesWriter() throw();

    /// Propose select a directory where to save the DICOM files.
    IODICOMEXT_API virtual void configureWithIHM();


protected:

    /// Does nothing
    IODICOMEXT_API virtual void starting() throw(::fwTools::Failed);

    /// Does nothing
    IODICOMEXT_API virtual void stopping() throw(::fwTools::Failed);

    /// Write the DicomSeries in DICOM format.
    IODICOMEXT_API void updating() throw(::fwTools::Failed);

    /// Return path type managed by the service, here FOLDER
    IODICOMEXT_API ::io::IOPathType getIOPathType() const;

private :
    /// Save the selected Dicom series
    void saveDicomSeries( const ::boost::filesystem::path folder, SPTR(::fwDicomData::DicomSeries) series ) const;
};

} // namespace gdcm
} // namespace ioDicomExt

#endif //__IODICOMEXT_GDCM_SDICOMSERIESWRITER_HPP__
