/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2011.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWITKIO_INRPATIENTDBREADER_HPP_
#define _FWITKIO_INRPATIENTDBREADER_HPP_

#include <vector>
#include <boost/filesystem/path.hpp>

#include <fwData/location/SingleFile.hpp>
#include <fwData/location/MultiFiles.hpp>
#include <fwData/location/Folder.hpp>

#include <fwDataIO/reader/GenericObjectReader.hpp>

#include <fwTools/ProgressAdviser.hpp>
#include "itkIO/config.hpp"

namespace itkIO
{

class InrPatientDBReader :  public ::fwDataIO::reader::GenericObjectReader< ::fwData::PatientDB >,
                            public ::fwData::location::enableSingleFile< ::fwDataIO::reader::IObjectReader >,
                            public ::fwData::location::enableMultiFiles< ::fwDataIO::reader::IObjectReader >,
                            public ::fwData::location::enableFolder< ::fwDataIO::reader::IObjectReader >,
                            public ::fwTools::ProgressAdviser

{

public :

    fwCoreClassDefinitionsWithFactoryMacro((InrPatientDBReader)( ::fwDataIO::reader::GenericObjectReader< ::fwData::PatientDB >),
                                           (()),
                                           new InrPatientDBReader
                                          );
    fwCoreAllowSharedFromThis();

    FWITKIO_API void read();

protected:

    FWITKIO_API InrPatientDBReader();

    FWITKIO_API ~InrPatientDBReader();

private :

    ::fwData::Patient::sptr createPatient( const ::boost::filesystem::path inrFilePath,
                                                 int commonPathSize, float startPercent,
                                                 int nbFiles );

    void searchRecursivelyInrFile (
            const ::boost::filesystem::path & path,
            ::std::vector< ::boost::filesystem::path > & inrFiles,
            bool recursiveSearch );

    std::string findCommonPath( ::std::vector< ::boost::filesystem::path > & inrFilePathes );

};

} // namespace itkIO

#endif /*_FWITKIO_INRPATIENTDBREADER_HPP_*/
