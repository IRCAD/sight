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

#include "inr_image_io.hpp"

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

inr_image_io::inr_image_io()
{
    this->SetNumberOfDimensions(3);
    m_PixelType = SCALAR;
    //m_swapBytes = false;
}

//------------------------------------------------------------------------------

inr_image_io::~inr_image_io()
= default;

//------------------------------------------------------------------------------

void inr_image_io::PrintSelf(std::ostream& _os, Indent _indent) const
{
    superclass_t::PrintSelf(_os, _indent);
    _os << _indent << "pixel_t " << m_PixelType << "\n";
    _os << _indent << "component_t " << m_ComponentType << "\n";
}

//------------------------------------------------------------------------------

bool inr_image_io::CanReadFile(const char* _file_name_to_read)
{
    // Do not perform extension checking, magic number in the header is better.
    gzFile input_file = gzopen(_file_name_to_read, "rb");
    if(input_file == nullptr)
    {
        return false;
    }

    std::string first_line(14, '\0');
    first_line.reserve(15);

    gzgets(input_file, first_line.data(), 15);

    gzclose(input_file);

    std::string const magic("#INRIMAGE-4#{\n");
    return first_line == magic;
}

//------------------------------------------------------------------------------

void inr_image_io::ReadImageInformation()
{
    using namespace std::literals::string_literals;

    gzFile input_file = gzopen(this->GetFileName(), "rb");
    if(input_file == nullptr)
    {
        gzclose(input_file);
        throw ExceptionObject(__FILE__, __LINE__, "File "s + GetFileName() + " could not be read");
    }

    const int line_buf_size = 256;
    std::array<char, line_buf_size> linebuf {};
    std::map<std::string, std::string> header_values;
    m_header_size = 0;
    while((gzgets(input_file, linebuf.data(), line_buf_size) != nullptr) && (gzeof(input_file) == 0))
    {
        std::string line(linebuf.data());
        // Count the number of characters we just read : length of the line
        m_header_size += std::streamoff(line.length());
        // Remove eventual trailing '\n'
        std::string::size_type const end_of_line = line.find('\n');
        line = line.substr(0, end_of_line);

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
                gzclose(input_file);
                throw ExceptionObject(__FILE__, __LINE__, "Invalid INR file.\nOffending line : "s + line);
            }

            // Tokenize [KEY] = [VALUE] and store it in map
            std::string key   = line.substr(0, std::size_t(delimiter - line.begin()));
            std::string value = line.substr(std::size_t(delimiter - line.begin() + 1), std::string::npos);
            if(header_values.find(key) != header_values.end())
            {
                // duplicate key
                gzclose(input_file);
                throw ExceptionObject(__FILE__, __LINE__, "Invalid INR file.\nDuplicate key : "s + key);
            }

            header_values[key] = value;
        }
    }

    // We have finished reading the header
    gzclose(input_file);

    // Process headerValue map
    for(auto& header_value : header_values)
    {
        if(header_value.first == "XDIM")
        {
            auto xdim = boost::lexical_cast<std::size_t>(header_value.second);
            this->SetDimensions(0, xdim);
        }
        else if(header_value.first == "YDIM")
        {
            auto ydim = boost::lexical_cast<std::size_t>(header_value.second);
            this->SetDimensions(1, ydim);
        }
        else if(header_value.first == "ZDIM")
        {
            auto zdim = boost::lexical_cast<std::size_t>(header_value.second);
            this->SetDimensions(2, zdim);
        }
        else if(header_value.first == "VDIM")
        {
            auto vdim = boost::lexical_cast<std::uint32_t>(header_value.second);
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
        else if(header_value.first == "VX")
        {
            auto vx = boost::lexical_cast<float>(header_value.second);
            this->SetSpacing(0, vx);
        }
        else if(header_value.first == "VY")
        {
            auto vy = boost::lexical_cast<float>(header_value.second);
            this->SetSpacing(1, vy);
        }
        else if(header_value.first == "VZ")
        {
            auto vz = boost::lexical_cast<float>(header_value.second);
            this->SetSpacing(2, vz);
        }
        else if(header_value.first == "TYPE")
        {
            auto pixsize_it = header_values.find("PIXSIZE");
            if(pixsize_it == header_values.end())
            {
                // Missing pixsize
                throw ExceptionObject(__FILE__, __LINE__, "Invalid INR file.\nPIXSIZE key not found.");
            }

            std::string type    = header_value.second;
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
        else if(header_value.first == "PIXSIZE" || header_value.first == "SCALE")
        {
            // PIXSIZE: nothing to do, processed with "TYPE"
            // SCALE: For fixed point arithmetic only. We don't use it.
        }
        else if(header_value.first == "CPU")
        {
            // Big endian are : sun, sgi
            if(header_value.second == "sun" || header_value.second == "sgi")
            {
                // File is big-endian, swap if system is little endian.
                //m_swapBytes = itk::ByteSwapper<char>::SystemIsLE();
                m_ByteOrder = BigEndian;
            }
            // Little endian are : pc, alpha, decm
            else if(header_value.second == "pc" || header_value.second == "alpha" || header_value.second == "decm")
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
                          "Invalid INR file.\nUnknown CPU value : "s + header_value.second
                );
            }
        }
    }
}

//------------------------------------------------------------------------------

void inr_image_io::Read(void* _buffer)
{
    using namespace std::literals::string_literals;

    gzFile file = gzopen(GetFileName(), "rb");

    if(file == nullptr)
    {
        throw ExceptionObject(__FILE__, __LINE__, "INR file "s + GetFileName() + " could not be read");
    }

    // Skip the header
    int bytes_skipped = int(gzseek(file, m_header_size, SEEK_CUR));
    if(bytes_skipped != m_header_size)
    {
        throw ExceptionObject(
                  __FILE__,
                  __LINE__,
                  "INR file "s + GetFileName() + " could not be read (header size : " + std::to_string(
                      bytes_skipped
                  ) + "/" + std::to_string(m_header_size) + ")."
        );
    }

    // And read the buffer

    // Replace this line :
    //int bytesRead = gzread(file, buffer, GetImageSizeInBytes());
    // by
    int bytes_read    = 0;
    int nb_step       = 10;
    std::int64_t size = ((GetImageSizeInBytes() > 1024LL * 1024 * 10)
                         ? (GetImageSizeInBytes() / nb_step) + 1
                         : GetImageSizeInBytes());

    try
    {
        int step = 0;

        while(bytes_read < GetImageSizeInBytes() && step < nb_step)
        {
            step++;
            UpdateProgress(((float) bytes_read) / float(GetImageSizeInBytes()));
            bytes_read +=
                gzread(
                    file,
                    reinterpret_cast<char*>(_buffer) + bytes_read,
                    std::uint32_t(std::min(size, GetImageSizeInBytes() - bytes_read))
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

    if(bytes_read != GetImageSizeInBytes())
    {
        gzclose(file);
        throw ExceptionObject(__FILE__, __LINE__, "INR file "s + GetFileName() + " could not be read");
    }

    const auto image_size_in_components = std::size_t(GetImageSizeInComponents());

    // Swap bytes if necessary
    if(m_ByteOrder == LittleEndian)
    {
        switch(m_ComponentType)
        {
            case CHAR:
                ByteSwapper<char>::SwapRangeFromSystemToLittleEndian(
                    reinterpret_cast<char*>(_buffer),
                    image_size_in_components
                );
                break;

            case UCHAR:
                ByteSwapper<unsigned char>::SwapRangeFromSystemToLittleEndian(
                    reinterpret_cast<unsigned char*>(_buffer),
                    image_size_in_components
                );
                break;

            case SHORT:
                ByteSwapper<short>::SwapRangeFromSystemToLittleEndian( // NOLINT(google-runtime-int)
                    reinterpret_cast<short*>(_buffer),                 // NOLINT(google-runtime-int)
                    image_size_in_components
                );
                break;

            case USHORT:
                ByteSwapper<unsigned short>::SwapRangeFromSystemToLittleEndian( // NOLINT(google-runtime-int)
                    reinterpret_cast<unsigned short*>(_buffer),                 // NOLINT(google-runtime-int)
                    image_size_in_components
                );
                break;

            case INT:
                ByteSwapper<int>::SwapRangeFromSystemToLittleEndian(
                    reinterpret_cast<int*>(_buffer),
                    image_size_in_components
                );
                break;

            case UINT:
                ByteSwapper<unsigned int>::SwapRangeFromSystemToLittleEndian(
                    reinterpret_cast<unsigned int*>(_buffer),
                    image_size_in_components
                );
                break;

            case LONG:
                ByteSwapper<long>::SwapRangeFromSystemToLittleEndian( // NOLINT(google-runtime-int)
                    reinterpret_cast<long*>(_buffer),                 // NOLINT(google-runtime-int)
                    image_size_in_components
                );
                break;

            case ULONG:
                ByteSwapper<unsigned long>::SwapRangeFromSystemToLittleEndian( // NOLINT(google-runtime-int)
                    reinterpret_cast<unsigned long*>(_buffer),                 // NOLINT(google-runtime-int)
                    image_size_in_components
                );
                break;

            case FLOAT:
                ByteSwapper<float>::SwapRangeFromSystemToLittleEndian(
                    reinterpret_cast<float*>(_buffer),
                    image_size_in_components
                );
                break;

            case DOUBLE:
                ByteSwapper<double>::SwapRangeFromSystemToLittleEndian(
                    reinterpret_cast<double*>(_buffer),
                    image_size_in_components
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
                    reinterpret_cast<char*>(_buffer),
                    image_size_in_components
                );
                break;

            case UCHAR:
                ByteSwapper<unsigned char>::SwapRangeFromSystemToBigEndian(
                    reinterpret_cast<unsigned char*>(_buffer),
                    image_size_in_components
                );
                break;

            case SHORT:
                ByteSwapper<short>::SwapRangeFromSystemToBigEndian( // NOLINT(google-runtime-int)
                    reinterpret_cast<short*>(_buffer),              // NOLINT(google-runtime-int)
                    image_size_in_components
                );
                break;

            case USHORT:
                ByteSwapper<unsigned short>::SwapRangeFromSystemToBigEndian( // NOLINT(google-runtime-int)
                    reinterpret_cast<unsigned short*>(_buffer),              // NOLINT(google-runtime-int)
                    image_size_in_components
                );
                break;

            case INT:
                ByteSwapper<int>::SwapRangeFromSystemToBigEndian(
                    reinterpret_cast<int*>(_buffer),
                    image_size_in_components
                );
                break;

            case UINT:
                ByteSwapper<unsigned int>::SwapRangeFromSystemToBigEndian(
                    reinterpret_cast<unsigned int*>(_buffer),
                    image_size_in_components
                );
                break;

            case LONG:
                ByteSwapper<long>::SwapRangeFromSystemToBigEndian( // NOLINT(google-runtime-int)
                    reinterpret_cast<long*>(_buffer),              // NOLINT(google-runtime-int)
                    image_size_in_components
                );
                break;

            case ULONG:
                ByteSwapper<unsigned long>::SwapRangeFromSystemToBigEndian( // NOLINT(google-runtime-int)
                    reinterpret_cast<unsigned long*>(_buffer),              // NOLINT(google-runtime-int)
                    image_size_in_components
                );
                break;

            case FLOAT:
                ByteSwapper<float>::SwapRangeFromSystemToBigEndian(
                    reinterpret_cast<float*>(_buffer),
                    image_size_in_components
                );
                break;

            case DOUBLE:
                ByteSwapper<double>::SwapRangeFromSystemToBigEndian(
                    reinterpret_cast<double*>(_buffer),
                    image_size_in_components
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

bool inr_image_io::CanWriteFile(const char* _file_name_to_write)
{
    // Extension must be .inr or .inr.gz
    std::string const filename(_file_name_to_write);

    std::string::size_type index = filename.rfind(".inr");
    if(index == filename.length() - std::string(".inr").length())
    {
        return true;
    }

    index = filename.rfind(".inr.gz");
    return index == filename.length() - std::string(".inr.gz").length();
}

//------------------------------------------------------------------------------

void inr_image_io::WriteImageInformation()
{
    using namespace std::literals::string_literals;

    std::stringstream header_stream;

    // Magic
    header_stream << "#INRIMAGE-4#{" << "\n";
    // Dimensions : always write a 3D InrImage
    header_stream << "XDIM=" << ((GetDimensions(0) < 1) ? 1 : GetDimensions(0)) << "\n";
    header_stream << "YDIM=" << ((GetDimensions(1) < 1) ? 1 : GetDimensions(1)) << "\n";
    header_stream << "ZDIM=" << ((GetDimensions(2) < 1) ? 1 : GetDimensions(2)) << "\n";
    // Number of components per pixel
    header_stream << "VDIM=" << GetNumberOfComponents() << "\n";
    // Spacing : if it's 0, put it to one instead
    header_stream << "VX=" << ((GetSpacing(0) == 0.0) ? 1.0 : GetSpacing(0)) << "\n";
    header_stream << "VY=" << ((GetSpacing(1) == 0.0) ? 1.0 : GetSpacing(1)) << "\n";
    header_stream << "VZ=" << ((GetSpacing(2) == 0.0) ? 1.0 : GetSpacing(2)) << "\n";
    // Scale
    header_stream << "SCALE=2**0" << "\n";
    // Endianness
    header_stream << "CPU=" << (itk::ByteSwapper<char>::SystemIsLE() ? "pc" : "sgi") << "\n";
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

    header_stream << "TYPE=" << type << "\n";
    // PixelSize
    int pixel_size = 0;
    if(m_ComponentType == UCHAR || m_ComponentType == CHAR)
    {
        pixel_size = sizeof(char);
    }
    else if(m_ComponentType == USHORT || m_ComponentType == SHORT)
    {
        pixel_size = sizeof(short); // NOLINT(google-runtime-int)
    }
    else if(m_ComponentType == UINT || m_ComponentType == INT)
    {
        pixel_size = sizeof(int);
    }
    else if(m_ComponentType == ULONG || m_ComponentType == LONG)
    {
        pixel_size = sizeof(long); // NOLINT(google-runtime-int)
    }
    else if(m_ComponentType == FLOAT)
    {
        pixel_size = sizeof(float);
    }
    else if(m_ComponentType == DOUBLE)
    {
        pixel_size = sizeof(double);
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

    pixel_size *= 8;
    header_stream << "PIXSIZE=" << pixel_size << " bits" << "\n";

    int const padding = int(256 - header_stream.str().length() - std::string("##}\n").length());
    for(int i = 0 ; i < padding ; ++i)
    {
        header_stream << "\n";
    }

    header_stream << "##}\n";
    if(!header_stream)
    {
        // Something could not be written to headerStream
        throw ExceptionObject(
                  __FILE__,
                  __LINE__,
                  "File "s + GetFileName() + " could not be written : error while writing to headerStream."
        );
    }

    std::string const header = header_stream.str();
    std::string const filename(this->GetFileName());
    std::string::size_type const index = filename.rfind(".inr.gz");

    if(index == filename.length() - std::string(".inr.gz").length())
    {
        gzFile output_file = gzopen(this->GetFileName(), "wb9");
        if(output_file == nullptr)
        {
            throw ExceptionObject(
                      __FILE__,
                      __LINE__,
                      "File "s + GetFileName() + " could not be written : error writing header."
            );
        }

        std::string header_string = header_stream.str();
        gzwrite(output_file, header_string.c_str(), std::uint32_t(header_string.length()));
        gzclose(output_file);
    }
    else
    {
        FILE* output_file = fopen(this->GetFileName(), "wb");
        if(output_file == nullptr)
        {
            throw ExceptionObject(
                      __FILE__,
                      __LINE__,
                      "File "s + GetFileName() + " could not be written : error writing header."
            );
        }

        std::string const header_string = header_stream.str();
        if(fwrite(header_string.c_str(), sizeof(char), header_string.length(), output_file) == 0)
        {
            perror("fwrite");
        }

        if(fclose(output_file) != 0)
        {
            perror("fclose");
        }
    }
}

//------------------------------------------------------------------------------

void inr_image_io::Write(const void* _buffer)
{
    using namespace std::literals::string_literals;

    this->WriteImageInformation();
    std::string const filename(GetFileName());
    std::string const suffix(".inr.gz");
    std::string::size_type const index = filename.rfind(suffix);
    if(index == filename.length() - suffix.length())
    {
        gzFile output_file = gzopen(this->GetFileName(), "ab");
        if(output_file == nullptr)
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
                        output_file,
                        reinterpret_cast<const char*>(_buffer) + written,
                        std::uint32_t(std::min(size, GetImageSizeInBytes() - written))
                    );
            }
        }
        catch(sight::core::exception& /*e*/) // catch progress bar cancel exception
        {
            gzclose(output_file);
            throw;
        }
        UpdateProgress(1.0);
        // End replace

        gzclose(output_file);
    }
    else
    {
        FILE* output_file = fopen(this->GetFileName(), "ab");
        if(output_file == nullptr)
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
                            reinterpret_cast<const char*>(_buffer) + written,
                            1,
                            std::size_t(std::min(size, GetImageSizeInBytes() - written)),
                            output_file
                    ));
            }
        }
        catch(sight::core::exception& /*e*/) // catch progress bar cancel exception
        {
            std::ignore = fclose(output_file);
            throw;
        }
        UpdateProgress(1.0);
        // End replace

        std::ignore = fclose(output_file);
    }
}

} // namespace itk
