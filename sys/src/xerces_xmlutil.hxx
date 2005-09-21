//Dear emacs, this is -*- c++ -*-

/**
 * @file sys/src/xerces_xmlutil.cxx
 *
 * Implements a set of utilities to parse XML configuration files.
 */

#include <sstream>
#include "sys/xerces_xmlutil.h"
#include "sys/util.h"
#include "sys/debug.h"
#include <xercesc/util/XMLString.hpp>
#include <xercesc/dom/DOMDocument.hpp>
#include <xercesc/dom/DOMText.hpp>
#include <xercesc/dom/DOMNodeList.hpp>

std::string sys::normalise (const std::string& s)
{
  std::string retval;
  size_t start = 0;
  while (start < s.length()) {
    while (start < s.length() && std::isspace(s[start])) ++start;
    if (start >= s.length()) break;
    size_t end = start+1; //next
    while (!std::isspace(s[end])) {
      if (end == s.length()) break;
      ++end;
    }
    retval += s.substr(start, end-start) + " ";
    start = end+1;
  }
  retval = retval.substr(0, retval.length()-1); //remove last space
  return retval;
}

void sys::tokenize (const std::string& s, std::vector<std::string>& tokens)
{
  size_t start = 0;
  while (start < s.length()) {
    while (start < s.length() && std::isspace(s[start])) ++start;
    if (start >= s.length()) break;
    size_t end = start+1; //next
    while (!std::isspace(s[end])) {
      if (end == s.length()) break;
      ++end;
    }
    tokens.push_back(s.substr(start, end-start));
    start = end+1;
  }
}

const xercesc::DOMElement* sys::get_next_element (const xercesc::DOMElement* node)
{
  const xercesc::DOMNode* retval = node->getNextSibling();
  while (retval && retval->getNodeType() != xercesc::DOMElement::ELEMENT_NODE)
    retval = retval->getNextSibling();
  return dynamic_cast<const xercesc::DOMElement*>(retval);
}

const xercesc::DOMElement* sys::get_first_child (const xercesc::DOMElement* top)
{
  if (!top->hasChildNodes()) return 0;
  const xercesc::DOMNode* retval = top->getFirstChild();
  while (retval && retval->getNodeType() != xercesc::DOMElement::ELEMENT_NODE)
    retval = retval->getNextSibling();
  return dynamic_cast<const xercesc::DOMElement*>(retval);
}

std::string sys::get_element_name (const xercesc::DOMElement* node)
{
  return xercesc::XMLString::transcode(node->getTagName());
}

std::string sys::get_element_string (const xercesc::DOMElement* node)
{
  RINGER_DEBUG3("Getting string on element \"" 
		<< xercesc::XMLString::transcode(node->getTagName()) << "\".");
  if (node->getChildNodes()->item(0)->getNodeType() == 
      xercesc::DOMElement::TEXT_NODE)
    return xercesc::XMLString::transcode
      (node->getChildNodes()->item(0)->getNodeValue());
  RINGER_DEBUG1("This node does NOT seem to be a text node.");
  return std::string("");
}

unsigned int sys::get_element_uint (const xercesc::DOMElement* node)
{
  std::string tmp = normalise(get_element_string(node));
  return strtoul(tmp.c_str(), 0, 0);
}

time_t sys::get_element_date (const xercesc::DOMElement* node)
{
  std::string tmp = normalise(get_element_string(node));
  return sys::iso8601totime(tmp);
}

double sys::get_element_double (const xercesc::DOMElement* node)
{
  std::string tmp = normalise(get_element_string(node));
  return strtod(tmp.c_str(), 0);
}

void sys::get_element_doubles (const xercesc::DOMElement* node, 
			       std::vector<double>& v)
{
  //A hack to workaround libxml2 (2.6.8) limitations
  std::vector<std::string> tokens;
  sys::tokenize(get_element_string(node), tokens);
  for (std::vector<std::string>::const_iterator it = tokens.begin();
       it != tokens.end(); ++it) {
    RINGER_DEBUG3("Converting token \"" << *it << "\" to double.");
    v.push_back(std::strtod(it->c_str(), 0));
  }
}

std::string sys::get_attribute_string (const xercesc::DOMElement* node,
				       const std::string& name)
{
  return xercesc::XMLString::transcode
    (node->getAttribute(xercesc::XMLString::transcode(name.c_str())));
}

unsigned sys::get_attribute_uint (const xercesc::DOMElement* node,
				  const std::string& name)
{
  std::string c = normalise(get_attribute_string(node, name));
  return strtoul(c.c_str(), 0, 0);
}

double sys::get_attribute_double (const xercesc::DOMElement* node,
				  const std::string& name)
{
  std::string c = normalise(get_attribute_string(node, name));
  return strtod(c.c_str(), 0);
} 

xercesc::DOMElement* sys::make_node (xercesc::DOMElement* any,
				     const std::string& name)
{
  xercesc::DOMDocument* doc = any->getOwnerDocument();
  return doc->createElement(xercesc::XMLString::transcode(name.c_str()));
}

xercesc::DOMElement* sys::put_node (xercesc::DOMElement* top, 
				    xercesc::DOMElement* child)
{
  top->appendChild(child);
  return top;
}

xercesc::DOMElement* sys::put_element (xercesc::DOMElement* parent, 
				       const std::string& name)
{
  xercesc::DOMElement* node = sys::make_node(parent, name);
  return sys::put_node(parent, node);
}

xercesc::DOMElement* sys::put_element_text (xercesc::DOMElement* parent, 
					    const std::string& name,
					    const std::string& content)
{
  xercesc::DOMDocument* doc = parent->getOwnerDocument();
  xercesc::DOMElement* node = sys::put_element(parent, name);
  xercesc::DOMText* new_text = 
    doc->createTextNode(xercesc::XMLString::transcode(content.c_str()));
  node->appendChild(new_text);
  return node;
}

xercesc::DOMElement* sys::put_element_date (xercesc::DOMElement* parent, 
					    const std::string& name,
					    const time_t& content)
{
  return put_element_text(parent, name, sys::timetoiso8601(&content));
}

xercesc::DOMElement* sys::put_element_double (xercesc::DOMElement* parent, 
					      const std::string& name,
					      const double& content)
{
  std::ostringstream os;
  os << content;
  return put_element_text(parent, name, os.str());
}

xercesc::DOMElement* sys::put_attribute_text (xercesc::DOMElement* e, 
					      const std::string& name,
					      const std::string& value)
{
  e->setAttribute(xercesc::XMLString::transcode(name.c_str()),
		  xercesc::XMLString::transcode(value.c_str()));
  return e;
}

xercesc::DOMElement* sys::put_attribute_uint (xercesc::DOMElement* e, 
					      const std::string& name,
					      unsigned int value)
{
  std::ostringstream os;
  os << value;
  return put_attribute_text(e, name, os.str());
}

xercesc::DOMElement* sys::put_attribute_double (xercesc::DOMElement* e, 
						const std::string& name,
						const double& value)
{
  std::ostringstream os;
  os << value;
  return put_attribute_text(e, name, os.str());
}

xercesc::DOMElement* sys::put_element_doubles (xercesc::DOMElement* root, 
					       const std::string& name,
					       const std::vector<double>& content)
{
  std::ostringstream oss;
  for (size_t i=0; i<content.size()-1; ++i) oss << content[i] << " ";
  oss << content[content.size()-1];
  return put_element_text(root, name, oss.str());
}

