#include <vector>
#include <iostream>

extern "C" {
#include <Python.h>
}

namespace graph {
	typedef std::vector< double > edge_t;
	typedef std::vector< edge_t > graph_t;
	typedef std::vector< std::vector< double > > result_t;
	
	static result_t FloydWarshallPlate( const graph_t& cpp_graph )
	{
		int n = 0;
		for( size_t i = 0; i < cpp_graph.size(); i++ )
		{
			n = ( cpp_graph[i][0] > n ) ? int( cpp_graph[i][0] ) : n;
			n = ( cpp_graph[i][1] > n ) ? int( cpp_graph[i][1] ) : n;
		}
		n++;
		result_t res( n, std::vector<double>( n, 0. ) );
		
		for( size_t i = 0; i < cpp_graph.size(); i++ )
		{
			int from = int( cpp_graph[ i ][ 0 ] ); 
			int to = int( cpp_graph[ i ][ 1 ] );
			double resistance = cpp_graph[ i ][ 2 ];

			if( res[ from ][ to ] == 0 )
				res[ from ][ to ] = cpp_graph[ i ][ 2 ];
			else
				res[ from ][ to ] = 1. / ( 1. / res[ from ][ to ] + 1. / resistance  );
		}
		
		for( int k = 0; k < n; k++ )
			for( int i = 0; i < n; i++ )
				for( int j = 0; j < n; j++ )
					if ( res[ i ][ j ] != 0. && ( res[ i ][ k ] + res[ k ][ j ] ) != 0. )
						res[ i ][ j ] = 1. / ( 1. / res[ i ][ j ] + 1. / ( res[ i ][ k ] + res[ k ][ j ] ) );
			
		return res;
	}
}

static graph::graph_t pyobject_to_cxx( PyObject * py_graph )
{
	graph::graph_t cpp_graph;
	cpp_graph.resize( PyObject_Length( py_graph ) );
	
	for( size_t i = 0; i < cpp_graph.size(); i++ ) 
	{
		PyObject * py_edge = PyList_GetItem( py_graph, i );
		graph::edge_t cpp_edge;
		cpp_edge.resize( PyObject_Length( py_edge ) );
	
		for( size_t j = 0; j < cpp_edge.size(); j++ ) 
		{
			PyObject * py_double = PyTuple_GetItem( py_edge, j );
			cpp_edge[ j ] = PyFloat_AsDouble( py_double );
		}
		cpp_graph[ i ] = cpp_edge;
	}
	
	return cpp_graph;
}

static PyObject * cxx_to_pyobject( const graph::result_t cpp_result )
{
	PyObject * py_result = PyList_New( cpp_result.size() );
	for ( size_t i = 0; i < cpp_result.size(); i++ ) 
	{
		PyObject * py_result_row = PyList_New( cpp_result[ i ].size() );
		
		for ( size_t j = 0; j < cpp_result[ i ].size(); j++ ) 
		{
			PyObject * py_elem = PyFloat_FromDouble( cpp_result[ i ][ j ] );
			PyList_SetItem(py_result_row, j, py_elem);
		}
		
		PyList_SetItem( py_result, i, py_result_row );
	}
	return py_result;
}

static PyObject * FloydWarshallPlate(PyObject * module, PyObject * args)
{
	PyObject * py_graph = PyTuple_GetItem( args, 0 );
	const graph::graph_t cpp_graph = pyobject_to_cxx( py_graph );
	const graph::result_t cpp_result = graph::FloydWarshallPlate( cpp_graph );
	PyObject * py_result = cxx_to_pyobject( cpp_result );
	return py_result;
}

PyMODINIT_FUNC PyInit_graph()
{
	static PyMethodDef ModuleMethods[] = 
	{
		{ "FloydWarshallPlate", FloydWarshallPlate, METH_VARARGS, "Some description" },
		{ NULL, NULL, 0, NULL }
	};
	static PyModuleDef ModuleDef = 
	{
		PyModuleDef_HEAD_INIT,
		"graph",
		"One freaky graph operations",
		-1, ModuleMethods, 
		NULL, NULL, NULL, NULL
	};
	PyObject * module = PyModule_Create(&ModuleDef);
	return module;
}

