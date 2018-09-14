
set( NAME fwItkIO )
set( VERSION 0.1 )
set( TYPE LIBRARY )
set( DEPENDENCIES fwCore fwData fwDataIO fwTools )
set( REQUIREMENTS  )
set( USE_PCH_FROM_TARGET pchData )
set( CONAN_DEPS
    libjpeg/9c@fw4spl/stable
    itk/4.13.0@fw4spl/stable
    zlib/1.2.11@fw4spl/stable
)