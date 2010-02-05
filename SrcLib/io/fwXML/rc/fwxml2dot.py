# ***** BEGIN LICENSE BLOCK *****
# FW4SPL - Copyright (C) IRCAD, 2009-2010.
# Distributed under the terms of the GNU Lesser General Public License (LGPL) as
# published by the Free Software Foundation.
# ****** END LICENSE BLOCK ******

 #default python package
import xml.dom.minidom
import sys
#to install
import pygraphviz as pgv



def nextChildElement( parent ):
    for child in parent.childNodes:
        if child.nodeType ==  xml.dom.Node.ELEMENT_NODE:
            yield child

def nextChildElementClass( parent ):
    for child in parent.childNodes:
        if child.nodeType ==  xml.dom.Node.ELEMENT_NODE and child.hasAttribute("class"):
            yield child

def getText( elt ):
    for child in elt.childNodes:
        if child.nodeType ==  xml.dom.Node.TEXT_NODE:
            return str(child.nodeValue)
    return ""


 
def nodeId(elt):
    assert( len(str(elt.getAttribute("id"))) ) 
    return   str(elt.getAttribute("id"))
    #return  "UUID" + str(elt.getAttribute("id")).replace("-", "_")

def processClassicNode( elt , graph):
    nodeLabel = str(elt.getAttribute("class"))  #+ r'\n' + nodeId(elt)
    graph.add_node(  nodeId(elt) ,  label = nodeLabel )
    return nodeId(elt)
    
def processField( elt , graph):
    nodeLabel = str(elt.getAttribute("class"))  + r'\n' + str(elt.getAttribute("label"))  #+ r'\n' + nodeId(elt)
    graph.add_node(  nodeId(elt) ,  label = nodeLabel ,  style="rounded,filled",  color="lightgrey" )
    return nodeId(elt)
    
def processComposite( elt , graph):
    compoNode = graph.get_node( processClassicNode( elt , graph) )
    compoNode.attr["style"]="bold"
    compoNode.attr["color"]="blue"
    compoNode.attr["shape"] = "record"
    elements = elt.getElementsByTagName("element") # warning (W1) about composite of composite
    subLinks = [] # list of tuple (keyText , valueXMLNode )
    for i,  element in enumerate(elements):
        if element.parentNode == elt: #W1
            keyNode = element.getElementsByTagName("key")[0]
            assert( keyNode.tagName == "key" )
            valueNode =  nextChildElementClass( element.getElementsByTagName("value")[0] ).next()
            subLinks.append(  ( getText(keyNode),  valueNode )  )
            parse( valueNode , graph )
    extraLabel =  ""
    for i,  element in enumerate(subLinks):
        keyText= element[0]
        extraLabel += " | <f" + str(i)+">" + keyText
    compoNode.attr["label"]  +=  extraLabel
    for i,  element in enumerate(subLinks):
        valueId = nodeId( element[1] ) 
        graph.add_edge(  nodeId(elt)  ,  valueId )
        graph.get_edge(  nodeId(elt)  ,  valueId ).attr["tailport"] = "f"+str(i)
        # nA:f0 -- nB:f2 : f0 is tailport and  f2is headport
    



def parse( elt , graph,  level=0):
    childNodes = [] 
    if  not elt.hasAttribute("class"):
        return childNodes
    # ok contain class
    className = elt.getAttribute("class")
    if graph.has_node( nodeId(elt) ):
        return childNodes
    #ok not processed
    if className== "::fwTools::Field":
        newNode = processField(elt, graph)
    elif className== "::fwData::Composite":
        newNode = processComposite( elt , graph)
    else:
        newNode = processClassicNode( elt , graph)        
    for  child in nextChildElementClass(elt):
        parse( child , graph,  level+1)
        graph.add_edge(  nodeId(elt)    ,  nodeId(child) )




def main():
    if len( sys.argv) != 3:
        print"Usage : " ,  sys.argv[0]  + " infwData.xml outGraph.[dot|svg|png|pdf]"
        return
    
    file= open( sys.argv[1] )
    dom = xml.dom.minidom.parse( file )
    contentNode = dom.getElementsByTagName("content")
    assert( len(contentNode) == 1 )
    fwrootObject = nextChildElement(contentNode[0]).next()
    g=pgv.AGraph(rankdir="LR")
    g.node_attr['shape']='box'
    g.graph_attr['splines']='false'
    
    parse( fwrootObject ,  graph=g)
    
    # output
    g.layout("dot")
    fileOut = sys.argv[2]
    if fileOut.endswith(".dot"):
        g.write(fileOut)
    else:
        g.draw(fileOut)


if __name__ == "__main__":
    main()


