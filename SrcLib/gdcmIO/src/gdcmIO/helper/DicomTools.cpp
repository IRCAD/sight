/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2014.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <cmath>

#include <gdcmGlobal.h>
#include <gdcmDicts.h>
#include <gdcmPhotometricInterpretation.h>
#include <gdcmPixelFormat.h>
#include <gdcmImageApplyLookupTable.h>
#include <gdcmScanner.h>
#include <gdcmRescaler.h>

#include <boost/foreach.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/classification.hpp>

#include <boost/bimap/bimap.hpp>
#include <boost/assign/list_of.hpp>

#include <fwCore/base.hpp>

#include <fwTools/Dispatcher.hpp>
#include <fwTools/IntrinsicTypes.hpp>
#include <fwTools/DynamicTypeKeyTypeMapping.hpp>

#include <fwComEd/helper/Array.hpp>

#include "gdcmIO/helper/DicomTools.hpp"

namespace gdcmIO
{
namespace helper
{

//------------------------------------------------------------------------------

const char* DicomTools::convertToPresentationTypeString(::fwData::Material::REPRESENTATION_MODE representationMode)
{
    switch (representationMode)
    {
    case ::fwData::Material::MODE_SURFACE:
        return "SURFACE";
        break;
    case ::fwData::Material::MODE_POINT:
        return "POINTS";
        break;
    case ::fwData::Material::MODE_WIREFRAME:
        return "WIREFRAME";
        break;
    default:
        OSLM_WARN( "Presentation type not handle (changed to : SURFACE)" );
        return "SURFACE";
    }
}

//------------------------------------------------------------------------------

} //namespace helper

} //namespace gdcmIO


