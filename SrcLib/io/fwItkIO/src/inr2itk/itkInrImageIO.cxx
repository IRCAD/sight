/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2011.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <algorithm>
#include <map>
#include <utility>
#include <cstdio>

#include <boost/lexical_cast.hpp>

#include <itk_zlib.h>
#include <itkByteSwapper.h>
#include <itkIOCommon.h>
#include <itkExceptionObject.h>

#include <fwCore/spyLog.hpp>
#include <fwCore/Exception.hpp>

#include "inr2itk/itkInrImageIO.h"

#define max(a,b) ((a) > (b) ? (a) : (b))
#define min(a,b) ((a) <= (b) ? (a) : (b))

namespace itk
{

InrImageIO::InrImageIO()
{
    this->SetNumberOfDimensions(3);
    m_PixelType = SCALAR;
    //m_swapBytes = false;
}

//------------------------------------------------------------------------------

InrImageIO::~InrImageIO()
{
    // Nothing to do.
}

//------------------------------------------------------------------------------

void InrImageIO::PrintSelf(std::ostream& os, Indent indent) const
{
    Superclass::PrintSelf(os, indent);
    os << indent << "PixelType " << m_PixelType << "\n";
    os << indent << "ComponentType " << m_ComponentType << "\n";
}

//------------------------------------------------------------------------------

bool InrImageIO::CanReadFile( const char* FileNameToRead )
{
    // Do not perform extension checking, magic number in the header is better.
    gzFile inputFile = gzopen(FileNameToRead, "rb");
    if(inputFile == 0)
    {
        return false;
    }

    char firstLine[256];

    gzgets(inputFile, firstLine, 256);

    gzclose(inputFile);

    std::string const magic("#INRIMAGE-4#{\n");
    if(firstLine!=magic)
    {
        return false;
    }
    else
    {
        return true;
    }
}

//------------------------------------------------------------------------------

void InrImageIO::ReadImageInformation()
{
    gzFile inputFile = gzopen(this->GetFileName(), "rb");
    if(inputFile==NULL)
    {
        gzclose(inputFile);
        ExceptionObject exception(__FILE__, __LINE__);
        std::stringstream errorMessage;
        errorMessage << "File " << GetFileName() << " could not be read";
        exception.SetDescription(errorMessage.str());
        throw exception;
    }

    const int lineBufSize = 256;
    char linebuf[lineBufSize];
    std::map<std::string, std::string> headerValues;
    m_headerSize = 0;
    while( gzgets(inputFile, linebuf, lineBufSize) && !gzeof(inputFile) )
    {
        std::string line(linebuf);
        // Count the number of characters we just read : length of the line
        m_headerSize += line.length();
        // Remove eventual trailing '\n'
        std::string::size_type const endOfLine = line.find('\n');
        line = line.substr(0, endOfLine);

        if(line=="##}")
        {
            // End of header : get outta here
            break;
        }

        if(line.empty())
        {
            // Empty line, nothing to do
        }
        else if(line[0]=='#')
        {
            // Comment line, nothing to do
        }
        else
        {
            // Look for key=value
            std::string::const_iterator delimiter = std::find(line.begin(), line.end(), '=');
            if(delimiter==line.end())
            {
                // invalid line : missing '='
                gzclose(inputFile);
                ExceptionObject exception(__FILE__, __LINE__);
                std::stringstream errorMessage;
                errorMessage << "Invalid INR file.\n" << "Offending line : " << line;
                exception.SetDescription(errorMessage.str());
                throw exception;
            }

            // Tokenize [KEY] = [VALUE] and store it in map
            std::string key = line.substr(0, delimiter-line.begin());
            std::string value = line.substr(delimiter-line.begin()+1, std::string::npos);
            OSLM_TRACE("[" << key << "] = [" << value << "]");
            if(headerValues.find(key)!=headerValues.end())
            {
                // duplicate key
                ExceptionObject exception(__FILE__, __LINE__);
                gzclose(inputFile);
                std::stringstream errorMessage;
                errorMessage << "Invalid INR file.\n" << "Duplicate key : " << key;
                exception.SetDescription(errorMessage.str());
                throw exception;
            }
            else
            {
                headerValues[key] = value;
            }
        }
    }
    // We have finished reading the header
    gzclose(inputFile);

    // Process headerValue map
    for(std::map<std::string, std::string>::const_iterator it=headerValues.begin(); it!=headerValues.end(); ++it)
    {
        if(it->first=="XDIM")
        {
            int xdim = ::boost::lexical_cast<int>( it->second );
            this->SetDimensions(0, xdim);
        }
        else if(it->first=="YDIM")
        {
            int ydim = ::boost::lexical_cast<int>( it->second );
            this->SetDimensions(1, ydim);
        }
        else if(it->first=="ZDIM")
        {
            int zdim = ::boost::lexical_cast<int>( it->second );
            this->SetDimensions(2, zdim);
        }
        else if(it->first=="VDIM")
        {
            int vdim = ::boost::lexical_cast<int>( it->second );
            if(vdim == 1)
            {
                SetPixelType(SCALAR);
            }
            else
            {
                SetPixelType(VECTOR);
            }
            SetNumberOfComponents(vdim);
        }
        else if(it->first=="VX")
        {
            float vx = ::boost::lexical_cast< float >( it->second );
            this->SetSpacing(0, vx);
        }
        else if(it->first=="VY")
        {
            float vy = ::boost::lexical_cast< float >( it->second );
            this->SetSpacing(1, vy);
        }
        else if(it->first=="VZ")
        {
            float vz = ::boost::lexical_cast< float >( it->second );
            this->SetSpacing(2, vz);
        }
        else if(it->first=="TYPE")
        {
            std::map<std::string, std::string>::const_iterator pixsize_it = headerValues.find("PIXSIZE");
            if(pixsize_it==headerValues.end())
            {
                // Missing pixsize
                ExceptionObject exception(__FILE__, __LINE__);
                std::stringstream errorMessage;
                errorMessage << "Invalid INR file.\n" << "PIXSIZE key not found.";
                exception.SetDescription(errorMessage.str());
                throw exception;
            }
            std::string type = it->second;
            std::string pixsize = pixsize_it->second;
            if(type=="unsigned fixed")
            {
                if(pixsize=="8 bits")
                {
                    m_ComponentType = UCHAR;
                }
                else if(pixsize=="16 bits")
                {
                    m_ComponentType = USHORT;
                }
                else if(pixsize=="32 bits")
                {
                    if ( 4 == sizeof(unsigned int) )
                    {
                        m_ComponentType = UINT;
                    }
                    else if ( 4 == sizeof(unsigned long) )
                    {
                        m_ComponentType = ULONG;
                    }
                    else
                    {
                        ExceptionObject exception(__FILE__, __LINE__);
                        std::stringstream errorMessage;
                        errorMessage << "Invalid INR file.\n" << "Invalid PIXSIZE.";
                        exception.SetDescription(errorMessage.str());
                        throw exception;
                    }
                }
                else if(pixsize=="64 bits")
                {
                    if ( 8 == sizeof(unsigned int) )
                    {
                        m_ComponentType = UINT;
                    }
                    else if ( 8 == sizeof(unsigned long) )
                    {
                        m_ComponentType = ULONG;
                    }
                    else
                    {
                        ExceptionObject exception(__FILE__, __LINE__);
                        std::stringstream errorMessage;
                        errorMessage << "Invalid INR file.\n" << "Invalid PIXSIZE.";
                        exception.SetDescription(errorMessage.str());
                        throw exception;
                    }
                }
                else
                {
                    ExceptionObject exception(__FILE__, __LINE__);
                    std::stringstream errorMessage;
                    errorMessage << "Invalid INR file.\n" << "Invalid PIXSIZE.";
                    exception.SetDescription(errorMessage.str());
                    throw exception;
                }
            }
            else if(type=="signed fixed")
            {
                if(pixsize=="8 bits")
                {
                    m_ComponentType = CHAR;
                }
                else if(pixsize=="16 bits")
                {
                    m_ComponentType = SHORT;
                }
                else if(pixsize=="32 bits")
                {
                    if ( 4 == sizeof(int) )
                    {
                        m_ComponentType = INT;
                    }
                    else if ( 4 == sizeof(long) )
                    {
                        m_ComponentType = LONG;
                    }
                    else
                    {
                        ExceptionObject exception(__FILE__, __LINE__);
                        std::stringstream errorMessage;
                        errorMessage << "Invalid INR file.\n" << "Invalid PIXSIZE.";
                        exception.SetDescription(errorMessage.str());
                        throw exception;
                    }
                }
                else if(pixsize=="64 bits")
                {
                    if ( 8 == sizeof(int) )
                    {
                        m_ComponentType = INT;
                    }
                    else if ( 8 == sizeof(long) )
                    {
                        m_ComponentType = LONG;
                    }
                    else
                    {
                        ExceptionObject exception(__FILE__, __LINE__);
                        std::stringstream errorMessage;
                        errorMessage << "Invalid INR file.\n" << "Invalid PIXSIZE.";
                        exception.SetDescription(errorMessage.str());
                        throw exception;
                    }
                }
                else
                {
                    ExceptionObject exception(__FILE__, __LINE__);
                    std::stringstream errorMessage;
                    errorMessage << "Invalid INR file.\n" << "Invalid PIXSIZE.";
                    exception.SetDescription(errorMessage.str());
                    throw exception;
                }
            }
            else if(type=="float")
            {
                /*
                if(pixsize=="8 bits")
                {
                }
                else if(pixsize=="16 bits")
                {
                }
                */
                /*else*/ if(pixsize=="32 bits")
                {
                    m_ComponentType = FLOAT;
                }
                else if(pixsize=="64 bits")
                {
                    m_ComponentType = DOUBLE;
                }
                else
                {
                    ExceptionObject exception(__FILE__, __LINE__);
                    std::stringstream errorMessage;
                    errorMessage << "Invalid INR file.\n" << "Invalid PIXSIZE.";
                    exception.SetDescription(errorMessage.str());
                    throw exception;
                }
            }
            else
            {
                ExceptionObject exception(__FILE__, __LINE__);
                std::stringstream errorMessage;
                errorMessage << "Invalid INR file.\n" << "Invalid TYPE.";
                exception.SetDescription(errorMessage.str());
                throw exception;
            }
        } // TYPE
        else if(it->first=="PIXSIZE")
        {
            // nothing to do, processed with "TYPE"
        }
        else if(it->first=="SCALE")
        {
            // For fixed point arithmetic only. We don't use it.
        }
        else if(it->first=="CPU")
        {
            // Big endian are : sun, sgi
            if(it->second=="sun" || it->second=="sgi")
            {
                // File is big-endian, swap if system is little endian.
                //m_swapBytes = itk::ByteSwapper<char>::SystemIsLE();
                m_ByteOrder = BigEndian;
            }
            // Little endian are : pc, alpha, decm
            else if(it->second=="pc" || it->second=="alpha" || it->second=="decm")
            {
                // File is little-endian, swap if system is big-endian.
                //m_swapBytes = itk::ByteSwapper<char>::SystemIsBE();
                m_ByteOrder = LittleEndian;
            }
            else
            {
                ExceptionObject exception(__FILE__, __LINE__);
                std::stringstream errorMessage;
                errorMessage << "Invalid INR file.\n" << "Unknown CPU value : " << it->second;
                exception.SetDescription(errorMessage.str());
                throw exception;
            }
        }
    }
}

//------------------------------------------------------------------------------

void InrImageIO::Read(void * buffer)
{
    gzFile file = gzopen(GetFileName(), "rb");

    if(file == NULL)
    {
        ExceptionObject exception(__FILE__, __LINE__);
        std::stringstream errorMessage;
        errorMessage << "INR file " << GetFileName() << " could not be read";
        exception.SetDescription(errorMessage.str());
        throw exception;
    }

    // Skip the header
    int bytesSkipped = gzseek(file, m_headerSize, SEEK_CUR);
    if(bytesSkipped!=m_headerSize)
    {
        ExceptionObject exception(__FILE__, __LINE__);
        std::stringstream errorMessage;
        errorMessage << "INR file " << GetFileName() << " could not be read (header size : " << bytesSkipped << "/" << m_headerSize <<").";
        exception.SetDescription(errorMessage.str());
        throw exception;
    }

    // And read the buffer

    // Replace this line :
    //int bytesRead = gzread(file, buffer, GetImageSizeInBytes());
    // by
    int bytesRead = 0;
    int nbStep = 10;
    long long int size = (  (GetImageSizeInBytes()>1024*1024*10)?  ( GetImageSizeInBytes() / nbStep ) + 1 : GetImageSizeInBytes()  );
    int step = 0;
    try
    {
        while ( bytesRead < GetImageSizeInBytes() && step < nbStep )
        {
            step++;
            UpdateProgress(  ((float)bytesRead)/GetImageSizeInBytes() );
            bytesRead += gzread( file , ((char *)buffer)+bytesRead,  min(size, GetImageSizeInBytes() - bytesRead)  );
        }
    }
    catch(::fwCore::Exception &e) // catch progress bar cancel exception
    {
        gzclose(file);
        throw e;
    }
    UpdateProgress( 1.0 );
    // End replace

    if(bytesRead!=GetImageSizeInBytes())
    {
        gzclose(file);
        ExceptionObject exception(__FILE__, __LINE__);
        std::stringstream errorMessage;
        errorMessage << "INR file " << GetFileName() << " could not be read";
        exception.SetDescription(errorMessage.str());
        throw exception;
    }

    // Swap bytes if necessary
    if ( m_ByteOrder == LittleEndian )
    {
        switch(m_ComponentType)
        {
            case CHAR:
                ByteSwapper<char>::SwapRangeFromSystemToLittleEndian((char*)buffer, GetImageSizeInComponents() );
                break;
            case UCHAR:
                ByteSwapper<unsigned char>::SwapRangeFromSystemToLittleEndian((unsigned char*)buffer, GetImageSizeInComponents() );
                break;
            case SHORT:
                ByteSwapper<short>::SwapRangeFromSystemToLittleEndian((short*)buffer, GetImageSizeInComponents() );
                break;
            case USHORT:
                ByteSwapper<unsigned short>::SwapRangeFromSystemToLittleEndian((unsigned short*)buffer, GetImageSizeInComponents() );
                break;
            case INT:
                ByteSwapper<int>::SwapRangeFromSystemToLittleEndian((int*)buffer, GetImageSizeInComponents() );
                break;
            case UINT:
                ByteSwapper<unsigned int>::SwapRangeFromSystemToLittleEndian((unsigned int*)buffer, GetImageSizeInComponents() );
                break;
            case LONG:
                ByteSwapper<long>::SwapRangeFromSystemToLittleEndian((long*)buffer, GetImageSizeInComponents() );
                break;
            case ULONG:
                ByteSwapper<unsigned long>::SwapRangeFromSystemToLittleEndian((unsigned long*)buffer, GetImageSizeInComponents() );
                break;
            case FLOAT:
                ByteSwapper<float>::SwapRangeFromSystemToLittleEndian((float*)buffer, GetImageSizeInComponents() );
                break;
            case DOUBLE:
                ByteSwapper<double>::SwapRangeFromSystemToLittleEndian((double*)buffer, GetImageSizeInComponents() );
                break;
            default:
                gzclose(file);
                ExceptionObject exception(__FILE__, __LINE__);
                exception.SetDescription("Pixel Type Unknown");
                throw exception;
        }
    }
    else if ( m_ByteOrder == BigEndian )
    {
        switch(m_ComponentType)
        {
            case CHAR:
                ByteSwapper<char>::SwapRangeFromSystemToBigEndian((char*)buffer, GetImageSizeInComponents() );
                break;
            case UCHAR:
                ByteSwapper<unsigned char>::SwapRangeFromSystemToBigEndian((unsigned char*)buffer, GetImageSizeInComponents() );
                break;
            case SHORT:
                ByteSwapper<short>::SwapRangeFromSystemToBigEndian((short*)buffer, GetImageSizeInComponents() );
                break;
            case USHORT:
                ByteSwapper<unsigned short>::SwapRangeFromSystemToBigEndian((unsigned short*)buffer, GetImageSizeInComponents() );
                break;
            case INT:
                ByteSwapper<int>::SwapRangeFromSystemToBigEndian((int*)buffer, GetImageSizeInComponents() );
                break;
            case UINT:
                ByteSwapper<unsigned int>::SwapRangeFromSystemToBigEndian((unsigned int*)buffer, GetImageSizeInComponents() );
                break;
            case LONG:
                ByteSwapper<long>::SwapRangeFromSystemToBigEndian((long*)buffer, GetImageSizeInComponents() );
                break;
            case ULONG:
                ByteSwapper<unsigned long>::SwapRangeFromSystemToBigEndian((unsigned long*)buffer, GetImageSizeInComponents() );
                break;
            case FLOAT:
                ByteSwapper<float>::SwapRangeFromSystemToBigEndian((float*)buffer, GetImageSizeInComponents() );
                break;
            case DOUBLE:
                ByteSwapper<double>::SwapRangeFromSystemToBigEndian((double*)buffer, GetImageSizeInComponents() );
                break;
            default:
                gzclose(file);
                ExceptionObject exception(__FILE__, __LINE__);
                exception.SetDescription("Pixel Type Unknown");
                throw exception;
        }
    }

    gzclose(file);
}

//------------------------------------------------------------------------------

bool InrImageIO::CanWriteFile(const char * FileNameToWrite)
{
    // Extension must be .inr or .inr.gz
    std::string const filename(FileNameToWrite);

    std::string::size_type index = filename.rfind(".inr");
    if(index==filename.length() - std::string(".inr").length())
    {
        return true;
    }

    index = filename.rfind(".inr.gz");
    if(index==filename.length() - std::string(".inr.gz").length())
    {
        return true;
    }

    return false;
}

//------------------------------------------------------------------------------

void InrImageIO::WriteImageInformation()
{
    std::stringstream headerStream;

    // Magic
    headerStream << "#INRIMAGE-4#{" << "\n";
    // Dimensions : always write a 3D InrImage
    headerStream << "XDIM=" << ((GetDimensions(0)<1)?1:GetDimensions(0)) << "\n";
    headerStream << "YDIM=" << ((GetDimensions(1)<1)?1:GetDimensions(1)) << "\n";
    headerStream << "ZDIM=" << ((GetDimensions(2)<1)?1:GetDimensions(2)) << "\n";
    // Number of components per pixel
    headerStream << "VDIM=" << GetNumberOfComponents() << "\n";
    // Spacing : if it's 0, put it to one instead
    headerStream << "VX=" << ((GetSpacing(0)==0.0)?1.0:GetSpacing(0)) << "\n";
    headerStream << "VY=" << ((GetSpacing(1)==0.0)?1.0:GetSpacing(1)) << "\n";
    headerStream << "VZ=" << ((GetSpacing(2)==0.0)?1.0:GetSpacing(2)) << "\n";
    // Scale
    headerStream << "SCALE=2**0" << "\n";
    // Endianness
    headerStream << "CPU=" << (itk::ByteSwapper<char>::SystemIsLE()?"pc":"sgi") << "\n";
    // Point type
    std::string type;
    if(m_ComponentType==UCHAR || m_ComponentType==USHORT || m_ComponentType==UINT || m_ComponentType==ULONG)
    {
        type="unsigned fixed";
    }
    else if(m_ComponentType==CHAR || m_ComponentType==SHORT || m_ComponentType==INT || m_ComponentType==LONG)
    {
        type="signed fixed";
    }
    else if(m_ComponentType==FLOAT || m_ComponentType==DOUBLE)
    {
        type="float";
    }
    else
    {
        // Unknown/incorrect component type
        ExceptionObject exception(__FILE__, __LINE__);
        std::stringstream errorMessage;
        errorMessage << "File " << GetFileName()
                     << " could not be written : unknown pixel type.";
        exception.SetDescription(errorMessage.str());
        throw exception;
    }
    headerStream << "TYPE=" << type << "\n";
    // PixelSize
    int pixelSize;
    if(m_ComponentType==UCHAR || m_ComponentType==CHAR)
    {
        pixelSize = sizeof(char);
    }
    else if(m_ComponentType==USHORT || m_ComponentType==SHORT)
    {
        pixelSize = sizeof(short);
    }
    else if(m_ComponentType==UINT || m_ComponentType==INT)
    {
        pixelSize = sizeof(int);
    }
    else if(m_ComponentType==ULONG || m_ComponentType==LONG)
    {
        pixelSize = sizeof(long);
    }
    else if(m_ComponentType==FLOAT)
    {
        pixelSize = sizeof(float);
    }
    else if(m_ComponentType==DOUBLE)
    {
        pixelSize = sizeof(double);
    }
    else
    {
        // Unknown/incorrect component type
        ExceptionObject exception(__FILE__, __LINE__);
        std::stringstream errorMessage;
        errorMessage << "File " << GetFileName()
                     << " could not be written : unknown pixel type.";
        exception.SetDescription(errorMessage.str());
        throw exception;
    }

    pixelSize *= 8;
    headerStream << "PIXSIZE=" << pixelSize << " bits" << "\n";

    int const padding = 256 - headerStream.str().length() - std::string("##}\n").length();
    for(int i=0; i<padding; ++i)
    {
        headerStream << "\n";
    }
    headerStream << "##}\n";
    if(!headerStream)
    {
        // Something could not be written to headerStream
        ExceptionObject exception(__FILE__, __LINE__);
        std::stringstream errorMessage;
        errorMessage << "File " << GetFileName()
                     << " could not be written : error while writing to headerStream.";
        exception.SetDescription(errorMessage.str());
        throw exception;
    }

    std::string const header = headerStream.str();
    std::string const filename(this->GetFileName());
    std::string::size_type const index = filename.rfind(".inr.gz");

    if(index==filename.length() - std::string(".inr.gz").length())
    {
        gzFile outputFile = gzopen(this->GetFileName(), "wb9");
        if(outputFile == 0)
        {
            ExceptionObject exception(__FILE__, __LINE__);
            std::stringstream errorMessage;
            errorMessage << "File " << GetFileName()
                         << " could not be written : error writing header.";
            exception.SetDescription(errorMessage.str());
            throw exception;
        }
        std::string const headerString = headerStream.str();
        gzwrite( outputFile, const_cast<char*>(headerString.c_str()), headerString.length() );
        gzclose(outputFile);
    }
    else
    {
        FILE* outputFile = fopen(this->GetFileName(), "wb");
        if(outputFile == 0)
        {
            ExceptionObject exception(__FILE__, __LINE__);
            std::stringstream errorMessage;
            errorMessage << "File " << GetFileName()
                         << " could not be written : error writing header.";
            exception.SetDescription(errorMessage.str());
            throw exception;
        }

        std::string const headerString = headerStream.str();
        fwrite( headerString.c_str(), sizeof(char), headerString.length(), outputFile);
        fclose(outputFile);
    }
}

//------------------------------------------------------------------------------

void InrImageIO::Write(const void* buffer)
{
    this->WriteImageInformation();
    std::string const filename(GetFileName());
    std::string const suffix(".inr.gz");
    std::string::size_type const index = filename.rfind(suffix);
    if(index==filename.length() - suffix.length())
    {
        gzFile outputFile = gzopen(this->GetFileName(), "ab");
        if(outputFile == 0)
        {
            ExceptionObject exception(__FILE__, __LINE__);
            std::stringstream errorMessage;
            errorMessage << "File " << GetFileName()
                         << " could not be written : error writing buffer.";
            exception.SetDescription(errorMessage.str());
            throw exception;
        }

        // Replace this line :
        // gzwrite(outputFile, const_cast<void*>(buffer), GetImageSizeInBytes());
        // by
        int written = 0;
        long long int size = (  (GetImageSizeInBytes()>1024*1024*10)? ( GetImageSizeInBytes() / 10 ) + 1 : GetImageSizeInBytes()  );
        try
        {
            while ( written <   GetImageSizeInBytes() )
            {
                UpdateProgress(  ((float)written)/GetImageSizeInBytes() );
                written += gzwrite( outputFile , ((char *)buffer)+written,  min(size, GetImageSizeInBytes() - written)  );

            }
        }
        catch(::fwCore::Exception &e) // catch progress bar cancel exception
        {
            gzclose(outputFile);
            throw e;
        }
        UpdateProgress( 1.0 );
        // End replace

        gzclose(outputFile);
    }
    else
    {
        FILE* outputFile = fopen(this->GetFileName(), "ab");
        if(outputFile == 0)
        {
            ExceptionObject exception(__FILE__, __LINE__);
            std::stringstream errorMessage;
            errorMessage << "File " << GetFileName()
                         << " could not be written : error writing buffer.";
            exception.SetDescription(errorMessage.str());
            throw exception;
        }

        // Replace this line :
        // fwrite(buffer, 1, GetImageSizeInBytes(), outputFile);
        // by :
        int written = 0;
        long long int size = (  (GetImageSizeInBytes()>1024*1024*10)? ( GetImageSizeInBytes() / 10 ) + 1 : GetImageSizeInBytes()  );
        try
        {
            while ( written <   GetImageSizeInBytes() )
            {
                UpdateProgress(  ((float)written)/GetImageSizeInBytes() );
                written += fwrite( ((char *)buffer)+written, 1,  min(size, GetImageSizeInBytes() - written)  , outputFile);
            }
        }
        catch(::fwCore::Exception &e) // catch progress bar cancel exception
        {
            fclose(outputFile);
            throw e;
        }
        UpdateProgress( 1.0 );
        // End replace

        fclose(outputFile);
    }
}

} // namespace itk
