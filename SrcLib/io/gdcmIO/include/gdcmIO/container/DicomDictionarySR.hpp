/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __GDCMIO_CONTAINER_DICOMDICTIONARYSR_HPP__
#define __GDCMIO_CONTAINER_DICOMDICTIONARYSR_HPP__

#include <gdcmIO/config.hpp>

namespace gdcmIO
{
namespace container
{

/**
 * @brief   This class implement a dictionary for DICOM SR (Structured Report).
 *
 * Concept : Singleton
 *
 * @class   DicomDictionarySR.
 */
class GDCMIO_CLASS_API DicomDictionarySR
{
public:

    /// Define type of SR content
    typedef enum {
        LANDMARK = 0,
        DISTANCE,
        SRCONTENT_END
    } SRContent;

    /// Define type of value
    typedef enum {
        CONTAINER = 0,
        TEXT,
        SCOORD,
        IMAGE,
        NUM,
        SRTYPE_END
    } SRType;

    /// Define relationship with parent node
    typedef enum {
        CONTAINS = 0,
        HAS_PROPERTIES,
        INFERRED_FROM,
        SELECTED_FROM,
        SRRELATIONSHIP_END
    } SRRelationship;

    /// Define graphic type
    typedef enum {
        POINT = 0,
        MULTIPOINT,
        POLYLINE,
        SRGRAPHICTYPE_END
    } SRGraphicType;

    /// Define continuity of content
    typedef enum {
        SEPARATE = 0,
        CONTINUOUS,
        SRCONTINUITY_END
    } SRContinuity;

    //**    Getters for a concept name code sequence    **//
    static const char * getCodeValue(SRContent a_type);
    static const char * getCodeScheme(SRContent a_type);
    static const char * getCodeMeaning(SRContent a_type);

    /**
     * @brief   Get string of a type of value.
     */
    static const char * getTypeString(SRType a_type);

    /**
     * @brief   Get string of a relationship.
     */
    static const char * getRelationshipString(SRRelationship a_type);

    /**
     * @brief   Get string of a graphic type.
     */
    static const char * getGraphicTypeString(SRGraphicType a_type);

    /**
     * @brief   Get string of a continuity of content.
     */
    static const char * getContinuityString(SRContinuity a_type);

};

} //namespace container
} //namespace gdcmIO

#endif /* __GDCMIO_CONTAINER_DICOMDICTIONARYSR_HPP__ */