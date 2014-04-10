/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2014.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/foreach.hpp>
#include <fwCore/spyLog.hpp>
#include "gdcmIO/container/DicomSCoord.hpp"

namespace gdcmIO
{
namespace container
{

//------------------------------------------------------------------------------

DicomSCoord::DicomSCoord() :
        m_graphicData(1, 0),
        m_graphicType(""),
        m_context()
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

DicomSCoord::~DicomSCoord()
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

unsigned int DicomSCoord::getGraphicDataSize() const
{
    return this->m_graphicData.size();
}

//------------------------------------------------------------------------------

void DicomSCoord::setGraphicData(const std::vector< float > &coords)
{
    m_graphicData.clear();
    m_graphicData.resize(coords.size());

    unsigned int index = 0;
    BOOST_FOREACH(float value, coords)
    {
        this->m_graphicData[index++] = toGraphicData< float >(value);
    }

}

//------------------------------------------------------------------------------

void DicomSCoord::setGraphicData(const float * coords, const unsigned int length)
{
    m_graphicData.clear();
    m_graphicData.resize(length);

    for (unsigned int index = 0; index < length; ++index)
    {
        this->m_graphicData[index] = toGraphicData< float >(coords[index]);
    }
}

//------------------------------------------------------------------------------

float DicomSCoord::operator[](const unsigned int i) const
{
    return m_graphicData[i];
}

} //namespace container
} //namespace gdcmIO
