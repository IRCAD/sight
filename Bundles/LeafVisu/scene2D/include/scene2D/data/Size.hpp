/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _SCENE2D_DATA_SIZE_HPP_
#define _SCENE2D_DATA_SIZE_HPP_

#include <fwData/Object.hpp>

#include "scene2D/config.hpp"

namespace scene2D
{
namespace data
{

class SCENE2D_CLASS_API Size
{
public :

    SCENE2D_API Size(){m_w=0;m_h=0;}
    SCENE2D_API Size( double w , double h ){m_w=w;m_h=h;}
    SCENE2D_API double getWidth() const {return m_w;}
    SCENE2D_API void setWidth ( double w ){m_w=w;}
    SCENE2D_API double getHeight() const {return m_h;}
    SCENE2D_API void setHeight ( double h ){m_h=h;}

private :

    double m_w;
    double m_h;
};

} // namespace data
} // namespace scene2D


#endif // _SCENE2D_DATA_SIZE_HPP_

