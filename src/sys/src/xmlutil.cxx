//Dear emacs, this is -*- c++ -*-

/**
 * @file sys/src/xmlutil.cxx
 *
 * @brief Chooser between the different XML Parsing backends.
 */

#include "sys/xmlutil.h"
#include "sys/libxml2_ustring.h"
#include "sys/libxml2_Codec.h"
#include "sys/util.h"
#include "sys/debug.h"
#include <string>
#include <sstream>
#include <cstring>
#include <cstdlib>

sys::xml_ptr_const sys::get_next_element (sys::xml_ptr_const node)
{
  sys::xml_ptr retval = node->next;
  while (retval && !sys::is_element(retval)) retval = retval->next;
  return retval;
}

sys::xml_ptr_const sys::get_first_child (sys::xml_ptr_const top)
{
  sys::xml_ptr_const retval = top->children;
  if (retval && !sys::is_element(retval)) retval = sys::get_next_element(retval);
  return retval;
}

bool sys::is_element (sys::xml_ptr_const node)
{
  return node->type == XML_ELEMENT_NODE;
}

std::string sys::get_element_name (sys::xml_ptr_const node)
{
  return sys::default_codec.transcode(node->name);
}

std::string sys::get_element_string (sys::xml_ptr_const node)
{
  RINGER_DEBUG3("Getting string on element \"" << node->name << "\".");
  sys::xml_ptr_const text = node->children;
  if (text->type == XML_TEXT_NODE)
    return sys::default_codec.transcode
	    (xmlNodeGetContent(const_cast<sys::xml_ptr>(text)));
  RINGER_DEBUG1("This node does NOT seem to be a text node.");
  return std::string("");
}

std::string sys::get_attribute_string (sys::xml_ptr_const node,
				       const std::string& name)
{
  xmlChar* temp = 
    xmlGetProp(const_cast<sys::xml_ptr>(node), 
		    sys::default_codec.transcode(name).c_str());
  if (!temp) return "";
  sys::ustring retval = temp;
  xmlFree(temp);
  return sys::normalise(sys::default_codec.transcode(retval));
}

sys::xml_ptr sys::make_node (sys::xml_ptr, const std::string& name)
{
  return xmlNewNode(NULL, sys::default_codec.transcode(name).c_str());
}

sys::xml_ptr sys::put_node (sys::xml_ptr top, sys::xml_ptr child)
{
  xmlAddChild(top, child);
  return child;
}

sys::xml_ptr sys::put_element_text (sys::xml_ptr parent, const std::string& name,
				    const std::string& content)
{
  sys::xml_ptr node = put_element(parent, name);
  sys::xml_ptr text = 
    xmlNewText(sys::default_codec.transcode(content).c_str());
  xmlAddChild (node, text);
  return node;
}

sys::xml_ptr sys::put_attribute_text (sys::xml_ptr e, const std::string& name,
				      const std::string& value)
{
  sys::ustring xml_name = sys::default_codec.transcode(name);
  sys::ustring xml_value = sys::default_codec.transcode(value);
  xmlNewProp(e, xml_name.c_str(), xml_value.c_str());
  return e;
}
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

unsigned int sys::get_element_uint (sys::xml_ptr_const node)
{
  std::string tmp = normalise(get_element_string(node));
  return strtoul(tmp.c_str(), 0, 0);
}

time_t sys::get_element_date (sys::xml_ptr_const node)
{
  std::string tmp = normalise(get_element_string(node));
  return sys::iso8601totime(tmp);
}

double sys::get_element_double (sys::xml_ptr_const node)
{
  std::string tmp = normalise(get_element_string(node));
  return strtod(tmp.c_str(), 0);
}

void sys::get_element_doubles (sys::xml_ptr_const node, std::vector<double>& v)
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

unsigned sys::get_attribute_uint (sys::xml_ptr_const node,
				  const std::string& name)
{
  std::string c = normalise(get_attribute_string(node, name));
  return strtoul(c.c_str(), 0, 0);
}

double sys::get_attribute_double (sys::xml_ptr_const node,
				  const std::string& name)
{
  std::string c = normalise(get_attribute_string(node, name));
  return strtod(c.c_str(), 0);
}

sys::xml_ptr sys::put_element (sys::xml_ptr parent, const std::string& name)
{
  sys::xml_ptr node = sys::make_node(parent, name);
  return sys::put_node(parent, node);
}

sys::xml_ptr sys::put_element_date (sys::xml_ptr parent, const std::string& name,
				    const time_t& content)
{
  return put_element_text(parent, name, sys::timetoiso8601(&content));
}

sys::xml_ptr sys::put_element_double (sys::xml_ptr parent, const std::string& name,
				      const double& content)
{
  std::ostringstream os;
  os << std::scientific << content;
  return put_element_text(parent, name, os.str());
}

sys::xml_ptr sys::put_attribute_uint (sys::xml_ptr e, const std::string& name,
				      unsigned int value)
{
  std::ostringstream os;
  os << value;
  return put_attribute_text(e, name, os.str());
}

sys::xml_ptr sys::put_attribute_double (sys::xml_ptr e, const std::string& name,
					const double& value)
{
  std::ostringstream os;
  os << std::scientific << value;
  return put_attribute_text(e, name, os.str());
}

sys::xml_ptr sys::put_element_doubles (sys::xml_ptr root, 
				       const std::string& name,
				       const std::vector<double>& content)
{
  std::ostringstream oss;
  oss << std::scientific;
  for (size_t i=0; i<content.size()-1; ++i) oss << content[i] << " ";
  oss << content[content.size()-1];
  return put_element_text(root, name, oss.str());
}
