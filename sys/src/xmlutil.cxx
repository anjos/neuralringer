//Dear emacs, this is -*- c++ -*-

/**
 * @file sys/src/xmlutil.cxx
 *
 * Implements a set of utilities to parse XML configuration files.
 */

#include "sys/xmlutil.h"
#include "sys/Codec.h"
#include "sys/ustring.h"
#include "sys/util.h"
#include "sys/debug.h"
#include <string>
#include <sstream>

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

xmlNodePtr sys::get_next_element (const xmlNodePtr node)
{
  xmlNodePtr retval = node->next;
  while (retval && retval->type != XML_ELEMENT_NODE) retval = retval->next;
  return retval;
}

std::string sys::get_element_name (const xmlNodePtr node)
{
  return sys::default_codec.transcode(node->name);
}

std::string sys::get_element_string (const xmlNodePtr node)
{
  RINGER_DEBUG3("Getting string on element \"" << node->name 
	      << "\" at line " << node->line);
  xmlNodePtr text = node->children;
  if (text->type == XML_TEXT_NODE)
    return sys::default_codec.transcode(xmlNodeGetContent(text));
  RINGER_DEBUG1("This node does NOT seem to be a text node.");
  return std::string("");
}

unsigned int sys::get_element_uint (const xmlNodePtr node)
{
  std::string tmp = normalise(get_element_string(node));
  return strtoul(tmp.c_str(), 0, 0);
}

time_t sys::get_element_date (const xmlNodePtr node)
{
  std::string tmp = normalise(get_element_string(node));
  return sys::iso8601totime(tmp);
}

double sys::get_element_double (const xmlNodePtr node)
{
  std::string tmp = normalise(get_element_string(node));
  return strtod(tmp.c_str(), 0);
}

void sys::get_element_doubles (const xmlNodePtr node, std::vector<double>& v)
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

std::string sys::get_attribute_string (const xmlNodePtr node,
				       const std::string& name)
{
  xmlChar* temp = 
    xmlGetProp(node, sys::default_codec.transcode(name).c_str());
  if (!temp) return "";
  sys::ustring retval = temp;
  xmlFree(temp);
  return sys::normalise(sys::default_codec.transcode(retval));
}

unsigned sys::get_attribute_uint (const xmlNodePtr node,
				  const std::string& name)
{
  std::string c = normalise(get_attribute_string(node, name));
  return strtoul(c.c_str(), 0, 0);
}

double sys::get_attribute_double (const xmlNodePtr node,
				  const std::string& name)
{
  std::string c = normalise(get_attribute_string(node, name));
  return strtod(c.c_str(), 0);
}

xmlNodePtr sys::make_node (const std::string& name)
{
  return xmlNewNode(NULL, sys::default_codec.transcode(name).c_str());
}

xmlNodePtr sys::put_element (xmlNodePtr parent, const std::string& name)
{
  xmlNodePtr node = sys::make_node(name);
  xmlAddChild(parent, node);
  return node;
}

xmlNodePtr sys::put_element_text (xmlNodePtr parent, const std::string& name,
				  const std::string& content)
{
  xmlNodePtr node = put_element(parent, name);
  xmlNodePtr text = 
    xmlNewText(sys::default_codec.transcode(content).c_str());
  xmlAddChild (node, text);
  return node;
}

xmlNodePtr sys::put_element_date (xmlNodePtr parent, const std::string& name,
				  const time_t& content)
{
  return put_element_text(parent, name, sys::timetoiso8601(&content));
}

xmlNodePtr sys::put_element_double (xmlNodePtr parent, const std::string& name,
				    const double& content)
{
  std::ostringstream os;
  os << content;
  return put_element_text(parent, name, os.str());
}

xmlNodePtr sys::put_attribute_text (xmlNodePtr e, const std::string& name,
				    const std::string& value)
{
  sys::ustring xml_name = sys::default_codec.transcode(name);
  sys::ustring xml_value = sys::default_codec.transcode(value);
  xmlNewProp(e, xml_name.c_str(), xml_value.c_str());
  return e;
}

xmlNodePtr sys::put_attribute_uint (xmlNodePtr e, const std::string& name,
				    unsigned int value)
{
  std::ostringstream os;
  os << value;
  return put_attribute_text(e, name, os.str());
}

xmlNodePtr sys::put_attribute_double (xmlNodePtr e, const std::string& name,
					 const double& value)
{
  std::ostringstream os;
  os << value;
  return put_attribute_text(e, name, os.str());
}

xmlNodePtr sys::put_element_doubles (xmlNodePtr root, const std::string& name,
				     const std::vector<double>& content)
{
  xmlNodePtr entry = put_element(root, name);
  std::ostringstream oss;
  for (size_t i=0; i<content.size()-1; ++i) oss << content[i] << " ";
  oss << content[content.size()-1];
  xmlNodePtr text = 
    xmlNewText(sys::default_codec.transcode(oss.str()).c_str());
  xmlAddChild (entry, text);
  return entry;
}
