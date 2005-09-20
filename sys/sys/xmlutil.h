//Dear emacs, this is -*- c++ -*-

/**
 * @file sys/sys/xmlutil.h
 *
 * @brief Chooser between the different XML Parsing backends.
 */

#ifndef RINGER_SYS_XMLUTIL_H
#define RINGER_SYS_XMLUTIL_H

#ifdef XERCES_XML_BACK_END 
#include "sys/xerces_xmlutil.h"
#else
#include "sys/libxml2_xmlutil.h"
#endif

#endif /* RINGER_SYS_XMLUTIL_H */
