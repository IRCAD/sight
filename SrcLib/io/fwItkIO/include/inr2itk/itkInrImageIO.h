/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2011.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef itk_InrImageIO_h
#define itk_InrImageIO_h

#include <fstream>
#include "itkImageIOBase.h"

#include "itkIO/config.hpp"

namespace itk
{

/**
 * \ingroup IOFilters
 * \author R&D team
 * \brief Class that defines how to read/write in the InrImage file format.
 * @todo RGB and vector images.
 */
class FWITKIO_CLASS_API InrImageIO : public ImageIOBase
{
public :
    /** Standard typedef's */
    typedef InrImageIO Self;
    typedef ImageIOBase SuperClass;
    typedef SmartPointer<Self> Pointer;
    
    /** Method for creation through the object factory. */
    itkNewMacro(Self);

    /** Run-time type information (and related methods). */
    itkTypeMacro(InrImageIO, Superclass);
    
    /*-------- This part of the interfaces deals with reading data. ----- */
    
    /** Determine if the file can be read with this ImageIO implementation.
     * \param FileNameToRead The name of the file to test for reading.
     * \post Sets classes ImageIOBase::m_FileName variable to be FileNameToWrite
     * \return Returns true if this ImageIO can read the file specified.
     */
    virtual bool CanReadFile(const char* FileNameToRead);
    
    /** Set the spacing and dimension information for the set filename. */
    virtual void ReadImageInformation();
    
    /** Reads the data from disk into the memory buffer provided. */
    virtual void Read(void* buffer);
    
    /*-------- This part of the interfaces deals with writing data. ----- */

    /** Determine if the file can be written with this ImageIO implementation.
     * \param FileNameToWrite The name of the file to test for writing.
     * \post Sets classes ImageIOBase::m_FileName variable to be FileNameToWrite
     * \return Returns true if this ImageIO can write the file specified.
     */
    virtual bool CanWriteFile(const char * FileNameToWrite);

    /** Set the spacing and dimension information for the set filename. */
    virtual void WriteImageInformation();

    /** Writes the data to disk from the memory buffer provided. Make sure
     * that the IORegions has been set properly. */
    virtual void Write(const void* buffer);
    
protected:
    FWITKIO_API InrImageIO();
    FWITKIO_API ~InrImageIO();
    void PrintSelf(std::ostream& os, Indent indent) const;
private:
    InrImageIO(const Self&); //purposely not implemented
    void operator=(const Self&); //purposely not implemented
    
    std::streampos m_headerSize;
    //bool m_swapBytes;
}; // class InrImageIO()

} // namespace itk

#endif // itk_InrImageIO_h
