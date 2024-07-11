/************************************************************************
 *
 * Copyright (C) 2023-2024 IRCAD France
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

#pragma once

#include "reader_impl.hxx"

#include <cuda.h>
#include <nppi.h>
#include <nvjpeg.h>

#include <ostream>

// cspell:ignore nvjpeg BGRI RGBI NOLINTNEXTLINE

namespace sight::io::bitmap::detail
{

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define CHECK_CUDA(func, success) \
        if(const auto& status = func; status != (success)) \
        { \
            SIGHT_THROW("The function " #func " failed: " << status); \
        }

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define CHECK_CUDA_NOEXCEPT(func, success) \
        try \
        { \
            if(const auto status = func; status != (success)) \
            { \
                SIGHT_THROW("The function " #func " failed: " << status); \
            } \
        } \
        catch(const std::exception& e) \
        { \
            SIGHT_ERROR(e.what()); \
        }

class nv_jpeg_reader final
{
public:

    /// Delete copy constructors and assignment operators
    nv_jpeg_reader(const nv_jpeg_reader&)            = delete;
    nv_jpeg_reader& operator=(const nv_jpeg_reader&) = delete;

    /// Constructor
    inline nv_jpeg_reader() noexcept
    {
        try
        {
            // Allocate structures
            CHECK_CUDA(nvjpegCreateSimple(&m_handle), NVJPEG_STATUS_SUCCESS);
            CHECK_CUDA(cudaStreamCreate(&m_stream), cudaSuccess);
            CHECK_CUDA(nvjpegJpegStateCreate(m_handle, &m_state), NVJPEG_STATUS_SUCCESS);

            m_valid = true;
        }
        catch(const std::exception& e)
        {
            // Log error
            SIGHT_ERROR(e.what());

            // Free already allocated resources...
            free();
        }
    }

    /// Destructor
    inline ~nv_jpeg_reader() noexcept
    {
        free();
    }

    /// Reading
    inline void read(data::image& _image, std::istream& _istream, flag /*flag*/)
    {
        // Get input size
        _istream.seekg(0, std::ios::end);
        const auto stream_size = _istream.tellg();
        _istream.seekg(0, std::ios::beg);

        SIGHT_THROW_IF("The stream cannot be read.", stream_size <= 0);

        // Allocate input buffer
        const std::size_t input_buffer_size = std::size_t(stream_size);
        if(m_input_buffer.size() < input_buffer_size)
        {
            m_input_buffer.resize(input_buffer_size);
        }

        // Read input data..
        _istream.read(reinterpret_cast<char*>(m_input_buffer.data()), stream_size);

        // Decode JPEG metadata
        const auto& [num_components, subsampling, width, height] =
            [&]
            {
                int components                        = 0;
                nvjpegChromaSubsampling_t subsampling = NVJPEG_CSS_UNKNOWN;
                int widths[NVJPEG_MAX_COMPONENT] {};
                int heights[NVJPEG_MAX_COMPONENT] {};

                CHECK_CUDA(
                    nvjpegGetImageInfo(
                        m_handle,
                        m_input_buffer.data(),
                        input_buffer_size,
                        &components,
                        &subsampling,
                        widths,
                        heights
                    ),
                    NVJPEG_STATUS_SUCCESS
                );

                return std::make_tuple(
                    std::size_t(components),
                    subsampling,
                    std::size_t(widths[0]),
                    std::size_t(heights[0])
                );
            }();

        // Allocate GPU buffer
        const std::size_t size_in_bytes = width * height * num_components * sizeof(std::uint8_t);

        if(m_gpu_buffer_size < size_in_bytes)
        {
            if(m_gpu_buffer != nullptr)
            {
                CHECK_CUDA(cudaFree(m_gpu_buffer), cudaSuccess);
            }

            CHECK_CUDA(cudaMalloc(&m_gpu_buffer, size_in_bytes), cudaSuccess);
            m_gpu_buffer_size = size_in_bytes;
        }

        // Fill nvjpegImage_t structure
        nvjpegImage_t nv_image {};
        nv_image.channel[0] = reinterpret_cast<unsigned char*>(m_gpu_buffer);
        nv_image.pitch[0]   = width * num_components;

        // Decode JPEG
        CHECK_CUDA(
            nvjpegDecode(
                m_handle,
                m_state,
                m_input_buffer.data(),
                input_buffer_size,
                subsampling == NVJPEG_CSS_GRAY ? NVJPEG_OUTPUT_UNCHANGED : NVJPEG_OUTPUT_RGBI,
                &nv_image,
                m_stream
            ),
            NVJPEG_STATUS_SUCCESS
        );

        // Allocate destination image
        _image.resize(
            {width, height, 0},
            core::type::UINT8,
            data::image::pixel_format::rgb
        );

        // Synchronize CUDA streams
        CHECK_CUDA(cudaStreamSynchronize(m_stream), cudaSuccess);

        // Copy GPU memory so we can convert to planar there
        CHECK_CUDA(
            cudaMemcpy(
                _image.buffer(),
                m_gpu_buffer,
                size_in_bytes,
                cudaMemcpyDeviceToHost
            ),
            cudaSuccess
        );
    }

private:

    //------------------------------------------------------------------------------

    inline void free() noexcept
    {
        if(m_gpu_buffer != nullptr)
        {
            CHECK_CUDA_NOEXCEPT(cudaFree(m_gpu_buffer), cudaSuccess);
            m_gpu_buffer      = nullptr;
            m_gpu_buffer_size = 0;
        }

        if(m_state != nullptr)
        {
            CHECK_CUDA_NOEXCEPT(nvjpegJpegStateDestroy(m_state), NVJPEG_STATUS_SUCCESS);
            m_state = nullptr;
        }

        if(m_stream != nullptr)
        {
            CHECK_CUDA_NOEXCEPT(cudaStreamDestroy(m_stream), cudaSuccess);
            m_stream = nullptr;
        }

        if(m_handle != nullptr)
        {
            CHECK_CUDA_NOEXCEPT(nvjpegDestroy(m_handle), NVJPEG_STATUS_SUCCESS);
            m_handle = nullptr;
        }

        m_valid = false;
    }

    cudaStream_t m_stream {nullptr};
    nvjpegHandle_t m_handle {nullptr};
    nvjpegJpegState_t m_state {nullptr};

    void* m_gpu_buffer {nullptr};
    std::size_t m_gpu_buffer_size {0};

    std::vector<unsigned char> m_input_buffer;

public:

    bool m_valid {false};
    static constexpr std::string_view m_name {"NvJPEGReader"};
};

} // namespace sight::io::bitmap::detail
