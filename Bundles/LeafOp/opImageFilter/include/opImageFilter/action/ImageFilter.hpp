/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __OPIMAGEFILTER_ACTION_IMAGEFILTER_HPP__
#define __OPIMAGEFILTER_ACTION_IMAGEFILTER_HPP__

#include <fwGui/IActionSrv.hpp>

#include "opImageFilter/config.hpp"

namespace opImageFilter
{

namespace action
{

class OPIMAGEFILTER_CLASS_API ImageFilter : public ::fwGui::IActionSrv
{

public:

    fwCoreServiceClassDefinitionsMacro ( (ImageFilter)(::fwGui::IActionSrv) );

    OPIMAGEFILTER_API ImageFilter() throw();

    OPIMAGEFILTER_API virtual ~ImageFilter() throw();

protected:

    OPIMAGEFILTER_API void starting() throw ( ::fwTools::Failed );

    OPIMAGEFILTER_API void stopping() throw ( ::fwTools::Failed );

    OPIMAGEFILTER_API void receiving( ::fwServices::ObjectMsg::csptr _pMsg ) throw ( ::fwTools::Failed );

    OPIMAGEFILTER_API void configuring() throw ( ::fwTools::Failed );

    OPIMAGEFILTER_API void updating() throw ( ::fwTools::Failed );

    OPIMAGEFILTER_API void info ( std::ostream &_sstream );

private:

    std::string m_image1UID;
    std::string m_image2UID;

};


} // namespace action
} // namespace opImageFilter


#endif // __OPIMAGEFILTER_ACTION_IMAGEFILTER_HPP__
