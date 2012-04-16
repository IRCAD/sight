/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2011-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWMATH_COMPARE_HPP__
#define __FWMATH_COMPARE_HPP__

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
bool isEqual(TYPE a, TYPE b, const float epsilon = 0.00001)
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
bool isContainerEqual(CONTAINER&  containerA, CONTAINER& containerB, const float epsilon = 0.00001)
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
