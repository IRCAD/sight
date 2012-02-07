/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2012-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _GDCMIO_DICOMGLOBALWRITERMANAGER_HPP_
#define _GDCMIO_DICOMGLOBALWRITERMANAGER_HPP_

#include <fwTools/ProgressAdviser.hpp>

#include <fwDataIO/writer/GenericObjectWriter.hpp>

#include <fwData/Patient.hpp>
#include <fwData/location/Folder.hpp>

#include "gdcmIO/config.hpp"

namespace gdcmIO
{

namespace writer
{

/**
 * @brief   This class handles DICOM files writing.
 *
 * It defines needs of data storage and delegates writing to appropriate tools.
 * It manages all related data of one patient.
 *
 * @class   DicomGlobalWriterManager
 * @author  IRCAD (Research and Development Team).
 * @date    2011.
 */
class GDCMIO_CLASS_API DicomGlobalWriterManager : public ::fwDataIO::writer::GenericObjectWriter< ::fwData::Patient >,
                                                  public ::fwData::location::enableFolder< ::fwDataIO::writer::IObjectWriter >,
                                                  public ::fwTools::ProgressAdviser
{

public :

    fwCoreClassDefinitionsWithFactoryMacro( (DicomGlobalWriterManager)( ::fwDataIO::writer::GenericObjectWriter< ::fwData::Patient >),
                                            (()),
                                            new  DicomGlobalWriterManager
                                           );

    GDCMIO_API DicomGlobalWriterManager();

    GDCMIO_API ~DicomGlobalWriterManager();

    /**
     * @brief   Load and start appropriate writing tools.
     * In addition, it creates an adapted folder tree where DICOM files will be stored.
     * It manages writing of the following general DICOM modules : patient, study and series
     *
     * @note    Currently, it handles image, SR and RT document writing.
     */
    GDCMIO_API void write() throw(::fwTools::Failed);

    /**
     * Overriding
     * @brief   Do nothing
     */
    GDCMIO_API std::string extension();

    /**
     * @brief   Set the patient ID.
     * It will be used to set the name of patient folder.
     */
    GDCMIO_API void setPatientID(const unsigned int a_id);

private :

    /**
     * @brief   Check if there is an image in acquisition.
     *
     * @param   a_series    Acquisition to check.
     */
    bool hasImage( ::fwData::Acquisition::csptr a_series ) const;

    /**
     * @brief   Check if there is comment on acquisition.
     *
     * @param   a_series    Acquisition to check.
     */
    bool hasDocumentSR( ::fwData::Acquisition::csptr a_series ) const;

    /**
     * @brief   Check if there is 3D reconstruction in acquisition.
     *
     * @param   a_series    Acquisition to check.
     */
    bool hasDocumentRT( ::fwData::Acquisition::csptr a_series ) const;


    unsigned int m_patientID;   ///< Patient ID (eg: 1, 4, ...)

};

}   // namespace writer

}   // namespace gdcmIO

#endif // _GDCMIO_DICOMGLOBALWRITERMANAGER_HPP_
