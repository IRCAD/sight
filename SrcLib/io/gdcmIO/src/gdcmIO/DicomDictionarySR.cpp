/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2012-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "gdcmIO/DicomDictionarySR.hpp"

namespace gdcmIO
{

static const char * CVStrings[] = {
        "121071",
        "121206",

        0
};

//static const char *CSStrings[] = {
//  "DCM",
//
//  0
//};

static const char * CMStrings[] = {
        "Finding",
        "Distance",

        0
};

static const char * TypeStrings[] = {
        "CONTAINER",
        "TEXT",
        "SCOORD",
        "IMAGE",
        "NUM",

        0
};

static const char * RelationshipStrings[] = {
        "CONTAINS",
        "HAS PROPERTIES",
        "INFERRED FROM",
        "SELECTED FROM",

        0
};

static const char * GraphicTypeStrings[] = {
        "POINT",
        "MULTIPOINT",
        "POLYLINE",

        0
};

static const char * ContinuityStrings[] = {
        "SEPARATE",
        "CONTINUOUS",

        0
};

//------------------------------------------------------------------------------

const char * DicomDictionarySR::getCodeValue(SRContent a_type)
{
    assert( a_type <= SRCONTENT_END );
    return CVStrings[(unsigned int)a_type];
}

//------------------------------------------------------------------------------

const char * DicomDictionarySR::getCodeScheme(SRContent a_type)
{
    assert( a_type <= SRCONTENT_END );
    return "DCM";   // TODO : adapt if SRContent has others coding schemes
}

//------------------------------------------------------------------------------

const char * DicomDictionarySR::getCodeMeaning(SRContent a_type)
{
    assert( a_type <= SRCONTENT_END );
    return CMStrings[(unsigned int)a_type];
}

//------------------------------------------------------------------------------

const char * DicomDictionarySR::getTypeString(SRType a_type)
{
    assert( a_type <= SRTYPE_END );
    return TypeStrings[(unsigned int)a_type];
}

//------------------------------------------------------------------------------

const char * DicomDictionarySR::getRelationshipString(SRRelationship a_type)
{
    assert( a_type <= SRRELATIONSHIP_END );
    return RelationshipStrings[(unsigned int)a_type];
}

//------------------------------------------------------------------------------

const char * DicomDictionarySR::getGraphicTypeString(SRGraphicType a_type)
{
    assert( a_type <= SRGRAPHICTYPE_END );
    return GraphicTypeStrings[(unsigned int)a_type];
}

//------------------------------------------------------------------------------

const char * DicomDictionarySR::getContinuityString(SRContinuity a_type)
{
    assert( a_type <= SRCONTINUITY_END );
    return ContinuityStrings[(unsigned int)a_type];
}

}  // namespace gdcmIO
