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

#include "vtkGdcmIO/config.hpp"


namespace vtkGdcmIO
{

class DicomPatientDBReader : public ::fwDataIO::reader::GenericObjectReader< ::fwData::PatientDB >,
                             public ::fwData::location::enableFolder< ::fwDataIO::reader::IObjectReader > ,
                             public ::fwTools::ProgressAdviser

{

public :

    VTKGDCMIO_API DicomPatientDBReader();

    VTKGDCMIO_API ~DicomPatientDBReader();

    VTKGDCMIO_API void read();


private :

    ::boost::shared_ptr< ::fwData::PatientDB > createPatientDB( const ::boost::filesystem::path dicomDir );

    void addPatients( ::boost::shared_ptr< ::fwData::PatientDB > patientDB, std::vector< std::string > filenames);

};

} // namespace vtkGdcmIO

#endif /*_VTKGDCMIO_DICOMPATIENTDBREADER_HPP_*/
