//Dear emacs, this is -*- c++ -*-

/**
 * @file sys/sys/xerces_xmlutil.h
 * @author <a href="mailto:Andre.dos.Anjos@cern.ch">Andre Rabello dos Anjos</a> 
 * $Author$
 * $Revision$
 * $Date$
 *
 * @brief Describes a set of utilities to parse XML data files.
 */

#ifndef RINGER_SYS_XERCES_XMLUTIL_H
#define RINGER_SYS_XERCES_XMLUTIL_H

#include <vector>
#include <string>
#include <ctime>
#include <xercesc/dom/DOMElement.hpp>

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
  const xercesc::DOMElement* get_next_element (const xercesc::DOMElement* node);

  /**
   * Returns the first child of a node
   *
   * @param top The top node to consider
   */
  const xercesc::DOMElement* get_first_child (const xercesc::DOMElement* top);

  /**
   * Checks if a node is a valid element
   *
   * @param node The node to check
   */
  bool is_element (const xercesc::DOMElement* node);

  /**
   * Returns the "translated" name of the given node
   *
   * @param node The node from where to extract the name
   */
  std::string get_element_name (const xercesc::DOMElement* node);

  /**
   * Returns the contents of an element, as if it contained a string
   *
   * @param node The DOM node from where to extract the element value
   */
  std::string get_element_string (const xercesc::DOMElement* node);

  /**
   * Returns the contents of an element, as if it contained an unsigned integer
   *
   * @param node The DOM node from where to extract the element value
   */
  unsigned int get_element_uint (const xercesc::DOMElement* node);

  /**
   * Returns the contents of an element, as if it contained a xsd:dateTime
   *
   * @param node The DOM node from where to extract the element value
   */
  time_t get_element_date (const xercesc::DOMElement* node);

  /**
   * Returns the contents of an element, as if it contained a double
   *
   * @param node The DOM node from where to extract the element value
   */
  double get_element_double (const xercesc::DOMElement* node);

  /**
   * Returns the contents of an element, as if it contained a list of doubles
   *
   * @param node The DOM node from where to extract the element value
   * @param v Where to put the listed values
   */
  void get_element_doubles (const xercesc::DOMElement* node, 
			    std::vector<double>& v);

  /**
   * Get an attribute of type text
   *
   * @param node The node from where to extract the attribute
   * @param name The name of the attribute to take from the node
   */
  std::string get_attribute_string (const xercesc::DOMElement* node, 
				    const std::string& name);

  /**
   * Get an attribute of type unsigned integer
   *
   * @param node The node from where to extract the attribute
   * @param name The name of the attribute to take from the node
   */
  unsigned int get_attribute_uint (const xercesc::DOMElement* node, 
				   const std::string& name);

  /**
   * Get an attribute of type double
   *
   * @param node The node from where to extract the attribute
   * @param name The name of the attribute to take from the node
   */
  double get_attribute_double (const xercesc::DOMElement* node, 
			       const std::string& name);

  /**
   * Creates a new XML node with the given name
   *
   * @param any A pointer to any node of the document where you want to insert
   * the new node. It does @b not have to be the "future" parent of the new
   * node, just any node in the document will do the trick.
   * @param name The name of the node to create
   *
   * @return The newly created node
   */
  xercesc::DOMElement* make_node (xercesc::DOMElement* any,
				  const std::string& name);

  /**
   * Places an XML node under another node
   *
   * @param top The node that will become the top node
   * @param child The node that will become the child node
   *
   * @return The top node
   */
  xercesc::DOMElement* put_node (xercesc::DOMElement* top,
				 xercesc::DOMElement* child);

  /**
   * Places an attribute-less element inside another element with the
   * parameters given
   *
   * @param root The root element to insert the new component
   * @param name The node name
   *
   * @return The newly created node.
   */
  xercesc::DOMElement* put_element (xercesc::DOMElement* root,
				    const std::string& name);

  /**
   * Places a textual element inside another element with the parameters given
   *
   * @param root The root element to insert the new component
   * @param name The node name
   * @param content The node content
   *
   * @return The newly created element.
   */
  xercesc::DOMElement* put_element_text (xercesc::DOMElement* root,	 
					 const std::string& name,
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
  xercesc::DOMElement* put_element_date (xercesc::DOMElement* root, 
					 const std::string& name,
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
  xercesc::DOMElement* put_element_double (xercesc::DOMElement* root, 
					   const std::string& name,
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
  xercesc::DOMElement* put_element_doubles (xercesc::DOMElement* root, 
					    const std::string& name,
					    const std::vector<double>& content);

  /**
   * Places a text attribute on a element
   *
   * @param e The element to place the attribute on
   * @param name The attribute name
   * @param value The atribute value
   */
  xercesc::DOMElement* put_attribute_text (xercesc::DOMElement* e, 
					   const std::string& name,
					   const std::string& value);

  /**
   * Places an unsigned integer attribute on a element
   *
   * @param e The element to place the attribute on
   * @param name The attribute name
   * @param value The atribute value
   */
  xercesc::DOMElement* put_attribute_uint (xercesc::DOMElement* e, 
					   const std::string& name,
					   unsigned int value);

  /**
   * Places a double attribute on a element
   *
   * @param e The element to place the attribute on
   * @param name The attribute name
   * @param value The atribute value
   */
  xercesc::DOMElement* put_attribute_double (xercesc::DOMElement* e, 
					     const std::string& name,
					     const double& value);

}

#endif /* RINGER_SYS_XERCES_XMLUTIL_H */
