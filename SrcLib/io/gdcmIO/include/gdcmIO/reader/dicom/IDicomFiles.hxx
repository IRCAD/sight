/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __GDCMIO_READER_DICOM_IDICOMFILES_HPP__
#define __GDCMIO_READER_DICOM_IDICOMFILES_HPP__

#include <gdcmReader.h>

#include <fwData/macros.hpp>

#include "gdcmIO/reader/dicom/IDicomObject.hxx"
#include "gdcmIO/config.hpp"

namespace gdcmIO
{
namespace reader
{
namespace dicom
{

/**
 * @brief   This class contains common members to read DICOM files.
 *
 * It has a generic or specific GDCM reader (eg: Reader or ImageReader).
 * This reader has to be instantiated by a sub-class. All DICOM file reader
 * in gdcmIO must inherit of this class.
 *
 * @class   DicomFiles.
 */
template <class DATATYPE>
class GDCMIO_CLASS_API IDicomFiles : public IDicomObject< DATATYPE >
{

public:

    IDicomFiles()
    {}

    virtual ~IDicomFiles()
    {}

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
::boost::shared_ptr< ::gdcm::Reader > IDicomFiles<DATATYPE>::getReader() const
{
    return this->m_gReader;
}

//------------------------------------------------------------------------------

template <class DATATYPE>
void IDicomFiles<DATATYPE>::setReader(::boost::shared_ptr< ::gdcm::Reader > a_gReader)
{
    this->m_gReader = a_gReader;
}

//------------------------------------------------------------------------------

template <class DATATYPE>
const ::gdcm::DataSet & IDicomFiles<DATATYPE>::getDataSet() const
{
    return this->m_gReader->GetFile().GetDataSet();
}

}  // namespace dicom
}   // namespace reader
}   // namespace gdcmIO

#endif /* __GDCMIO_READER_DICOM_IDICOMFILES_HPP__ */
