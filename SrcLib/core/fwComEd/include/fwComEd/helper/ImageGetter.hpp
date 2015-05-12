/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWCOMED_HELPER_IMAGEGETTER_HPP__
#define __FWCOMED_HELPER_IMAGEGETTER_HPP__

#include <vector>

#include <fwData/Image.hpp>

#include <fwServices/IService.hpp>

#include "fwComEd/config.hpp"


namespace fwComEd
{

namespace helper
{


template < typename SERVICE = ::fwServices::IService >
class FWCOMED_CLASS_API ImageGetter
{
public:
    fwCoreNonInstanciableClassDefinitionsMacro( (ImageGetter) );

    ::fwData::Image::sptr getImage();
};


} //namespace helper

} //namespace fwComEd

#include "fwComEd/helper/ImageGetter.hxx"

#endif // __FWCOMED_HELPER_IMAGEGETTER_HPP__

