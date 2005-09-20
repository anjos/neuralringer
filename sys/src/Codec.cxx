//Dear emacs, this is -*- c++ -*-

/**
 * @file sys/src/Codec.cxx
 *
 * @brief Chooser between the different XML Parsing backends.
 */

#ifdef XERCES_XML_BACK_END
#else
#include "libxml2_Codec.hxx"
#endif
