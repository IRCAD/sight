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
#include "fwData/DictionaryOrgan.hpp"
#include "fwData/DownCastIterator.hpp"

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
    fwCoreClassDefinitionsWithFactoryMacro( (Dictionary)(::fwData::Object::Baseclass), (()), ::fwTools::Factory::New< Dictionary >) ;

    /// Constructor
    FWDATA_API Dictionary();

    /// Destructor
    FWDATA_API virtual ~Dictionary();

    //--------------------------------------------------------------------------
    /// Field identifier for organ dictionary
    static const Object::FieldID ID_DICTIONARY_ORGANS;

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

    // to scan dictionary organ
    typedef ContainerCaster< DictionaryOrgan >::iterator        DictionaryOrganIterator;
    typedef ContainerCaster< DictionaryOrgan >::const_iterator  DictionaryOrganConstIterator;

    /**@{
     * Get iterator on the first and the last organ.
     * @return std::pair( DictionaryOrgans.begin(), DictionaryOrgans.end() )
     */
    FWDATA_API std::pair< DictionaryOrganIterator, DictionaryOrganIterator > getDictionaryOrgans();
    FWDATA_API std::pair< DictionaryOrganConstIterator, DictionaryOrganConstIterator > getDictionaryOrgans() const;
    //@}
};

} // namespace fwData

#endif // _FWDATA_ORGAN_HPP_

