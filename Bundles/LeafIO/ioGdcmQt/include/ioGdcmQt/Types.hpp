/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2012-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _IOGDCMQT_TYPES_HPP_
#define _IOGDCMQT_TYPES_HPP_

namespace ioGdcm
{

typedef enum {
//         DCMTK,
//         GDCM,
         ITK_GDCM = 0,
         IRCAD,
         VTK_GDCM
    } DicomReader;

static DicomReader toDicomReader(int val)
{
    DicomReader enumVal;
    switch(val)
    {
    case 0:
        enumVal = ITK_GDCM;
        break;
    case 1:
        enumVal = IRCAD;
        break;
    case 2:
    default :
        enumVal = VTK_GDCM;
    }
    return enumVal;
}

}

#endif  // _IOGDCMQT_TYPES_HPP_
