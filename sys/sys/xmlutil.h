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

namespace sys {

  typedef xercesc::DOMElement* xml_ptr;
  typedef const xercesc::DOMElement* xml_ptr_const;

}

#else
#include "sys/libxml2_xmlutil.h"

namespace sys {

  typedef xmlNodePtr xml_ptr;
  typedef const xmlNodePtr xml_ptr_const;

}

#endif

#endif /* RINGER_SYS_XMLUTIL_H */
