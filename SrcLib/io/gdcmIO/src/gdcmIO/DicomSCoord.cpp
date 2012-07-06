/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2012-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwCore/spyLog.hpp>        // for SLM_TRACE_FUNC()

#include "gdcmIO/DicomSCoord.hpp"

namespace gdcmIO
{

//------------------------------------------------------------------------------

SCoord::SCoord():
        m_graphicData(1,0),
        m_graphicType(""),
        m_context()
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

SCoord::~SCoord()
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

unsigned int SCoord::getGraphicDataSize() const
{
    return this->m_graphicData.size();
}

//------------------------------------------------------------------------------

void SCoord::setGraphicData(const std::vector< float > & a_coords)
{
    const unsigned int nbCoords = a_coords.size();

    this->m_graphicData.resize(nbCoords);
    for (unsigned int i = 0; i < nbCoords; ++i)
        this->m_graphicData[i] = toGraphicData<float>( a_coords[i] );
}

//------------------------------------------------------------------------------

void SCoord::setGraphicData(const float * a_coords, const unsigned int length)
{
    this->m_graphicData.resize(length);
    for (unsigned int i = 0; i < length; ++i)
        this->m_graphicData[i] = toGraphicData<float>( a_coords[i] );
}

//------------------------------------------------------------------------------

float SCoord::operator[](const unsigned int i) const
{
    return m_graphicData[i];
}

}
