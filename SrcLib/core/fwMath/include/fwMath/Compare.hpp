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

#ifndef __FWMATH_COMPARE_HPP__
#define __FWMATH_COMPARE_HPP__

#include <cmath>

namespace fwMath
{

/**
 * @brief Returns true iff a and b are equal with 'epsilon' error margin
 * @param a value to compare
 * @param b value to compare
 * @param epsilon : error margin
 * @return true iff a and b are equal with 'epsilon' error margin
 */
template < typename TYPE >
bool isEqual(TYPE a, TYPE b, const float epsilon = 0.00001F)
{
    return fabs(a - b) < epsilon;
}

/**
 * @brief Returns true iff container a and b are equal with 'epsilon' error margin
 * @param containerA container to compare
 * @param containerB container to compare
 * @param epsilon : error margin
 * @return true iff container a and b are equal with 'epsilon' error margin
 */
template < typename CONTAINER >
bool isContainerEqual(CONTAINER&  containerA, CONTAINER& containerB, const float epsilon = 0.00001F)
{
    bool isEqual = true;
    if(containerA.size() != containerB.size())
    {
        return false;
    }

    typename CONTAINER::const_iterator iterA = containerA.begin();
    typename CONTAINER::const_iterator iterB = containerB.begin();

    while(isEqual && iterA != containerA.end())
    {
        isEqual = ::fwMath::isEqual(*iterA, *iterB, epsilon);
        ++iterA;
        ++iterB;
    }

    return isEqual;
}

} // namespace fwMath

#endif /* __FWMATH_COMPARE_HPP__ */
