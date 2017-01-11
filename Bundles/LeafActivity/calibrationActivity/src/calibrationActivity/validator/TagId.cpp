/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "calibrationActivity/validator/TagId.hpp"

#include <fwActivities/validator/registry/macros.hpp>

#include <fwData/Composite.hpp>
#include <fwData/String.hpp>
#include <fwData/Vector.hpp>

#include <boost/tokenizer.hpp>
#include <boost/lexical_cast.hpp>

#include <vector>


namespace calibrationActivity
{
namespace validator
{

fwActivitiesValidatorRegisterMacro(::calibrationActivity::validator::TagId, "::calibrationActivity::validator::TagId");

//-----------------------------------------------------------------------------

TagId::TagId(::fwActivities::IValidator::Key key)
{
}

//-----------------------------------------------------------------------------

TagId::~TagId()
{
}

//-----------------------------------------------------------------------------

::fwActivities::IValidator::ValidationType TagId::validate(const CSPTR(::fwData::Object) &currentData ) const
{
    IValidator::ValidationType validation;

    validation.first  = true;
    validation.second = "";

    ::fwData::Vector::csptr vector       = ::fwData::Vector::dynamicConstCast(currentData);
    ::fwData::Composite::csptr composite = ::fwData::Composite::dynamicConstCast(currentData);
    ::fwData::String::csptr str          = ::fwData::String::dynamicConstCast(currentData);

    std::vector< std::string > tagList;

    if (str)
    {
        tagList.push_back(str->value());
    }
    else if (vector)
    {
        for (const auto & obj : vector->getContainer())
        {
            ::fwData::String::csptr str = ::fwData::String::dynamicConstCast(obj);
            if (!str)
            {
                validation.first  = false;
                validation.second = "The current data does not contain String.";
                return validation;
            }
            tagList.push_back(str->value());
        }
    }
    else if (composite)
    {
        for (const auto & obj : composite->getContainer())
        {
            ::fwData::String::csptr str = ::fwData::String::dynamicConstCast(obj.second);
            if (!str)
            {
                validation.first  = false;
                validation.second = "The current data does not contain String.";
                return validation;
            }
            tagList.push_back(str->value());
        }
    }

    if (tagList.empty())
    {
        validation.first  = false;
        validation.second = "The current data does not contain String.";
    }
    else
    {
        for (const std::string & str : tagList)
        {
            ::boost::tokenizer<> tok(str);
            if (tok.begin() == tok.end())
            {
                validation.first  = false;
                validation.second = "The current list '"+ str +"' does not contain valid tag id. "
                                    "Example: 101, 102, 103";
                return validation;
            }
            for( ::boost::tokenizer<>::iterator it = tok.begin(); it!=tok.end(); ++it)
            {
                try
                {
                    const int id = ::boost::lexical_cast< int >(*it);

                    if (id < 0 || id >= 1024)
                    {
                        validation.first  = false;
                        validation.second = "The current list '"+ str +"' does not contain valid tag id. "
                                            "Id should be between 0 and 1023. Example: 101, 102, 103";
                        return validation;
                    }
                }
                catch (std::exception &)
                {
                    validation.first  = false;
                    validation.second = "The current list '"+ str +"' does not contain valid tag id. "
                                        "Example: 101, 102, 103";
                    return validation;
                }
            }
        }
    }

    return validation;
}

//-----------------------------------------------------------------------------

} // namespace validator
} // namespace calibrationActivity



