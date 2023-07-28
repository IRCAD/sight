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

#include "FiducialsSeries.hpp"

#include "data/dicom/Sop.hpp"

#include "detail/SeriesImpl.hxx"

#include <gdcmSequenceOfItems.h>
#include <gdcmSmartPointer.h>
#include <gdcmTagKeywords.h>

namespace sight::data
{

namespace kw = gdcm::Keywords;

//------------------------------------------------------------------------------

namespace
{

//------------------------------------------------------------------------------

std::string shapeToString(FiducialsSeries::Shape shape)
{
    switch(shape)
    {
        case FiducialsSeries::Shape::POINT:
            return "POINT";

        case FiducialsSeries::Shape::LINE:
            return "LINE";

        case FiducialsSeries::Shape::PLANE:
            return "PLANE";

        case FiducialsSeries::Shape::SURFACE:
            return "SURFACE";

        case FiducialsSeries::Shape::RULER:
            return "RULER";

        case FiducialsSeries::Shape::L_SHAPE:
            return "L_SHAPE";

        case FiducialsSeries::Shape::T_SHAPE:
            return "T_SHAPE";

        case FiducialsSeries::Shape::SHAPE:
            return "SHAPE";

        default:
            SIGHT_WARN("Unknown shape " << static_cast<int>(shape));
            return "";
    }
}

//------------------------------------------------------------------------------

std::vector<float> toFloats(const std::vector<FiducialsSeries::Point2>& points)
{
    std::vector<float> res;
    res.reserve(points.size() * 2);
    for(const FiducialsSeries::Point2& point : points)
    {
        res.push_back(static_cast<float>(point.x));
        res.push_back(static_cast<float>(point.y));
    }

    return res;
}

//------------------------------------------------------------------------------

std::vector<double> toFloats(const std::vector<FiducialsSeries::Point3>& points)
{
    std::vector<double> res;
    res.reserve(points.size() * 3);
    for(const FiducialsSeries::Point3& point : points)
    {
        res.push_back(point.x);
        res.push_back(point.y);
        res.push_back(point.z);
    }

    return res;
}

//------------------------------------------------------------------------------

gdcm::DataSet toGdcm(detail::SeriesImpl& pimpl, FiducialsSeries::ReferencedImage referencedImage)
{
    gdcm::DataSet dataSet;
    pimpl.setStringValue<kw::ReferencedSOPClassUID>(referencedImage.referencedSOPClassUID, dataSet);
    pimpl.setStringValue<kw::ReferencedSOPInstanceUID>(referencedImage.referencedSOPInstanceUID, dataSet);
    pimpl.setValues<kw::ReferencedFrameNumber>(referencedImage.referencedFrameNumber, dataSet);
    pimpl.setValues<kw::ReferencedSegmentNumber>(referencedImage.referencedSegmentNumber, dataSet);
    return dataSet;
}

//------------------------------------------------------------------------------

gdcm::DataSet toGdcm(detail::SeriesImpl& pimpl, FiducialsSeries::GraphicCoordinatesData graphicCoordinatesData)
{
    gdcm::DataSet dataSet;
    auto referencedImageSequence = gdcm::SequenceOfItems::New();
    gdcm::Item item;
    item.SetNestedDataSet(toGdcm(pimpl, graphicCoordinatesData.referencedImageSequence));
    referencedImageSequence->AddItem(item);
    detail::SeriesImpl::setSequence(kw::ReferencedImageSequence::GetTag(), referencedImageSequence, dataSet);
    pimpl.setValues<kw::GraphicData>(toFloats(graphicCoordinatesData.graphicData), dataSet);
    return dataSet;
}

//------------------------------------------------------------------------------

gdcm::DataSet toGdcm(detail::SeriesImpl& pimpl, FiducialsSeries::Fiducial fiducial)
{
    gdcm::DataSet dataSet;
    pimpl.setStringValue<kw::ShapeType>(shapeToString(fiducial.shapeType), dataSet);
    pimpl.setStringValue<kw::FiducialDescription>(fiducial.fiducialDescription, dataSet);
    pimpl.setStringValue<kw::FiducialIdentifier>(fiducial.fiducialIdentifier, dataSet);
    if(fiducial.graphicCoordinatesDataSequence)
    {
        auto gcds = gdcm::SequenceOfItems::New();
        for(const FiducialsSeries::GraphicCoordinatesData& gcd : *fiducial.graphicCoordinatesDataSequence)
        {
            gdcm::Item item;
            item.SetNestedDataSet(toGdcm(pimpl, gcd));
            gcds->AddItem(item);
        }

        detail::SeriesImpl::setSequence(kw::GraphicCoordinatesDataSequence::GetTag(), gcds, dataSet);
    }

    pimpl.setStringValue<kw::FiducialUID>(fiducial.fiducialUID.value_or(""), dataSet);
    if(!fiducial.contourData.empty())
    {
        pimpl.setValue<kw::NumberOfContourPoints>(static_cast<int>(fiducial.contourData.size()), dataSet);
        pimpl.setValues<kw::ContourData>(toFloats(fiducial.contourData), dataSet);
    }

    return dataSet;
}

//------------------------------------------------------------------------------

gdcm::DataSet toGdcm(detail::SeriesImpl& pimpl, FiducialsSeries::FiducialSet fiducialSet)
{
    gdcm::DataSet dataSet;
    if(fiducialSet.referencedImageSequence)
    {
        auto referencedImageSequence = gdcm::SequenceOfItems::New();
        for(const FiducialsSeries::ReferencedImage& referencedImage : *fiducialSet.referencedImageSequence)
        {
            gdcm::Item item;
            item.SetNestedDataSet(toGdcm(pimpl, referencedImage));
            referencedImageSequence->AddItem(item);
        }

        detail::SeriesImpl::setSequence(kw::ReferencedImageSequence::GetTag(), referencedImageSequence, dataSet);
    }

    if(fiducialSet.frameOfReferenceUID)
    {
        pimpl.setStringValue<kw::FrameOfReferenceUID>(*fiducialSet.frameOfReferenceUID, dataSet);
    }

    auto fiducialSequence = gdcm::SequenceOfItems::New();
    for(const FiducialsSeries::Fiducial& fiducial : fiducialSet.fiducialSequence)
    {
        gdcm::Item item;
        item.SetNestedDataSet(toGdcm(pimpl, fiducial));
        fiducialSequence->AddItem(item);
    }

    detail::SeriesImpl::setSequence(kw::FiducialSequence::GetTag(), fiducialSequence, dataSet);
    detail::SeriesImpl::setPrivateValue(0, fiducialSet.groupName, dataSet);
    return dataSet;
}

//------------------------------------------------------------------------------

template<typename T>
gdcm::SmartPointer<gdcm::SequenceOfItems> appendInSequence(
    detail::SeriesImpl& pimpl,
    gdcm::SmartPointer<gdcm::SequenceOfItems> sequence,
    T element
)
{
    auto res = sequence == nullptr ? gdcm::SequenceOfItems::New() : sequence;
    gdcm::Item item;
    item.SetNestedDataSet(toGdcm(pimpl, element));
    res->AddItem(item);
    return res;
}

//------------------------------------------------------------------------------

template<typename T>
void appendInSequence(
    detail::SeriesImpl& pimpl,
    gdcm::Tag tag,
    T element,
    const std::vector<std::pair<gdcm::Tag, std::size_t> >& indices = {})
{
    pimpl.setSequence(tag, appendInSequence(pimpl, pimpl.getSequence(tag, 0, indices), element));
}

} // namespace

//------------------------------------------------------------------------------

bool FiducialsSeries::Point2::operator==(Point2 other) const
{
    return x == other.x && y == other.y;
}

//------------------------------------------------------------------------------

bool FiducialsSeries::Point3::operator==(Point3 other) const
{
    return x == other.x && y == other.y && z == other.z;
}

//------------------------------------------------------------------------------

bool FiducialsSeries::ReferencedImage::operator==(const ReferencedImage& other) const
{
    return referencedSOPClassUID == other.referencedSOPClassUID
           && referencedSOPInstanceUID == other.referencedSOPInstanceUID
           && referencedFrameNumber == other.referencedFrameNumber
           && referencedSegmentNumber == other.referencedSegmentNumber;
}

//------------------------------------------------------------------------------

bool FiducialsSeries::ReferencedImage::operator!=(const ReferencedImage& other) const
{
    return !(*this == other);
}

//------------------------------------------------------------------------------

bool FiducialsSeries::GraphicCoordinatesData::operator==(const GraphicCoordinatesData& other) const
{
    return referencedImageSequence == other.referencedImageSequence && graphicData == other.graphicData;
}

//------------------------------------------------------------------------------

bool FiducialsSeries::GraphicCoordinatesData::operator!=(const GraphicCoordinatesData& other) const
{
    return !(*this == other);
}

//------------------------------------------------------------------------------

bool FiducialsSeries::Fiducial::operator==(const Fiducial& other) const
{
    return shapeType == other.shapeType && fiducialDescription == other.fiducialDescription
           && fiducialIdentifier == other.fiducialIdentifier
           && graphicCoordinatesDataSequence == other.graphicCoordinatesDataSequence && fiducialUID == other.fiducialUID
           && contourData == other.contourData;
}

//------------------------------------------------------------------------------

bool FiducialsSeries::Fiducial::operator!=(const Fiducial& other) const
{
    return !(*this == other);
}

//------------------------------------------------------------------------------

bool FiducialsSeries::FiducialSet::operator==(const FiducialSet& other) const
{
    return referencedImageSequence == other.referencedImageSequence && frameOfReferenceUID == other.frameOfReferenceUID
           && fiducialSequence == other.fiducialSequence && groupName == other.groupName;
}

FiducialsSeries::FiducialsSeries(Key key) :
    Series(key)
{
    setSOPKeyword(dicom::sop::Keyword::SpatialFiducialsStorage);
}

//------------------------------------------------------------------------------

bool FiducialsSeries::operator==(const FiducialsSeries& other) const
{
    return getContentDate() == other.getContentDate() && getContentLabel() == other.getContentLabel()
           && getContentDescription() == other.getContentDescription()
           && getContentCreatorName() == other.getContentCreatorName() && getFiducialSets() == other.getFiducialSets()
           && BaseClass::operator==(other);
}

//------------------------------------------------------------------------------

bool FiducialsSeries::operator!=(const FiducialsSeries& other) const
{
    return !(*this == other);
}

//------------------------------------------------------------------------------

std::string FiducialsSeries::getContentDate() const noexcept
{
    return m_pimpl->getStringValue<kw::ContentDate>();
}

//------------------------------------------------------------------------------

void FiducialsSeries::setContentDate(const std::string& contentDate)
{
    m_pimpl->setStringValue<kw::ContentDate>(contentDate);
}

//------------------------------------------------------------------------------

std::string FiducialsSeries::getContentLabel() const noexcept
{
    return m_pimpl->getStringValue<kw::ContentLabel>();
}

//------------------------------------------------------------------------------

void FiducialsSeries::setContentLabel(const std::string& contentLabel)
{
    m_pimpl->setStringValue<kw::ContentLabel>(contentLabel);
}

//------------------------------------------------------------------------------

std::string FiducialsSeries::getContentDescription() const noexcept
{
    return m_pimpl->getStringValue<kw::ContentDescription>();
}

//------------------------------------------------------------------------------

void FiducialsSeries::setContentDescription(const std::string& contentDescription)
{
    m_pimpl->setStringValue<kw::ContentDescription>(contentDescription);
}

//------------------------------------------------------------------------------

std::string FiducialsSeries::getContentCreatorName() const noexcept
{
    return m_pimpl->getStringValue<kw::ContentCreatorName>();
}

//------------------------------------------------------------------------------

void FiducialsSeries::setContentCreatorName(const std::string& contentCreatorName)
{
    m_pimpl->setStringValue<kw::ContentCreatorName>(contentCreatorName);
}

//------------------------------------------------------------------------------

std::vector<FiducialsSeries::FiducialSet> FiducialsSeries::getFiducialSets() const noexcept
{
    return toVector<FiducialSet>(m_pimpl->getSequence<kw::FiducialSetSequence>()).value_or(std::vector<FiducialSet> {});
}

//------------------------------------------------------------------------------

void FiducialsSeries::setFiducialSets(const std::vector<FiducialSet>& fiducialSets)
{
    m_pimpl->setSequence(kw::FiducialSetSequence::GetTag(), toSequence(fiducialSets));
}

//------------------------------------------------------------------------------

void FiducialsSeries::setFiducialSet(std::size_t fiducialSetNumber, FiducialSet fiducialSet)
{
    m_pimpl->getOrCreateDataSet(0, {{kw::FiducialSetSequence::GetTag(), fiducialSetNumber}}) = toGdcm(
        *m_pimpl,
        fiducialSet
    );
}

//------------------------------------------------------------------------------

void FiducialsSeries::appendFiducialSet(FiducialSet fiducialSet)
{
    appendInSequence(*m_pimpl, kw::FiducialSetSequence::GetTag(), fiducialSet);
}

//------------------------------------------------------------------------------

std::optional<std::vector<FiducialsSeries::ReferencedImage> > FiducialsSeries::getReferencedImages(
    std::size_t fiducialSetNumber
) const noexcept
{
    return toVector<ReferencedImage>(
        m_pimpl->getSequence(
            kw::ReferencedImageSequence::GetTag(),
            0,
            {{kw::FiducialSetSequence::GetTag(), fiducialSetNumber
            }
            })
    );
}

//------------------------------------------------------------------------------

void FiducialsSeries::setReferencedImages(
    std::size_t fiducialSetNumber,
    const std::optional<std::vector<ReferencedImage> >& referencedImages
)
{
    m_pimpl->setSequence(
        kw::ReferencedImageSequence::GetTag(),
        toSequence(
            referencedImages
        ),
        0,
        {{kw::FiducialSetSequence::GetTag(), fiducialSetNumber
        }
        });
}

//------------------------------------------------------------------------------

void FiducialsSeries::setReferencedImage(
    std::size_t fiducialSetNumber,
    std::size_t referencedImageNumber,
    ReferencedImage referencedImage
)
{
    m_pimpl->getOrCreateDataSet(
        0,
        {{kw::FiducialSetSequence::GetTag(), fiducialSetNumber
        }, {kw::ReferencedImageSequence::GetTag(), referencedImageNumber
            }
        }) = toGdcm(*m_pimpl, referencedImage);
}

//------------------------------------------------------------------------------

void FiducialsSeries::appendReferencedImage(std::size_t fiducialSetNumber, ReferencedImage referencedImage)
{
    appendInSequence(
        *m_pimpl,
        kw::ReferencedImageSequence::GetTag(),
        referencedImage,
        {{kw::FiducialSetSequence::GetTag(), fiducialSetNumber
        }
        });
}

//------------------------------------------------------------------------------

std::optional<std::string> FiducialsSeries::getReferencedSOPClassUID(
    std::size_t fiducialSetNumber,
    std::size_t referencedImageNumber
) const noexcept
{
    return m_pimpl->getValue<kw::ReferencedSOPClassUID>(
        0,
        {{kw::FiducialSetSequence::GetTag(), fiducialSetNumber
        }, {kw::ReferencedImageSequence::GetTag(), referencedImageNumber
            }
        });
}

//------------------------------------------------------------------------------

void FiducialsSeries::setReferencedSOPClassUID(
    std::size_t fiducialSetNumber,
    std::size_t referencedImageNumber,
    const std::string& referencedSOPClassUID
)
{
    m_pimpl->setStringValue<kw::ReferencedSOPClassUID>(
        referencedSOPClassUID,
        0,
        {{kw::FiducialSetSequence::GetTag(), fiducialSetNumber
        }, {kw::ReferencedImageSequence::GetTag(), referencedImageNumber
            }
        });
}

//------------------------------------------------------------------------------

std::optional<std::string> FiducialsSeries::getReferencedSOPInstanceUID(
    std::size_t fiducialSetNumber,
    std::size_t referencedImageNumber
) const noexcept
{
    return m_pimpl->getValue<kw::ReferencedSOPInstanceUID>(
        0,
        {{kw::FiducialSetSequence::GetTag(), fiducialSetNumber
        },
            {kw::ReferencedImageSequence::GetTag(), referencedImageNumber
            }
        });
}

//------------------------------------------------------------------------------

void FiducialsSeries::setReferencedSOPInstanceUID(
    std::size_t fiducialSetNumber,
    std::size_t referencedImageNumber,
    const std::string& referencedSOPInstanceUID
)
{
    m_pimpl->setStringValue<kw::ReferencedSOPInstanceUID>(
        referencedSOPInstanceUID,
        0,
        {{kw::FiducialSetSequence::GetTag(), fiducialSetNumber
        },
            {kw::ReferencedImageSequence::GetTag(), referencedImageNumber
            }
        });
}

//------------------------------------------------------------------------------

std::vector<std::int32_t> FiducialsSeries::getReferencedFrameNumber(
    std::size_t fiducialSetNumber,
    std::size_t referencedImageNumber
) const noexcept
{
    return m_pimpl->getValues<kw::ReferencedFrameNumber>(
        0,
        {{kw::FiducialSetSequence::GetTag(), fiducialSetNumber
        },
            {kw::ReferencedImageSequence::GetTag(), referencedImageNumber
            }
        }).value_or(std::vector<std::int32_t> {});
}

//------------------------------------------------------------------------------

void FiducialsSeries::setReferencedFrameNumber(
    std::size_t fiducialSetNumber,
    std::size_t referencedImageNumber,
    std::vector<std::int32_t> referencedFrameNumber
)
{
    m_pimpl->setValues<kw::ReferencedFrameNumber>(
        referencedFrameNumber,
        0,
        {{kw::FiducialSetSequence::GetTag(), fiducialSetNumber
        }, {kw::ReferencedImageSequence::GetTag(), referencedImageNumber
            }
        });
}

//------------------------------------------------------------------------------

std::vector<std::uint16_t> FiducialsSeries::getReferencedSegmentNumber(
    std::size_t fiducialSetNumber,
    std::size_t referencedImageNumber
) const noexcept
{
    return m_pimpl->getValues<kw::ReferencedSegmentNumber>(
        0,
        {{kw::FiducialSetSequence::GetTag(), fiducialSetNumber
        },
            {kw::ReferencedImageSequence::GetTag(), referencedImageNumber
            }
        }).value_or(std::vector<std::uint16_t> {});
}

//------------------------------------------------------------------------------

void FiducialsSeries::setReferencedSegmentNumber(
    std::size_t fiducialSetNumber,
    std::size_t referencedImageNumber,
    std::vector<std::uint16_t> referencedSegmentNumber
)
{
    m_pimpl->setValues<kw::ReferencedSegmentNumber>(
        referencedSegmentNumber,
        0,
        {{kw::FiducialSetSequence::GetTag(), fiducialSetNumber
        }, {kw::ReferencedImageSequence::GetTag(), referencedImageNumber
            }
        });
}

//------------------------------------------------------------------------------

std::optional<std::string> FiducialsSeries::getFrameOfReferenceUID(std::size_t fiducialSetNumber) const noexcept
{
    return m_pimpl->getValue<kw::FrameOfReferenceUID>(
        0,
        {{kw::FiducialSetSequence::GetTag(), fiducialSetNumber
        }
        });
}

//------------------------------------------------------------------------------

void FiducialsSeries::setFrameOfReferenceUID(
    std::size_t fiducialSetNumber,
    const std::optional<std::string>& frameOfReferenceUID
)
{
    m_pimpl->setStringValue<kw::FrameOfReferenceUID>(
        frameOfReferenceUID.value_or(
            ""
        ),
        0,
        {{kw::FiducialSetSequence::GetTag(), fiducialSetNumber
        }
        });
}

//------------------------------------------------------------------------------

std::vector<FiducialsSeries::Fiducial> FiducialsSeries::getFiducials(std::size_t fiducialSetNumber) const noexcept
{
    return toVector<Fiducial>(
        m_pimpl->getSequence(
            kw::FiducialSequence::GetTag(),
            0,
            {{kw::FiducialSetSequence::GetTag(), fiducialSetNumber
            }
            })
    ).value_or(std::vector<Fiducial> {});
}

//------------------------------------------------------------------------------

void FiducialsSeries::setFiducials(std::size_t fiducialSetNumber, const std::vector<Fiducial>& fiducials)
{
    m_pimpl->setSequence(
        kw::FiducialSequence::GetTag(),
        toSequence(fiducials),
        0,
        {{kw::FiducialSetSequence::GetTag(), fiducialSetNumber
        }
        });
}

//------------------------------------------------------------------------------

void FiducialsSeries::setFiducial(std::size_t fiducialSetNumber, std::size_t fiducialNumber, Fiducial fiducial)
{
    m_pimpl->getOrCreateDataSet(
        0,
        {{kw::FiducialSetSequence::GetTag(), fiducialSetNumber
        }, {kw::FiducialSequence::GetTag(), fiducialNumber
            }
        }) = toGdcm(*m_pimpl, fiducial);
}

//------------------------------------------------------------------------------

void FiducialsSeries::appendFiducial(std::size_t fiducialSetNumber, Fiducial fiducial)
{
    appendInSequence(
        *m_pimpl,
        kw::FiducialSequence::GetTag(),
        fiducial,
        {{kw::FiducialSetSequence::GetTag(), fiducialSetNumber
        }
        });
}

//------------------------------------------------------------------------------

FiducialsSeries::Shape FiducialsSeries::getShapeType(
    std::size_t fiducialSetNumber,
    std::size_t fiducialNumber
) const noexcept
{
    return stringToShape(
        m_pimpl->getValue<kw::ShapeType>(
            0,
            {{kw::FiducialSetSequence::GetTag(), fiducialSetNumber
            }, {kw::FiducialSequence::GetTag(), fiducialNumber
                }
            })
    );
}

//------------------------------------------------------------------------------

void FiducialsSeries::setShapeType(
    std::size_t fiducialSetNumber,
    std::size_t fiducialNumber,
    FiducialsSeries::Shape shapeType
)
{
    m_pimpl->setValue<kw::ShapeType>(
        shapeToString(shapeType),
        0,
        {{kw::FiducialSetSequence::GetTag(), fiducialSetNumber
        }, {kw::FiducialSequence::GetTag(), fiducialNumber
            }
        });
}

//------------------------------------------------------------------------------

std::string FiducialsSeries::getFiducialDescription(
    std::size_t fiducialSetNumber,
    std::size_t fiducialNumber
) const noexcept
{
    return m_pimpl->getValue<kw::FiducialDescription>(
        0,
        {{kw::FiducialSetSequence::GetTag(), fiducialSetNumber
        }, {kw::FiducialSequence::GetTag(), fiducialNumber
            }
        }).value_or("");
}

//------------------------------------------------------------------------------

void FiducialsSeries::setFiducialDescription(
    std::size_t fiducialSetNumber,
    std::size_t fiducialNumber,
    const std::string& fiducialDescription
)
{
    m_pimpl->setStringValue<kw::FiducialDescription>(
        fiducialDescription,
        0,
        {{kw::FiducialSetSequence::GetTag(), fiducialSetNumber
        }, {kw::FiducialSequence::GetTag(), fiducialNumber
            }
        });
}

//------------------------------------------------------------------------------

std::string FiducialsSeries::getFiducialIdentifier(
    std::size_t fiducialSetNumber,
    std::size_t fiducialNumber
) const noexcept
{
    return m_pimpl->getValue<kw::FiducialIdentifier>(
        0,
        {{kw::FiducialSetSequence::GetTag(), fiducialSetNumber
        }, {kw::FiducialSequence::GetTag(), fiducialNumber
            }
        }).value_or("");
}

//------------------------------------------------------------------------------

void FiducialsSeries::setFiducialIdentifier(
    std::size_t fiducialSetNumber,
    std::size_t fiducialNumber,
    const std::string& fiducialIdentifier
)
{
    m_pimpl->setStringValue<kw::FiducialIdentifier>(
        fiducialIdentifier,
        0,
        {{kw::FiducialSetSequence::GetTag(), fiducialSetNumber
        }, {kw::FiducialSequence::GetTag(), fiducialNumber
            }
        });
}

//------------------------------------------------------------------------------

std::optional<std::vector<FiducialsSeries::GraphicCoordinatesData> > FiducialsSeries::getGraphicCoordinatesDataSequence(
    std::size_t fiducialSetNumber,
    std::size_t fiducialNumber
) const noexcept
{
    return toVector<GraphicCoordinatesData>(
        m_pimpl->getSequence(
            kw::GraphicCoordinatesDataSequence::GetTag(),
            0,
            {{kw::FiducialSetSequence::GetTag(), fiducialSetNumber
            }, {kw::FiducialSequence::GetTag(), fiducialNumber
                }
            })
    );
}

//------------------------------------------------------------------------------

void FiducialsSeries::setGraphicCoordinatesDataSequence(
    std::size_t fiducialSetNumber,
    std::size_t fiducialNumber,
    const std::optional<std::vector<GraphicCoordinatesData> >& graphicCoordinatesDataSequence
)
{
    m_pimpl->setSequence(
        kw::GraphicCoordinatesDataSequence::GetTag(),
        toSequence(
            graphicCoordinatesDataSequence
        ),
        0,
        {{kw::FiducialSetSequence::GetTag(), fiducialSetNumber
        }, {kw::FiducialSequence::GetTag(), fiducialNumber
            }
        });
}

//------------------------------------------------------------------------------

void FiducialsSeries::setGraphicCoordinatesData(
    std::size_t fiducialSetNumber,
    std::size_t fiducialNumber,
    std::size_t graphicCoordinatesDataNumber,
    GraphicCoordinatesData graphicCoordinatesData
)
{
    m_pimpl->getOrCreateDataSet(
        0,
        {{kw::FiducialSetSequence::GetTag(), fiducialSetNumber
        }, {kw::FiducialSequence::GetTag(), fiducialNumber
            }, {kw::GraphicCoordinatesDataSequence::GetTag(), graphicCoordinatesDataNumber
            }
        }) = toGdcm(*m_pimpl, graphicCoordinatesData);
}

//------------------------------------------------------------------------------

void FiducialsSeries::appendGraphicCoordinatesData(
    std::size_t fiducialSetNumber,
    std::size_t fiducialNumber,
    GraphicCoordinatesData graphicCoordinatesData
)
{
    appendInSequence(
        *m_pimpl,
        kw::GraphicCoordinatesDataSequence::GetTag(),
        graphicCoordinatesData,
        {{kw::FiducialSetSequence::GetTag(), fiducialSetNumber
        }, {kw::FiducialSequence::GetTag(), fiducialNumber
            }
        });
}

//------------------------------------------------------------------------------

FiducialsSeries::ReferencedImage FiducialsSeries::getReferencedImage(
    std::size_t fiducialSetNumber,
    std::size_t fiducialNumber,
    std::size_t graphicCoordinatesDataNumber
) const noexcept
{
    std::optional<gdcm::DataSet> dataSet = m_pimpl->getDataSet(
        0,
        {{kw::FiducialSetSequence::GetTag(), fiducialSetNumber
        }, {kw::FiducialSequence::GetTag(), fiducialNumber
            },
            {kw::GraphicCoordinatesDataSequence::GetTag(), graphicCoordinatesDataNumber
            }, {kw::ReferencedImageSequence::GetTag(), 0
            }
        });
    if(!dataSet)
    {
        return {};
    }

    return toReferencedImage(*dataSet);
}

//------------------------------------------------------------------------------

void FiducialsSeries::setReferencedImage(
    std::size_t fiducialSetNumber,
    std::size_t fiducialNumber,
    std::size_t graphicCoordinatesDataNumber,
    ReferencedImage referencedImage
)
{
    m_pimpl->getOrCreateDataSet(
        0,
        {{kw::FiducialSetSequence::GetTag(), fiducialSetNumber
        }, {kw::FiducialSequence::GetTag(), fiducialNumber
            }, {kw::GraphicCoordinatesDataSequence::GetTag(), graphicCoordinatesDataNumber
            }, {kw::ReferencedImageSequence::GetTag(), 0
            }
        }) = toGdcm(*m_pimpl, referencedImage);
}

//------------------------------------------------------------------------------

std::string FiducialsSeries::getReferencedSOPClassUID(
    std::size_t fiducialSetNumber,
    std::size_t fiducialNumber,
    std::size_t graphicCoordinatesDataNumber
) const noexcept
{
    return m_pimpl->getValue<kw::ReferencedSOPClassUID>(
        0,
        {{kw::FiducialSetSequence::GetTag(), fiducialSetNumber
        }, {kw::FiducialSequence::GetTag(), fiducialNumber
            },
            {kw::GraphicCoordinatesDataSequence::GetTag(), graphicCoordinatesDataNumber
            }, {kw::ReferencedImageSequence::GetTag(), 0
            }
        }).value_or("");
}

//------------------------------------------------------------------------------

void FiducialsSeries::setReferencedSOPClassUID(
    std::size_t fiducialSetNumber,
    std::size_t fiducialNumber,
    std::size_t graphicCoordinatesDataNumber,
    const std::string& referencedSOPClassUID
)
{
    m_pimpl->setStringValue<kw::ReferencedSOPClassUID>(
        referencedSOPClassUID,
        0,
        {{kw::FiducialSetSequence::GetTag(), fiducialSetNumber
        }, {kw::FiducialSequence::GetTag(), fiducialNumber
            },
            {kw::GraphicCoordinatesDataSequence::GetTag(), graphicCoordinatesDataNumber
            }, {kw::ReferencedImageSequence::GetTag(), 0
            }
        });
}

//------------------------------------------------------------------------------

std::string FiducialsSeries::getReferencedSOPInstanceUID(
    std::size_t fiducialSetNumber,
    std::size_t fiducialNumber,
    std::size_t graphicCoordinatesDataNumber
) const noexcept
{
    return m_pimpl->getValue<kw::ReferencedSOPInstanceUID>(
        0,
        {{kw::FiducialSetSequence::GetTag(), fiducialSetNumber
        }, {kw::FiducialSequence::GetTag(), fiducialNumber
            },
            {kw::GraphicCoordinatesDataSequence::GetTag(), graphicCoordinatesDataNumber
            }, {kw::ReferencedImageSequence::GetTag(), 0
            }
        }).value_or("");
}

//------------------------------------------------------------------------------

void FiducialsSeries::setReferencedSOPInstanceUID(
    std::size_t fiducialSetNumber,
    std::size_t fiducialNumber,
    std::size_t graphicCoordinatesDataNumber,
    const std::string& referencedSOPInstanceUID
)
{
    m_pimpl->setStringValue<kw::ReferencedSOPInstanceUID>(
        referencedSOPInstanceUID,
        0,
        {{kw::FiducialSetSequence::GetTag(), fiducialSetNumber
        }, {kw::FiducialSequence::GetTag(), fiducialNumber
            },
            {kw::GraphicCoordinatesDataSequence::GetTag(), graphicCoordinatesDataNumber
            }, {kw::ReferencedImageSequence::GetTag(), 0
            }
        });
}

//------------------------------------------------------------------------------

std::vector<std::int32_t> FiducialsSeries::getReferencedFrameNumber(
    std::size_t fiducialSetNumber,
    std::size_t fiducialNumber,
    std::size_t graphicCoordinatesDataNumber
) const noexcept
{
    return m_pimpl->getValues<kw::ReferencedFrameNumber>(
        0,
        {{kw::FiducialSetSequence::GetTag(), fiducialSetNumber
        }, {kw::FiducialSequence::GetTag(), fiducialNumber
            },
            {kw::GraphicCoordinatesDataSequence::GetTag(), graphicCoordinatesDataNumber
            }, {kw::ReferencedImageSequence::GetTag(), 0
            }
        }).value_or(std::vector<std::int32_t> {});
}

//------------------------------------------------------------------------------

void FiducialsSeries::setReferencedFrameNumber(
    std::size_t fiducialSetNumber,
    std::size_t fiducialNumber,
    std::size_t graphicCoordinatesDataNumber,
    std::vector<std::int32_t> referencedFrameNumber
)
{
    m_pimpl->setValues<kw::ReferencedFrameNumber>(
        referencedFrameNumber,
        0,
        {{kw::FiducialSetSequence::GetTag(), fiducialSetNumber
        }, {kw::FiducialSequence::GetTag(), fiducialNumber
            },
            {kw::GraphicCoordinatesDataSequence::GetTag(), graphicCoordinatesDataNumber
            }, {kw::ReferencedImageSequence::GetTag(), 0
            }
        });
}

//------------------------------------------------------------------------------

std::vector<std::uint16_t> FiducialsSeries::getReferencedSegmentNumber(
    std::size_t fiducialSetNumber,
    std::size_t fiducialNumber,
    std::size_t graphicCoordinatesDataNumber
) const noexcept
{
    return m_pimpl->getValues<kw::ReferencedSegmentNumber>(
        0,
        {{kw::FiducialSetSequence::GetTag(), fiducialSetNumber
        }, {kw::FiducialSequence::GetTag(), fiducialNumber
            },
            {kw::GraphicCoordinatesDataSequence::GetTag(), graphicCoordinatesDataNumber
            }, {kw::ReferencedImageSequence::GetTag(), 0
            }
        }).value_or(std::vector<std::uint16_t> {});
}

//------------------------------------------------------------------------------

void FiducialsSeries::setReferencedSegmentNumber(
    std::size_t fiducialSetNumber,
    std::size_t fiducialNumber,
    std::size_t graphicCoordinatesDataNumber,
    std::vector<std::uint16_t> referencedSegmentNumber
)
{
    m_pimpl->setValues<kw::ReferencedSegmentNumber>(
        referencedSegmentNumber,
        0,
        {{kw::FiducialSetSequence::GetTag(), fiducialSetNumber
        }, {kw::FiducialSequence::GetTag(), fiducialNumber
            },
            {kw::GraphicCoordinatesDataSequence::GetTag(), graphicCoordinatesDataNumber
            }, {kw::ReferencedImageSequence::GetTag(), 0
            }
        });
}

//------------------------------------------------------------------------------

std::vector<FiducialsSeries::Point2> FiducialsSeries::getGraphicData(
    std::size_t fiducialSetNumber,
    std::size_t fiducialNumber,
    std::size_t graphicCoordinatesDataNumber
) const noexcept
{
    std::optional<std::vector<float> > graphicData = m_pimpl->getValues<kw::GraphicData>(
        0,
        {{kw::FiducialSetSequence::GetTag(), fiducialSetNumber
        },
            {kw::FiducialSequence::GetTag(), fiducialNumber
            },
            {kw::GraphicCoordinatesDataSequence::GetTag(), graphicCoordinatesDataNumber
            }
        });
    if(!graphicData)
    {
        return {};
    }

    return toPoint2(*graphicData);
}

//------------------------------------------------------------------------------

void FiducialsSeries::setGraphicData(
    std::size_t fiducialSetNumber,
    std::size_t fiducialNumber,
    std::size_t graphicCoordinatesDataNumber,
    const std::vector<FiducialsSeries::Point2>& graphicData
)
{
    m_pimpl->setValues<kw::GraphicData>(
        toFloats(graphicData),
        0,
        {{kw::FiducialSetSequence::GetTag(), fiducialSetNumber
        }, {kw::FiducialSequence::GetTag(), fiducialNumber
            },
            {kw::GraphicCoordinatesDataSequence::GetTag(), graphicCoordinatesDataNumber
            }
        });
}

//------------------------------------------------------------------------------

std::optional<std::string> FiducialsSeries::getFiducialUID(
    std::size_t fiducialSetNumber,
    std::size_t fiducialNumber
) const noexcept
{
    return m_pimpl->getValue<kw::FiducialUID>(
        0,
        {{kw::FiducialSetSequence::GetTag(), fiducialSetNumber
        }, {kw::FiducialSequence::GetTag(), fiducialNumber
            }
        });
}

//------------------------------------------------------------------------------

void FiducialsSeries::setFiducialUID(
    std::size_t fiducialSetNumber,
    std::size_t fiducialNumber,
    const std::optional<std::string>& fiducialUID
)
{
    m_pimpl->setStringValue<kw::FiducialUID>(
        fiducialUID.value_or(""),
        0,
        {{kw::FiducialSetSequence::GetTag(), fiducialSetNumber
        }, {kw::FiducialSequence::GetTag(), fiducialNumber
            }
        });
}

//------------------------------------------------------------------------------

std::vector<FiducialsSeries::Point3> FiducialsSeries::getContourData(
    std::size_t fiducialSetNumber,
    std::size_t fiducialNumber
) const noexcept
{
    std::optional<std::vector<double> > contourData = m_pimpl->getValues<kw::ContourData>(
        0,
        {{kw::FiducialSetSequence::GetTag(), fiducialSetNumber
        },
            {kw::FiducialSequence::GetTag(), fiducialNumber
            }
        });
    if(!contourData)
    {
        return {};
    }

    return toPoint3(*contourData);
}

//------------------------------------------------------------------------------

void FiducialsSeries::setContourData(
    std::size_t fiducialSetNumber,
    std::size_t fiducialNumber,
    const std::vector<FiducialsSeries::Point3>& contourData
)
{
    m_pimpl->setValues<kw::ContourData>(
        toFloats(contourData),
        0,
        {{kw::FiducialSetSequence::GetTag(), fiducialSetNumber
        }, {kw::FiducialSequence::GetTag(), fiducialNumber
            }
        });
}

//------------------------------------------------------------------------------

std::optional<std::string> FiducialsSeries::getGroupName(std::size_t fiducialSetNumber) const noexcept
{
    return m_pimpl->getPrivateValue(0, 0, {{kw::FiducialSetSequence::GetTag(), fiducialSetNumber}});
}

//------------------------------------------------------------------------------

void FiducialsSeries::setGroupName(std::size_t fiducialSetNumber, const std::string& groupName)
{
    m_pimpl->setPrivateValue(0, groupName, 0, {{kw::FiducialSetSequence::GetTag(), fiducialSetNumber}});
}

//------------------------------------------------------------------------------

FiducialsSeries::Shape FiducialsSeries::stringToShape(const std::optional<std::string>& string)
{
    static const std::map<std::string, Shape> stringToShapeMap {{"POINT", Shape::POINT
    }, {"LINE", Shape::LINE
        }, {"PLANE", Shape::POINT
        }, {"SURFACE", Shape::SURFACE
        }, {"RULER", Shape::RULER
        }, {"L_SHAPE", Shape::L_SHAPE
        }, {"T_SHAPE", Shape::T_SHAPE
        }, {"SHAPE", Shape::SHAPE
        }
    };
    if(!string || !stringToShapeMap.contains(*string))
    {
        return Shape::INVALID;
    }

    return stringToShapeMap.at(*string);
}

//------------------------------------------------------------------------------

FiducialsSeries::FiducialSet FiducialsSeries::toFiducialSet(const gdcm::DataSet& dataSet) const
{
    FiducialSet fiducialSet;
    if(auto ris = m_pimpl->getSequence(kw::ReferencedImageSequence::GetTag(), dataSet))
    {
        fiducialSet.referencedImageSequence = std::vector<ReferencedImage> {};
        for(std::size_t i = 1 ; i <= ris->GetNumberOfItems() ; i++) // GDCM Sequence of Items is 1-indexed
        {
            fiducialSet.referencedImageSequence->push_back(toReferencedImage(ris->GetItem(i).GetNestedDataSet()));
        }
    }

    fiducialSet.frameOfReferenceUID = m_pimpl->getValue<kw::FrameOfReferenceUID>(dataSet);
    if(auto fiducialSequence = m_pimpl->getSequence(kw::FiducialSequence::GetTag(), dataSet))
    {
        for(std::size_t i = 1 ; i <= fiducialSequence->GetNumberOfItems() ; i++) // GDCM Sequence of Items is 1-indexed
        {
            fiducialSet.fiducialSequence.push_back(toFiducial(fiducialSequence->GetItem(i).GetNestedDataSet()));
        }
    }

    fiducialSet.groupName = m_pimpl->getPrivateValue(0, dataSet);
    return fiducialSet;
}

//------------------------------------------------------------------------------

template<>
FiducialsSeries::FiducialSet FiducialsSeries::to<FiducialsSeries::FiducialSet>(const gdcm::DataSet& dataSet) const
{
    return toFiducialSet(dataSet);
}

//------------------------------------------------------------------------------

FiducialsSeries::ReferencedImage FiducialsSeries::toReferencedImage(const gdcm::DataSet& dataSet) const
{
    return ReferencedImage {
        .referencedSOPClassUID    = m_pimpl->getValue<kw::ReferencedSOPClassUID>(dataSet).value_or(""),
        .referencedSOPInstanceUID = m_pimpl->getValue<kw::ReferencedSOPInstanceUID>(dataSet).value_or(""),
        .referencedFrameNumber    = m_pimpl->getValues<kw::ReferencedFrameNumber>(dataSet).value_or(
            std::vector<std::int32_t> {
            }),
        .referencedSegmentNumber = m_pimpl->getValues<kw::ReferencedSegmentNumber>(dataSet).value_or(
            std::vector<std::uint16_t> {
            })
    };
}

//------------------------------------------------------------------------------

template<>
FiducialsSeries::ReferencedImage FiducialsSeries::to<FiducialsSeries::ReferencedImage>(const gdcm::DataSet& dataSet)
const
{
    return toReferencedImage(dataSet);
}

//------------------------------------------------------------------------------

FiducialsSeries::Fiducial FiducialsSeries::toFiducial(const gdcm::DataSet& dataSet) const
{
    Fiducial fiducial;
    fiducial.shapeType           = stringToShape(m_pimpl->getValue<kw::ShapeType>(dataSet));
    fiducial.fiducialDescription = m_pimpl->getValue<kw::FiducialDescription>(dataSet).value_or("");
    fiducial.fiducialIdentifier  = m_pimpl->getValue<kw::FiducialIdentifier>(dataSet).value_or("");
    if(auto gcds = m_pimpl->getSequence(kw::GraphicCoordinatesDataSequence::GetTag(), dataSet))
    {
        fiducial.graphicCoordinatesDataSequence = std::vector<GraphicCoordinatesData> {};
        for(std::size_t i = 1 ; i <= gcds->GetNumberOfItems() ; i++) // GDCM Sequence of Items is 1-indexed
        {
            fiducial.graphicCoordinatesDataSequence->push_back(
                toGraphicCoordinatesData(
                    gcds->GetItem(i).
                    GetNestedDataSet()
                )
            );
        }
    }

    fiducial.fiducialUID = m_pimpl->getValue<kw::FiducialUID>(dataSet);
    if(auto contourData = m_pimpl->getValues<kw::ContourData>(dataSet))
    {
        fiducial.contourData = toPoint3(*contourData);
    }

    return fiducial;
}

//------------------------------------------------------------------------------

template<>
FiducialsSeries::Fiducial FiducialsSeries::to<FiducialsSeries::Fiducial>(const gdcm::DataSet& dataSet) const
{
    return toFiducial(dataSet);
}

//------------------------------------------------------------------------------

FiducialsSeries::GraphicCoordinatesData FiducialsSeries::toGraphicCoordinatesData(const gdcm::DataSet& dataSet) const
{
    GraphicCoordinatesData gcd;
    if(auto ris = m_pimpl->getSequence(kw::ReferencedImageSequence::GetTag(), dataSet);
       ris != nullptr && ris->GetNumberOfItems() > 0)
    {
        // GDCM Sequence of Items is 1-indexed
        gcd.referencedImageSequence = toReferencedImage(ris->GetItem(1).GetNestedDataSet());
    }

    if(auto graphicData = m_pimpl->getValues<kw::GraphicData>(dataSet))
    {
        gcd.graphicData = toPoint2(*graphicData);
    }

    return gcd;
}

//------------------------------------------------------------------------------

template<>
FiducialsSeries::GraphicCoordinatesData FiducialsSeries::to<FiducialsSeries::GraphicCoordinatesData>(
    const gdcm::DataSet& dataSet
) const
{
    return toGraphicCoordinatesData(dataSet);
}

//------------------------------------------------------------------------------

std::vector<FiducialsSeries::Point2> FiducialsSeries::toPoint2(const std::vector<float>& floats)
{
    SIGHT_ASSERT("The number of elements must be a multiple of 2", (floats.size() % 2) == 0);
    std::vector<Point2> res;
    res.reserve(floats.size() / 2);
    for(std::size_t i = 0 ; i < floats.size() ; i += 2)
    {
        res.push_back({.x = floats[i], .y = floats[i + 1]});
    }

    return res;
}

//------------------------------------------------------------------------------

std::vector<FiducialsSeries::Point3> FiducialsSeries::toPoint3(const std::vector<double>& floats)
{
    SIGHT_ASSERT("The number of elements must be a multiple of 3", (floats.size() % 3) == 0);
    std::vector<Point3> res;
    res.reserve(floats.size() / 3);
    for(std::size_t i = 0 ; i < floats.size() ; i += 3)
    {
        res.push_back({.x = floats[i], .y = floats[i + 1], .z = floats[i + 2]});
    }

    return res;
}

//------------------------------------------------------------------------------

template<typename T>
std::optional<std::vector<T> > FiducialsSeries::toVector(gdcm::SmartPointer<gdcm::SequenceOfItems> sequence) const
{
    if(sequence == nullptr)
    {
        return std::nullopt;
    }

    std::vector<T> res;
    res.reserve(sequence->GetNumberOfItems());
    for(std::size_t i = 1 ; i <= sequence->GetNumberOfItems() ; i++) // GDCM Sequence of Items is 1-indexed
    {
        res.push_back(to<T>(sequence->GetItem(i).GetNestedDataSet()));
    }

    return res;
}

//------------------------------------------------------------------------------

template<typename T>
gdcm::SmartPointer<gdcm::SequenceOfItems> FiducialsSeries::toSequence(const std::optional<std::vector<T> >& vector)
const
{
    if(!vector)
    {
        return nullptr;
    }

    auto sequence = gdcm::SequenceOfItems::New();
    for(const T& element : *vector)
    {
        gdcm::Item item;
        item.SetNestedDataSet(toGdcm(*m_pimpl, element));
        sequence->AddItem(item);
    }

    return sequence;
}

//------------------------------------------------------------------------------

template<typename T>
gdcm::SmartPointer<gdcm::SequenceOfItems> FiducialsSeries::toSequence(const std::vector<T>& vector) const
{
    return toSequence(std::optional {vector});
}

} // namespace sight::data
