//Dear emacs, this is -*- c++ -*-

/**
 * @file sys/src/xmlutil.cxx
 *
 * @brief Chooser between the different XML Parsing backends.
 */

#ifdef XERCES_XML_BACK_END
#include "xerces_xmlutil.hxx"
#else
#include "libxml2_xmlutil.hxx"
#endif
