/************************************************************************
 *
 * Copyright (C) 2014-2018 IRCAD France
 * Copyright (C) 2014-2018 IHU Strasbourg
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

#include "fwVideoQt/helper/formats.hpp"

#include <boost/assign.hpp>

namespace fwVideoQt
{
namespace helper
{

// The format of the code is shitty, that's because of uncrustify who doesn't allow us to write a pair per line.
PixelFormatTranslatorType pixelFormatTranslator = ::boost::assign::list_of< PixelFormatTranslatorType::relation >
                                                      (QVideoFrame::PixelFormat::Format_Invalid,
                                                      ::arData::Camera::PixelFormat::INVALID               )
                                                      (QVideoFrame::PixelFormat::Format_ARGB32,
                                                      ::arData::Camera::PixelFormat::ARGB32                )
                                                      (QVideoFrame::PixelFormat::Format_ARGB32_Premultiplied,
                                                      ::arData::Camera::PixelFormat::ARGB32_PREMULTIPLIED  )
                                                      (QVideoFrame::PixelFormat::Format_RGB32,
                                                      ::arData::Camera::PixelFormat::RGB32                 )
                                                      (QVideoFrame::PixelFormat::Format_RGB24,
                                                      ::arData::Camera::PixelFormat::RGB24                 )
                                                      (QVideoFrame::PixelFormat::Format_RGB565,
                                                      ::arData::Camera::PixelFormat::RGB565                )
                                                      (QVideoFrame::PixelFormat::Format_RGB555,
                                                      ::arData::Camera::PixelFormat::RGB555                )
                                                      (QVideoFrame::PixelFormat::Format_ARGB8565_Premultiplied,
                                                      ::arData::Camera::PixelFormat::ARGB8565_PREMULTIPLIED)
                                                      (QVideoFrame::PixelFormat::Format_BGRA32,
                                                      ::arData::Camera::PixelFormat::BGRA32                )
                                                      (QVideoFrame::PixelFormat::Format_BGRA32_Premultiplied,
                                                      ::arData::Camera::PixelFormat::BGRA32_PREMULTIPLIED  )
                                                      (QVideoFrame::PixelFormat::Format_BGR32,
                                                      ::arData::Camera::PixelFormat::BGR32                 )
                                                      (QVideoFrame::PixelFormat::Format_BGR24,
                                                      ::arData::Camera::PixelFormat::BGR24                 )
                                                      (QVideoFrame::PixelFormat::Format_BGR565,
                                                      ::arData::Camera::PixelFormat::BGR565                )
                                                      (QVideoFrame::PixelFormat::Format_BGR555,
                                                      ::arData::Camera::PixelFormat::BGR555                )
                                                      (QVideoFrame::PixelFormat::Format_BGRA5658_Premultiplied,
                                                      ::arData::Camera::PixelFormat::BGRA5658_PREMULTIPLIED)
                                                      (QVideoFrame::PixelFormat::Format_AYUV444,
                                                      ::arData::Camera::PixelFormat::AYUV444               )
                                                      (QVideoFrame::PixelFormat::Format_AYUV444_Premultiplied,
                                                      ::arData::Camera::PixelFormat::AYUV444_PREMULTIPLIED )
                                                      (QVideoFrame::PixelFormat::Format_YUV444,
                                                      ::arData::Camera::PixelFormat::YUV444                )
                                                      (QVideoFrame::PixelFormat::Format_YUV420P,
                                                      ::arData::Camera::PixelFormat::YUV420P               )
                                                      (QVideoFrame::PixelFormat::Format_YV12,
                                                      ::arData::Camera::PixelFormat::YV12                  )
                                                      (QVideoFrame::PixelFormat::Format_UYVY,
                                                      ::arData::Camera::PixelFormat::UYVY                  )
                                                      (QVideoFrame::PixelFormat::Format_YUYV,
                                                      ::arData::Camera::PixelFormat::YUYV                  )
                                                      (QVideoFrame::PixelFormat::Format_NV12,
                                                      ::arData::Camera::PixelFormat::NV12                  )
                                                      (QVideoFrame::PixelFormat::Format_NV21,
                                                      ::arData::Camera::PixelFormat::NV21                  )
                                                      (QVideoFrame::PixelFormat::Format_IMC1,
                                                      ::arData::Camera::PixelFormat::IMC1                  )
                                                      (QVideoFrame::PixelFormat::Format_IMC2,
                                                      ::arData::Camera::PixelFormat::IMC2                  )
                                                      (QVideoFrame::PixelFormat::Format_IMC3,
                                                      ::arData::Camera::PixelFormat::IMC3                  )
                                                      (QVideoFrame::PixelFormat::Format_IMC4,
                                                      ::arData::Camera::PixelFormat::IMC4                  )
                                                      (QVideoFrame::PixelFormat::Format_Y8,
                                                      ::arData::Camera::PixelFormat::Y8                    )
                                                      (QVideoFrame::PixelFormat::Format_Y16,
                                                      ::arData::Camera::PixelFormat::Y16                   )
                                                      (QVideoFrame::PixelFormat::Format_Jpeg,
                                                      ::arData::Camera::PixelFormat::JPEG                  )
                                                      (QVideoFrame::PixelFormat::Format_CameraRaw,
                                                      ::arData::Camera::PixelFormat::CAMERARAW             )
                                                      (QVideoFrame::PixelFormat::Format_AdobeDng,
                                                      ::arData::Camera::PixelFormat::ADOBEDNG              )
                                                      (QVideoFrame::PixelFormat::Format_User,
                                                      ::arData::Camera::PixelFormat::USER                  );

} // helper
} // fwVideoQt
