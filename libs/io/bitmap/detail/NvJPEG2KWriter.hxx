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

#include <nvjpeg2k.h>

// cspell:ignore nvjpeg nvjpeg2k nppi bitstream LRCP NOLINTNEXTLINE

namespace sight::io::bitmap::detail
{

class NvJPEG2KWriter final
{
public:

    /// Delete copy constructors and assignment operators
    NvJPEG2KWriter(const NvJPEG2KWriter&)            = delete;
    NvJPEG2KWriter& operator=(const NvJPEG2KWriter&) = delete;

    /// Constructor
    inline NvJPEG2KWriter() noexcept
    {
        try
        {
            // Allocate structures
            CHECK_CUDA(nvjpeg2kEncoderCreateSimple(&m_handle), NVJPEG2K_STATUS_SUCCESS);
            CHECK_CUDA(nvjpeg2kEncodeStateCreate(m_handle, &m_state), NVJPEG2K_STATUS_SUCCESS);
            CHECK_CUDA(nvjpeg2kEncodeParamsCreate(&m_params), NVJPEG2K_STATUS_SUCCESS);
            CHECK_CUDA(cudaStreamCreate(&m_stream), cudaSuccess);

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
    inline ~NvJPEG2KWriter() noexcept
    {
        free();
    }

    /// Writing
    template<
        typename O,
        std::enable_if_t<
            std::is_base_of_v<std::ostream, O>
            || std::is_same_v<std::uint8_t*, O>
            || std::is_same_v<std::uint8_t**, O>
            || std::is_same_v<std::vector<uint8_t>, O>,
            bool
        > = true
    >
    inline std::size_t write(
        const data::image& image,
        O& output,
        Writer::Mode mode,
        Flag flag = Flag::NONE
)
    {
        const auto& pixel_type = image.getType();
        SIGHT_THROW_IF(
            m_name << " - Unsupported image type: " << pixel_type,
            pixel_type != core::type::UINT8
            && pixel_type != core::type::UINT16
        );

        const auto& pixel_format = image.getPixelFormat();
        SIGHT_THROW_IF(
            m_name << " - Unsupported image format: " << pixel_format,
            pixel_format != data::image::PixelFormat::GRAY_SCALE
            && pixel_format != data::image::PixelFormat::RGB
            && pixel_format != data::image::PixelFormat::RGBA
            && pixel_format != data::image::PixelFormat::BGR
            && pixel_format != data::image::PixelFormat::BGRA
        );

        /// @todo Check new version of nvjpeg2k (>0.6).
        /// No idea why the decoding fails with unsigned 16 bits and 4 components images, we mark this as unsupported.
        SIGHT_THROW_IF(
            m_name << " - Unsupported format (" << pixel_format << ") and type (" << pixel_type << ") combination",
            (pixel_format == data::image::PixelFormat::RGBA || pixel_format == data::image::PixelFormat::BGRA)
            && pixel_type == core::type::UINT16
        );

        // Copy the image to the GPU and make it planar
        toGPU(image);

        // Fill nvjpeg2kImageComponentInfo_t
        const auto& sizes          = image.size();
        const auto& num_components = image.numComponents();
        std::vector<nvjpeg2kImageComponentInfo_t> components_info(num_components);
        for(auto& component_info : components_info)
        {
            component_info.component_width  = std::uint32_t(sizes[0]);
            component_info.component_height = std::uint32_t(sizes[1]);
            component_info.precision        = std::uint8_t(pixel_type.size() * 8);
            component_info.sgn              = pixel_type.is_signed() ? 1 : 0;
        }

        // Fill component pitches
        const std::size_t pitch_in_bytes = sizes[0] * pixel_type.size();
        std::vector<size_t> pitches_in_bytes(num_components);
        for(auto& pitch : pitches_in_bytes)
        {
            pitch = pitch_in_bytes;
        }

        // Fill nvjpeg2kImage_t
        // Use the gpu planar buffers
        nvjpeg2kImage_t input_image {
            .pixel_data     = m_planar_gpu_buffers.data(),
            .pitch_in_bytes = pitches_in_bytes.data(),
            .pixel_type     = pixel_type == core::type::INT16 || pixel_type == core::type::UINT16
                              ? NVJPEG2K_UINT16
                              : NVJPEG2K_UINT8,
            .num_components = std::uint32_t(num_components)
        };

        // Fill nvjpeg2kEncodeConfig_t
        nvjpeg2kEncodeConfig_t encode_config {};

        // The bitstream will be in J2K format
        // JP2 have xml meta data which are unsupported for DICOM
        encode_config.stream_type = flag == Flag::J2K_STREAM
                                    ? NVJPEG2K_STREAM_J2K
                                    : NVJPEG2K_STREAM_JP2;

        // Only one component means grayscale
        encode_config.color_space =
            pixel_format == data::image::GRAY_SCALE
            ? NVJPEG2K_COLORSPACE_GRAY
            : NVJPEG2K_COLORSPACE_SRGB;

        // Image configuration
        encode_config.image_width     = std::uint32_t(sizes[0]);
        encode_config.image_height    = std::uint32_t(sizes[1]);
        encode_config.num_components  = std::uint32_t(num_components);
        encode_config.image_comp_info = components_info.data();

        // Code block size: 64*64 (a bit better compression), 32*32 (almost 2x faster) - in lossless mode
        switch(mode)
        {
            case Writer::Mode::BEST:
                encode_config.code_block_w = 64;
                encode_config.code_block_h = 64;
                break;

            default:
                encode_config.code_block_w = 32;
                encode_config.code_block_h = 32;
                break;
        }

        // 0 (lossless), 1 (lossy)
        encode_config.irreversible = 0;

        // 0 (YCC and Grayscale), 1 (RGB)
        encode_config.mct_mode = num_components == 1 ? 0 : 1;

        // Progression orders defined in the JPEG2000 standard. LRCP is the only one supported for encoding for now
        encode_config.prog_order = NVJPEG2K_LRCP;

        // Wavelet decomposition levels. 6-5 Seems to be a good default
        // 1 is the fastest, but with an huge compression penalty (+20% size) - in lossless mode
        encode_config.num_resolutions = std::min(
            std::uint32_t(6),
            std::min(
                std::uint32_t(sizes[0]) / encode_config.code_block_w,
                std::uint32_t(sizes[1]) / encode_config.code_block_h
            )
        );

        CHECK_CUDA(
            nvjpeg2kEncodeParamsSetEncodeConfig(
                m_params,
                &encode_config
            ),
            NVJPEG2K_STATUS_SUCCESS
        );

        // Finally encode the image
        CHECK_CUDA(
            nvjpeg2kEncode(
                m_handle,
                m_state,
                m_params,
                &input_image,
                m_stream
            ),
            NVJPEG2K_STATUS_SUCCESS
        );

        // Get the GPU encoded buffer size
        std::size_t encoded_size = 0;
        CHECK_CUDA(
            nvjpeg2kEncodeRetrieveBitstream(
                m_handle,
                m_state,
                nullptr,
                &encoded_size,
                m_stream
            ),
            NVJPEG2K_STATUS_SUCCESS
        );

        CHECK_CUDA(cudaStreamSynchronize(m_stream), cudaSuccess);

        if constexpr(std::is_base_of_v<std::ostream, O>)
        {
            if(m_output_buffer.size() < encoded_size)
            {
                m_output_buffer.resize(encoded_size);
            }

            // Retrieve the buffer from GPU
            CHECK_CUDA(
                nvjpeg2kEncodeRetrieveBitstream(
                    m_handle,
                    m_state,
                    m_output_buffer.data(),
                    &encoded_size,
                    m_stream
                ),
                NVJPEG2K_STATUS_SUCCESS
            );

            CHECK_CUDA(cudaStreamSynchronize(m_stream), cudaSuccess);

            // Write to disk...
            output.write(reinterpret_cast<char*>(m_output_buffer.data()), std::streamsize(encoded_size));
        }
        else if constexpr(std::is_same_v<std::uint8_t**, O>)
        {
            (*output) = new std::uint8_t[encoded_size];

            // Retrieve the buffer from GPU
            CHECK_CUDA(
                nvjpeg2kEncodeRetrieveBitstream(
                    m_handle,
                    m_state,
                    (*output),
                    &encoded_size,
                    m_stream
                ),
                NVJPEG2K_STATUS_SUCCESS
            );

            CHECK_CUDA(cudaStreamSynchronize(m_stream), cudaSuccess);
        }
        else if constexpr(std::is_same_v<std::uint8_t*, O>)
        {
            // Retrieve the buffer from GPU
            CHECK_CUDA(
                nvjpeg2kEncodeRetrieveBitstream(
                    m_handle,
                    m_state,
                    output,
                    &encoded_size,
                    m_stream
                ),
                NVJPEG2K_STATUS_SUCCESS
            );

            CHECK_CUDA(cudaStreamSynchronize(m_stream), cudaSuccess);
        }
        else if constexpr(std::is_same_v<std::vector<std::uint8_t>, O>)
        {
            if(output.size() < encoded_size)
            {
                output.resize(encoded_size);
            }

            // Retrieve the buffer from GPU
            CHECK_CUDA(
                nvjpeg2kEncodeRetrieveBitstream(
                    m_handle,
                    m_state,
                    output.data(),
                    &encoded_size,
                    m_stream
                ),
                NVJPEG2K_STATUS_SUCCESS
            );

            CHECK_CUDA(cudaStreamSynchronize(m_stream), cudaSuccess);
        }
        else
        {
            SIGHT_THROW("No output stream or buffer provided.");
        }

        return encoded_size;
    }

private:

    /// Copy an image packed data to planar in the GPU
    inline void toGPU(const data::image& image)
    {
        const auto size_in_bytes = image.getSizeInBytes();

        // Realloc if GPU buffer is smaller
        if(m_packed_gpu_buffer_size < size_in_bytes)
        {
            // Source packed buffer
            if(m_packed_gpu_buffer != nullptr)
            {
                CHECK_CUDA(cudaFree(m_packed_gpu_buffer), cudaSuccess);
            }

            CHECK_CUDA(cudaMalloc(&m_packed_gpu_buffer, size_in_bytes), cudaSuccess);
            m_packed_gpu_buffer_size = size_in_bytes;
        }

        // Copy source data to GPU memory so we can convert to planar there
        CHECK_CUDA(
            cudaMemcpy(
                m_packed_gpu_buffer,
                image.buffer(),
                size_in_bytes,
                cudaMemcpyHostToDevice
            ),
            cudaSuccess
        );

        if(const auto& num_components = image.numComponents(); num_components == 1)
        {
            // No need to unpack buffer with only 1 component !
            auto& buffer = m_planar_gpu_buffers[0];
            if(buffer != nullptr)
            {
                CHECK_CUDA(cudaFree(buffer), cudaSuccess);
            }

            // Simply reuse the already allocated area
            buffer                   = m_packed_gpu_buffer;
            m_planar_gpu_buffer_size = size_in_bytes;

            // Avoid double free
            m_packed_gpu_buffer      = nullptr;
            m_packed_gpu_buffer_size = 0;
        }
        else if(num_components == 3 || num_components == 4)
        {
            const auto pixel_format = image.getPixelFormat();

            // Realloc if GPU planar buffers are smaller
            const std::size_t new_planar_size = size_in_bytes / num_components;
            if(m_planar_gpu_buffer_size < new_planar_size)
            {
                for(std::size_t i = 0 ; i < num_components ; ++i)
                {
                    auto& buffer = m_planar_gpu_buffers[i];

                    if(buffer != nullptr)
                    {
                        CHECK_CUDA(cudaFree(buffer), cudaSuccess);
                    }

                    CHECK_CUDA(cudaMalloc(&buffer, new_planar_size), cudaSuccess);
                }

                m_planar_gpu_buffer_size = new_planar_size;
            }

            const auto& type  = image.getType();
            const auto& sizes = image.size();
            const int width   = int(sizes[0]);
            const int height  = int(sizes[1]);
            const NppiSize nppi_size {.width = width, .height = height};

            if(type == core::type::INT8 || type == core::type::UINT8)
            {
                const auto* in_buffer = reinterpret_cast<const Npp8u*>(m_packed_gpu_buffer);
                const int out_step    = width * int(sizeof(Npp8u));

                if(num_components == 3)
                {
                    if(pixel_format == data::image::PixelFormat::RGB)
                    {
                        std::array out_buffer {
                            reinterpret_cast<Npp8u*>(m_planar_gpu_buffers[0]),
                            reinterpret_cast<Npp8u*>(m_planar_gpu_buffers[1]),
                            reinterpret_cast<Npp8u*>(m_planar_gpu_buffers[2])
                        };

                        CHECK_CUDA(
                            nppiCopy_8u_C3P3R(
                                in_buffer,
                                out_step * int(num_components),
                                out_buffer.data(),
                                out_step,
                                nppi_size
                            ),
                            NPP_SUCCESS
                        );
                    }
                    else if(pixel_format == data::image::PixelFormat::BGR)
                    {
                        std::array out_buffer {
                            reinterpret_cast<Npp8u*>(m_planar_gpu_buffers[2]),
                            reinterpret_cast<Npp8u*>(m_planar_gpu_buffers[1]),
                            reinterpret_cast<Npp8u*>(m_planar_gpu_buffers[0])
                        };

                        CHECK_CUDA(
                            nppiCopy_8u_C3P3R(
                                in_buffer,
                                out_step * int(num_components),
                                out_buffer.data(),
                                out_step,
                                nppi_size
                            ),
                            NPP_SUCCESS
                        );
                    }
                    else
                    {
                        SIGHT_THROW("Unsupported pixel format for 3 components image");
                    }
                }
                else if(num_components == 4)
                {
                    if(pixel_format == data::image::PixelFormat::RGBA)
                    {
                        std::array out_buffer {
                            reinterpret_cast<Npp8u*>(m_planar_gpu_buffers[0]),
                            reinterpret_cast<Npp8u*>(m_planar_gpu_buffers[1]),
                            reinterpret_cast<Npp8u*>(m_planar_gpu_buffers[2]),
                            reinterpret_cast<Npp8u*>(m_planar_gpu_buffers[3])
                        };

                        CHECK_CUDA(
                            nppiCopy_8u_C4P4R(
                                in_buffer,
                                out_step * int(num_components),
                                out_buffer.data(),
                                out_step,
                                nppi_size
                            ),
                            NPP_SUCCESS
                        );
                    }
                    else if(pixel_format == data::image::PixelFormat::BGRA)
                    {
                        std::array out_buffer {
                            reinterpret_cast<Npp8u*>(m_planar_gpu_buffers[2]),
                            reinterpret_cast<Npp8u*>(m_planar_gpu_buffers[1]),
                            reinterpret_cast<Npp8u*>(m_planar_gpu_buffers[0]),
                            reinterpret_cast<Npp8u*>(m_planar_gpu_buffers[3])
                        };

                        CHECK_CUDA(
                            nppiCopy_8u_C4P4R(
                                in_buffer,
                                out_step * int(num_components),
                                out_buffer.data(),
                                out_step,
                                nppi_size
                            ),
                            NPP_SUCCESS
                        );
                    }
                    else
                    {
                        SIGHT_THROW("Unsupported pixel format for 4 components image");
                    }
                }
            }
            else if(type == core::type::UINT16)
            {
                const auto* in_buffer = reinterpret_cast<const Npp16u*>(m_packed_gpu_buffer);
                const int out_step    = width * int(sizeof(Npp16u));

                if(num_components == 3)
                {
                    if(pixel_format == data::image::PixelFormat::RGB)
                    {
                        std::array out_buffer {
                            reinterpret_cast<Npp16u*>(m_planar_gpu_buffers[0]),
                            reinterpret_cast<Npp16u*>(m_planar_gpu_buffers[1]),
                            reinterpret_cast<Npp16u*>(m_planar_gpu_buffers[2])
                        };

                        CHECK_CUDA(
                            nppiCopy_16u_C3P3R(
                                in_buffer,
                                out_step * int(num_components),
                                out_buffer.data(),
                                out_step,
                                nppi_size
                            ),
                            NPP_SUCCESS
                        );
                    }
                    else if(pixel_format == data::image::PixelFormat::BGR)
                    {
                        std::array out_buffer {
                            reinterpret_cast<Npp16u*>(m_planar_gpu_buffers[2]),
                            reinterpret_cast<Npp16u*>(m_planar_gpu_buffers[1]),
                            reinterpret_cast<Npp16u*>(m_planar_gpu_buffers[0])
                        };

                        CHECK_CUDA(
                            nppiCopy_16u_C3P3R(
                                in_buffer,
                                out_step * int(num_components),
                                out_buffer.data(),
                                out_step,
                                nppi_size
                            ),
                            NPP_SUCCESS
                        );
                    }
                    else
                    {
                        SIGHT_THROW("Unsupported pixel format for 3 components image");
                    }
                }
                else
                {
                    if(pixel_format == data::image::PixelFormat::RGBA)
                    {
                        std::array out_buffer {
                            reinterpret_cast<Npp16u*>(m_planar_gpu_buffers[0]),
                            reinterpret_cast<Npp16u*>(m_planar_gpu_buffers[1]),
                            reinterpret_cast<Npp16u*>(m_planar_gpu_buffers[2]),
                            reinterpret_cast<Npp16u*>(m_planar_gpu_buffers[3])
                        };

                        CHECK_CUDA(
                            nppiCopy_16u_C4P4R(
                                in_buffer,
                                out_step * int(num_components),
                                out_buffer.data(),
                                out_step,
                                nppi_size
                            ),
                            NPP_SUCCESS
                        );
                    }
                    else if(pixel_format == data::image::PixelFormat::BGRA)
                    {
                        std::array out_buffer {
                            reinterpret_cast<Npp16u*>(m_planar_gpu_buffers[2]),
                            reinterpret_cast<Npp16u*>(m_planar_gpu_buffers[1]),
                            reinterpret_cast<Npp16u*>(m_planar_gpu_buffers[0]),
                            reinterpret_cast<Npp16u*>(m_planar_gpu_buffers[3])
                        };

                        CHECK_CUDA(
                            nppiCopy_16u_C4P4R(
                                in_buffer,
                                out_step * int(num_components),
                                out_buffer.data(),
                                out_step,
                                nppi_size
                            ),
                            NPP_SUCCESS
                        );
                    }
                    else
                    {
                        SIGHT_THROW("Unsupported pixel format for 4 components image");
                    }
                }
            }
            else if(type == core::type::INT16)
            {
                const auto* in_buffer = reinterpret_cast<const Npp16s*>(m_packed_gpu_buffer);
                const int out_step    = width * int(sizeof(Npp16s));

                if(num_components == 3)
                {
                    if(pixel_format == data::image::PixelFormat::RGB)
                    {
                        std::array out_buffer {
                            reinterpret_cast<Npp16s*>(m_planar_gpu_buffers[0]),
                            reinterpret_cast<Npp16s*>(m_planar_gpu_buffers[1]),
                            reinterpret_cast<Npp16s*>(m_planar_gpu_buffers[2])
                        };

                        CHECK_CUDA(
                            nppiCopy_16s_C3P3R(
                                in_buffer,
                                out_step * int(num_components),
                                out_buffer.data(),
                                out_step,
                                nppi_size
                            ),
                            NPP_SUCCESS
                        );
                    }
                    else if(pixel_format == data::image::PixelFormat::BGR)
                    {
                        std::array out_buffer {
                            reinterpret_cast<Npp16s*>(m_planar_gpu_buffers[2]),
                            reinterpret_cast<Npp16s*>(m_planar_gpu_buffers[1]),
                            reinterpret_cast<Npp16s*>(m_planar_gpu_buffers[0])
                        };

                        CHECK_CUDA(
                            nppiCopy_16s_C3P3R(
                                in_buffer,
                                out_step * int(num_components),
                                out_buffer.data(),
                                out_step,
                                nppi_size
                            ),
                            NPP_SUCCESS
                        );
                    }
                    else
                    {
                        SIGHT_THROW("Unsupported pixel format for 3 components image");
                    }
                }
                else
                {
                    if(pixel_format == data::image::PixelFormat::RGBA)
                    {
                        std::array out_buffer {
                            reinterpret_cast<Npp16s*>(m_planar_gpu_buffers[0]),
                            reinterpret_cast<Npp16s*>(m_planar_gpu_buffers[1]),
                            reinterpret_cast<Npp16s*>(m_planar_gpu_buffers[2]),
                            reinterpret_cast<Npp16s*>(m_planar_gpu_buffers[3])
                        };

                        CHECK_CUDA(
                            nppiCopy_16s_C4P4R(
                                in_buffer,
                                out_step * int(num_components),
                                out_buffer.data(),
                                out_step,
                                nppi_size
                            ),
                            NPP_SUCCESS
                        );
                    }
                    else if(pixel_format == data::image::PixelFormat::BGRA)
                    {
                        std::array out_buffer {
                            reinterpret_cast<Npp16s*>(m_planar_gpu_buffers[2]),
                            reinterpret_cast<Npp16s*>(m_planar_gpu_buffers[1]),
                            reinterpret_cast<Npp16s*>(m_planar_gpu_buffers[0]),
                            reinterpret_cast<Npp16s*>(m_planar_gpu_buffers[3])
                        };

                        CHECK_CUDA(
                            nppiCopy_16s_C4P4R(
                                in_buffer,
                                out_step * int(num_components),
                                out_buffer.data(),
                                out_step,
                                nppi_size
                            ),
                            NPP_SUCCESS
                        );
                    }
                    else
                    {
                        SIGHT_THROW("Unsupported pixel format for 4 components image");
                    }
                }
            }
            else
            {
                SIGHT_THROW("Unsupported image type: " << type.name());
            }
        }
        else if(num_components != 1)
        {
            SIGHT_THROW("Unsupported number of component: " << num_components);
        }
    }

    //------------------------------------------------------------------------------

    inline void free() noexcept
    {
        if(m_packed_gpu_buffer != nullptr)
        {
            CHECK_CUDA_NOEXCEPT(cudaFree(m_packed_gpu_buffer), cudaSuccess);
            m_packed_gpu_buffer      = nullptr;
            m_packed_gpu_buffer_size = 0;
        }

        for(auto& buffer : m_planar_gpu_buffers)
        {
            if(buffer != nullptr)
            {
                CHECK_CUDA_NOEXCEPT(cudaFree(buffer), cudaSuccess);
                buffer = nullptr;
            }

            m_planar_gpu_buffer_size = 0;
        }

        if(m_stream != nullptr)
        {
            CHECK_CUDA_NOEXCEPT(cudaStreamDestroy(m_stream), cudaSuccess);
            m_stream = nullptr;
        }

        if(m_params != nullptr)
        {
            CHECK_CUDA_NOEXCEPT(nvjpeg2kEncodeParamsDestroy(m_params), NVJPEG2K_STATUS_SUCCESS);
            m_params = nullptr;
        }

        if(m_state != nullptr)
        {
            CHECK_CUDA_NOEXCEPT(nvjpeg2kEncodeStateDestroy(m_state), NVJPEG2K_STATUS_SUCCESS);
            m_state = nullptr;
        }

        if(m_handle != nullptr)
        {
            CHECK_CUDA_NOEXCEPT(nvjpeg2kEncoderDestroy(m_handle), NVJPEG2K_STATUS_SUCCESS);
            m_handle = nullptr;
        }

        m_valid = false;
    }

    cudaStream_t m_stream {nullptr};
    nvjpeg2kEncoder_t m_handle {nullptr};
    nvjpeg2kEncodeState_t m_state {nullptr};
    nvjpeg2kEncodeParams_t m_params {nullptr};

    void* m_packed_gpu_buffer {nullptr};
    std::size_t m_packed_gpu_buffer_size {0};

    std::array<void*, 4> m_planar_gpu_buffers {nullptr, nullptr, nullptr, nullptr};
    std::size_t m_planar_gpu_buffer_size {0};

    std::vector<unsigned char> m_output_buffer;

public:

    bool m_valid {false};
    static constexpr std::string_view m_name {"NvJPEG2KWriter"};
};

} // namespace sight::io::bitmap::detail
