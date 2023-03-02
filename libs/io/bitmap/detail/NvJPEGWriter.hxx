/************************************************************************
 *
 * Copyright (C) 2023 IRCAD France
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

#include "WriterImpl.hxx"

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

class NvJPEGWriter final
{
public:

    /// Delete copy constructors and assignment operators
    NvJPEGWriter(const NvJPEGWriter&)            = delete;
    NvJPEGWriter& operator=(const NvJPEGWriter&) = delete;

    /// Constructor
    inline NvJPEGWriter() noexcept
    {
        try
        {
            // Allocate structures
            CHECK_CUDA(nvjpegCreateSimple(&m_handle), NVJPEG_STATUS_SUCCESS);
            CHECK_CUDA(cudaStreamCreate(&m_stream), cudaSuccess);
            CHECK_CUDA(nvjpegEncoderStateCreate(m_handle, &m_state, m_stream), NVJPEG_STATUS_SUCCESS);

            // Create the encoder parameters
            CHECK_CUDA(nvjpegEncoderParamsCreate(m_handle, &m_params, m_stream), NVJPEG_STATUS_SUCCESS);

            // Set the sampling factors parameter
            CHECK_CUDA(
                nvjpegEncoderParamsSetSamplingFactors(m_params, NVJPEG_CSS_444, m_stream),
                NVJPEG_STATUS_SUCCESS
            );

            // Set quality to 100 (best)
            CHECK_CUDA(nvjpegEncoderParamsSetQuality(m_params, 100, m_stream), NVJPEG_STATUS_SUCCESS);

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
    inline ~NvJPEGWriter() noexcept
    {
        free();
    }

    /// Writing
    inline void write(const data::Image& image, std::ostream& ostream, Writer::Mode mode, Flag = Flag::NONE)
    {
        const auto& pixel_format = image.getPixelFormat();
        SIGHT_THROW_IF(
            m_name << " - Unsupported image pixel format: " << pixel_format,
            pixel_format != data::Image::PixelFormat::RGB && pixel_format != data::Image::PixelFormat::BGR
        );

        const auto& pixel_type = image.getType();
        SIGHT_THROW_IF(
            m_name << " - Unsupported image type: " << pixel_type,
            pixel_type != core::Type::UINT8
        );

        // Optimize Huffman, normally slower but smaller file size (1)
        CHECK_CUDA(
            nvjpegEncoderParamsSetOptimizedHuffman(
                m_params,
                mode == Writer::Mode::BEST ? 1 : 0,
                m_stream
            ),
            NVJPEG_STATUS_SUCCESS
        );

        // Realloc if GPU buffer is smaller
        // Beware, some images are volume...
        const auto& sizes = image.getSize();
        const auto num_components = image.numComponents();
        const auto size_in_bytes = sizes[0] * sizes[1] * num_components* pixel_type.size();
        if(m_gpu_buffer_size < size_in_bytes)
        {
            if(m_gpu_buffer != nullptr)
            {
                CHECK_CUDA(cudaFree(m_gpu_buffer), cudaSuccess);
            }

            CHECK_CUDA(cudaMalloc(&m_gpu_buffer, size_in_bytes), cudaSuccess);
            m_gpu_buffer_size = size_in_bytes;
        }

        // Copy the source image buffer to the GPU
        CHECK_CUDA(
            cudaMemcpy(
                m_gpu_buffer,
                image.getBuffer(),
                size_in_bytes,
                cudaMemcpyHostToDevice
            ),
            cudaSuccess
        );

        // Prepare the input buffer
        nvjpegImage_t nv_image {};
        nv_image.channel[0] = reinterpret_cast<unsigned char*>(m_gpu_buffer);
        nv_image.pitch[0]   = sizes[0] * num_components;

        // Encode the buffer
        CHECK_CUDA(
            nvjpegEncodeImage(
                m_handle,
                m_state,
                m_params,
                &nv_image,
                pixel_format == data::Image::PixelFormat::RGB ? NVJPEG_INPUT_RGBI : NVJPEG_INPUT_BGRI,
                int(sizes[0]),
                int(sizes[1]),
                m_stream
            ),
            NVJPEG_STATUS_SUCCESS
        );

        // Get the GPU encoded buffer size
        std::size_t encoded_size = 0;
        CHECK_CUDA(
            nvjpegEncodeRetrieveBitstream(
                m_handle,
                m_state,
                nullptr,
                &encoded_size,
                m_stream
            ),
            NVJPEG_STATUS_SUCCESS
        );

        // Retrieve the buffer from GPU
        CHECK_CUDA(cudaStreamSynchronize(m_stream), cudaSuccess);

        if(m_output_buffer.size() < encoded_size)
        {
            m_output_buffer.resize(encoded_size);
        }

        CHECK_CUDA(
            nvjpegEncodeRetrieveBitstream(
                m_handle,
                m_state,
                m_output_buffer.data(),
                &encoded_size,
                m_stream
            ),
            NVJPEG_STATUS_SUCCESS
        );

        CHECK_CUDA(cudaStreamSynchronize(m_stream), cudaSuccess);

        // Write to disk...
        ostream.write(reinterpret_cast<char*>(m_output_buffer.data()), std::streamsize(encoded_size));
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

        if(m_params != nullptr)
        {
            CHECK_CUDA_NOEXCEPT(nvjpegEncoderParamsDestroy(m_params), NVJPEG_STATUS_SUCCESS);
            m_params = nullptr;
        }

        if(m_state != nullptr)
        {
            CHECK_CUDA_NOEXCEPT(nvjpegEncoderStateDestroy(m_state), NVJPEG_STATUS_SUCCESS);
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
    nvjpegEncoderState_t m_state {nullptr};
    nvjpegEncoderParams_t m_params {nullptr};

    void* m_gpu_buffer {nullptr};
    std::size_t m_gpu_buffer_size {0};

    std::vector<unsigned char> m_output_buffer;

public:

    bool m_valid {false};
    static constexpr std::string_view m_name {"NvJPEGWriter"};
};

} // namespace sight::io::bitmap::detail
