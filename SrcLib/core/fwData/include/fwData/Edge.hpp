/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWDATA_EDGE_HPP_
#define _FWDATA_EDGE_HPP_

#include <utility>

#include "fwData/config.hpp"
#include "fwData/Object.hpp"

namespace fwData
{
/**
 * @class   Edge
 * @brief   This class defines an edge object.
 *
 * An edge is reprensented by a to identifier ("from" and "to") and a nature.
 *
 * @author  IRCAD (Research and Development Team).
 * @date    2007-2009.
 */
class FWDATA_CLASS_API Edge : public ::fwData::Object
{
public:
    fwCoreClassDefinitionsWithFactoryMacro( (Edge)(::fwData::Object), (()), ::fwTools::Factory::New< Edge >) ;

    FWDATA_API static std::string NATURE_FLOW;
    FWDATA_API static std::string NATURE_DATA;

    /// Constructor
    FWDATA_API Edge();
    /// Destructor
    FWDATA_API virtual ~Edge();


    /// @brief do a shallow copy of edge
    FWDATA_API void shallowCopy( Edge::csptr _source );

    /// @brief do a deep copy of edge
    FWDATA_API void deepCopy( Edge::csptr _source );

    /**
     * @brief Set the edge identifier ("ID_SIZEX" , ...)
     * @param[in] _fromPortIndentifier identifier of port "from"
     * @param[in] _toPortIndentifier identifier of port "to"
     */
    FWDATA_API void setIdentifiers(const std::string & _fromPortIndentifier, const std::string & _toPortIndentifier);

    /**
     * @brief Get the edge identifier
     *
     * @return std::pair <from,to>
     */
    FWDATA_API std::pair<std::string,std::string> getIdentifiers() const;

    /// @brief return "from" identifier
    FWDATA_API std::string getFromPortID() const;

    /// @brief return "to" identifier
    FWDATA_API std::string getToPortID() const;

    /// @brief return "to or from" identifier accroding to upStream
    FWDATA_API std::string getPortID(bool upStream) const;

    /// @brief Set edge nature ("FLOW" , "FWDATA")
    FWDATA_API void setNature(std::string nature);

    /// @brief Get edge nature ("FLOW" , "FWDATA")
    FWDATA_API const std::string &getNature() const;

protected :

    std::string m_fromPortIdentifier; // "ID_SIZEX" , ...
    std::string m_toPortIdentifier; // "ID_SIZEX" , ...
    std::string m_nature; // "FLOW" , "FWDATA"
};

} // namespace fwData

#endif // _FWDATA_EDGE_HPP_
