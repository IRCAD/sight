/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWRENDERQT_DATA_SIZE_HPP__
#define __FWRENDERQT_DATA_SIZE_HPP__

#include "fwRenderQt/config.hpp"

#include <fwData/Object.hpp>

namespace fwRenderQt
{
namespace data
{

class FWRENDERQT_CLASS_API Size
{
public:

    Size() :
        m_w(0.),
        m_h(0.)
    {
    }

    Size( double w, double h )
    {
        m_w = w; m_h = h;
    }

    //------------------------------------------------------------------------------

    double getWidth() const
    {
        return m_w;
    }

    //------------------------------------------------------------------------------

    void setWidth ( double w )
    {
        m_w = w;
    }

    //------------------------------------------------------------------------------

    double getHeight() const
    {
        return m_h;
    }

    //------------------------------------------------------------------------------

    void setHeight ( double h )
    {
        m_h = h;
    }

private:

    double m_w;
    double m_h;
};

} // namespace data
} // namespace fwRenderQt

#endif // __FWRENDERQT_DATA_SIZE_HPP__

