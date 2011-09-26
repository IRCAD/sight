/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _SCENE2D_ADAPTOR_SIN_HPP_
#define _SCENE2D_ADAPTOR_SIN_HPP_

#include <scene2D/adaptor/IAdaptor.hpp>

#include "scene2D/config.hpp"

namespace scene2D
{
namespace adaptor
{


class SCENE2D_CLASS_API Sin : public ::scene2D::adaptor::IAdaptor
{

public:
    fwCoreServiceClassDefinitionsMacro ( (Sin)(::scene2D::adaptor::IAdaptor) ) ;

    SCENE2D_API Sin() throw();
    SCENE2D_API virtual ~Sin() throw();

protected:
    SCENE2D_API void configuring() throw (::fwTools::Failed);
    SCENE2D_API void doStart();
    SCENE2D_API void doUpdate();
    SCENE2D_API void doUpdate( fwServices::ObjectMsg::csptr _msg );
    SCENE2D_API void doSwap();
    SCENE2D_API void doStop();

private:
    float m_xMin, m_xMax;
};


} // namespace adaptor
} // namespace scene2D


#endif // _SCENE2D_ADAPTOR_SIN_HPP_

