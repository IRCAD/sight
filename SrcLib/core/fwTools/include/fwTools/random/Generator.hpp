/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2011.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWTOOLS_RANDOM_GENERATOR_HPP__
#define __FWTOOLS_RANDOM_GENERATOR_HPP__

#include <ctime>

#include <boost/random/variate_generator.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_real.hpp>
#include <boost/random/uniform_int.hpp>
#include <boost/type_traits/is_floating_point.hpp>
#include <boost/mpl/if.hpp>
#include <boost/type_traits/is_same.hpp>
#include <boost/concept_check.hpp>

#include <fwCore/base.hpp>

namespace fwTools
{
namespace random
{


/**
 * @brief On each invocation, it returns a random value uniformly distributed in the range [min..max).
 * @param min      The "min" parameter of the distribution.
 * @param max      The "max" parameter of the distribution.
 * @param seedVal  Seed for generator.
 * @pre   min <= max
 *
 * @return returns a random value uniformly distributed in the range [min..max)
 */
template <typename T>
T getValue(T min, T max, ::boost::uint32_t seedVal = std::time(NULL))
{
    SLM_ASSERT("Wrong min/max value", min <= max);
    typedef typename ::boost::mpl::if_<
            ::boost::is_floating_point<T>,
            ::boost::uniform_real<>,
            ::boost::uniform_int<>
    >::type DistroType;

    ::boost::mt19937 seed(seedVal);
    DistroType dist(min, max);
    ::boost::variate_generator< ::boost::mt19937&, DistroType > random(seed, dist);
    return random();
}

/**
 * @brief On each invocation, this method fills specified container with random values uniformly distributed in the range [min..max).
 * @param[in] min            The "min" parameter of the distribution.
 * @param[in] max            The "max" parameter of the distribution.
 * @param[out] randContainer Container to fill with random values.
 * @param[in] seedVal        Seed for generator.
 * @pre  min <= max
 * @pre  CONTAINER type same as template T
 */
template <typename T, typename CONTAINER>
void fillContainer(T min, T max, CONTAINER& randContainer, ::boost::uint32_t seedVal = std::time(NULL))
{
    SLM_ASSERT("Wrong min/max value", min <= max);
    SLM_ASSERT("Container type not same as T", (::boost::is_same< T, CONTAINER::value_type>::value) );
    typedef typename ::boost::mpl::if_<
            ::boost::is_floating_point<T>,
            ::boost::uniform_real<>,
            ::boost::uniform_int<>
    >::type DistroType;

    ::boost::mt19937 seed(seedVal);
    DistroType dist(min, max);
    ::boost::variate_generator< ::boost::mt19937&, DistroType > random(seed, dist);
    std::generate(randContainer.begin(), randContainer.end(), random);
}

} // namespace random
} // namespace fwTools

#endif //__FWTOOLS_RANDOM_GENERATOR_HPP__
