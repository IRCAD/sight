/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWDATA_DICTIONARY_HPP_
#define _FWDATA_DICTIONARY_HPP_

#include <map>

#include <boost/cstdint.hpp>

#include "fwData/config.hpp"
#include "fwData/Object.hpp"
#include "fwData/Factory.hpp"
#include "fwData/DictionaryOrgan.hpp"

namespace fwData
{
/**
 * @class   Dictionary
 * @brief   This class defines a dictionary which contains ::fwData::DictionaryOrgan
 * @author  IRCAD (Research and Development Team).
 * @date    2007-2009.
 */
//------------------------------------------------------------------------------

class FWDATA_CLASS_API Dictionary : public Object
{

public :
    fwCoreClassDefinitionsWithFactoryMacro( (Dictionary)(::fwData::Object), (()), ::fwData::Factory::New< Dictionary >) ;

    fwDataObjectMacro();

    /**
     * @brief Check if the organ is in dictionary
     * @param[in] _dictionaryOrganName name of the checked organ
     * @return True if the organ definition is in dictionary
     */
    FWDATA_API bool hasDictionaryOrgan( const std::string & _dictionaryOrganName ) const;

    /**
     * @brief Add or update organ
     *
     * If the organ definition is in dictionary the definition is updated
     * else the definition is added.
     *
     * @param[in] _dictionaryOrgan organ to be add or update.
     */
    FWDATA_API void setDictionaryOrgan( ::fwData::DictionaryOrgan::sptr _dictionaryOrgan );

    /**
     * @brief Get the definition of organ
     *
     * The shared_ptr is null if the definition does not exist.
     *
     * @param[in] _dictionaryOrganName name of the organ.
     * @return organ definition.
     */
    FWDATA_API ::fwData::DictionaryOrgan::sptr getDictionaryOrgan( const std::string & _dictionaryOrganName );

    typedef std::string DictionaryOrganNameType;
    typedef std::map< DictionaryOrganNameType, ::fwData::DictionaryOrgan::sptr > DictionaryOrganContainerType;

    /**
     * @brief Get/Set the organ map
     */
    fwDataGetSetCRefMacro(DictionaryOrgans, DictionaryOrganContainerType);

    /**
     * @brief Shallow/deep copy
     * @{
     */
    void shallowCopy( Dictionary::csptr _source );
    void deepCopy( Dictionary::csptr _source );
    /** @} */

protected:
    /// Constructor
    FWDATA_API Dictionary();

    /// Destructor
    FWDATA_API virtual ~Dictionary();

    DictionaryOrganContainerType m_attrDictionaryOrgans;
};

} // namespace fwData

#endif // _FWDATA_ORGAN_HPP_

