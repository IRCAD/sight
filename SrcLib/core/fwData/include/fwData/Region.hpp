/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWDATA_REGION_HPP_
#define _FWDATA_REGION_HPP_

#include <vector>

#include <boost/cstdint.hpp>

#include "fwData/Object.hpp"

namespace fwData
{

/**
 * @class       Region
 * @brief       Defines region, including origin and size
 * @author      IRCAD (Research and Development Team).
 * @date        2007-2009.
 */
class FWDATA_CLASS_API Region : public Object
{
public:
        fwCoreClassDefinitionsWithFactoryMacro( (Region)(::fwData::Object::Baseclass), (()), ::fwTools::Factory::New< Region >) ;

        /// Constructor
        FWDATA_API      Region();

        /**
         * @brief Constructor with specified dimension
         * @param[in] _dim dimension of the region
         */
        FWDATA_API      Region(boost::uint8_t  _dim);

        /**
         * @brief Destructor
         */
        FWDATA_API      virtual ~Region();

        fwGettersSettersDocMacro(Origin, vOrigin, std::vector<double>, the region origin);

        fwGettersSettersDocMacro(Size, vSize, std::vector< ::boost::int32_t >, the region size);

protected:

        //! Origin of the region
        std::vector<double> m_vOrigin;

        //! Size of the region (in terms of points)
        std::vector<boost::int32_t>  m_vSize;

};

}

#endif // _FWDATA_REGION_HPP_
