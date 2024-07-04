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

#include "fiducials_series.hpp"

#include "data/dicom/sop.hpp"

#include "detail/series_impl.hxx"

#include <core/tools/uuid.hpp>

#include <data/helper/fiducials_series.hpp>

#include <gdcmSequenceOfItems.h>
#include <gdcmSmartPointer.h>
#include <gdcmTagKeywords.h>

#include <array>

namespace sight::data
{

namespace kw = gdcm::Keywords;

//------------------------------------------------------------------------------

namespace
{

//------------------------------------------------------------------------------

std::string shape_to_string(fiducials_series::shape _shape)
{
    switch(_shape)
    {
        case fiducials_series::shape::point:
            return "POINT";

        case fiducials_series::shape::line:
            return "LINE";

        case fiducials_series::shape::plane:
            return "PLANE";

        case fiducials_series::shape::surface:
            return "SURFACE";

        case fiducials_series::shape::ruler:
            return "RULER";

        case fiducials_series::shape::l_shape:
            return "L_SHAPE";

        case fiducials_series::shape::t_shape:
            return "T_SHAPE";

        case fiducials_series::shape::shape:
            return "SHAPE";

        default:
            SIGHT_WARN("Unknown shape " << static_cast<int>(_shape));
            return "";
    }
}

//------------------------------------------------------------------------------

std::optional<std::string> color_to_string(const std::optional<std::array<float, 4> >& _color)
{
    if(!_color.has_value())
    {
        return std::nullopt;
    }

    return std::to_string((*_color)[0]) + ',' + std::to_string((*_color)[1]) + ',' + std::to_string((*_color)[2]) + ','
           + std::to_string((*_color)[3]);
}

//------------------------------------------------------------------------------

std::optional<std::string> private_shape_to_string(const std::optional<fiducials_series::private_shape>& _private_shape)
{
    if(!_private_shape.has_value())
    {
        return std::nullopt;
    }

    switch(*_private_shape)
    {
        case fiducials_series::private_shape::sphere:
            return "SPHERE";

        case fiducials_series::private_shape::cube:
            return "CUBE";

        default:
            return std::nullopt;
    }
}

//------------------------------------------------------------------------------

std::vector<float> to_floats(const std::vector<fiducials_series::point2>& _points)
{
    std::vector<float> res;
    res.reserve(_points.size() * 2);
    for(const fiducials_series::point2& point : _points)
    {
        res.push_back(static_cast<float>(point.x));
        res.push_back(static_cast<float>(point.y));
    }

    return res;
}

//------------------------------------------------------------------------------

std::vector<double> to_floats(const std::vector<fiducials_series::point3>& _points)
{
    std::vector<double> res;
    res.reserve(_points.size() * 3);
    for(const fiducials_series::point3& point : _points)
    {
        res.push_back(point.x);
        res.push_back(point.y);
        res.push_back(point.z);
    }

    return res;
}

//------------------------------------------------------------------------------

gdcm::DataSet to_gdcm(detail::series_impl& _pimpl, fiducials_series::referenced_image _referenced_image)
{
    std::unique_lock lock(_pimpl.m_mutex);

    gdcm::DataSet data_set;
    _pimpl.set_string_value<kw::ReferencedSOPClassUID>(_referenced_image.referenced_sop_class_uid, data_set);
    _pimpl.set_string_value<kw::ReferencedSOPInstanceUID>(_referenced_image.referenced_sop_instance_uid, data_set);
    _pimpl.set_values<kw::ReferencedFrameNumber>(_referenced_image.referenced_frame_number, data_set);
    _pimpl.set_values<kw::ReferencedSegmentNumber>(_referenced_image.referenced_segment_number, data_set);
    return data_set;
}

//------------------------------------------------------------------------------

gdcm::DataSet to_gdcm(detail::series_impl& _pimpl, fiducials_series::graphic_coordinates_data _graphic_coordinates_data)
{
    std::unique_lock lock(_pimpl.m_mutex);

    gdcm::DataSet data_set;
    auto referenced_image_sequence = gdcm::SequenceOfItems::New();
    gdcm::Item item;
    item.SetNestedDataSet(to_gdcm(_pimpl, _graphic_coordinates_data.referenced_image_sequence));
    referenced_image_sequence->AddItem(item);
    _pimpl.set_sequence(kw::ReferencedImageSequence::GetTag(), referenced_image_sequence, data_set);
    _pimpl.set_values<kw::GraphicData>(to_floats(_graphic_coordinates_data.graphic_data), data_set);
    return data_set;
}

//------------------------------------------------------------------------------

gdcm::DataSet to_gdcm(detail::series_impl& _pimpl, fiducials_series::fiducial _fiducial)
{
    std::unique_lock lock(_pimpl.m_mutex);

    gdcm::DataSet data_set;
    _pimpl.set_string_value<kw::ShapeType>(shape_to_string(_fiducial.shape_type), data_set);
    _pimpl.set_string_value<kw::FiducialDescription>(_fiducial.fiducial_description, data_set);
    _pimpl.set_string_value<kw::FiducialIdentifier>(_fiducial.fiducial_identifier, data_set);
    if(_fiducial.graphic_coordinates_data_sequence)
    {
        auto gcds = gdcm::SequenceOfItems::New();
        for(const fiducials_series::graphic_coordinates_data& gcd : *_fiducial.graphic_coordinates_data_sequence)
        {
            gdcm::Item item;
            item.SetNestedDataSet(to_gdcm(_pimpl, gcd));
            gcds->AddItem(item);
        }

        _pimpl.set_sequence(kw::GraphicCoordinatesDataSequence::GetTag(), gcds, data_set);
    }

    _pimpl.set_string_value<kw::FiducialUID>(_fiducial.fiducial_uid.value_or(""), data_set);
    if(!_fiducial.contour_data.empty())
    {
        _pimpl.set_value<kw::NumberOfContourPoints>(static_cast<int>(_fiducial.contour_data.size()), data_set);
        _pimpl.set_values<kw::ContourData>(to_floats(_fiducial.contour_data), data_set);
    }

    return data_set;
}

//------------------------------------------------------------------------------

gdcm::DataSet to_gdcm(detail::series_impl& _pimpl, fiducials_series::fiducial_set _fiducial_set)
{
    std::unique_lock lock(_pimpl.m_mutex);

    gdcm::DataSet data_set;
    if(_fiducial_set.referenced_image_sequence)
    {
        auto referenced_image_sequence = gdcm::SequenceOfItems::New();
        for(const fiducials_series::referenced_image& referenced_image : *_fiducial_set.referenced_image_sequence)
        {
            gdcm::Item item;
            item.SetNestedDataSet(to_gdcm(_pimpl, referenced_image));
            referenced_image_sequence->AddItem(item);
        }

        _pimpl.set_sequence(kw::ReferencedImageSequence::GetTag(), referenced_image_sequence, data_set);
    }

    if(_fiducial_set.frame_of_reference_uid)
    {
        _pimpl.set_string_value<kw::FrameOfReferenceUID>(*_fiducial_set.frame_of_reference_uid, data_set);
    }

    auto fiducial_sequence = gdcm::SequenceOfItems::New();
    for(const fiducials_series::fiducial& fiducial : _fiducial_set.fiducial_sequence)
    {
        gdcm::Item item;
        item.SetNestedDataSet(to_gdcm(_pimpl, fiducial));
        fiducial_sequence->AddItem(item);
    }

    _pimpl.set_sequence(kw::FiducialSequence::GetTag(), fiducial_sequence, data_set);
    _pimpl.set_private_value(0, _fiducial_set.group_name, data_set);
    _pimpl.set_private_value(1, color_to_string(_fiducial_set.color), data_set);
    std::optional<std::string> size;
    if(_fiducial_set.size.has_value())
    {
        size = std::to_string(*_fiducial_set.size);
    }

    _pimpl.set_private_value(2, size, data_set);
    _pimpl.set_private_value(3, private_shape_to_string(_fiducial_set.shape), data_set);
    if(_fiducial_set.visibility.has_value())
    {
        _pimpl.set_private_value(4, *_fiducial_set.visibility ? "true" : "false", data_set);
    }

    return data_set;
}

//------------------------------------------------------------------------------

template<typename T>
gdcm::SmartPointer<gdcm::SequenceOfItems> append_in_sequence(
    detail::series_impl& _pimpl,
    const gdcm::SmartPointer<gdcm::SequenceOfItems>& _sequence,
    T _element
)
{
    auto res = _sequence == nullptr ? gdcm::SequenceOfItems::New() : _sequence;
    gdcm::Item item;
    item.SetNestedDataSet(to_gdcm(_pimpl, _element));
    res->AddItem(item);
    return res;
}

//------------------------------------------------------------------------------

template<typename T>
void append_in_sequence(
    detail::series_impl& _pimpl,
    gdcm::Tag _tag,
    T _element,
    const std::vector<std::pair<gdcm::Tag, std::size_t> >& _indices = {})
{
    _pimpl.set_sequence(_tag, append_in_sequence(_pimpl, _pimpl.get_sequence(_tag, 0, _indices), _element));
}

} // namespace

//------------------------------------------------------------------------------

void fiducials_series::shallow_copy(const object::csptr& _source)
{
    const auto& other = std::dynamic_pointer_cast<const fiducials_series>(_source);

    SIGHT_THROW_EXCEPTION_IF(
        exception(
            "Unable to copy " + (_source ? _source->get_classname() : std::string("<NULL>"))
            + " to " + get_classname()
        ),
        !bool(other)
    );

    base_class_t::shallow_copy(other);
}

//------------------------------------------------------------------------------

void fiducials_series::deep_copy(const object::csptr& _source, const std::unique_ptr<deep_copy_cache_t>& _cache)
{
    const auto& other = std::dynamic_pointer_cast<const fiducials_series>(_source);

    SIGHT_THROW_EXCEPTION_IF(
        exception(
            "Unable to copy " + (_source ? _source->get_classname() : std::string("<NULL>"))
            + " to " + get_classname()
        ),
        !bool(other)
    );

    base_class_t::deep_copy(other, _cache);
}

//------------------------------------------------------------------------------

bool fiducials_series::point2::operator==(point2 _other) const
{
    return x == _other.x && y == _other.y;
}

//------------------------------------------------------------------------------

bool fiducials_series::point3::operator==(point3 _other) const
{
    return x == _other.x && y == _other.y && z == _other.z;
}

//------------------------------------------------------------------------------

bool fiducials_series::referenced_image::operator==(const referenced_image& _other) const
{
    return referenced_sop_class_uid == _other.referenced_sop_class_uid
           && referenced_sop_instance_uid == _other.referenced_sop_instance_uid
           && referenced_frame_number == _other.referenced_frame_number
           && referenced_segment_number == _other.referenced_segment_number;
}

//------------------------------------------------------------------------------

bool fiducials_series::referenced_image::operator!=(const referenced_image& _other) const
{
    return !(*this == _other);
}

//------------------------------------------------------------------------------

bool fiducials_series::graphic_coordinates_data::operator==(const graphic_coordinates_data& _other) const
{
    return referenced_image_sequence == _other.referenced_image_sequence && graphic_data == _other.graphic_data;
}

//------------------------------------------------------------------------------

bool fiducials_series::graphic_coordinates_data::operator!=(const graphic_coordinates_data& _other) const
{
    return !(*this == _other);
}

//------------------------------------------------------------------------------

bool fiducials_series::fiducial::operator==(const fiducial& _other) const
{
    return shape_type == _other.shape_type && fiducial_description == _other.fiducial_description
           && fiducial_identifier == _other.fiducial_identifier
           && graphic_coordinates_data_sequence == _other.graphic_coordinates_data_sequence
           && fiducial_uid == _other.fiducial_uid
           && contour_data == _other.contour_data;
}

//------------------------------------------------------------------------------

bool fiducials_series::fiducial::operator!=(const fiducial& _other) const
{
    return !(*this == _other);
}

//------------------------------------------------------------------------------

bool fiducials_series::fiducial_set::operator==(const fiducial_set& _other) const
{
    return referenced_image_sequence == _other.referenced_image_sequence
           && frame_of_reference_uid == _other.frame_of_reference_uid
           && fiducial_sequence == _other.fiducial_sequence && group_name == _other.group_name;
}

fiducials_series::fiducials_series()
{
    set_sop_keyword(dicom::sop::Keyword::SpatialFiducialsStorage);
    set_modality(dicom::modality_t::fid);
}

//------------------------------------------------------------------------------

bool fiducials_series::operator==(const fiducials_series& _other) const
{
    return get_content_date() == _other.get_content_date() && get_content_label() == _other.get_content_label()
           && get_content_description() == _other.get_content_description()
           && get_content_creator_name() == _other.get_content_creator_name()
           && get_fiducial_sets() == _other.get_fiducial_sets()
           && base_class_t::operator==(_other);
}

//------------------------------------------------------------------------------

bool fiducials_series::operator!=(const fiducials_series& _other) const
{
    return !(*this == _other);
}

//------------------------------------------------------------------------------

std::string fiducials_series::get_content_date() const noexcept
{
    return m_pimpl->get_string_value<kw::ContentDate>();
}

//------------------------------------------------------------------------------

void fiducials_series::set_content_date(const std::string& _content_date)
{
    m_pimpl->set_string_value<kw::ContentDate>(_content_date);
}

//------------------------------------------------------------------------------

std::string fiducials_series::get_content_label() const noexcept
{
    return m_pimpl->get_string_value<kw::ContentLabel>();
}

//------------------------------------------------------------------------------

void fiducials_series::set_content_label(const std::string& _content_label)
{
    m_pimpl->set_string_value<kw::ContentLabel>(_content_label);
}

//------------------------------------------------------------------------------

std::string fiducials_series::get_content_description() const noexcept
{
    return m_pimpl->get_string_value<kw::ContentDescription>();
}

//------------------------------------------------------------------------------

void fiducials_series::set_content_description(const std::string& _content_description)
{
    m_pimpl->set_string_value<kw::ContentDescription>(_content_description);
}

//------------------------------------------------------------------------------

std::string fiducials_series::get_content_creator_name() const noexcept
{
    return m_pimpl->get_string_value<kw::ContentCreatorName>();
}

//------------------------------------------------------------------------------

void fiducials_series::set_content_creator_name(const std::string& _content_creator_name)
{
    m_pimpl->set_string_value<kw::ContentCreatorName>(_content_creator_name);
}

//------------------------------------------------------------------------------

std::vector<fiducials_series::fiducial_set> fiducials_series::get_fiducial_sets() const noexcept
{
    return to_vector<fiducial_set>(m_pimpl->get_sequence<kw::FiducialSetSequence>()).value_or(
        std::vector<fiducial_set> {
        });
}

//------------------------------------------------------------------------------

void fiducials_series::set_fiducial_sets(const std::vector<fiducial_set>& _fiducial_sets)
{
    m_pimpl->set_sequence(kw::FiducialSetSequence::GetTag(), to_sequence(_fiducial_sets));
}

//------------------------------------------------------------------------------

void fiducials_series::set_fiducial_set(std::size_t _fiducial_set_number, fiducial_set _fiducial_set)
{
    m_pimpl->get_or_create_data_set(0, {{kw::FiducialSetSequence::GetTag(), _fiducial_set_number}}) = to_gdcm(
        *m_pimpl,
        _fiducial_set
    );
}

//------------------------------------------------------------------------------

void fiducials_series::append_fiducial_set(fiducial_set _fiducial_set)
{
    append_in_sequence(*m_pimpl, kw::FiducialSetSequence::GetTag(), _fiducial_set);
}

//------------------------------------------------------------------------------

std::optional<std::vector<fiducials_series::referenced_image> > fiducials_series::get_referenced_images(
    std::size_t _fiducial_set_number
) const noexcept
{
    return to_vector<referenced_image>(
        m_pimpl->get_sequence(
            kw::ReferencedImageSequence::GetTag(),
            0,
            {{kw::FiducialSetSequence::GetTag(), _fiducial_set_number
            }
            })
    );
}

//------------------------------------------------------------------------------

void fiducials_series::set_referenced_images(
    std::size_t _fiducial_set_number,
    const std::optional<std::vector<referenced_image> >& _referenced_images
)
{
    m_pimpl->set_sequence(
        kw::ReferencedImageSequence::GetTag(),
        to_sequence(
            _referenced_images
        ),
        0,
        {{kw::FiducialSetSequence::GetTag(), _fiducial_set_number
        }
        });
}

//------------------------------------------------------------------------------

void fiducials_series::set_referenced_image(
    std::size_t _fiducial_set_number,
    std::size_t _referenced_image_number,
    referenced_image _referenced_image
)
{
    m_pimpl->get_or_create_data_set(
        0,
        {{kw::FiducialSetSequence::GetTag(), _fiducial_set_number
        }, {kw::ReferencedImageSequence::GetTag(), _referenced_image_number
            }
        }) = to_gdcm(*m_pimpl, _referenced_image);
}

//------------------------------------------------------------------------------

void fiducials_series::append_referenced_image(std::size_t _fiducial_set_number, referenced_image _referenced_image)
{
    append_in_sequence(
        *m_pimpl,
        kw::ReferencedImageSequence::GetTag(),
        _referenced_image,
        {{kw::FiducialSetSequence::GetTag(), _fiducial_set_number
        }
        });
}

//------------------------------------------------------------------------------

std::optional<std::string> fiducials_series::get_referenced_sop_class_uid(
    std::size_t _fiducial_set_number,
    std::size_t _referenced_image_number
) const noexcept
{
    return m_pimpl->get_value<kw::ReferencedSOPClassUID>(
        0,
        {{kw::FiducialSetSequence::GetTag(), _fiducial_set_number
        }, {kw::ReferencedImageSequence::GetTag(), _referenced_image_number
            }
        });
}

//------------------------------------------------------------------------------

void fiducials_series::set_referenced_sop_class_uid(
    std::size_t _fiducial_set_number,
    std::size_t _referenced_image_number,
    const std::string& _referenced_sop_class_uid
)
{
    m_pimpl->set_string_value<kw::ReferencedSOPClassUID>(
        _referenced_sop_class_uid,
        0,
        {{kw::FiducialSetSequence::GetTag(), _fiducial_set_number
        }, {kw::ReferencedImageSequence::GetTag(), _referenced_image_number
            }
        });
}

//------------------------------------------------------------------------------

std::optional<std::string> fiducials_series::get_referenced_sop_instance_uid(
    std::size_t _fiducial_set_number,
    std::size_t _referenced_image_number
) const noexcept
{
    return m_pimpl->get_value<kw::ReferencedSOPInstanceUID>(
        0,
        {{kw::FiducialSetSequence::GetTag(), _fiducial_set_number
        },
            {kw::ReferencedImageSequence::GetTag(), _referenced_image_number
            }
        });
}

//------------------------------------------------------------------------------

void fiducials_series::set_referenced_sop_instance_uid(
    std::size_t _fiducial_set_number,
    std::size_t _referenced_image_number,
    const std::string& _referenced_sop_instance_uid
)
{
    m_pimpl->set_string_value<kw::ReferencedSOPInstanceUID>(
        _referenced_sop_instance_uid,
        0,
        {{kw::FiducialSetSequence::GetTag(), _fiducial_set_number
        },
            {kw::ReferencedImageSequence::GetTag(), _referenced_image_number
            }
        });
}

//------------------------------------------------------------------------------

std::vector<std::int32_t> fiducials_series::get_referenced_frame_number(
    std::size_t _fiducial_set_number,
    std::size_t _referenced_image_number
) const noexcept
{
    return m_pimpl->get_values<kw::ReferencedFrameNumber>(
        0,
        {{kw::FiducialSetSequence::GetTag(), _fiducial_set_number
        },
            {kw::ReferencedImageSequence::GetTag(), _referenced_image_number
            }
        }).value_or(std::vector<std::int32_t> {});
}

//------------------------------------------------------------------------------

void fiducials_series::set_referenced_frame_number(
    std::size_t _fiducial_set_number,
    std::size_t _referenced_image_number,
    std::vector<std::int32_t> _referenced_frame_number
)
{
    m_pimpl->set_values<kw::ReferencedFrameNumber>(
        _referenced_frame_number,
        0,
        {{kw::FiducialSetSequence::GetTag(), _fiducial_set_number
        }, {kw::ReferencedImageSequence::GetTag(), _referenced_image_number
            }
        });
}

//------------------------------------------------------------------------------

std::vector<std::uint16_t> fiducials_series::get_referenced_segment_number(
    std::size_t _fiducial_set_number,
    std::size_t _referenced_image_number
) const noexcept
{
    return m_pimpl->get_values<kw::ReferencedSegmentNumber>(
        0,
        {{kw::FiducialSetSequence::GetTag(), _fiducial_set_number
        },
            {kw::ReferencedImageSequence::GetTag(), _referenced_image_number
            }
        }).value_or(std::vector<std::uint16_t> {});
}

//------------------------------------------------------------------------------

void fiducials_series::set_referenced_segment_number(
    std::size_t _fiducial_set_number,
    std::size_t _referenced_image_number,
    std::vector<std::uint16_t> _referenced_segment_number
)
{
    m_pimpl->set_values<kw::ReferencedSegmentNumber>(
        _referenced_segment_number,
        0,
        {{kw::FiducialSetSequence::GetTag(), _fiducial_set_number
        }, {kw::ReferencedImageSequence::GetTag(), _referenced_image_number
            }
        });
}

//------------------------------------------------------------------------------

std::optional<std::string> fiducials_series::get_frame_of_reference_uid(std::size_t _fiducial_set_number) const noexcept
{
    return m_pimpl->get_value<kw::FrameOfReferenceUID>(
        0,
        {{kw::FiducialSetSequence::GetTag(), _fiducial_set_number
        }
        });
}

//------------------------------------------------------------------------------

void fiducials_series::set_frame_of_reference_uid(
    std::size_t _fiducial_set_number,
    const std::optional<std::string>& _frame_of_reference_uid
)
{
    m_pimpl->set_string_value<kw::FrameOfReferenceUID>(
        _frame_of_reference_uid.value_or(
            ""
        ),
        0,
        {{kw::FiducialSetSequence::GetTag(), _fiducial_set_number
        }
        });
}

//------------------------------------------------------------------------------

std::vector<fiducials_series::fiducial> fiducials_series::get_fiducials(std::size_t _fiducial_set_number) const noexcept
{
    return to_vector<fiducial>(
        m_pimpl->get_sequence(
            kw::FiducialSequence::GetTag(),
            0,
            {{kw::FiducialSetSequence::GetTag(), _fiducial_set_number
            }
            })
    ).value_or(std::vector<fiducial> {});
}

//------------------------------------------------------------------------------

std::vector<data::fiducials_series::fiducial> fiducials_series::filter_fiducials(
    const std::optional<data::fiducials_series::shape> _shape,
    const std::optional<std::int32_t> _referenced_frame_number
) const
{
    std::vector<data::fiducials_series::fiducial> fiducials;
    std::vector<data::fiducials_series::fiducial_set> fiducial_sets = this->get_fiducial_sets();

    for(auto& fiducial_set : fiducial_sets)
    {
        auto tmp = helper::fiducials_series::filter_fiducials(fiducial_set, _shape, _referenced_frame_number);
        std::copy(tmp.begin(), tmp.end(), std::back_inserter(fiducials));
    }

    return fiducials;
}

//------------------------------------------------------------------------------

void fiducials_series::set_fiducials(std::size_t _fiducial_set_number, const std::vector<fiducial>& _fiducials)
{
    m_pimpl->set_sequence(
        kw::FiducialSequence::GetTag(),
        to_sequence(_fiducials),
        0,
        {{kw::FiducialSetSequence::GetTag(), _fiducial_set_number
        }
        });
}

//------------------------------------------------------------------------------

void fiducials_series::set_fiducial(std::size_t _fiducial_set_number, std::size_t _fiducial_number, fiducial _fiducial)
{
    m_pimpl->get_or_create_data_set(
        0,
        {{kw::FiducialSetSequence::GetTag(), _fiducial_set_number
        }, {kw::FiducialSequence::GetTag(), _fiducial_number
            }
        }) = to_gdcm(*m_pimpl, _fiducial);
}

//------------------------------------------------------------------------------

void fiducials_series::append_fiducial(std::size_t _fiducial_set_number, fiducial _fiducial)
{
    append_in_sequence(
        *m_pimpl,
        kw::FiducialSequence::GetTag(),
        _fiducial,
        {{kw::FiducialSetSequence::GetTag(), _fiducial_set_number
        }
        });
}

//------------------------------------------------------------------------------

fiducials_series::shape fiducials_series::get_shape_type(
    std::size_t _fiducial_set_number,
    std::size_t _fiducial_number
) const noexcept
{
    return string_to_shape(
        m_pimpl->get_value<kw::ShapeType>(
            0,
            {{kw::FiducialSetSequence::GetTag(), _fiducial_set_number
            }, {kw::FiducialSequence::GetTag(), _fiducial_number
                }
            })
    );
}

//------------------------------------------------------------------------------

void fiducials_series::set_shape_type(
    std::size_t _fiducial_set_number,
    std::size_t _fiducial_number,
    fiducials_series::shape _shape_type
)
{
    m_pimpl->set_value<kw::ShapeType>(
        shape_to_string(_shape_type),
        0,
        {{kw::FiducialSetSequence::GetTag(), _fiducial_set_number
        }, {kw::FiducialSequence::GetTag(), _fiducial_number
            }
        });
}

//------------------------------------------------------------------------------

std::string fiducials_series::get_fiducial_description(
    std::size_t _fiducial_set_number,
    std::size_t _fiducial_number
) const noexcept
{
    return m_pimpl->get_value<kw::FiducialDescription>(
        0,
        {{kw::FiducialSetSequence::GetTag(), _fiducial_set_number
        }, {kw::FiducialSequence::GetTag(), _fiducial_number
            }
        }).value_or("");
}

//------------------------------------------------------------------------------

void fiducials_series::set_fiducial_description(
    std::size_t _fiducial_set_number,
    std::size_t _fiducial_number,
    const std::string& _fiducial_description
)
{
    m_pimpl->set_string_value<kw::FiducialDescription>(
        _fiducial_description,
        0,
        {{kw::FiducialSetSequence::GetTag(), _fiducial_set_number
        }, {kw::FiducialSequence::GetTag(), _fiducial_number
            }
        });
}

//------------------------------------------------------------------------------

std::string fiducials_series::get_fiducial_identifier(
    std::size_t _fiducial_set_number,
    std::size_t _fiducial_number
) const noexcept
{
    return m_pimpl->get_value<kw::FiducialIdentifier>(
        0,
        {{kw::FiducialSetSequence::GetTag(), _fiducial_set_number
        }, {kw::FiducialSequence::GetTag(), _fiducial_number
            }
        }).value_or("");
}

//------------------------------------------------------------------------------

void fiducials_series::set_fiducial_identifier(
    std::size_t _fiducial_set_number,
    std::size_t _fiducial_number,
    const std::string& _fiducial_identifier
)
{
    m_pimpl->set_string_value<kw::FiducialIdentifier>(
        _fiducial_identifier,
        0,
        {{kw::FiducialSetSequence::GetTag(), _fiducial_set_number
        }, {kw::FiducialSequence::GetTag(), _fiducial_number
            }
        });
}

//------------------------------------------------------------------------------

std::optional<std::vector<fiducials_series::graphic_coordinates_data> > fiducials_series::
get_graphic_coordinates_data_sequence(
    std::size_t _fiducial_set_number,
    std::size_t _fiducial_number
) const noexcept
{
    return to_vector<graphic_coordinates_data>(
        m_pimpl->get_sequence(
            kw::GraphicCoordinatesDataSequence::GetTag(),
            0,
            {{kw::FiducialSetSequence::GetTag(), _fiducial_set_number
            }, {kw::FiducialSequence::GetTag(), _fiducial_number
                }
            })
    );
}

//------------------------------------------------------------------------------

void fiducials_series::set_graphic_coordinates_data_sequence(
    std::size_t _fiducial_set_number,
    std::size_t _fiducial_number,
    const std::optional<std::vector<graphic_coordinates_data> >& _graphic_coordinates_data_sequence
)
{
    m_pimpl->set_sequence(
        kw::GraphicCoordinatesDataSequence::GetTag(),
        to_sequence(
            _graphic_coordinates_data_sequence
        ),
        0,
        {{kw::FiducialSetSequence::GetTag(), _fiducial_set_number
        }, {kw::FiducialSequence::GetTag(), _fiducial_number
            }
        });
}

//------------------------------------------------------------------------------

void fiducials_series::set_graphic_coordinates_data(
    std::size_t _fiducial_set_number,
    std::size_t _fiducial_number,
    std::size_t _graphic_coordinates_data_number,
    graphic_coordinates_data _graphic_coordinates_data
)
{
    m_pimpl->get_or_create_data_set(
        0,
        {{kw::FiducialSetSequence::GetTag(), _fiducial_set_number
        }, {kw::FiducialSequence::GetTag(), _fiducial_number
            }, {kw::GraphicCoordinatesDataSequence::GetTag(), _graphic_coordinates_data_number
            }
        }) = to_gdcm(*m_pimpl, _graphic_coordinates_data);
}

//------------------------------------------------------------------------------

void fiducials_series::append_graphic_coordinates_data(
    std::size_t _fiducial_set_number,
    std::size_t _fiducial_number,
    graphic_coordinates_data _graphic_coordinates_data
)
{
    append_in_sequence(
        *m_pimpl,
        kw::GraphicCoordinatesDataSequence::GetTag(),
        _graphic_coordinates_data,
        {{kw::FiducialSetSequence::GetTag(), _fiducial_set_number
        }, {kw::FiducialSequence::GetTag(), _fiducial_number
            }
        });
}

//------------------------------------------------------------------------------

fiducials_series::referenced_image fiducials_series::get_referenced_image(
    std::size_t _fiducial_set_number,
    std::size_t _fiducial_number,
    std::size_t _graphic_coordinates_data_number
) const noexcept
{
    std::optional<gdcm::DataSet> data_set = m_pimpl->get_data_set(
        0,
        {{kw::FiducialSetSequence::GetTag(), _fiducial_set_number
        }, {kw::FiducialSequence::GetTag(), _fiducial_number
            },
            {kw::GraphicCoordinatesDataSequence::GetTag(), _graphic_coordinates_data_number
            }, {kw::ReferencedImageSequence::GetTag(), 0
            }
        });
    if(!data_set)
    {
        return {};
    }

    return to_referenced_image(*data_set);
}

//------------------------------------------------------------------------------

void fiducials_series::set_referenced_image(
    std::size_t _fiducial_set_number,
    std::size_t _fiducial_number,
    std::size_t _graphic_coordinates_data_number,
    referenced_image _referenced_image
)
{
    m_pimpl->get_or_create_data_set(
        0,
        {{kw::FiducialSetSequence::GetTag(), _fiducial_set_number
        }, {kw::FiducialSequence::GetTag(), _fiducial_number
            }, {kw::GraphicCoordinatesDataSequence::GetTag(), _graphic_coordinates_data_number
            }, {kw::ReferencedImageSequence::GetTag(), 0
            }
        }) = to_gdcm(*m_pimpl, _referenced_image);
}

//------------------------------------------------------------------------------

std::string fiducials_series::get_referenced_sop_class_uid(
    std::size_t _fiducial_set_number,
    std::size_t _fiducial_number,
    std::size_t _graphic_coordinates_data_number
) const noexcept
{
    return m_pimpl->get_value<kw::ReferencedSOPClassUID>(
        0,
        {{kw::FiducialSetSequence::GetTag(), _fiducial_set_number
        }, {kw::FiducialSequence::GetTag(), _fiducial_number
            },
            {kw::GraphicCoordinatesDataSequence::GetTag(), _graphic_coordinates_data_number
            }, {kw::ReferencedImageSequence::GetTag(), 0
            }
        }).value_or("");
}

//------------------------------------------------------------------------------

void fiducials_series::set_referenced_sop_class_uid(
    std::size_t _fiducial_set_number,
    std::size_t _fiducial_number,
    std::size_t _graphic_coordinates_data_number,
    const std::string& _referenced_sop_class_uid
)
{
    m_pimpl->set_string_value<kw::ReferencedSOPClassUID>(
        _referenced_sop_class_uid,
        0,
        {{kw::FiducialSetSequence::GetTag(), _fiducial_set_number
        }, {kw::FiducialSequence::GetTag(), _fiducial_number
            },
            {kw::GraphicCoordinatesDataSequence::GetTag(), _graphic_coordinates_data_number
            }, {kw::ReferencedImageSequence::GetTag(), 0
            }
        });
}

//------------------------------------------------------------------------------

std::string fiducials_series::get_referenced_sop_instance_uid(
    std::size_t _fiducial_set_number,
    std::size_t _fiducial_number,
    std::size_t _graphic_coordinates_data_number
) const noexcept
{
    return m_pimpl->get_value<kw::ReferencedSOPInstanceUID>(
        0,
        {{kw::FiducialSetSequence::GetTag(), _fiducial_set_number
        }, {kw::FiducialSequence::GetTag(), _fiducial_number
            },
            {kw::GraphicCoordinatesDataSequence::GetTag(), _graphic_coordinates_data_number
            }, {kw::ReferencedImageSequence::GetTag(), 0
            }
        }).value_or("");
}

//------------------------------------------------------------------------------

void fiducials_series::set_referenced_sop_instance_uid(
    std::size_t _fiducial_set_number,
    std::size_t _fiducial_number,
    std::size_t _graphic_coordinates_data_number,
    const std::string& _referenced_sop_instance_uid
)
{
    m_pimpl->set_string_value<kw::ReferencedSOPInstanceUID>(
        _referenced_sop_instance_uid,
        0,
        {{kw::FiducialSetSequence::GetTag(), _fiducial_set_number
        }, {kw::FiducialSequence::GetTag(), _fiducial_number
            },
            {kw::GraphicCoordinatesDataSequence::GetTag(), _graphic_coordinates_data_number
            }, {kw::ReferencedImageSequence::GetTag(), 0
            }
        });
}

//------------------------------------------------------------------------------

std::vector<std::int32_t> fiducials_series::get_referenced_frame_number(
    std::size_t _fiducial_set_number,
    std::size_t _fiducial_number,
    std::size_t _graphic_coordinates_data_number
) const noexcept
{
    return m_pimpl->get_values<kw::ReferencedFrameNumber>(
        0,
        {{kw::FiducialSetSequence::GetTag(), _fiducial_set_number
        }, {kw::FiducialSequence::GetTag(), _fiducial_number
            },
            {kw::GraphicCoordinatesDataSequence::GetTag(), _graphic_coordinates_data_number
            }, {kw::ReferencedImageSequence::GetTag(), 0
            }
        }).value_or(std::vector<std::int32_t> {});
}

//------------------------------------------------------------------------------

void fiducials_series::set_referenced_frame_number(
    std::size_t _fiducial_set_number,
    std::size_t _fiducial_number,
    std::size_t _graphic_coordinates_data_number,
    std::vector<std::int32_t> _referenced_frame_number
)
{
    m_pimpl->set_values<kw::ReferencedFrameNumber>(
        _referenced_frame_number,
        0,
        {{kw::FiducialSetSequence::GetTag(), _fiducial_set_number
        }, {kw::FiducialSequence::GetTag(), _fiducial_number
            },
            {kw::GraphicCoordinatesDataSequence::GetTag(), _graphic_coordinates_data_number
            }, {kw::ReferencedImageSequence::GetTag(), 0
            }
        });
}

//------------------------------------------------------------------------------

std::vector<std::uint16_t> fiducials_series::get_referenced_segment_number(
    std::size_t _fiducial_set_number,
    std::size_t _fiducial_number,
    std::size_t _graphic_coordinates_data_number
) const noexcept
{
    return m_pimpl->get_values<kw::ReferencedSegmentNumber>(
        0,
        {{kw::FiducialSetSequence::GetTag(), _fiducial_set_number
        }, {kw::FiducialSequence::GetTag(), _fiducial_number
            },
            {kw::GraphicCoordinatesDataSequence::GetTag(), _graphic_coordinates_data_number
            }, {kw::ReferencedImageSequence::GetTag(), 0
            }
        }).value_or(std::vector<std::uint16_t> {});
}

//------------------------------------------------------------------------------

void fiducials_series::set_referenced_segment_number(
    std::size_t _fiducial_set_number,
    std::size_t _fiducial_number,
    std::size_t _graphic_coordinates_data_number,
    std::vector<std::uint16_t> _referenced_segment_number
)
{
    m_pimpl->set_values<kw::ReferencedSegmentNumber>(
        _referenced_segment_number,
        0,
        {{kw::FiducialSetSequence::GetTag(), _fiducial_set_number
        }, {kw::FiducialSequence::GetTag(), _fiducial_number
            },
            {kw::GraphicCoordinatesDataSequence::GetTag(), _graphic_coordinates_data_number
            }, {kw::ReferencedImageSequence::GetTag(), 0
            }
        });
}

//------------------------------------------------------------------------------

std::vector<fiducials_series::point2> fiducials_series::get_graphic_data(
    std::size_t _fiducial_set_number,
    std::size_t _fiducial_number,
    std::size_t _graphic_coordinates_data_number
) const noexcept
{
    std::optional<std::vector<float> > graphic_data = m_pimpl->get_values<kw::GraphicData>(
        0,
        {{kw::FiducialSetSequence::GetTag(), _fiducial_set_number
        },
            {kw::FiducialSequence::GetTag(), _fiducial_number
            },
            {kw::GraphicCoordinatesDataSequence::GetTag(), _graphic_coordinates_data_number
            }
        });
    if(!graphic_data)
    {
        return {};
    }

    return to_point2(*graphic_data);
}

//------------------------------------------------------------------------------

void fiducials_series::set_graphic_data(
    std::size_t _fiducial_set_number,
    std::size_t _fiducial_number,
    std::size_t _graphic_coordinates_data_number,
    const std::vector<fiducials_series::point2>& _graphic_data
)
{
    m_pimpl->set_values<kw::GraphicData>(
        to_floats(_graphic_data),
        0,
        {{kw::FiducialSetSequence::GetTag(), _fiducial_set_number
        }, {kw::FiducialSequence::GetTag(), _fiducial_number
            },
            {kw::GraphicCoordinatesDataSequence::GetTag(), _graphic_coordinates_data_number
            }
        });
}

//------------------------------------------------------------------------------

std::optional<std::string> fiducials_series::get_fiducial_uid(
    std::size_t _fiducial_set_number,
    std::size_t _fiducial_number
) const noexcept
{
    return m_pimpl->get_value<kw::FiducialUID>(
        0,
        {{kw::FiducialSetSequence::GetTag(), _fiducial_set_number
        }, {kw::FiducialSequence::GetTag(), _fiducial_number
            }
        });
}

//------------------------------------------------------------------------------

void fiducials_series::set_fiducial_uid(
    std::size_t _fiducial_set_number,
    std::size_t _fiducial_number,
    const std::optional<std::string>& _fiducial_uid
)
{
    m_pimpl->set_string_value<kw::FiducialUID>(
        _fiducial_uid.value_or(""),
        0,
        {{kw::FiducialSetSequence::GetTag(), _fiducial_set_number
        }, {kw::FiducialSequence::GetTag(), _fiducial_number
            }
        });
}

//------------------------------------------------------------------------------

std::vector<fiducials_series::point3> fiducials_series::get_contour_data(
    std::size_t _fiducial_set_number,
    std::size_t _fiducial_number
) const noexcept
{
    std::optional<std::vector<double> > contour_data = m_pimpl->get_values<kw::ContourData>(
        0,
        {{kw::FiducialSetSequence::GetTag(), _fiducial_set_number
        },
            {kw::FiducialSequence::GetTag(), _fiducial_number
            }
        });
    if(!contour_data)
    {
        return {};
    }

    return to_point3(*contour_data);
}

//------------------------------------------------------------------------------

void fiducials_series::set_contour_data(
    std::size_t _fiducial_set_number,
    std::size_t _fiducial_number,
    const std::vector<fiducials_series::point3>& _contour_data
)
{
    m_pimpl->set_values<kw::ContourData>(
        to_floats(_contour_data),
        0,
        {{kw::FiducialSetSequence::GetTag(), _fiducial_set_number
        }, {kw::FiducialSequence::GetTag(), _fiducial_number
            }
        });
}

//------------------------------------------------------------------------------

std::optional<std::string> fiducials_series::get_group_name(std::size_t _fiducial_set_number) const noexcept
{
    return m_pimpl->get_private_value(0, 0, {{kw::FiducialSetSequence::GetTag(), _fiducial_set_number}});
}

//------------------------------------------------------------------------------

void fiducials_series::set_group_name(std::size_t _fiducial_set_number, const std::string& _group_name)
{
    m_pimpl->set_private_value(0, _group_name, 0, {{kw::FiducialSetSequence::GetTag(), _fiducial_set_number}});
}

//------------------------------------------------------------------------------

std::optional<std::array<float, 4> > fiducials_series::get_color(std::size_t _fiducial_set_number) const noexcept
{
    return string_to_color(
        m_pimpl->get_private_value(
            1,
            0,
            {{kw::FiducialSetSequence::GetTag(), _fiducial_set_number
            }
            })
    );
}

//------------------------------------------------------------------------------

void fiducials_series::set_color(std::size_t _fiducial_set_number, const std::array<float, 4>& _color)
{
    m_pimpl->set_private_value(
        1,
        color_to_string(_color),
        0,
        {{kw::FiducialSetSequence::GetTag(), _fiducial_set_number
        }
        });
}

//------------------------------------------------------------------------------

std::optional<float> fiducials_series::get_size(std::size_t _fiducial_set_number) const noexcept
{
    std::optional<std::string> private_value = m_pimpl->get_private_value(
        2,
        0,
        {{kw::FiducialSetSequence::GetTag(), _fiducial_set_number
        }
        });
    if(!private_value.has_value())
    {
        return std::nullopt;
    }

    return std::stof(*private_value);
}

//------------------------------------------------------------------------------

void fiducials_series::set_size(std::size_t _fiducial_set_number, float _size)
{
    m_pimpl->set_private_value(
        2,
        std::to_string(_size),
        0,
        {{kw::FiducialSetSequence::GetTag(), _fiducial_set_number
        }
        });
}

//------------------------------------------------------------------------------

std::optional<fiducials_series::private_shape> fiducials_series::get_shape(std::size_t _fiducial_set_number) const
noexcept
{
    return string_to_private_shape(
        m_pimpl->get_private_value(3, 0, {{kw::FiducialSetSequence::GetTag(), _fiducial_set_number}})
    );
}

//------------------------------------------------------------------------------

void fiducials_series::set_shape(std::size_t _fiducial_set_number, private_shape _shape)
{
    m_pimpl->set_private_value(
        3,
        private_shape_to_string(_shape),
        0,
        {{kw::FiducialSetSequence::GetTag(), _fiducial_set_number
        }
        });
}

//------------------------------------------------------------------------------

std::optional<bool> fiducials_series::get_visibility(std::size_t _fiducial_set_number) const noexcept
{
    if(std::optional<std::string> visibility = m_pimpl->get_private_value(
           4,
           0,
           {{kw::FiducialSetSequence::GetTag(), _fiducial_set_number
           }
           }))
    {
        return *visibility == "true";
    }

    return std::nullopt;
}

//------------------------------------------------------------------------------

void fiducials_series::set_visibility(std::size_t _fiducial_set_number, bool _visibility)
{
    m_pimpl->set_private_value(
        4,
        _visibility ? "true" : "false",
        0,
        {{kw::FiducialSetSequence::GetTag(), _fiducial_set_number
        }
        });
}

//------------------------------------------------------------------------------

void fiducials_series::set_group_names_for_point_fiducials()
{
    std::vector<fiducial_set> fiducial_sets = get_fiducial_sets();
    for(auto& fiducial_set : fiducial_sets)
    {
        if(fiducial_set.group_name.has_value() && !fiducial_set.group_name->empty())
        {
            continue;
        }

        bool contains_a_point_fiducial = false;
        for(const fiducial& fiducial : fiducial_set.fiducial_sequence)
        {
            if(fiducial.shape_type == shape::point)
            {
                contains_a_point_fiducial = true;
                break;
            }
        }

        if(contains_a_point_fiducial)
        {
            std::size_t i          = 0;
            std::string group_name = "Group_" + std::to_string(i);
            while(std::ranges::find_if(
                      fiducial_sets,
                      [&group_name](const struct fiducial_set& _fiducial_set)
                {
                    return _fiducial_set.group_name.value_or("") == group_name;
                }) != fiducial_sets.end())
            {
                i++;
                group_name = "Group_" + std::to_string(i);
            }

            fiducial_set.group_name = group_name;
        }
    }

    set_fiducial_sets(fiducial_sets);
}

//------------------------------------------------------------------------------

std::vector<std::string> fiducials_series::get_point_fiducials_group_names() const
{
    std::vector<std::string> res;
    std::vector<fiducial_set> fiducial_sets = get_fiducial_sets();
    res.reserve(fiducial_sets.size());
    // Ignore fiducial sets which doesn't contain fiducials with shape_t == POINT (= Landmarks)
    std::vector<fiducial_set> point_fiducial_sets;
    std::ranges::copy_if(
        fiducial_sets,
        std::back_inserter(point_fiducial_sets),
        [](const fiducial_set& _fiducial_set)
        {
            return _fiducial_set.fiducial_sequence.empty() || std::ranges::find_if(
                _fiducial_set.fiducial_sequence,
                [](
                    const fiducial& _fiducial){return _fiducial.shape_type == shape::point;}) != _fiducial_set.fiducial_sequence.end();
        });
    std::ranges::for_each(
        point_fiducial_sets,
        [&res](const fiducial_set& _fs)
        {
            if(_fs.group_name.has_value())
            {
                res.push_back(*_fs.group_name);
            }
        });
    return res;
}

//------------------------------------------------------------------------------

std::optional<std::pair<fiducials_series::fiducial_set, std::size_t> > fiducials_series::get_fiducial_set_and_index(
    const std::string& _group_name
) const
{
    for(std::size_t index = 0 ;
        const fiducial_set& fiducial_set : get_fiducial_sets())
    {
        if(fiducial_set.group_name == _group_name)
        {
            return {{fiducial_set, index}};
        }

        index++;
    }

    return std::nullopt;
}

//------------------------------------------------------------------------------

std::optional<std::size_t> fiducials_series::get_number_of_points_in_group(const std::string& _group_name) const
{
    std::optional<std::pair<fiducial_set, std::size_t> > fiducial_set = get_fiducial_set_and_index(_group_name);
    if(!fiducial_set.has_value())
    {
        SIGHT_ASSERT("The group name '" << _group_name << "' doesn't exist", false);
        return {};
    }

    return std::size_t(std::ranges::count_if(
                           fiducial_set->first.fiducial_sequence,
                           [](const fiducial& _fiducial)
        {
            return _fiducial.shape_type == shape::point;
        }));
}

//------------------------------------------------------------------------------

[[nodiscard]] std::optional<std::array<double, 3> > fiducials_series::get_point(
    const fiducials_series::fiducial& _fiducial
)
{
    if(_fiducial.shape_type != shape::point)
    {
        // Only point fiducials are supported
        return std::nullopt;
    }

    if(!_fiducial.contour_data.empty())
    {
        data::fiducials_series::point3 point = _fiducial.contour_data[0];
        return {{point.x, point.y, point.z}};
    }

    // Position with Graphic Coordinates Data Sequence isn't supported
    return std::nullopt;
}

//------------------------------------------------------------------------------

[[nodiscard]] std::optional<std::array<double, 3> > fiducials_series::get_point(
    const std::string& _group_name,
    std::size_t _index
) const
{
    std::optional<std::pair<fiducial_set, std::size_t> > fiducial_set = get_fiducial_set_and_index(_group_name);
    if(!fiducial_set.has_value())
    {
        return std::nullopt;
    }

    for(std::size_t i = 0 ; const fiducial& fiducial : fiducial_set->first.fiducial_sequence)
    {
        if(fiducial.shape_type == shape::point)
        {
            if(i == _index)
            {
                return get_point(fiducial);
            }

            i++;
        }
    }

    return std::nullopt;
}

//------------------------------------------------------------------------------

[[nodiscard]] std::optional<landmarks::landmarks_group> fiducials_series::get_group(const std::string& _group_name)
const
{
    std::optional<std::pair<fiducial_set, std::size_t> > fiducial_set = get_fiducial_set_and_index(_group_name);
    if(!fiducial_set.has_value())
    {
        return std::nullopt;
    }

    landmarks::color_t color = fiducial_set->first.color.value_or(
        std::array {1.F, 1.F, 1.F, 1.F
        });
    landmarks::size_t size                                 = fiducial_set->first.size.value_or(10);
    fiducials_series::private_shape fiducial_private_shape = fiducial_set->first.shape.value_or(private_shape::sphere);
    landmarks::shape shape                                 = landmarks::shape::sphere;
    switch(fiducial_private_shape)
    {
        case private_shape::sphere:
            shape = landmarks::shape::sphere;
            break;

        case private_shape::cube:
            shape = landmarks::shape::cube;

        default:
            break;
    }

    bool visibility = fiducial_set->first.visibility.value_or(true);

    landmarks::landmarks_group group(color, size, shape, visibility);
    std::ranges::for_each(
        data::helper::fiducials_series::filter_fiducials(fiducial_set->first, data::fiducials_series::shape::point),
        [&group](const data::fiducials_series::fiducial& _fiducial)
        {
            if(auto point = get_point(_fiducial))
            {
                group.m_points.push_back(*point);
            }
        });
    return group;
}

//------------------------------------------------------------------------------

void fiducials_series::remove_point(const std::string& _group_name, std::size_t _index)
{
    std::optional<std::pair<fiducial_set, std::size_t> > fiducial_set = get_fiducial_set_and_index(_group_name);
    if(!fiducial_set.has_value())
    {
        return;
    }

    std::size_t point_index = 0;
    for(auto it = fiducial_set->first.fiducial_sequence.begin() ;
        it < fiducial_set->first.fiducial_sequence.end() ;
        ++it)
    {
        if(it->shape_type == data::fiducials_series::shape::point)
        {
            if(point_index == _index)
            {
                fiducial_set->first.fiducial_sequence.erase(it);
                break;
            }

            point_index++;
        }
    }

    set_fiducial_set(fiducial_set->second, fiducial_set->first);
}

//------------------------------------------------------------------------------

void fiducials_series::remove_group(const std::string& _group_name)
{
    std::vector<fiducial_set> fiducial_sets = get_fiducial_sets();
    std::erase_if(fiducial_sets, [&_group_name](const fiducial_set& _fs){return _fs.group_name == _group_name;});
    set_fiducial_sets(fiducial_sets);
}

//------------------------------------------------------------------------------

void fiducials_series::add_group(const std::string& _group_name, const std::array<float, 4>& _color, float _size)
{
    fiducial_set fiducial_set;
    fiducial_set.group_name = _group_name;
    fiducial_set.color      = _color;
    fiducial_set.size       = _size;
    fiducial_set.shape      = private_shape::sphere;
    append_fiducial_set(fiducial_set);
}

//------------------------------------------------------------------------------

void fiducials_series::add_point(const std::string& _group_name, const std::array<double, 3>& _pos)
{
    std::optional<std::pair<fiducial_set, std::size_t> > fiducial_set = get_fiducial_set_and_index(_group_name);
    if(!fiducial_set.has_value())
    {
        SIGHT_WARN("Couldn't add point in fiducial set '" << _group_name << "', the group doesn't exist.");
        return;
    }

    std::string fiducial_name = _group_name + '_'
                                + std::to_string(
        data::helper::fiducials_series::filter_fiducials(
            fiducial_set->first,
            data::fiducials_series::shape::point
        ).size()
                                );
    fiducial fiducial;
    fiducial.shape_type           = shape::point;
    fiducial.fiducial_description = fiducial_name;
    fiducial.fiducial_identifier  = fiducial_name;
    fiducial.fiducial_uid         = core::tools::uuid::generate();
    fiducial.contour_data         = {{.x = _pos[0], .y = _pos[1], .z = _pos[2]}};
    append_fiducial(fiducial_set->second, fiducial);
}

//------------------------------------------------------------------------------

fiducials_series::shape fiducials_series::string_to_shape(const std::optional<std::string>& _string)
{
    static const std::map<std::string, shape> s_STRING_TO_SHAPE_MAP {{"POINT", shape::point
    }, {"LINE", shape::line
        }, {"PLANE", shape::point
        }, {"SURFACE", shape::surface
        }, {"RULER", shape::ruler
        }, {"L_SHAPE", shape::l_shape
        }, {"T_SHAPE", shape::t_shape
        }, {"SHAPE", shape::shape
        }
    };
    if(!_string || !s_STRING_TO_SHAPE_MAP.contains(*_string))
    {
        return shape::invalid;
    }

    return s_STRING_TO_SHAPE_MAP.at(*_string);
}

//------------------------------------------------------------------------------

std::optional<std::array<float, 4> > fiducials_series::string_to_color(const std::optional<std::string>& _string)
{
    if(!_string.has_value())
    {
        return std::nullopt;
    }

    std::vector<std::string> splits;
    boost::split(splits, *_string, boost::is_any_of(","));
    if(splits.size() != 4)
    {
        return std::nullopt;
    }

    std::array<float, 4> res {};
    std::ranges::transform(splits, res.begin(), [](const std::string& _value){return std::stof(_value);});
    return res;
}

//------------------------------------------------------------------------------

std::optional<fiducials_series::private_shape> fiducials_series::string_to_private_shape(
    const std::optional<std::string>& _string
)
{
    static const std::map<std::string, private_shape> s_STRING_TO_PRIVATE_SHAPE_MAP {
        {"SPHERE", private_shape::sphere},
        {"CUBE", private_shape::cube}
    };
    if(!_string.has_value())
    {
        return std::nullopt;
    }

    if(!s_STRING_TO_PRIVATE_SHAPE_MAP.contains(*_string))
    {
        return std::nullopt;
    }

    return s_STRING_TO_PRIVATE_SHAPE_MAP.at(*_string);
}

//------------------------------------------------------------------------------

fiducials_series::fiducial_set fiducials_series::to_fiducial_set(const gdcm::DataSet& _data_set) const
{
    fiducial_set fiducial_set;
    if(auto ris = m_pimpl->get_sequence(kw::ReferencedImageSequence::GetTag(), _data_set))
    {
        fiducial_set.referenced_image_sequence = std::vector<referenced_image> {};
        for(std::size_t i = 1 ; i <= ris->GetNumberOfItems() ; i++) // GDCM Sequence of Items is 1-indexed
        {
            fiducial_set.referenced_image_sequence->push_back(to_referenced_image(ris->GetItem(i).GetNestedDataSet()));
        }
    }

    fiducial_set.frame_of_reference_uid = m_pimpl->get_value<kw::FrameOfReferenceUID>(_data_set);
    if(auto fiducial_sequence = m_pimpl->get_sequence(kw::FiducialSequence::GetTag(), _data_set))
    {
        for(std::size_t i = 1 ; i <= fiducial_sequence->GetNumberOfItems() ; i++) // GDCM Sequence of Items is 1-indexed
        {
            fiducial_set.fiducial_sequence.push_back(to_fiducial(fiducial_sequence->GetItem(i).GetNestedDataSet()));
        }
    }

    fiducial_set.group_name = m_pimpl->get_private_value(0, _data_set);
    fiducial_set.color      = string_to_color(m_pimpl->get_private_value(1, _data_set));
    if(std::optional<std::string> size = m_pimpl->get_private_value(2, _data_set))
    {
        fiducial_set.size = std::stof(*size);
    }

    fiducial_set.shape = string_to_private_shape(m_pimpl->get_private_value(3, _data_set));
    if(std::optional<std::string> visibility = m_pimpl->get_private_value(4, _data_set))
    {
        fiducial_set.visibility = (*visibility == "true");
    }

    return fiducial_set;
}

//------------------------------------------------------------------------------

template<>
fiducials_series::fiducial_set fiducials_series::to<fiducials_series::fiducial_set>(const gdcm::DataSet& _data_set)
const
{
    return to_fiducial_set(_data_set);
}

//------------------------------------------------------------------------------

fiducials_series::referenced_image fiducials_series::to_referenced_image(const gdcm::DataSet& _data_set) const
{
    std::unique_lock lock(m_pimpl->m_mutex);

    return referenced_image {
        .referenced_sop_class_uid    = m_pimpl->get_value<kw::ReferencedSOPClassUID>(_data_set).value_or(""),
        .referenced_sop_instance_uid = m_pimpl->get_value<kw::ReferencedSOPInstanceUID>(_data_set).value_or(""),
        .referenced_frame_number     = m_pimpl->get_values<kw::ReferencedFrameNumber>(_data_set).value_or(
            std::vector<std::int32_t> {
            }),
        .referenced_segment_number = m_pimpl->get_values<kw::ReferencedSegmentNumber>(_data_set).value_or(
            std::vector<std::uint16_t> {
            })
    };
}

//------------------------------------------------------------------------------

template<>
fiducials_series::referenced_image fiducials_series::to<fiducials_series::referenced_image>(
    const gdcm::DataSet& _data_set
)
const
{
    return to_referenced_image(_data_set);
}

//------------------------------------------------------------------------------

fiducials_series::fiducial fiducials_series::to_fiducial(const gdcm::DataSet& _data_set) const
{
    std::unique_lock lock(m_pimpl->m_mutex);

    fiducial fiducial;
    fiducial.shape_type           = string_to_shape(m_pimpl->get_value<kw::ShapeType>(_data_set));
    fiducial.fiducial_description = m_pimpl->get_value<kw::FiducialDescription>(_data_set).value_or("");
    fiducial.fiducial_identifier  = m_pimpl->get_value<kw::FiducialIdentifier>(_data_set).value_or("");
    if(const auto& gcds = m_pimpl->get_sequence(kw::GraphicCoordinatesDataSequence::GetTag(), _data_set))
    {
        fiducial.graphic_coordinates_data_sequence = std::vector<graphic_coordinates_data> {};
        for(std::size_t i = 1 ; i <= gcds->GetNumberOfItems() ; i++) // GDCM Sequence of Items is 1-indexed
        {
            fiducial.graphic_coordinates_data_sequence->push_back(
                to_graphic_coordinates_data(
                    gcds->GetItem(i).
                    GetNestedDataSet()
                )
            );
        }
    }

    fiducial.fiducial_uid = m_pimpl->get_value<kw::FiducialUID>(_data_set);
    if(auto contour_data = m_pimpl->get_values<kw::ContourData>(_data_set))
    {
        fiducial.contour_data = to_point3(*contour_data);
    }

    return fiducial;
}

//------------------------------------------------------------------------------

template<>
fiducials_series::fiducial fiducials_series::to<fiducials_series::fiducial>(const gdcm::DataSet& _data_set) const
{
    return to_fiducial(_data_set);
}

//------------------------------------------------------------------------------

fiducials_series::graphic_coordinates_data fiducials_series::to_graphic_coordinates_data(const gdcm::DataSet& _data_set)
const
{
    std::unique_lock lock(m_pimpl->m_mutex);

    graphic_coordinates_data gcd;
    if(const auto& ris = m_pimpl->get_sequence(kw::ReferencedImageSequence::GetTag(), _data_set);
       ris != nullptr && ris->GetNumberOfItems() > 0)
    {
        // GDCM Sequence of Items is 1-indexed
        gcd.referenced_image_sequence = to_referenced_image(ris->GetItem(1).GetNestedDataSet());
    }

    if(auto graphic_data = m_pimpl->get_values<kw::GraphicData>(_data_set))
    {
        gcd.graphic_data = to_point2(*graphic_data);
    }

    return gcd;
}

//------------------------------------------------------------------------------

template<>
fiducials_series::graphic_coordinates_data fiducials_series::to<fiducials_series::graphic_coordinates_data>(
    const gdcm::DataSet& _data_set
) const
{
    return to_graphic_coordinates_data(_data_set);
}

//------------------------------------------------------------------------------

std::vector<fiducials_series::point2> fiducials_series::to_point2(const std::vector<float>& _floats)
{
    SIGHT_ASSERT("The number of elements must be a multiple of 2", (_floats.size() % 2) == 0);
    std::vector<point2> res;
    res.reserve(_floats.size() / 2);
    for(std::size_t i = 0 ; i < _floats.size() ; i += 2)
    {
        res.push_back({.x = _floats[i], .y = _floats[i + 1]});
    }

    return res;
}

//------------------------------------------------------------------------------

std::vector<fiducials_series::point3> fiducials_series::to_point3(const std::vector<double>& _floats)
{
    SIGHT_ASSERT("The number of elements must be a multiple of 3", (_floats.size() % 3) == 0);
    std::vector<point3> res;
    res.reserve(_floats.size() / 3);
    for(std::size_t i = 0 ; i < _floats.size() ; i += 3)
    {
        res.push_back({.x = _floats[i], .y = _floats[i + 1], .z = _floats[i + 2]});
    }

    return res;
}

//------------------------------------------------------------------------------

template<typename T>
std::optional<std::vector<T> > fiducials_series::to_vector(const gdcm::SmartPointer<gdcm::SequenceOfItems>& _sequence)
const
{
    std::unique_lock lock(m_pimpl->m_mutex);

    if(_sequence == nullptr)
    {
        return std::nullopt;
    }

    std::vector<T> res;
    res.reserve(_sequence->GetNumberOfItems());
    for(std::size_t i = 1 ; i <= _sequence->GetNumberOfItems() ; i++) // GDCM Sequence of Items is 1-indexed
    {
        res.push_back(to<T>(_sequence->GetItem(i).GetNestedDataSet()));
    }

    return res;
}

//------------------------------------------------------------------------------

template<typename T>
gdcm::SmartPointer<gdcm::SequenceOfItems> fiducials_series::to_sequence(const std::optional<std::vector<T> >& _vector)
const
{
    std::unique_lock lock(m_pimpl->m_mutex);

    if(!_vector)
    {
        return nullptr;
    }

    auto sequence = gdcm::SequenceOfItems::New();
    for(const T& element : *_vector)
    {
        gdcm::Item item;
        item.SetNestedDataSet(to_gdcm(*m_pimpl, element));
        sequence->AddItem(item);
    }

    return sequence;
}

//------------------------------------------------------------------------------

template<typename T>
gdcm::SmartPointer<gdcm::SequenceOfItems> fiducials_series::to_sequence(const std::vector<T>& _vector) const
{
    return to_sequence(std::optional {_vector});
}

} // namespace sight::data
