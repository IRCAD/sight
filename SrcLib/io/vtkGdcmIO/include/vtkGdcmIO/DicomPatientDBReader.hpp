/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _VTKGDCMIO_DICOMPATIENTDBREADER_HPP_
#define _VTKGDCMIO_DICOMPATIENTDBREADER_HPP_


#include <fwTools/ProgressAdviser.hpp>
#include <fwDataIO/reader/GenericObjectReader.hpp>
#include <fwData/location/Folder.hpp>
#include <fwData/location/MultiFiles.hpp>
#include <fwData/patient.hpp>
#include <fwData/Study.hpp>

#include "vtkGdcmIO/config.hpp"


namespace vtkGdcmIO
{

class DicomPatientDBReader : public ::fwDataIO::reader::GenericObjectReader< ::fwData::PatientDB >,
                             public ::fwData::location::enableFolder< ::fwDataIO::reader::IObjectReader > ,
                             public ::fwData::location::enableMultiFiles< ::fwDataIO::reader::IObjectReader > ,
                             public ::fwTools::ProgressAdviser
{

public :

    fwCoreClassDefinitionsWithFactoryMacro((DicomPatientDBReader)( ::fwDataIO::reader::GenericObjectReader< ::fwData::PatientDB >),
                                           (()),
                                           new DicomPatientDBReader
                                          );
    fwCoreAllowSharedFromThis();

    VTKGDCMIO_API void read();

protected:

    VTKGDCMIO_API DicomPatientDBReader();

    VTKGDCMIO_API ~DicomPatientDBReader();

private :

    ::boost::shared_ptr< ::fwData::PatientDB > createPatientDB( const ::boost::filesystem::path dicomDir );

    void addPatients( ::boost::shared_ptr< ::fwData::PatientDB > patientDB, std::vector< std::string > filenames);
    void extractIdentity(const std::string patientName, std::string& name, std::string& firstname);


};

} // namespace vtkGdcmIO

#endif /*_VTKGDCMIO_DICOMPATIENTDBREADER_HPP_*/
