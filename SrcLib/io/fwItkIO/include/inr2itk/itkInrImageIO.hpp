/************************************************************************
 *
 * Copyright (C) 2009-2017 IRCAD France
 * Copyright (C) 2012-2017 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#ifndef __INR2ITK_ITKINRIMAGEIO_HPP__
#define __INR2ITK_ITKINRIMAGEIO_HPP__

#include "fwItkIO/config.hpp"

#include <itkImageIOBase.h>

#include <fstream>

namespace itk
{

/**
 * \ingroup IOFilters
 * \brief Class that defines how to read/write in the InrImage file format.
 * @todo RGB and vector images.
 */
class FWITKIO_CLASS_API InrImageIO : public ImageIOBase
{
public:
    /** Standard typedef's */
    typedef InrImageIO Self;
    typedef ImageIOBase SuperClass;
    typedef SmartPointer<Self> Pointer;

    /** Method for creation through the object factory. */
    itkNewMacro(Self)

    /** Run-time type information (and related methods). */
    itkTypeMacro(InrImageIO, Superclass)

    /*-------- This part of the interfaces deals with reading data. ----- */

    /** Determine if the file can be read with this ImageIO implementation.
     * \param FileNameToRead The name of the file to test for reading.
     * \post Sets classes ImageIOBase::m_FileName variable to be FileNameToWrite
     * \return Returns true if this ImageIO can read the file specified.
     */
    virtual bool CanReadFile(const char* FileNameToRead) override;

    /** Set the spacing and dimension information for the set filename. */
    virtual void ReadImageInformation() override;

    /** Reads the data from disk into the memory buffer provided. */
    virtual void Read(void* buffer) override;

    /*-------- This part of the interfaces deals with writing data. ----- */

    /** Determine if the file can be written with this ImageIO implementation.
     * \param FileNameToWrite The name of the file to test for writing.
     * \post Sets classes ImageIOBase::m_FileName variable to be FileNameToWrite
     * \return Returns true if this ImageIO can write the file specified.
     */
    virtual bool CanWriteFile(const char* FileNameToWrite) override;

    /** Set the spacing and dimension information for the set filename. */
    virtual void WriteImageInformation() override;

    /** Writes the data to disk from the memory buffer provided. Make sure
     * that the IORegions has been set properly. */
    virtual void Write(const void* buffer) override;

protected:
    FWITKIO_API InrImageIO();
    FWITKIO_API ~InrImageIO();
    void PrintSelf(std::ostream& os, Indent indent) const override;
private:
    InrImageIO(const Self&); //purposely not implemented
    void operator=(const Self&); //purposely not implemented

    std::streampos m_headerSize;
    //bool m_swapBytes;
}; // class InrImageIO()

} // namespace itk

#endif // __INR2ITK_ITKINRIMAGEIO_HPP__
