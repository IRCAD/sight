/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __IODCMTK_SDICOMSERIESDBREADER_HPP__
#define __IODCMTK_SDICOMSERIESDBREADER_HPP__

#include "ioDcmtk/config.hpp"

#include <io/IReader.hpp>

#include <boost/filesystem/path.hpp>

#include <string>

namespace fwMedData
{
class SeriesDB;
class Patient;
}

namespace ioDcmtk
{

/**
 * @brief Read DICOM Series from a DICOM with DCMTK
 */
class IODCMTK_CLASS_API SDicomSeriesDBReader : public ::io::IReader
{

public:
    fwCoreServiceClassDefinitionsMacro( (SDicomSeriesDBReader)( ::io::IReader) );
    /**
     * @brief   constructor
     *
     */
    IODCMTK_API SDicomSeriesDBReader() throw();

    /**
     * @brief   destructor
     */
    IODCMTK_API virtual ~SDicomSeriesDBReader() throw();

protected:

    /// Override
    IODCMTK_API virtual void starting() throw(::fwTools::Failed);

    /// Override
    IODCMTK_API virtual void stopping() throw(::fwTools::Failed);

    /// Override
    IODCMTK_API virtual void configuring() throw(::fwTools::Failed);

    /// Override
    IODCMTK_API void updating() throw(::fwTools::Failed);

    /// Override
    IODCMTK_API void info(std::ostream& _sstream );

    /// Override
    IODCMTK_API virtual std::string getSelectorDialogTitle();

    /// Override
    IODCMTK_API virtual void configureWithIHM();

    /// Return path type managed by the service, here FOLDER
    IODCMTK_API ::io::IOPathType getIOPathType() const;

private:

    void notificationOfDBUpdate();

    SPTR(::fwMedData::SeriesDB) createSeriesDB(const ::boost::filesystem::path& dicomDir);

};

} // namespace ioDcmtk

#endif //__IODCMTK_SDICOMSERIESDBREADER_HPP__
