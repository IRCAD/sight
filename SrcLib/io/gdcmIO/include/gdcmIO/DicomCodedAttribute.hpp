/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2012-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _GDCMIO_DICOMCODEDATTRIBUTE_HPP_
#define _GDCMIO_DICOMCODEDATTRIBUTE_HPP_

#include <string>

#include "gdcmIO/config.hpp"

namespace gdcmIO
{

/**
 * @brief   Structure for basic coded entry.
 *
 * It defines the identification of a coded content.
 *
 * @see     DICOM PS 3.3 Table 8.8-1
 *
 * @class   DicomCodedAttribute.
 * @author  IRCAD (Research and Development Team).
 * @date    2011.
 */
struct DicomCodedAttribute
{
    GDCMIO_API DicomCodedAttribute():
            CV(""),
            CSD(""),
            CSV(""),
            CM("")
    {

    }

    GDCMIO_API DicomCodedAttribute(const char * a_CV,
                                   const char * a_CSD,
                                   const char * a_CM,
                                   const char * a_CSV = ""):
                                   CV(a_CV),
                                   CSD(a_CSD),
                                   CSV(a_CSV),
                                   CM(a_CM)
    {


    }

    GDCMIO_API ~DicomCodedAttribute()
    {

    }

    std::string CV;    ///< Code Value             (see : Tag(0008,0100) )
    std::string CSD;   ///< Code Scheme Designator (see : Tag(0008,0102) )
    std::string CSV;   ///< Code Scheme Version    (see : Tag(0008,0103) )
    std::string CM;    ///< Code Meaning           (see : Tag(0008,0104) )

};

}

#endif /* _GDCMIO_DICOMCODEDATTRIBUTE_HPP_ */
