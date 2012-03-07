/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwCore/base.hpp>

#include "fwData/registry/macros.hpp"

#include "fwData/DictionaryOrgan.hpp"

REGISTER_DATA( ::fwData::DictionaryOrgan );
namespace fwData
{
//------------------------------------------------------------------------------

DictionaryOrgan::DictionaryOrgan()
{
    m_organName = ::fwData::String::New();
    m_roi4OrganNativeExp = ::fwData::String::New();
    m_roi4OrganExp = ::fwData::String::New();

    m_bIsMedicalStructure = false;
    m_ui32AOColor = 0;
    m_dAvgVolume = 0;
    m_dVolStdDeviation = 0;
    m_ui32NbExams = 0;
    m_dRed = 0;
    m_dGreen = 0;
    m_dBlue = 0;
    m_dAlpha = 0;
    m_ui32AvgTriangleNb = 0;
    m_dSegMin = 0;
    m_dSegMax = 0;
    m_dSurfaceElasticity = 0;
    m_dSurfaceRupture = 0;
    m_dInteriorRupture = 0;
}

//------------------------------------------------------------------------------

DictionaryOrgan::~DictionaryOrgan()
{}

//------------------------------------------------------------------------------

} // end namespace fwData
