/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __GDCMIO_WRITER_DICOMFILEWRITER_HXX_
#define __GDCMIO_WRITER_DICOMFILEWRITER_HXX_

#include <gdcmWriter.h>
#include <gdcmFileMetaInformation.h>

#include <fwCore/spyLog.hpp>        // for SLM_TRACE_FUNC()

#include <fwDataIO/writer/GenericObjectWriter.hpp>

#include <fwData/location/SingleFile.hpp>

#include "gdcmIO/exception/Failed.hpp"
#include "gdcmIO/config.hpp"

namespace gdcmIO
{

namespace writer
{

/**
 * @brief   This class contains common members to write a DICOM file.
 *
 * It allows to set a gdcm::DataSet from fwData::Object.
 * The GDCM writer has to be instantiated by a sub-class. All DICOM file
 * writer in gdcmIO must inherit of this class.
 *
 * @class   DicomFile.
 */
template <class DATATYPE>
class GDCMIO_CLASS_API DicomFile :  public ::fwDataIO::writer::GenericObjectWriter< DATATYPE >,
                                          public ::fwData::location::enableSingleFile< ::fwDataIO::writer::IObjectWriter >
{

public:

//    fwCoreClassDefinitionsWithFactoryMacro((DicomFile< DATATYPE >)( ::fwDataIO::writer::GenericObjectWriter< DATATYPE >),
//                                           (()),
//                                           new  DicomFile< DATATYPE >
//                                           );

    GDCMIO_API DicomFile() :
            ::fwData::location::enableSingleFile< ::fwDataIO::writer::IObjectWriter >(this)
    {
        SLM_TRACE_FUNC();
    }

    GDCMIO_API virtual ~DicomFile()
    {
        SLM_TRACE_FUNC();
    }

    GDCMIO_API void setDataSet(::gdcm::DataSet & a_gDataSet );

protected :

    /**
     * @brief   Write one file with a gdcm::Writer.
     * It defines transfer syntax before writing.
     */
    virtual void write();

    /**
     * @brief   Define the extension of DICOM file.
     */
    std::string extension();

    //*****     Member getters/setters      *****//
    ::boost::shared_ptr< ::gdcm::Writer > getWriter();

    void setWriter( ::boost::shared_ptr< ::gdcm::Writer > a_gWriter );

    ::gdcm::DataSet & getDataSet();

private :

    ::boost::shared_ptr< ::gdcm::Writer > m_gWriter;    ///< Generic GDCM writer
};

//------------------------------------------------------------------------------

template <class DATATYPE>
std::string DicomFile< DATATYPE >::extension()
{
    return ".dcm";
}

//------------------------------------------------------------------------------

template <class DATATYPE>
::boost::shared_ptr< ::gdcm::Writer > DicomFile< DATATYPE >::getWriter()
{
    return this->m_gWriter;
};

//------------------------------------------------------------------------------

template <class DATATYPE>
void DicomFile< DATATYPE >::setWriter( ::boost::shared_ptr< ::gdcm::Writer > a_gWriter )
{
    this->m_gWriter = a_gWriter;
};

//------------------------------------------------------------------------------

template <class DATATYPE>
::gdcm::DataSet & DicomFile< DATATYPE >::getDataSet()
{
    return this->m_gWriter->GetFile().GetDataSet();
};

//------------------------------------------------------------------------------

template <class DATATYPE>
void DicomFile< DATATYPE >::setDataSet(::gdcm::DataSet & a_gDataSet )
{
    this->m_gWriter->GetFile().SetDataSet( a_gDataSet );
};

//------------------------------------------------------------------------------

template <class DATATYPE>
void DicomFile< DATATYPE >::write()
{
    SLM_ASSERT("File name not set",!this->getFile().string().empty());

    //*****     Set the file header      *****//
    ::gdcm::FileMetaInformation & gFMI = m_gWriter->GetFile().GetHeader();

    // Transfer syntax
    gFMI.SetDataSetTransferSyntax( gdcm::TransferSyntax::ExplicitVRLittleEndian );  // Type 1

    //*****     Write the file      *****//
    // Initialize the file
    std::string fileName = this->getFile().string() + this->extension();
    OSLM_TRACE("File's path : " << fileName);

    m_gWriter->SetFileName(fileName.c_str());    // NOTE: Must be called when file is ready to be written

    // Write data
    if( !m_gWriter->Write() )
    {
        std::stringstream ss;
        ss << "Write error : " << fileName;
        throw ::gdcmIO::exception::Failed(ss.str());
    }
}

}   // namespace writer

}   // namespace gdcmIO

#endif /* __GDCMIO_WRITER_DICOMFILEWRITER_HXX_ */
