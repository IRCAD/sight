# ICONV_FOUND - system has Iconv 
# ICONV_INCLUDE_DIR - the Iconv include directory 
# ICONV_LIBRARIES - Link these to use Iconv 

find_path(ICONV_INCLUDE_DIR iconv.h) 
find_library(ICONV_LIBRARIES NAMES iconv libiconv libiconv-2 c)
 
if(ICONV_INCLUDE_DIR AND ICONV_LIBRARIES) 
    set(ICONV_FOUND TRUE) 
endif(ICONV_INCLUDE_DIR AND ICONV_LIBRARIES) 
