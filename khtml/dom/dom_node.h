/*
 * This file is part of the DOM implementation for KDE.
 *
 * (C) 1999 Lars Knoll (knoll@kde.org)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 *
 * $Id$
 */
#ifndef _DOM_Node_h_
#define _DOM_Node_h_

namespace DOM {

class Node;
class DOMString;
class NodeImpl;

/**
 * Objects implementing the <code> NamedNodeMap </code> interface are
 * used to represent collections of nodes that can be accessed by
 * name. Note that <code> NamedNodeMap </code> does not inherit from
 * <code> NodeList </code> ; <code> NamedNodeMap </code> s are not
 * maintained in any particular order. Objects contained in an object
 * implementing <code> NamedNodeMap </code> may also be accessed by an
 * ordinal index, but this is simply to allow convenient enumeration
 * of the contents of a <code> NamedNodeMap </code> , and does not
 * imply that the DOM specifies an order to these Nodes.
 *
 */
class NamedNodeMap
{
public:
    NamedNodeMap();
    NamedNodeMap(const NamedNodeMap &other);

    NamedNodeMap & operator = (const NamedNodeMap &other);

    ~NamedNodeMap();

    /**
     * The number of nodes in the map. The range of valid child node
     * indices is 0 to <code> length-1 </code> inclusive.
     *
     */
    unsigned long length() const;

    /**
     * Retrieves a node specified by name.
     *
     * @param name Name of a node to retrieve.
     *
     * @return A <code> Node </code> (of any type) with the specified
     * name, or <code> null </code> if the specified name did not
     * identify any node in the map.
     *
     */
    Node getNamedItem ( const DOMString &name );

    /**
     * Adds a node using its <code> nodeName </code> attribute.
     *
     *  As the <code> nodeName </code> attribute is used to derive the
     * name which the node must be stored under, multiple nodes of
     * certain types (those that have a "special" string value) cannot
     * be stored as the names would clash. This is seen as preferable
     * to allowing nodes to be aliased.
     *
     * @param arg A node to store in a named node map. The node will
     * later be accessible using the value of the <code> nodeName
     * </code> attribute of the node. If a node with that name is
     * already present in the map, it is replaced by the new one.
     *
     * @return If the new <code> Node </code> replaces an existing
     * node with the same name the previously existing <code> Node
     * </code> is returned, otherwise <code> null </code> is returned.
     *
     * @exception DOMException
     * WRONG_DOCUMENT_ERR: Raised if <code> arg </code> was created
     * from a different document than the one that created the <code>
     * NamedNodeMap </code> .
     *
     *  NO_MODIFICATION_ALLOWED_ERR: Raised if this <code>
     * NamedNodeMap </code> is readonly.
     *
     *  INUSE_ATTRIBUTE_ERR: Raised if <code> arg </code> is an <code>
     * Attr </code> that is already an attribute of another <code>
     * Element </code> object. The DOM user must explicitly clone
     * <code> Attr </code> nodes to re-use them in other elements.
     *
     */
    Node setNamedItem ( const Node &arg );

    /**
     * Removes a node specified by name. If the removed node is an
     * <code> Attr </code> with a default value it is immediately
     * replaced.
     *
     * @param name The name of a node to remove.
     *
     * @return The node removed from the map or <code> null </code> if
     * no node with such a name exists.
     *
     * @exception DOMException
     * NOT_FOUND_ERR: Raised if there is no node named <code> name
     * </code> in the map.
     *
     */
    Node removeNamedItem ( const DOMString &name );

    /**
     * Returns the <code> index </code> th item in the map. If <code>
     * index </code> is greater than or equal to the number of nodes
     * in the map, this returns <code> null </code> .
     *
     * @param index Index into the map.
     *
     * @return The node at the <code> index </code> th position in the
     * <code> NamedNodeMap </code> , or <code> null </code> if that is
     * not a valid index.
     *
     */
    Node item ( unsigned long index );

protected:
    NamedNodeMap( NodeImpl *i);

    NodeImpl *map;

    friend class Node;
};

class NamedNodeMap;
class NodeList;
class Document;
class DOMString;
class StyleSheet;
    
class NodeImpl;

/**
 * The <code> Node </code> interface is the primary datatype for the
 * entire Document Object Model. It represents a single node in the
 * document tree. While all objects implementing the <code> Node
 * </code> interface expose methods for dealing with children, not all
 * objects implementing the <code> Node </code> interface may have
 * children. For example, <code> Text </code> nodes may not have
 * children, and adding children to such nodes results in a <code>
 * DOMException </code> being raised.
 *
 *  The attributes <code> nodeName </code> , <code> nodeValue </code>
 * and <code> attributes </code> are included as a mechanism to get at
 * node information without casting down to the specific derived
 * interface. In cases where there is no obvious mapping of these
 * attributes for a specific <code> nodeType </code> (e.g., <code>
 * nodeValue </code> for an Element or <code> attributes </code> for a
 * Comment), this returns <code> null </code> . Note that the
 * specialized interfaces may contain additional and more convenient
 * mechanisms to get and set the relevant information.
 *
 */
class Node
{
    friend class NamedNodeMap;
    friend class NodeList;
    friend class HTMLCollection;
    friend class StyleSheet;
    
public:
    Node();
    Node(const Node &other);

    Node & operator = (const Node &other);

    virtual ~Node();
    /**
     * An integer indicating which type of node this is.
     *
     *
     * <p>The values of <code>nodeName</code>, <code>nodeValue</code>,
     *  and <code>attributes</code> vary according to the node type as follows:
     *   <table  border="1">
     *     <tbody>
     *     <tr>
     *       <td></td>
     *       <td>nodeName</td>
     *       <td>nodeValue</td>
     *       <td>attributes</td>
     *     </tr>
     *     <tr>
     *       <td>Element</td>
     *       <td>tagName</td>
     *       <td>null</td>
     *       <td>NamedNodeMap</td>
     *     </tr>
     *     <tr>
     *       <td>Attr</td>
     *       <td>name of attribute</td>
     *       <td>value of attribute</td>
     *       <td>null</td>
     *     </tr>
     *     <tr>
     *       <td>Text</td>
     *       <td>#text</td>
     *       <td>content of the text node</td>
     *       <td>null</td>
     *     </tr>
     *     <tr>
     *       <td>CDATASection</td>
     *       <td>#cdata-section</td>
     *       <td>content of the CDATA Section</td>
     *       <td>null</td>
     *     </tr>
     *     <tr>
     *       <td>EntityReference</td>
     *       <td>name of entity referenced</td>
     *       <td>null</td>
     *       <td>null</td>
     *       </tr>
     *     <tr>
     *       <td>Entity</td>
     *       <td>entity name</td>
     *       <td>null</td>
     *       <td>null</td>
     *     </tr>
     *     <tr>
     *       <td>ProcessingInstruction</td>
     *       <td>target</td>
     *       <td>entire content excluding the target</td>
     *       <td>null</td>
     *     </tr>
     *     <tr>
     *       <td>Comment</td>
     *       <td>#comment</td>
     *       <td>content of the comment</td>
     *       <td>null</td>
     *     </tr>
     *     <tr>
     *       <td>Document</td>
     *       <td>#document</td>
     *       <td>null</td>
     *       <td>null</td>
     *     </tr>
     *     <tr>
     *       <td>DocumentType</td>
     *       <td>document type name</td>
     *       <td>null</td>
     *       <td>null</td>
     *     </tr>
     *     <tr>
     *       <td>DocumentFragment</td>
     *       <td>#document-fragment</td>
     *       <td>null</td>
     *       <td>null</td>
     *     </tr>
     *     <tr>
     *       <td>Notation</td>
     *       <td>notation name</td>
     *       <td>null</td>
     *       <td>null</td>
     *     </tr>
     *     </tbody>
     *   </table>
     * </p>
     */
    enum NodeType {
        ELEMENT_NODE = 1,
        ATTRIBUTE_NODE = 2,
        TEXT_NODE = 3,
        CDATA_SECTION_NODE = 4,
        ENTITY_REFERENCE_NODE = 5,
        ENTITY_NODE = 6,
        PROCESSING_INSTRUCTION_NODE = 7,
        COMMENT_NODE = 8,
        DOCUMENT_NODE = 9,
        DOCUMENT_TYPE_NODE = 10,
        DOCUMENT_FRAGMENT_NODE = 11,
        NOTATION_NODE = 12
    };

    /**
     * The name of this node, depending on its type; see the table
     * above.
     *
     */
    DOMString nodeName() const;

    /**
     * The value of this node, depending on its type; see the table
     * above.
     *
     * @exception DOMException
     * DOMSTRING_SIZE_ERR: Raised when it would return more characters
     * than fit in a <code> DOMString </code> variable on the
     * implementation platform.
     *
     */
    DOMString nodeValue() const;

    /**
     * see @ref nodeValue
     * @exception DOMException
     * NO_MODIFICATION_ALLOWED_ERR: Raised when the node is readonly.
     *
     */
    void setNodeValue( const DOMString & );

    /**
     * A code representing the type of the underlying object, as
     * defined above.
     *
     */
    unsigned short nodeType() const;

    /**
     * The parent of this node. All nodes, except <code> Document
     * </code> , <code> DocumentFragment </code> , and <code> Attr
     * </code> may have a parent. However, if a node has just been
     * created and not yet added to the tree, or if it has been
     * removed from the tree, this is <code> null </code> .
     *
     */
    Node parentNode() const;

    /**
     * A <code> NodeList </code> that contains all children of this
     * node. If there are no children, this is a <code> NodeList
     * </code> containing no nodes. The content of the returned <code>
     * NodeList </code> is &quot;live&quot; in the sense that, for
     * instance, changes to the children of the node object that it
     * was created from are immediately reflected in the nodes
     * returned by the <code> NodeList </code> accessors; it is not a
     * static snapshot of the content of the node. This is true for
     * every <code> NodeList </code> , including the ones returned by
     * the <code> getElementsByTagName </code> method.
     *
     */
    NodeList childNodes() const;

    /**
     * The first child of this node. If there is no such node, this
     * returns <code> null </code> .
     *
     */
    Node firstChild() const;

    /**
     * The last child of this node. If there is no such node, this
     * returns <code> null </code> .
     *
     */
    Node lastChild() const;

    /**
     * The node immediately preceding this node. If there is no such
     * node, this returns <code> null </code> .
     *
     */
    Node previousSibling() const;

    /**
     * The node immediately following this node. If there is no such
     * node, this returns <code> null </code> .
     *
     */
    Node nextSibling() const;

    /**
     * A <code> NamedNodeMap </code> containing the attributes of this
     * node (if it is an <code> Element </code> ) or <code> null
     * </code> otherwise.
     *
     */
    NamedNodeMap attributes() const;

    /**
     * The <code> Document </code> object associated with this node.
     * This is also the <code> Document </code> object used to create
     * new nodes. When this node is a <code> Document </code> this is
     * <code> null </code> .
     *
     */
    Document ownerDocument() const;

    /**
     * Inserts the node <code> newChild </code> before the existing
     * child node <code> refChild </code> . If <code> refChild </code>
     * is <code> null </code> , insert <code> newChild </code> at the
     * end of the list of children.
     *
     *  If <code> newChild </code> is a <code> DocumentFragment
     * </code> object, all of its children are inserted, in the same
     * order, before <code> refChild </code> . If the <code> newChild
     * </code> is already in the tree, it is first removed.
     *
     * @param newChild The node to insert.
     *
     * @param refChild The reference node, i.e., the node before which
     * the new node must be inserted.
     *
     * @return The node being inserted.
     *
     * @exception DOMException
     * HIERARCHY_REQUEST_ERR: Raised if this node is of a type that
     * does not allow children of the type of the <code> newChild
     * </code> node, or if the node to insert is one of this node's
     * ancestors.
     *
     *  WRONG_DOCUMENT_ERR: Raised if <code> newChild </code> was
     * created from a different document than the one that created
     * this node.
     *
     *  NO_MODIFICATION_ALLOWED_ERR: Raised if this node is readonly.
     *
     *  NOT_FOUND_ERR: Raised if <code> refChild </code> is not a
     * child of this node.
     *
     */
    Node insertBefore ( const Node &newChild, const Node &refChild );

    /**
     * Replaces the child node <code> oldChild </code> with <code>
     * newChild </code> in the list of children, and returns the
     * <code> oldChild </code> node. If the <code> newChild </code> is
     * already in the tree, it is first removed.
     *
     * @param newChild The new node to put in the child list.
     *
     * @param oldChild The node being replaced in the list.
     *
     * @return The node replaced.
     *
     * @exception DOMException
     * HIERARCHY_REQUEST_ERR: Raised if this node is of a type that
     * does not allow children of the type of the <code> newChild
     * </code> node, or it the node to put in is one of this node's
     * ancestors.
     *
     *  WRONG_DOCUMENT_ERR: Raised if <code> newChild </code> was
     * created from a different document than the one that created
     * this node.
     *
     *  NO_MODIFICATION_ALLOWED_ERR: Raised if this node is readonly.
     *
     *  NOT_FOUND_ERR: Raised if <code> oldChild </code> is not a
     * child of this node.
     *
     */
    Node replaceChild ( const Node &newChild, const Node &oldChild );

    /**
     * Removes the child node indicated by <code> oldChild </code>
     * from the list of children, and returns it.
     *
     * @param oldChild The node being removed.
     *
     * @return The node removed.
     *
     * @exception DOMException
     * NO_MODIFICATION_ALLOWED_ERR: Raised if this node is readonly.
     *
     *  NOT_FOUND_ERR: Raised if <code> oldChild </code> is not a
     * child of this node.
     *
     */
    Node removeChild ( const Node &oldChild );

    /**
     * Adds the node <code> newChild </code> to the end of the list of
     * children of this node. If the <code> newChild </code> is
     * already in the tree, it is first removed.
     *
     * @param newChild The node to add.
     *
     *  If it is a <code> DocumentFragment </code> object, the entire
     * contents of the document fragment are moved into the child list
     * of this node
     *
     * @return The node added.
     *
     * @exception DOMException
     * HIERARCHY_REQUEST_ERR: Raised if this node is of a type that
     * does not allow children of the type of the <code> newChild
     * </code> node, or if the node to append is one of this node's
     * ancestors.
     *
     *  WRONG_DOCUMENT_ERR: Raised if <code> newChild </code> was
     * created from a different document than the one that created
     * this node.
     *
     *  NO_MODIFICATION_ALLOWED_ERR: Raised if this node is readonly.
     *
     */
    Node appendChild ( const Node &newChild );

    /**
     * This is a convenience method to allow easy determination of
     * whether a node has any children.
     *
     * @return <code> true </code> if the node has any children,
     * <code> false </code> if the node has no children.
     *
     */
    bool hasChildNodes (  );

    /**
     * Returns a duplicate of this node, i.e., serves as a generic
     * copy constructor for nodes. The duplicate node has no parent (
     * <code> parentNode </code> returns <code> null </code> .).
     *
     *  Cloning an <code> Element </code> copies all attributes and
     * their values, including those generated by the XML processor to
     * represent defaulted attributes, but this method does not copy
     * any text it contains unless it is a deep clone, since the text
     * is contained in a child <code> Text </code> node. Cloning any
     * other type of node simply returns a copy of this node.
     *
     * @param deep If <code> true </code> , recursively clone the
     * subtree under the specified node; if <code> false </code> ,
     * clone only the node itself (and its attributes, if it is an
     * <code> Element </code> ).
     *
     * @return The duplicate node.
     *
     */
    Node cloneNode ( bool deep );

    /**
     * @internal
     * not part of the DOM.
     * @returns the element id, in case this is an element, 0 otherwise
     */
    unsigned short elementId() const;

    /**
     * tests if this Node is 0. Useful especially, if casting to a derived class:
     *
     * <pre>
     * Node n = .....;
     * // try to convert into an Element:
     * Element e = n;
     * if( e.isNull() )
     *   printf("node isn't an element node\n");
     */
    bool isNull() { return (impl == 0); }

protected:
    NodeImpl *impl;

    Node( NodeImpl *_impl);
};


class NodeListImpl;

/**
 * The <code> NodeList </code> interface provides the abstraction of
 * an ordered collection of nodes, without defining or constraining
 * how this collection is implemented.
 *
 *  The items in the <code> NodeList </code> are accessible via an
 * integral index, starting from 0.
 *
 */
class NodeList
{
    friend class Element;
    friend class Node;
    friend class Document;
    friend class HTMLDocument;

public:
    NodeList();
    NodeList(const NodeList &other);

    NodeList & operator = (const NodeList &other);

    ~NodeList();

    /**
     * The number of nodes in the list. The range of valid child node
     * indices is 0 to <code> length-1 </code> inclusive.
     *
     */
    unsigned long length() const;

    /**
     * Returns the <code> index </code> th item in the collection. If
     * <code> index </code> is greater than or equal to the number of
     * nodes in the list, this returns <code> null </code> .
     *
     * @param index Index into the collection.
     *
     * @return The node at the <code> index </code> th position in the
     * <code> NodeList </code> , or <code> null </code> if that is not
     * a valid index.
     *
     */
    Node item ( unsigned long index );

protected:
    NodeList(const NodeListImpl *i);

    NodeListImpl *impl;
};

}; //namespace
#endif
