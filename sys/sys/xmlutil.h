//Dear emacs, this is -*- c++ -*-

/**
 * @file sys/xmlutil.h
 *
 * @brief Describes a set of utilities to parse XML data files.
 */

#ifndef RINGER_SYS_XMLUTIL_H
#define RINGER_SYS_XMLUTIL_H

#include <vector>
#include <string>
#include <ctime>
#include <libxml/tree.h>

namespace sys {

  /**
   * Takes a string as input and normalise it w.r.t. its white spaces. This
   * means space charaters as new-lines and tabulations are transformed to a
   * single white space. White space in the beginning and end of the string is
   * removed.
   *
   * @param s The string to be normalised.
   */
  std::string normalise (const std::string& s);

  /**
   * Takes a string and divide it into tokens taking the whitespaces
   * (tabulations, new-line characters) as separators.
   */
  void tokenize (const std::string& s, std::vector<std::string>& tokens);

  /**
   * Returns the next element in a set of elements
   */ 
  xmlNodePtr get_next_element (const xmlNodePtr node);

  /**
   * Returns the "translated" name of the given node
   *
   * @param node The node from where to extract the name
   */
  std::string get_element_name (const xmlNodePtr node);

  /**
   * Returns the contents of an element, as if it contained a string
   *
   * @param node The DOM node from where to extract the element value
   */
  std::string get_element_string (const xmlNodePtr node);

  /**
   * Returns the contents of an element, as if it contained an unsigned integer
   *
   * @param node The DOM node from where to extract the element value
   */
  unsigned int get_element_uint (const xmlNodePtr node);

  /**
   * Returns the contents of an element, as if it contained a xsd:dateTime
   *
   * @param node The DOM node from where to extract the element value
   */
  time_t get_element_date (const xmlNodePtr node);

  /**
   * Returns the contents of an element, as if it contained a double
   *
   * @param node The DOM node from where to extract the element value
   */
  double get_element_double (const xmlNodePtr node);

  /**
   * Returns the contents of an element, as if it contained a list of doubles
   *
   * @param node The DOM node from where to extract the element value
   * @param v Where to put the listed values
   */
  void get_element_doubles (const xmlNodePtr node, std::vector<double>& v);

  /**
   * Get an attribute of type text
   *
   * @param node The node from where to extract the attribute
   * @param name The name of the attribute to take from the node
   */
  std::string get_attribute_string (const xmlNodePtr node, 
				    const std::string& name);

  /**
   * Get an attribute of type unsigned integer
   *
   * @param node The node from where to extract the attribute
   * @param name The name of the attribute to take from the node
   */
  unsigned int get_attribute_uint (const xmlNodePtr node, 
				   const std::string& name);

  /**
   * Get an attribute of type double
   *
   * @param node The node from where to extract the attribute
   * @param name The name of the attribute to take from the node
   */
  double get_attribute_double (const xmlNodePtr node, const std::string& name);

  /**
   * Creates a new XML node with the given name
   *
   * @param name The name of the node to create
   *
   * @return The newly created node
   */
  xmlNodePtr make_node (const std::string& name);

  /**
   * Places an attribute-less element inside another element with the
   * parameters given
   *
   * @param root The root element to insert the new component
   * @param name The node name
   *
   * @return The newly created node.
   */
  xmlNodePtr put_element (xmlNodePtr root, const std::string& name);

  /**
   * Places a textual element inside another element with the parameters given
   *
   * @param root The root element to insert the new component
   * @param name The node name
   * @param content The node content
   *
   * @return The newly created element.
   */
  xmlNodePtr put_element_text (xmlNodePtr root,	 const std::string& name,
			       const std::string& content);

  /**
   * Places a textual element inside another element with the parameters given
   *
   * @param root The root element to insert the new component
   * @param name The node name
   * @param content The node content
   *
   * @return The newly created element.
   */
  xmlNodePtr put_element_date (xmlNodePtr root, const std::string& name,
			       const time_t& content);

  /**
   * Places a double element inside another element with the parameters given
   *
   * @param root The root element to insert the new component
   * @param name The node name
   * @param content The node content
   *
   * @return The newly created element.
   */
  xmlNodePtr put_element_double (xmlNodePtr root, const std::string& name,
				 const double& content);

  /**
   * Places a list of double element inside another element with the
   * parameters given
   *
   * @param root The root element to insert the new component
   * @param name The node name
   * @param content The node content
   *
   * @return The newly created element.
   */
  xmlNodePtr put_element_doubles (xmlNodePtr root, const std::string& name,
				  const std::vector<double>& content);

  /**
   * Places a text attribute on a element
   *
   * @param e The element to place the attribute on
   * @param name The attribute name
   * @param value The atribute value
   */
  xmlNodePtr put_attribute_text (xmlNodePtr e, const std::string& name,
				 const std::string& value);

  /**
   * Places an unsigned integer attribute on a element
   *
   * @param e The element to place the attribute on
   * @param name The attribute name
   * @param value The atribute value
   */
  xmlNodePtr put_attribute_uint (xmlNodePtr e, const std::string& name,
				 unsigned int value);

  /**
   * Places a double attribute on a element
   *
   * @param e The element to place the attribute on
   * @param name The attribute name
   * @param value The atribute value
   */
  xmlNodePtr put_attribute_double (xmlNodePtr e, const std::string& name,
				   const double& value);

}

#endif /* RINGER_SYS_XMLUTIL_H */
