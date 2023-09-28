/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

// cspell:ignore NOLINT NOLINTNEXTLINE

#include "InrImageIO.hpp"

#include <core/exception.hpp>
#include <core/spy_log.hpp>

#include <boost/lexical_cast.hpp>

#include <itk_zlib.h>
#include <itkByteSwapper.h>
#include <itkIOCommon.h>
#include <itkMacro.h>

#include <algorithm>
#include <cstdio>
#include <map>
#include <utility>

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
= default;

//------------------------------------------------------------------------------

void InrImageIO::PrintSelf(std::ostream& os, Indent indent) const
{
    Superclass::PrintSelf(os, indent);
    os << indent << "PixelType " << m_PixelType << "\n";
    os << indent << "ComponentType " << m_ComponentType << "\n";
}

//------------------------------------------------------------------------------

bool InrImageIO::CanReadFile(const char* FileNameToRead)
{
    // Do not perform extension checking, magic number in the header is better.
    gzFile inputFile = gzopen(FileNameToRead, "rb");
    if(inputFile == nullptr)
    {
        return false;
    }

    std::string firstLine(14, '\0');
    firstLine.reserve(15);

    gzgets(inputFile, firstLine.data(), 15);

    gzclose(inputFile);

    std::string const magic("#INRIMAGE-4#{\n");
    return firstLine == magic;
}

//------------------------------------------------------------------------------

void InrImageIO::ReadImageInformation()
{
    using namespace std::literals::string_literals;

    gzFile inputFile = gzopen(this->GetFileName(), "rb");
    if(inputFile == nullptr)
    {
        gzclose(inputFile);
        throw ExceptionObject(__FILE__, __LINE__, "File "s + GetFileName() + " could not be read");
    }

    const int lineBufSize = 256;
    std::array<char, lineBufSize> linebuf {};
    std::map<std::string, std::string> headerValues;
    m_headerSize = 0;
    while((gzgets(inputFile, linebuf.data(), lineBufSize) != nullptr) && (gzeof(inputFile) == 0))
    {
        std::string line(linebuf.data());
        // Count the number of characters we just read : length of the line
        m_headerSize += std::streamoff(line.length());
        // Remove eventual trailing '\n'
        std::string::size_type const endOfLine = line.find('\n');
        line = line.substr(0, endOfLine);

        if(line == "##}")
        {
            // End of header : get outta here
            break;
        }

        if(line.empty() || line[0] == '#')
        {
            // Empty line or comment line, nothing to do
        }
        else
        {
            // Look for key=value
            std::string::const_iterator delimiter = std::find(line.begin(), line.end(), '=');
            if(delimiter == line.end())
            {
                // invalid line : missing '='
                gzclose(inputFile);
                throw ExceptionObject(__FILE__, __LINE__, "Invalid INR file.\nOffending line : "s + line);
            }

            // Tokenize [KEY] = [VALUE] and store it in map
            std::string key   = line.substr(0, std::size_t(delimiter - line.begin()));
            std::string value = line.substr(std::size_t(delimiter - line.begin() + 1), std::string::npos);
            if(headerValues.find(key) != headerValues.end())
            {
                // duplicate key
                gzclose(inputFile);
                throw ExceptionObject(__FILE__, __LINE__, "Invalid INR file.\nDuplicate key : "s + key);
            }

            headerValues[key] = value;
        }
    }

    // We have finished reading the header
    gzclose(inputFile);

    // Process headerValue map
    for(auto& headerValue : headerValues)
    {
        if(headerValue.first == "XDIM")
        {
            auto xdim = boost::lexical_cast<std::size_t>(headerValue.second);
            this->SetDimensions(0, xdim);
        }
        else if(headerValue.first == "YDIM")
        {
            auto ydim = boost::lexical_cast<std::size_t>(headerValue.second);
            this->SetDimensions(1, ydim);
        }
        else if(headerValue.first == "ZDIM")
        {
            auto zdim = boost::lexical_cast<std::size_t>(headerValue.second);
            this->SetDimensions(2, zdim);
        }
        else if(headerValue.first == "VDIM")
        {
            auto vdim = boost::lexical_cast<std::uint32_t>(headerValue.second);
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
        else if(headerValue.first == "VX")
        {
            auto vx = boost::lexical_cast<float>(headerValue.second);
            this->SetSpacing(0, vx);
        }
        else if(headerValue.first == "VY")
        {
            auto vy = boost::lexical_cast<float>(headerValue.second);
            this->SetSpacing(1, vy);
        }
        else if(headerValue.first == "VZ")
        {
            auto vz = boost::lexical_cast<float>(headerValue.second);
            this->SetSpacing(2, vz);
        }
        else if(headerValue.first == "TYPE")
        {
            auto pixsize_it = headerValues.find("PIXSIZE");
            if(pixsize_it == headerValues.end())
            {
                // Missing pixsize
                throw ExceptionObject(__FILE__, __LINE__, "Invalid INR file.\nPIXSIZE key not found.");
            }

            std::string type    = headerValue.second;
            std::string pixsize = pixsize_it->second;
            if(type == "unsigned fixed")
            {
                if(pixsize == "8 bits")
                {
                    m_ComponentType = UCHAR;
                }
                else if(pixsize == "16 bits")
                {
                    m_ComponentType = USHORT;
                }
                else if(pixsize == "32 bits")
                {
                    if(4 == sizeof(unsigned int))
                    {
                        m_ComponentType = UINT;
                    }
                    else if(4 == sizeof(unsigned long)) // NOLINT(google-runtime-int)
                    {
                        m_ComponentType = ULONG;
                    }
                    else
                    {
                        throw ExceptionObject(__FILE__, __LINE__, "Invalid INR file.\nInvalid PIXSIZE.");
                    }
                }
                else if(pixsize == "64 bits")
                {
                    if(8 == sizeof(unsigned int))
                    {
                        m_ComponentType = UINT;
                    }
                    else if(8 == sizeof(unsigned long)) // NOLINT(google-runtime-int)
                    {
                        m_ComponentType = ULONG;
                    }
                    else
                    {
                        throw ExceptionObject(__FILE__, __LINE__, "Invalid INR file.\nInvalid PIXSIZE.");
                    }
                }
                else
                {
                    throw ExceptionObject(__FILE__, __LINE__, "Invalid INR file.\nInvalid PIXSIZE.");
                }
            }
            else if(type == "signed fixed")
            {
                if(pixsize == "8 bits")
                {
                    m_ComponentType = CHAR;
                }
                else if(pixsize == "16 bits")
                {
                    m_ComponentType = SHORT;
                }
                else if(pixsize == "32 bits")
                {
                    if(4 == sizeof(int))
                    {
                        m_ComponentType = INT;
                    }
                    else if(4 == sizeof(long)) // NOLINT(google-runtime-int)
                    {
                        m_ComponentType = LONG;
                    }
                    else
                    {
                        throw ExceptionObject(__FILE__, __LINE__, "Invalid INR file.\nInvalid PIXSIZE.");
                    }
                }
                else if(pixsize == "64 bits")
                {
                    if(8 == sizeof(int))
                    {
                        m_ComponentType = INT;
                    }
                    else if(8 == sizeof(long)) // NOLINT(google-runtime-int)
                    {
                        m_ComponentType = LONG;
                    }
                    else
                    {
                        throw ExceptionObject(__FILE__, __LINE__, "Invalid INR file.\nInvalid PIXSIZE.");
                    }
                }
                else
                {
                    throw ExceptionObject(__FILE__, __LINE__, "Invalid INR file.\nInvalid PIXSIZE.");
                }
            }
            else if(type == "float")
            {
                if(pixsize == "32 bits")
                {
                    m_ComponentType = FLOAT;
                }
                else if(pixsize == "64 bits")
                {
                    m_ComponentType = DOUBLE;
                }
                else
                {
                    throw ExceptionObject(__FILE__, __LINE__, "Invalid INR file.\nInvalid PIXSIZE.");
                }
            }
            else
            {
                throw ExceptionObject(__FILE__, __LINE__, "Invalid INR file.\nInvalid TYPE.");
            }
        } // TYPE
        else if(headerValue.first == "PIXSIZE" || headerValue.first == "SCALE")
        {
            // PIXSIZE: nothing to do, processed with "TYPE"
            // SCALE: For fixed point arithmetic only. We don't use it.
        }
        else if(headerValue.first == "CPU")
        {
            // Big endian are : sun, sgi
            if(headerValue.second == "sun" || headerValue.second == "sgi")
            {
                // File is big-endian, swap if system is little endian.
                //m_swapBytes = itk::ByteSwapper<char>::SystemIsLE();
                m_ByteOrder = BigEndian;
            }
            // Little endian are : pc, alpha, decm
            else if(headerValue.second == "pc" || headerValue.second == "alpha" || headerValue.second == "decm")
            {
                // File is little-endian, swap if system is big-endian.
                //m_swapBytes = itk::ByteSwapper<char>::SystemIsBE();
                m_ByteOrder = LittleEndian;
            }
            else
            {
                throw ExceptionObject(
                          __FILE__,
                          __LINE__,
                          "Invalid INR file.\nUnknown CPU value : "s + headerValue.second
                );
            }
        }
    }
}

//------------------------------------------------------------------------------

void InrImageIO::Read(void* buffer)
{
    using namespace std::literals::string_literals;

    gzFile file = gzopen(GetFileName(), "rb");

    if(file == nullptr)
    {
        throw ExceptionObject(__FILE__, __LINE__, "INR file "s + GetFileName() + " could not be read");
    }

    // Skip the header
    int bytesSkipped = int(gzseek(file, m_headerSize, SEEK_CUR));
    if(bytesSkipped != m_headerSize)
    {
        throw ExceptionObject(
                  __FILE__,
                  __LINE__,
                  "INR file "s + GetFileName() + " could not be read (header size : " + std::to_string(
                      bytesSkipped
                  ) + "/" + std::to_string(m_headerSize) + ")."
        );
    }

    // And read the buffer

    // Replace this line :
    //int bytesRead = gzread(file, buffer, GetImageSizeInBytes());
    // by
    int bytesRead     = 0;
    int nbStep        = 10;
    std::int64_t size = ((GetImageSizeInBytes() > 1024LL * 1024 * 10)
                         ? (GetImageSizeInBytes() / nbStep) + 1
                         : GetImageSizeInBytes());

    try
    {
        int step = 0;

        while(bytesRead < GetImageSizeInBytes() && step < nbStep)
        {
            step++;
            UpdateProgress(((float) bytesRead) / float(GetImageSizeInBytes()));
            bytesRead +=
                gzread(
                    file,
                    reinterpret_cast<char*>(buffer) + bytesRead,
                    std::uint32_t(std::min(size, GetImageSizeInBytes() - bytesRead))
                );
        }
    }
    catch(sight::core::exception& /*e*/) // catch progress bar cancel exception
    {
        gzclose(file);
        throw;
    }
    UpdateProgress(1.0);
    // End replace

    if(bytesRead != GetImageSizeInBytes())
    {
        gzclose(file);
        throw ExceptionObject(__FILE__, __LINE__, "INR file "s + GetFileName() + " could not be read");
    }

    const auto imageSizeInComponents = std::size_t(GetImageSizeInComponents());

    // Swap bytes if necessary
    if(m_ByteOrder == LittleEndian)
    {
        switch(m_ComponentType)
        {
            case CHAR:
                ByteSwapper<char>::SwapRangeFromSystemToLittleEndian(
                    reinterpret_cast<char*>(buffer),
                    imageSizeInComponents
                );
                break;

            case UCHAR:
                ByteSwapper<unsigned char>::SwapRangeFromSystemToLittleEndian(
                    reinterpret_cast<unsigned char*>(buffer),
                    imageSizeInComponents
                );
                break;

            case SHORT:
                ByteSwapper<short>::SwapRangeFromSystemToLittleEndian( // NOLINT(google-runtime-int)
                    reinterpret_cast<short*>(buffer),                  // NOLINT(google-runtime-int)
                    imageSizeInComponents
                );
                break;

            case USHORT:
                ByteSwapper<unsigned short>::SwapRangeFromSystemToLittleEndian( // NOLINT(google-runtime-int)
                    reinterpret_cast<unsigned short*>(buffer),                  // NOLINT(google-runtime-int)
                    imageSizeInComponents
                );
                break;

            case INT:
                ByteSwapper<int>::SwapRangeFromSystemToLittleEndian(
                    reinterpret_cast<int*>(buffer),
                    imageSizeInComponents
                );
                break;

            case UINT:
                ByteSwapper<unsigned int>::SwapRangeFromSystemToLittleEndian(
                    reinterpret_cast<unsigned int*>(buffer),
                    imageSizeInComponents
                );
                break;

            case LONG:
                ByteSwapper<long>::SwapRangeFromSystemToLittleEndian( // NOLINT(google-runtime-int)
                    reinterpret_cast<long*>(buffer),                  // NOLINT(google-runtime-int)
                    imageSizeInComponents
                );
                break;

            case ULONG:
                ByteSwapper<unsigned long>::SwapRangeFromSystemToLittleEndian( // NOLINT(google-runtime-int)
                    reinterpret_cast<unsigned long*>(buffer),                  // NOLINT(google-runtime-int)
                    imageSizeInComponents
                );
                break;

            case FLOAT:
                ByteSwapper<float>::SwapRangeFromSystemToLittleEndian(
                    reinterpret_cast<float*>(buffer),
                    imageSizeInComponents
                );
                break;

            case DOUBLE:
                ByteSwapper<double>::SwapRangeFromSystemToLittleEndian(
                    reinterpret_cast<double*>(buffer),
                    imageSizeInComponents
                );
                break;

            default:
                gzclose(file);
                throw ExceptionObject(__FILE__, __LINE__, "Pixel Type Unknown");
        }
    }
    else if(m_ByteOrder == BigEndian)
    {
        switch(m_ComponentType)
        {
            case CHAR:
                ByteSwapper<char>::SwapRangeFromSystemToBigEndian(
                    reinterpret_cast<char*>(buffer),
                    imageSizeInComponents
                );
                break;

            case UCHAR:
                ByteSwapper<unsigned char>::SwapRangeFromSystemToBigEndian(
                    reinterpret_cast<unsigned char*>(buffer),
                    imageSizeInComponents
                );
                break;

            case SHORT:
                ByteSwapper<short>::SwapRangeFromSystemToBigEndian( // NOLINT(google-runtime-int)
                    reinterpret_cast<short*>(buffer),               // NOLINT(google-runtime-int)
                    imageSizeInComponents
                );
                break;

            case USHORT:
                ByteSwapper<unsigned short>::SwapRangeFromSystemToBigEndian( // NOLINT(google-runtime-int)
                    reinterpret_cast<unsigned short*>(buffer),               // NOLINT(google-runtime-int)
                    imageSizeInComponents
                );
                break;

            case INT:
                ByteSwapper<int>::SwapRangeFromSystemToBigEndian(reinterpret_cast<int*>(buffer), imageSizeInComponents);
                break;

            case UINT:
                ByteSwapper<unsigned int>::SwapRangeFromSystemToBigEndian(
                    reinterpret_cast<unsigned int*>(buffer),
                    imageSizeInComponents
                );
                break;

            case LONG:
                ByteSwapper<long>::SwapRangeFromSystemToBigEndian( // NOLINT(google-runtime-int)
                    reinterpret_cast<long*>(buffer),               // NOLINT(google-runtime-int)
                    imageSizeInComponents
                );
                break;

            case ULONG:
                ByteSwapper<unsigned long>::SwapRangeFromSystemToBigEndian( // NOLINT(google-runtime-int)
                    reinterpret_cast<unsigned long*>(buffer),               // NOLINT(google-runtime-int)
                    imageSizeInComponents
                );
                break;

            case FLOAT:
                ByteSwapper<float>::SwapRangeFromSystemToBigEndian(
                    reinterpret_cast<float*>(buffer),
                    imageSizeInComponents
                );
                break;

            case DOUBLE:
                ByteSwapper<double>::SwapRangeFromSystemToBigEndian(
                    reinterpret_cast<double*>(buffer),
                    imageSizeInComponents
                );
                break;

            default:
                gzclose(file);
                throw ExceptionObject(__FILE__, __LINE__, "Pixel Type Unknown");
        }
    }

    gzclose(file);
}

//------------------------------------------------------------------------------

bool InrImageIO::CanWriteFile(const char* FileNameToWrite)
{
    // Extension must be .inr or .inr.gz
    std::string const filename(FileNameToWrite);

    std::string::size_type index = filename.rfind(".inr");
    if(index == filename.length() - std::string(".inr").length())
    {
        return true;
    }

    index = filename.rfind(".inr.gz");
    return index == filename.length() - std::string(".inr.gz").length();
}

//------------------------------------------------------------------------------

void InrImageIO::WriteImageInformation()
{
    using namespace std::literals::string_literals;

    std::stringstream headerStream;

    // Magic
    headerStream << "#INRIMAGE-4#{" << "\n";
    // Dimensions : always write a 3D InrImage
    headerStream << "XDIM=" << ((GetDimensions(0) < 1) ? 1 : GetDimensions(0)) << "\n";
    headerStream << "YDIM=" << ((GetDimensions(1) < 1) ? 1 : GetDimensions(1)) << "\n";
    headerStream << "ZDIM=" << ((GetDimensions(2) < 1) ? 1 : GetDimensions(2)) << "\n";
    // Number of components per pixel
    headerStream << "VDIM=" << GetNumberOfComponents() << "\n";
    // Spacing : if it's 0, put it to one instead
    headerStream << "VX=" << ((GetSpacing(0) == 0.0) ? 1.0 : GetSpacing(0)) << "\n";
    headerStream << "VY=" << ((GetSpacing(1) == 0.0) ? 1.0 : GetSpacing(1)) << "\n";
    headerStream << "VZ=" << ((GetSpacing(2) == 0.0) ? 1.0 : GetSpacing(2)) << "\n";
    // Scale
    headerStream << "SCALE=2**0" << "\n";
    // Endianness
    headerStream << "CPU=" << (itk::ByteSwapper<char>::SystemIsLE() ? "pc" : "sgi") << "\n";
    // Point type
    std::string type;
    if(m_ComponentType == UCHAR || m_ComponentType == USHORT || m_ComponentType == UINT || m_ComponentType == ULONG)
    {
        type = "unsigned fixed";
    }
    else if(m_ComponentType == CHAR || m_ComponentType == SHORT || m_ComponentType == INT || m_ComponentType == LONG)
    {
        type = "signed fixed";
    }
    else if(m_ComponentType == FLOAT || m_ComponentType == DOUBLE)
    {
        type = "float";
    }
    else
    {
        // Unknown/incorrect component type
        throw ExceptionObject(
                  __FILE__,
                  __LINE__,
                  "File "s + GetFileName() + " could not be written : unknown pixel type."
        );
    }

    headerStream << "TYPE=" << type << "\n";
    // PixelSize
    int pixelSize = 0;
    if(m_ComponentType == UCHAR || m_ComponentType == CHAR)
    {
        pixelSize = sizeof(char);
    }
    else if(m_ComponentType == USHORT || m_ComponentType == SHORT)
    {
        pixelSize = sizeof(short); // NOLINT(google-runtime-int)
    }
    else if(m_ComponentType == UINT || m_ComponentType == INT)
    {
        pixelSize = sizeof(int);
    }
    else if(m_ComponentType == ULONG || m_ComponentType == LONG)
    {
        pixelSize = sizeof(long); // NOLINT(google-runtime-int)
    }
    else if(m_ComponentType == FLOAT)
    {
        pixelSize = sizeof(float);
    }
    else if(m_ComponentType == DOUBLE)
    {
        pixelSize = sizeof(double);
    }
    else
    {
        // Unknown/incorrect component type
        throw ExceptionObject(
                  __FILE__,
                  __LINE__,
                  "File "s + GetFileName() + " could not be written : unknown pixel type."
        );
    }

    pixelSize *= 8;
    headerStream << "PIXSIZE=" << pixelSize << " bits" << "\n";

    int const padding = int(256 - headerStream.str().length() - std::string("##}\n").length());
    for(int i = 0 ; i < padding ; ++i)
    {
        headerStream << "\n";
    }

    headerStream << "##}\n";
    if(!headerStream)
    {
        // Something could not be written to headerStream
        throw ExceptionObject(
                  __FILE__,
                  __LINE__,
                  "File "s + GetFileName() + " could not be written : error while writing to headerStream."
        );
    }

    std::string const header = headerStream.str();
    std::string const filename(this->GetFileName());
    std::string::size_type const index = filename.rfind(".inr.gz");

    if(index == filename.length() - std::string(".inr.gz").length())
    {
        gzFile outputFile = gzopen(this->GetFileName(), "wb9");
        if(outputFile == nullptr)
        {
            throw ExceptionObject(
                      __FILE__,
                      __LINE__,
                      "File "s + GetFileName() + " could not be written : error writing header."
            );
        }

        std::string headerString = headerStream.str();
        gzwrite(outputFile, headerString.c_str(), std::uint32_t(headerString.length()));
        gzclose(outputFile);
    }
    else
    {
        FILE* outputFile = fopen(this->GetFileName(), "wb");
        if(outputFile == nullptr)
        {
            throw ExceptionObject(
                      __FILE__,
                      __LINE__,
                      "File "s + GetFileName() + " could not be written : error writing header."
            );
        }

        std::string const headerString = headerStream.str();
        if(fwrite(headerString.c_str(), sizeof(char), headerString.length(), outputFile) == 0)
        {
            perror("fwrite");
        }

        if(fclose(outputFile) != 0)
        {
            perror("fclose");
        }
    }
}

//------------------------------------------------------------------------------

void InrImageIO::Write(const void* buffer)
{
    using namespace std::literals::string_literals;

    this->WriteImageInformation();
    std::string const filename(GetFileName());
    std::string const suffix(".inr.gz");
    std::string::size_type const index = filename.rfind(suffix);
    if(index == filename.length() - suffix.length())
    {
        gzFile outputFile = gzopen(this->GetFileName(), "ab");
        if(outputFile == nullptr)
        {
            throw ExceptionObject(
                      __FILE__,
                      __LINE__,
                      "File "s + GetFileName() + " could not be written : error writing buffer."
            );
        }

        // Replace this line :
        // gzwrite(outputFile, const_cast<void*>(buffer), GetImageSizeInBytes());
        // by
        std::int64_t size = ((GetImageSizeInBytes() > 1024LL * 1024 * 10)
                             ? (GetImageSizeInBytes() / 10) + 1
                             : GetImageSizeInBytes());

        try
        {
            int written = 0;

            while(written < GetImageSizeInBytes())
            {
                UpdateProgress(((float) written) / float(GetImageSizeInBytes()));
                written +=
                    gzwrite(
                        outputFile,
                        reinterpret_cast<const char*>(buffer) + written,
                        std::uint32_t(std::min(size, GetImageSizeInBytes() - written))
                    );
            }
        }
        catch(sight::core::exception& /*e*/) // catch progress bar cancel exception
        {
            gzclose(outputFile);
            throw;
        }
        UpdateProgress(1.0);
        // End replace

        gzclose(outputFile);
    }
    else
    {
        FILE* outputFile = fopen(this->GetFileName(), "ab");
        if(outputFile == nullptr)
        {
            throw ExceptionObject(
                      __FILE__,
                      __LINE__,
                      "File "s + GetFileName() + " could not be written : error writing buffer."
            );
        }

        // Replace this line :
        // fwrite(buffer, 1, GetImageSizeInBytes(), outputFile);
        // by :
        std::int64_t size = ((GetImageSizeInBytes() > 1024LL * 1024 * 10)
                             ? (GetImageSizeInBytes() / 10) + 1
                             : GetImageSizeInBytes());

        try
        {
            int written = 0;

            while(written < GetImageSizeInBytes())
            {
                UpdateProgress(((float) written) / float(GetImageSizeInBytes()));
                written +=
                    int(fwrite(
                            reinterpret_cast<const char*>(buffer) + written,
                            1,
                            std::size_t(std::min(size, GetImageSizeInBytes() - written)),
                            outputFile
                    ));
            }
        }
        catch(sight::core::exception& /*e*/) // catch progress bar cancel exception
        {
            std::ignore = fclose(outputFile);
            throw;
        }
        UpdateProgress(1.0);
        // End replace

        std::ignore = fclose(outputFile);
    }
}

} // namespace itk
