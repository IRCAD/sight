/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWDATA_EDGE_HPP__
#define __FWDATA_EDGE_HPP__

#include "fwData/config.hpp"
#include "fwData/factory/new.hpp"
#include "fwData/Object.hpp"

#include <utility>

fwCampAutoDeclareDataMacro((fwData)(Edge), FWDATA_API);

namespace fwData
{
/**
 * @brief This class defines an edge object.
 *
 * An edge is represented by a to identifier ("from" and "to") and a nature.
 */
class FWDATA_CLASS_API Edge : public ::fwData::Object
{
public:
    fwCoreClassDefinitionsWithFactoryMacro( (Edge)(::fwData::Object), (()), ::fwData::factory::New< Edge >);
    fwCampMakeFriendDataMacro((fwData)(Edge));

    FWDATA_API static std::string NATURE_FLOW;
    FWDATA_API static std::string NATURE_DATA;

    /**
     * @brief Constructor
     * @param key Private construction key
     */
    FWDATA_API Edge( ::fwData::Object::Key key );
    /// Destructor
    FWDATA_API virtual ~Edge();

    /// @brief do a shallow copy of edge
    FWDATA_API void shallowCopy( const Object::csptr& _source ) override;

    /// @brief do a deep copy of edge
    FWDATA_API void cachedDeepCopy(const Object::csptr& _source, DeepCopyCacheType& cache) override;

    /**
     * @brief Set the edge identifier ("ID_SIZEX" , ...)
     * @param[in] _fromPortIndentifier identifier of port "from"
     * @param[in] _toPortIndentifier identifier of port "to"
     */
    FWDATA_API void setIdentifiers(const std::string& _fromPortIndentifier, const std::string& _toPortIndentifier);

    /**
     * @brief Get the edge identifier
     *
     * @return std::pair <from,to>
     */
    FWDATA_API std::pair<std::string, std::string> getIdentifiers() const;

    /// @brief return "from" identifier
    FWDATA_API std::string getFromPortID() const;

    /// @brief return "to" identifier
    FWDATA_API std::string getToPortID() const;

    /// @brief return "to or from" identifier according to upStream
    FWDATA_API std::string getPortID(bool upStream) const;

    /// @brief Set edge nature ("FLOW" , "FWDATA")
    FWDATA_API void setNature(std::string nature);

    /// @brief Get edge nature ("FLOW" , "FWDATA")
    FWDATA_API const std::string& getNature() const;

protected:

    std::string m_fromPortIdentifier; // "ID_SIZEX" , ...
    std::string m_toPortIdentifier; // "ID_SIZEX" , ...
    std::string m_nature; // "FLOW" , "FWDATA"
};

} // namespace fwData

#endif // __FWDATA_EDGE_HPP__

