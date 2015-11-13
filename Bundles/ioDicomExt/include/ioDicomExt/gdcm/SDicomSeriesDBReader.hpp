/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __IODICOMEXT_GDCM_SDICOMSERIESDBREADER_HPP__
#define __IODICOMEXT_GDCM_SDICOMSERIESDBREADER_HPP__

#include <string>
#include <boost/filesystem/path.hpp>

#include <io/IReader.hpp>

#include "ioDicomExt/config.hpp"

namespace fwMedData
{
class SeriesDB;
class Patient;
}


namespace ioDicomExt
{

namespace gdcm
{

/**
 * @brief   Reader used to read DicomSeries.
 * @class   SDicomSeriesDBReader
 * @date    2013.
 */
class IODICOMEXT_CLASS_API SDicomSeriesDBReader : public ::io::IReader
{

public:
    typedef std::string ExtensionType;
    typedef std::vector< ExtensionType > ExtensionsType;

    fwCoreServiceClassDefinitionsMacro ( (SDicomSeriesDBReader)( ::io::IReader) );
    /**
     * @brief   constructor
     */
    IODICOMEXT_API SDicomSeriesDBReader() throw();

    /**
     * @brief   destructor
     */
    IODICOMEXT_API virtual ~SDicomSeriesDBReader() throw();

protected:

    /// Override
    IODICOMEXT_API virtual void starting() throw(::fwTools::Failed);

    /// Override
    IODICOMEXT_API virtual void stopping() throw(::fwTools::Failed);

    /// Override
    IODICOMEXT_API void updating() throw(::fwTools::Failed);

    /// Override
    IODICOMEXT_API void info(std::ostream &_sstream );

    /// Override
    IODICOMEXT_API virtual ExtensionsType getSupportedExtensions();

    /// Override
    IODICOMEXT_API virtual std::string getSelectorDialogTitle();

    /// Override
    IODICOMEXT_API virtual void configureWithIHM();

    /// Return path type managed by the service, here FOLDER
    IODICOMEXT_API ::io::IOPathType getIOPathType() const;

private:

    void notificationOfDBUpdate();

    SPTR(::fwMedData::SeriesDB) createSeriesDB(const ::boost::filesystem::path& dicomDir) const;

};

} // namespace gdcm
} // namespace ioDicomExt

#endif //__IODICOMEXT_GDCM_SDICOMSERIESDBREADER_HPP__
