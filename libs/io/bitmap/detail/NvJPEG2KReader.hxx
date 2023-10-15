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

#include "ReaderImpl.hxx"

#include <nvjpeg2k.h>

// cspell:ignore nvjpeg nvjpeg2k nppi bitstream LRCP NOLINTNEXTLINE

namespace sight::io::bitmap::detail
{

class NvJPEG2KReader final
{
public:

    /// Delete copy constructors and assignment operators
    NvJPEG2KReader(const NvJPEG2KReader&)            = delete;
    NvJPEG2KReader& operator=(const NvJPEG2KReader&) = delete;

    /// Constructor
    inline NvJPEG2KReader() noexcept
    {
        try
        {
            // Allocate structures
            CHECK_CUDA(nvjpeg2kCreateSimple(&m_handle), NVJPEG2K_STATUS_SUCCESS);
            CHECK_CUDA(nvjpeg2kDecodeStateCreate(m_handle, &m_state), NVJPEG2K_STATUS_SUCCESS);
            CHECK_CUDA(nvjpeg2kStreamCreate(&m_stream), NVJPEG2K_STATUS_SUCCESS);
            CHECK_CUDA(cudaStreamCreate(&m_cuda_stream), cudaSuccess);
            CHECK_CUDA(nvjpeg2kDecodeParamsCreate(&m_params), NVJPEG2K_STATUS_SUCCESS);

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
    inline ~NvJPEG2KReader() noexcept
    {
        free();
    }

    /// Reading
    inline void read(data::image& image, std::istream& istream, Flag /*flag*/)
    {
        // Get input size
        istream.seekg(0, std::ios::end);
        const auto stream_size = istream.tellg();
        istream.seekg(0, std::ios::beg);

        SIGHT_THROW_IF("The stream cannot be read.", stream_size <= 0);

        // Allocate input buffer
        const std::size_t input_buffer_size = std::size_t(stream_size);
        if(m_input_buffer.size() < input_buffer_size)
        {
            m_input_buffer.resize(input_buffer_size);
        }

        // Read input data..
        istream.read(reinterpret_cast<char*>(m_input_buffer.data()), stream_size);

        // Initialize JPEG2000 stream
        CHECK_CUDA(
            nvjpeg2kStreamParse(
                m_handle,
                m_input_buffer.data(),
                input_buffer_size,
                0,
                0,
                m_stream
            ),
            NVJPEG2K_STATUS_SUCCESS
        );

        // Get image info
        nvjpeg2kImageInfo_t image_info {};
        CHECK_CUDA(nvjpeg2kStreamGetImageInfo(m_stream, &image_info), NVJPEG2K_STATUS_SUCCESS);

        // Get components info
        std::vector<nvjpeg2kImageComponentInfo_t> components_info(image_info.num_components);

        for(std::uint32_t i = 0 ; i < image_info.num_components ; ++i)
        {
            CHECK_CUDA(
                nvjpeg2kStreamGetImageComponentInfo(m_stream, &components_info[i], i),
                NVJPEG2K_STATUS_SUCCESS
            );
        }

        // Images with more than one component are assumed to be RGB
        CHECK_CUDA(
            nvjpeg2kDecodeParamsSetRGBOutput(
                m_params,
                image_info.num_components == 0 ? 0 : 1
            ),
            NVJPEG2K_STATUS_SUCCESS
        );

        // Allocate planar buffers and pitches
        std::vector<std::size_t> pitches_in_bytes(image_info.num_components);

        if(m_planar_gpu_buffers.size() < image_info.num_components)
        {
            m_planar_gpu_buffers.resize(image_info.num_components, nullptr);
            m_planar_sizes.resize(image_info.num_components, 0);
        }

        for(std::uint32_t i = 0 ; i < image_info.num_components ; ++i)
        {
            const auto& component_info = components_info[i];
            const auto new_planar_size = component_info.component_width * component_info.component_height
                                         * component_info.precision / 8;

            // If the needed pitch is bigger than the allocated one, reallocate
            if(m_planar_sizes[i] < new_planar_size)
            {
                auto& buffer = m_planar_gpu_buffers[i];

                if(buffer != nullptr)
                {
                    CHECK_CUDA(cudaFree(buffer), cudaSuccess);
                }

                CHECK_CUDA(cudaMalloc(&buffer, new_planar_size), cudaSuccess);
                m_planar_sizes[i] = new_planar_size;
            }

            pitches_in_bytes[i] = component_info.component_width * component_info.precision / 8;
        }

        // Fill nvjpeg2kImage structure
        nvjpeg2kImage_t output_image;

        output_image.num_components = image_info.num_components;
        output_image.pixel_data     = m_planar_gpu_buffers.data();
        output_image.pixel_type     = components_info[0].precision > 8 ? NVJPEG2K_UINT16 : NVJPEG2K_UINT8;
        output_image.pitch_in_bytes = pitches_in_bytes.data();

        // Decode JPEG2000 stream
        CHECK_CUDA(
            nvjpeg2kDecodeImage(m_handle, m_state, m_stream, m_params, &output_image, m_cuda_stream),
            NVJPEG2K_STATUS_SUCCESS
        );

        // Allocate destination image
        image.resize(
            {image_info.image_width, image_info.image_height, 0},
            output_image.pixel_type == NVJPEG2K_UINT16
            ? core::type::UINT16
            : core::type::UINT8,
            image_info.num_components == 2
            ? data::image::PixelFormat::RG
            : image_info.num_components == 3
            ? data::image::PixelFormat::RGB
            : image_info.num_components == 4
            ? data::image::PixelFormat::RGBA
            : data::image::PixelFormat::GRAY_SCALE
        );

        // Synchronize CUDA streams
        CHECK_CUDA(cudaStreamSynchronize(m_cuda_stream), cudaSuccess);

        // Convert planar to interleaved image
        if(image_info.num_components == 1)
        {
            // Copy from GPU memory
            CHECK_CUDA(
                cudaMemcpy(
                    image.buffer(),
                    m_planar_gpu_buffers[0],
                    image.getSizeInBytes(),
                    cudaMemcpyDeviceToHost
                ),
                cudaSuccess
            );
        }
        else if(image_info.num_components == 3 || image_info.num_components == 4)
        {
            // Use nppi to convert planar to interleaved
            // Realloc if GPU packed buffer is smaller
            const std::size_t new_packed_size = image.getSizeInBytes();
            if(m_packed_size < new_packed_size)
            {
                if(m_packed_gpu_buffer != nullptr)
                {
                    CHECK_CUDA(cudaFree(m_packed_gpu_buffer), cudaSuccess);
                }

                CHECK_CUDA(cudaMalloc(&m_packed_gpu_buffer, new_packed_size), cudaSuccess);
                m_packed_size = new_packed_size;
            }

            const NppiSize nppi_size {.width = int(image_info.image_width), .height = int(image_info.image_height)};

            if(output_image.pixel_type == NVJPEG2K_UINT8)
            {
                auto* out_buffer  = reinterpret_cast<Npp8u*>(m_packed_gpu_buffer);
                const int in_step = int(image_info.image_width) * int(sizeof(Npp8u));

                if(image_info.num_components == 3)
                {
                    const std::array in_buffer {
                        reinterpret_cast<const Npp8u*>(m_planar_gpu_buffers[0]),
                        reinterpret_cast<const Npp8u*>(m_planar_gpu_buffers[1]),
                        reinterpret_cast<const Npp8u*>(m_planar_gpu_buffers[2])
                    };

                    CHECK_CUDA(
                        nppiCopy_8u_P3C3R(
                            in_buffer.data(),
                            in_step,
                            out_buffer,
                            in_step * int(image_info.num_components),
                            nppi_size
                        ),
                        NPP_SUCCESS
                    );
                }
                else
                {
                    const std::array in_buffer {
                        reinterpret_cast<const Npp8u*>(m_planar_gpu_buffers[0]),
                        reinterpret_cast<const Npp8u*>(m_planar_gpu_buffers[1]),
                        reinterpret_cast<const Npp8u*>(m_planar_gpu_buffers[2]),
                        reinterpret_cast<const Npp8u*>(m_planar_gpu_buffers[3])
                    };

                    CHECK_CUDA(
                        nppiCopy_8u_P4C4R(
                            in_buffer.data(),
                            in_step,
                            out_buffer,
                            in_step * int(image_info.num_components),
                            nppi_size
                        ),
                        NPP_SUCCESS
                    );
                }
            }
            else
            {
                auto* out_buffer  = reinterpret_cast<Npp16u*>(m_packed_gpu_buffer);
                const int in_step = int(image_info.image_width) * int(sizeof(Npp16u));

                if(image_info.num_components == 3)
                {
                    const std::array in_buffer {
                        reinterpret_cast<const Npp16u*>(m_planar_gpu_buffers[0]),
                        reinterpret_cast<const Npp16u*>(m_planar_gpu_buffers[1]),
                        reinterpret_cast<const Npp16u*>(m_planar_gpu_buffers[2])
                    };

                    CHECK_CUDA(
                        nppiCopy_16u_P3C3R(
                            in_buffer.data(),
                            in_step,
                            out_buffer,
                            in_step * int(image_info.num_components),
                            nppi_size
                        ),
                        NPP_SUCCESS
                    );
                }
                else
                {
                    const std::array in_buffer {
                        reinterpret_cast<const Npp16u*>(m_planar_gpu_buffers[0]),
                        reinterpret_cast<const Npp16u*>(m_planar_gpu_buffers[1]),
                        reinterpret_cast<const Npp16u*>(m_planar_gpu_buffers[2]),
                        reinterpret_cast<const Npp16u*>(m_planar_gpu_buffers[3])
                    };

                    CHECK_CUDA(
                        nppiCopy_16u_P4C4R(
                            in_buffer.data(),
                            in_step,
                            out_buffer,
                            in_step * int(image_info.num_components),
                            nppi_size
                        ),
                        NPP_SUCCESS
                    );
                }
            }

            // Copy from GPU memory
            CHECK_CUDA(
                cudaMemcpy(
                    image.buffer(),
                    m_packed_gpu_buffer,
                    image.getSizeInBytes(),
                    cudaMemcpyDeviceToHost
                ),
                cudaSuccess
            );
        }
        else
        {
            SIGHT_THROW("Unsupported number of components: " << image_info.num_components);
        }

        // Synchronize CUDA streams
        CHECK_CUDA(cudaDeviceSynchronize(), cudaSuccess);
    }

private:

    //------------------------------------------------------------------------------

    inline void free() noexcept
    {
        for(auto& buffer : m_planar_gpu_buffers)
        {
            if(buffer != nullptr)
            {
                CHECK_CUDA_NOEXCEPT(cudaFree(buffer), cudaSuccess);
                buffer = nullptr;
            }
        }

        m_planar_gpu_buffers.clear();
        m_planar_sizes.clear();

        if(m_packed_gpu_buffer != nullptr)
        {
            CHECK_CUDA_NOEXCEPT(cudaFree(m_packed_gpu_buffer), cudaSuccess);
            m_packed_gpu_buffer = nullptr;
            m_packed_size       = 0;
        }

        if(m_params != nullptr)
        {
            CHECK_CUDA_NOEXCEPT(nvjpeg2kDecodeParamsDestroy(m_params), NVJPEG2K_STATUS_SUCCESS);
            m_params = nullptr;
        }

        if(m_cuda_stream != nullptr)
        {
            CHECK_CUDA_NOEXCEPT(cudaStreamDestroy(m_cuda_stream), cudaSuccess);
            m_cuda_stream = nullptr;
        }

        if(m_stream != nullptr)
        {
            CHECK_CUDA_NOEXCEPT(nvjpeg2kStreamDestroy(m_stream), NVJPEG2K_STATUS_SUCCESS);
            m_stream = nullptr;
        }

        if(m_state != nullptr)
        {
            CHECK_CUDA_NOEXCEPT(nvjpeg2kDecodeStateDestroy(m_state), NVJPEG2K_STATUS_SUCCESS);
            m_state = nullptr;
        }

        if(m_handle != nullptr)
        {
            CHECK_CUDA_NOEXCEPT(nvjpeg2kDestroy(m_handle), NVJPEG2K_STATUS_SUCCESS);
            m_handle = nullptr;
        }

        m_valid = false;
    }

    std::vector<unsigned char> m_input_buffer;

    nvjpeg2kHandle_t m_handle {nullptr};
    nvjpeg2kDecodeState_t m_state {nullptr};
    nvjpeg2kStream_t m_stream {nullptr};
    cudaStream_t m_cuda_stream {nullptr};
    nvjpeg2kDecodeParams_t m_params {nullptr};

    std::vector<void*> m_planar_gpu_buffers;
    std::vector<std::size_t> m_planar_sizes;

    void* m_packed_gpu_buffer {nullptr};
    std::size_t m_packed_size {0};

public:

    bool m_valid {false};
    static constexpr std::string_view m_name {"NvJPEG2KReader"};
};

} // namespace sight::io::bitmap::detail
