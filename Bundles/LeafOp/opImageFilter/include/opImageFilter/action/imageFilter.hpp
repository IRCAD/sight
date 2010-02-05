/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _OPIMAGEFILTER_ACTION_IMAGEFILTER_HPP_
#define _OPIMAGEFILTER_ACTION_IMAGEFILTER_HPP_

#include <gui/action/IAction.hpp>


#include "opImageFilter/config.hpp"

namespace opImageFilter
{

namespace action
{

class OPIMAGEFILTER_CLASS_API imageFilter : public ::gui::action::IAction
{

public :

        OPIMAGEFILTER_API imageFilter() throw() ;

        OPIMAGEFILTER_API virtual ~imageFilter() throw() ;

        OPIMAGEFILTER_API void starting() throw ( ::fwTools::Failed );

        OPIMAGEFILTER_API void stopping() throw ( ::fwTools::Failed );

        OPIMAGEFILTER_API void updating( fwServices::ObjectMsg::csptr _pMsg ) throw ( ::fwTools::Failed );

        OPIMAGEFILTER_API void configuring() throw ( ::fwTools::Failed );

        OPIMAGEFILTER_API void updating() throw ( ::fwTools::Failed );

        OPIMAGEFILTER_API void info ( std::ostream &_sstream ) ;

private :

        std::string m_image1UID;
        std::string m_image2UID;

};


} // namespace action
} // namespace opImageFilter


#endif // _OPIMAGEFILTER_ACTION_IMAGEFILTER_HPP_
