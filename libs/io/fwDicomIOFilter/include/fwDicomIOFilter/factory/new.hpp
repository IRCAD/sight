/************************************************************************
 *
 * Copyright (C) 2009-2017 IRCAD France
 * Copyright (C) 2012-2017 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#ifndef __FWDICOMIOFILTER_FACTORY_NEW_HPP__
#define __FWDICOMIOFILTER_FACTORY_NEW_HPP__

#include "fwDicomIOFilter/config.hpp"
#include "fwDicomIOFilter/registry/detail.hpp"

#include <string>

namespace fwDicomIOFilter
{

class IFilter;

namespace factory
{

template<class CLASSNAME > SPTR( CLASSNAME )  New();

/**
 * @brief Key class used to restrict access to Filter construction.
 * See http://www.drdobbs.com/184402053
 */
class Key
{
template<typename CLASSNAME>
friend SPTR( CLASSNAME ) fwDicomIOFilter::factory::New();

Key()
{
}
};

FWDICOMIOFILTER_API SPTR( ::fwDicomIOFilter::IFilter ) New( const ::fwDicomIOFilter::registry::KeyType & classname );

template<class CLASSNAME > SPTR( CLASSNAME )  New()
{
    SPTR(CLASSNAME) obj = std::make_shared< CLASSNAME >( Key() );
    return obj;
}

} // namespace factory

} // namespace fwDicomIOFilter

#endif /* __FWDICOMIOFILTER_FACTORY_NEW_HPP__ */

