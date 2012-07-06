/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2012-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _GDCMIO_DICOMFILESREADER_HPP_
#define _GDCMIO_DICOMFILESREADER_HPP_

#include <gdcmReader.h>

#include <fwData/macros.hpp>

#include "gdcmIO/reader/DicomObjectReader.hxx"

namespace gdcmIO
{

namespace reader
{

/**
 * @brief   This class contains common members to read DICOM files.
 *
 * It has a generic or specific GDCM reader (eg: Reader or ImageReader).
 * This reader has to be instantiated by a sub-class. All DICOM file reader
 * in gdcmIO must inherit of this class.
 *
 * @class   DicomFilesReader.
 * @author  IRCAD (Research and Development Team).
 * @date    2011.
 */
template <class DATATYPE>
class GDCMIO_CLASS_API DicomFilesReader : public DicomObjectReader< DATATYPE >
{

public:

    GDCMIO_API DicomFilesReader()
    {
        SLM_TRACE_FUNC();
    }

    GDCMIO_API virtual ~DicomFilesReader()
    {
        SLM_TRACE_FUNC();
    }

    /**
     * @brief   Interface which has to be overridden.
     */
    GDCMIO_API virtual void read() = 0;

    /**
     * @brief   Get the GDCM reader.
     */
    GDCMIO_API ::boost::shared_ptr< ::gdcm::Reader > getReader() const;

    /**
     * @brief   Set the GDCM reader.
     */
    GDCMIO_API void setReader(::boost::shared_ptr< ::gdcm::Reader > a_gReader);

    /**
     * @brief   Get the data set of the current file of GDCM reader.
     */
    GDCMIO_API const ::gdcm::DataSet & getDataSet() const;


    GDCMIO_API fwGettersSettersDocMacro(FileNames, filenames, std::vector< std::string >, A group of file(s) with common fields which have to be written.);

private :

    ::boost::shared_ptr< ::gdcm::Reader >   m_gReader;      ///< Pointer to the super class of GDCM reader.
                                                            ///< It could be downcast.

    std::vector< std::string >              m_filenames;    ///< List of file names to read.
};

//------------------------------------------------------------------------------

template <class DATATYPE>
::boost::shared_ptr< ::gdcm::Reader > DicomFilesReader<DATATYPE>::getReader() const
{
    return this->m_gReader;
}

//------------------------------------------------------------------------------

template <class DATATYPE>
void DicomFilesReader<DATATYPE>::setReader(::boost::shared_ptr< ::gdcm::Reader > a_gReader)
{
    this->m_gReader = a_gReader;
}

//------------------------------------------------------------------------------

template <class DATATYPE>
const ::gdcm::DataSet & DicomFilesReader<DATATYPE>::getDataSet() const
{
    return this->m_gReader->GetFile().GetDataSet();
}

}   // namespace reader

}   // namespace gdcmIO

#endif /* _GDCMIO_DICOMFILESREADER_HPP_ */
