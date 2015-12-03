import xml.etree.ElementTree as ET
import time
import graph

def getGraph(filename):
    #Parsing only one string
    tree = ET.parse(filename)
    root = tree.getroot()
    edges = []

    for elem in root:
        if elem.tag != "net" :
            net_from = int( elem.attrib[ "net_from" ] ) - 1
            net_to = int( elem.attrib[ "net_to" ] ) - 1
            resistance = float( elem.attrib[ "resistance" ] )
            edges.append( ( net_from, net_to, resistance ) )
            
            if elem.tag == "diode":
                reverse_resistance = float( elem.attrib[ "reverse_resistance" ] )
                edges.append( ( net_to, net_from, reverse_resistance ) )
            elif elem.tag == "capactor" or elem.tag == "resistor":
                edges.append( ( net_to, net_from, resistance ) )

    return edges

def floidWarshall( edges ):
    n = max( max( edges, key=lambda x: x[0] )[0], max( edges, key=lambda x: x[0] )[1] ) + 1
    res = [ [ 0. for i in range( n ) ] for j in range( n ) ]
    
    for edge in edges :
        net_from, net_to, resistance = edge
        if res[ net_from ][ net_to ] == 0. :
            res[ net_from ][ net_to ] = resistance
        else:
            res[ net_from ][ net_to ] = 1. / ( 1. / res[ net_from ][ net_to ] + 1. / resistance )
   
    for k in range( n ):
        for i in range( n ):
            for j in range( n ):
                if res[ i ][ j ] != 0. and res[ i ][ k ] + res[ k ][ j ] != 0. :
                    res[ i ][ j ] = 1. / ( 1. / res[ i ][ j ] + 1. / ( res[ i ][ k ] + res[ k ][ j ] ) )

    return res
        
def writeRes( res, filename ):
    out = open( filename, "w" )
    out.write( ",\n".join( [ ",".join( [ str(r) for r in rline ] ) for rline in res ] ) + "," )
    
if __name__ == "__main__":
    edges = getGraph( "input.xml" )
	
    python_start = time.process_time()
    res1 = floidWarshall( edges )
    python_end = time.process_time()
    python_time = python_end - python_start
    print("Python calculations time: {:.1f} sec".format(python_time))

    python_start = time.process_time()
    res2 = graph.FloydWarshallPlate( edges )
    python_end = time.process_time()
    python_time = python_end - python_start
    print("C++ calculations time: {:.1f} sec".format(python_time))
	
    writeRes( res1, "out1.csv" )
    writeRes( res2, "out2.csv" )
