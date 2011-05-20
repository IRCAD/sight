/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2011.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWITKIO_DICOMPATIENTDBREADER_HPP_
#define _FWITKIO_DICOMPATIENTDBREADER_HPP_

#include <fwTools/ProgressAdviser.hpp>
#include <fwDataIO/reader/GenericObjectReader.hpp>
#include <fwData/location/Folder.hpp>

#include <itkGDCMImageIO.h>

#include "itkIO/config.hpp"


namespace itkIO
{

class DicomPatientDBReader : public ::fwDataIO::reader::GenericObjectReader< ::fwData::PatientDB >,
                             public ::fwData::location::enableFolder< ::fwDataIO::reader::IObjectReader > ,
                             public ::fwTools::ProgressAdviser
{

public :

    fwCoreClassDefinitionsWithFactoryMacro((DicomPatientDBReader)( ::fwDataIO::reader::GenericObjectReader< ::fwData::PatientDB >),
                                               (()),
                                               new DicomPatientDBReader
                                              );
    fwCoreAllowSharedFromThis();

    FWITKIO_API void read();

protected:

    FWITKIO_API DicomPatientDBReader();

    FWITKIO_API ~DicomPatientDBReader();

private :

    std::string getDicomValue( ::itk::GDCMImageIO::Pointer dicomIO , const std::string entryId );

    ::fwData::PatientDB::sptr createPatientDB( const ::boost::filesystem::path dicomDir );

    void addPatients( ::fwData::PatientDB::sptr patientDB, const ::boost::filesystem::path dirPath );

    void searchRecursivelyDirectories( const ::boost::filesystem::path & path, ::std::vector< ::boost::filesystem::path > & dirs );

};

} // namespace itkIO

#endif /*_FWITKIO_DICOMPATIENTDBREADER_HPP_*/
