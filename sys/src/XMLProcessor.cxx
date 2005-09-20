//Dear emacs, this is -*- c++ -*-

/**
 * @file sys/src/XMLProcessor.cxx
 *
 * @brief Chooser between the different XML Parsing backends.
 */

#ifdef XERCES_XML_BACK_END
#include "xerces_XMLProcessor.hxx"
#else
#include "libxml2_XMLProcessor.hxx"
#endif
