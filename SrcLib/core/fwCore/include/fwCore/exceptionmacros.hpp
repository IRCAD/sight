#ifndef _FWCORE_EXCEPTION_MACRO_HPP_
# define _FWCORE_EXCEPTION_MACRO_HPP_

# include <boost/exception/exception.hpp>
# include <boost/throw_exception.hpp>

# include "fwCore/Exception.hpp"
# include "fwCore/spyLog.hpp"
# include "fwCore/Demangler.hpp"

# define LOG_FWEXCEPTIONS 1

# define __FWCORE_EXCEPTION_CLASS ::fwCore::Exception

# define __FWCORE_EXCEPTION_PREPARE_MSG(var, msg)       \
    std::stringstream var;                              \
    var << msg

# define __FWCORE_EXCEPTION_GETCLASSNAME(obj)           \
    ::fwCore::Demangler(typeid(obj)).getClassname()

# define __FWCORE_EXCEPTION_INFO(excep)         \
    "Exception: <"                              \
    << __FWCORE_EXCEPTION_GETCLASSNAME(excep)   \
    << ">: " << excep.what()

// --------------------------------------------------------------------------------

# ifdef LOG_FWEXCEPTIONS
#  define __FWCORE_EXCEPTION_LOG(excep)         \
    OSLM_WARN(__FWCORE_EXCEPTION_INFO(excep))
# else
# define __FWCORE_EXCEPTION_LOG(excep)
# endif

// --------------------------------------------------------------------------------

# define __FWCORE_EXCEPT_RAISE_EXCEPTION(excep) \
    __FWCORE_EXCEPTION_LOG(excep);              \
    BOOST_THROW_EXCEPTION(excep)

# define __FWCORE_EXCEPT_RAISE_EXCEPTION_MSG(excep_class, msg)          \
    __FWCORE_EXCEPTION_PREPARE_MSG(_fwcore_exception_msgstream, msg);   \
    __FWCORE_EXCEPT_RAISE_EXCEPTION(                                    \
        excep_class(_fwcore_exception_msgstream.str()))

# define __FWCORE_EXCEPT_RAISE(msg)                                     \
    __FWCORE_EXCEPT_RAISE_EXCEPTION_MSG(__FWCORE_EXCEPTION_CLASS, msg)

# define __FWCORE_EXCEPT_RAISE_EXCEPTION_IF(excep, cond)        \
    __FWCORE_IF(cond, __FWCORE_EXCEPT_RAISE_EXCEPTION(excep);)

# define __FWCORE_EXCEPT_RAISE_IF(msg, cond)            \
    __FWCORE_IF(cond, __FWCORE_EXCEPT_RAISE(msg);)

# define __FWCORE_EXCEPT_FORWARD_EXCEPTION(excep)       \
    BOOST_THROW_EXCEPTION(excep)

// -----------------------------------------------------------------------------

# ifdef FWEXCEPTIONS_AS_ASSERTS

#  undef __FWCORE_EXCEPT_RAISE_EXCEPTION
#  define __FWCORE_EXCEPT_RAISE_EXCEPTION(excep)        \
    OSLM_ASSERT(__FWCORE_EXCEPTION_INFO(excep), false)

#  undef __FWCORE_EXCEPT_FORWARD_EXCEPTION
#  define __FWCORE_EXCEPT_FORWARD_EXCEPTION(exception)          \
    OSLM_ASSERT(                                                \
        "[Forwarded] " << __FWCORE_EXCEPTION_INFO (excep),      \
        false)

# endif

// -----------------------------------------------------------------------------

# define FW_RAISE_EXCEPTION(excep)  __FWCORE_EXPR_BLOCK(        \
        __FWCORE_EXCEPT_RAISE_EXCEPTION(excep);                 \
        )

# define FW_RAISE_EXCEPTION_MSG(excep_class, msg) __FWCORE_EXPR_BLOCK(  \
        __FWCORE_EXCEPT_RAISE_EXCEPTION_MSG(excep_class, msg);          \
        )

# define FW_RAISE(msg) __FWCORE_EXPR_BLOCK(     \
        __FWCORE_EXCEPT_RAISE(msg);             \
        )

# define FW_RAISE_EXCEPTION_IF(excep, cond) __FWCORE_EXPR_BLOCK(        \
        __FWCORE_EXCEPT_RAISE_EXCEPTION_IF(excep, cond)                 \
        )

# define FW_RAISE_IF(msg, cond) __FWCORE_EXPR_BLOCK(    \
        __FWCORE_EXCEPT_RAISE_IF(msg, cond)             \
        )

# define FW_FORWARD_EXCEPTION(excep) __FWCORE_EXPR_BLOCK(       \
        __FWCORE_EXCEPT_FORWARD_EXCEPTION(excep);               \
        )

#endif  // _FWCORE_EXCEPTION_MACRO_HPP_
