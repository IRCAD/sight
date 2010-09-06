/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */
#ifndef _FWCOMED_IMAGEGETTER_HXX_
#define _FWCOMED_IMAGEGETTER_HXX_

#include <fwData/Image.hpp>

namespace fwComEd
{

namespace helper
{



template < typename SERVICE >
::fwData::Image::sptr ImageGetter< SERVICE >::getImage()
{
    SERVICE *image = dynamic_cast< SERVICE* > (this);
     return image->template getObject< ::fwData::Image >();
}

} //namespace helper

} //namespace fwComEd



#endif // _FWCOMED_IMAGEGETTER_HXX_

