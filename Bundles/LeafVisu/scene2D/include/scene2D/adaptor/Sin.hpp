/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __SCENE2D_ADAPTOR_SIN_HPP__
#define __SCENE2D_ADAPTOR_SIN_HPP__

#include "scene2D/adaptor/IAdaptor.hpp"
#include "scene2D/config.hpp"

namespace scene2D
{
namespace adaptor
{


class SCENE2D_CLASS_API Sin : public ::scene2D::adaptor::IAdaptor
{

public:
    fwCoreServiceClassDefinitionsMacro ( (Sin)(::scene2D::adaptor::IAdaptor) );

    SCENE2D_API Sin() noexcept;
    SCENE2D_API virtual ~Sin() noexcept;

protected:
    SCENE2D_API void configuring();
    SCENE2D_API void doStart();
    SCENE2D_API void doUpdate();
    SCENE2D_API void doSwap();
    SCENE2D_API void doStop();

private:
    float m_xMin, m_xMax;
};


} // namespace adaptor
} // namespace scene2D


#endif // __SCENE2D_ADAPTOR_SIN_HPP__

