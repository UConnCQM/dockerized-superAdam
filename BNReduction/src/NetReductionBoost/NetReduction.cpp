#include <boost/config.hpp>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstring>
#include <utility>
#include <algorithm>
#include <boost/utility.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>

using namespace std;
using namespace boost;

// Structure for Acyclic edges;
struct A_edge {
  int vj;
  int vi;
  int sign;
};


ofstream OUT( "reduction.log" ) ;

template <typename Graph>
void RemoveTerminal( Graph& G, vector<A_edge>& ListAEdges );  

template <typename Graph>
void PrintGraph( const Graph& G , ofstream& OUT );

template <typename Graph>
void RemoveSingleInput( Graph& G, vector<A_edge>& ListAEdges );

template <typename Graph, typename VertexDescriptor >
void InvertZero( VertexDescriptor ai,   Graph& G  ) ;

template <typename Graph >
void InvertZeroAll( Graph& G , vector<int>& ListZero );

template <typename Graph >
void FindConstant( Graph& G, vector<int>& ListZero ); 

template <typename Graph >
void RemoveConstant( Graph& G , vector<int>& ListConstant ) ;

template <typename Graph >
void RemovePositiveOut( Graph& G, vector<A_edge>& ListAEdges  ) ;

template <typename Graph >
void RemoveEdge( Graph& G ) ;

template <typename Graph >
void ClearZeroAll( Graph& G, vector<int>& ListZero  )  ;


// properties of a node
struct Molecule
{
  int name;
  int status; // 1:normal, 0:marked, -1:deleted, 2 initial normal nodes 
};
// properties of an edge
struct Interaction 
{
  int sign; //-1 : negativ effect, +1 positive effect
};

int main(int argc, char **argv)
{

    std::string matrixFile="";  // name of the input file with the matrix
    std::string outFile="" ;  //name of the output file

    int init_n_node_bn = -1; // numbeor of nodes of BN
    vector<int> inverted_nodes_bn ; 

    //--------------user interface to read the matrix form an input file
/*    for (int i = 1; i < argc; i++) {
        
        std::string arg = argv[i];
        int n = 1;
        while (i+n < argc && argv[i+n][0] != '-') n++;

        if (arg == "-i") {
            if (n == 2) matrixFile = argv[++i];
            else cerr << "-i requires a file input with the matrix "  << endl;
        }
        else if (arg == "-o") {
            if (n == 2) outFile = argv[++i];
            else cerr << "-o requires an output file... exiting"  << endl; 
        }

        else { //Invalid parameter
            cerr << "Invalid parameter " << i << ": " << arg << endl;
        }
    }  
    //-----------------------------------------------------------------
*/

    // checking if input file was given
    //if (matrixFile=="" || outFile=="" ) {
    //    cerr << "input and output file are required  ... exiting" << endl 
    //         << "usage: Reduction -i Ifile -o OFile"<<endl;
    //    exit(1);
    //}
    

    // trying to read the input file
    //ifstream IN(matrixFile.c_str());
    //if (!IN) {
    //    cerr << "Error openeing input file ...exiting" << endl;
    //    exit(1) ;
   // }

    // trying to open the output file for writing
    //ofstream OUT( outFile.c_str() ) ;
    //if (!OUT) {
    //    cerr << "Error creating(opening) output file ...exiting" << endl;
    //    exit(1) ;
   // }

    if (!OUT) {
        cerr << "Error creating(opening) log file ...exiting" << endl;
        exit(1) ;
    }





    // Read the number of nodes and edges    
    int NumberNodes, NumberEdges, OriginalNNodes;
    cin >> NumberNodes;
    cin >> NumberEdges;
    
    OriginalNNodes = NumberNodes;
    // Using Boost Graph library

    //Create a typedef for Graph type
    typedef adjacency_list<listS, listS, bidirectionalS,
            Molecule , Interaction > Graph;

    //definition used to get acces to vertex properties
    typedef graph_traits<Graph>::vertex_descriptor Vertex;

    // creating an array of Vertices 
    vector<Vertex> vertex_array(NumberNodes);
    Vertex ai, aj, ak; //this vertices help to create edges

    // List of inverted nodes
    vector<int> ListZero ;
    
    // List of Constant nodes ;
    vector<int> ListConstant ;

    // List of Acycylic edges
    vector<A_edge> ListAEdges ;     

    // creating the grpah G
    Graph G;

    bool ConstNode = false ; // helper variable


    // Adding vertices to the Graph one by one
    for (int i = 0  ; i < NumberNodes ; ++i){
        ai  = add_vertex(G); 
        vertex_array[i] = ai;
        G[ai].name = i+1;
        G[ai].status = 2;
    }

    // Adding the edges from the file
    // reading the edges from the file and put them into the graph G
    int idx_i, idx_j, sign , k=0;
    graph_traits<Graph>::edge_descriptor e, ej, ek ;
    bool inserted;



    while(k<NumberEdges){ /// Maybe this does not work in VC++ see the manual 
        cin >> idx_i >> idx_j >> sign ;
        ai = vertex_array[idx_i-1];
        aj = vertex_array[idx_j-1];
        
        tie(e, inserted) = add_edge(ai, aj , G);
        G[e].sign = sign;      
        k++;
    }  

    string label;

    // Looking For ZERO NODES 
    //cin.seekg (0, ios::beg);
    while ( getline(cin,label)  ) {
        if (label == "ZERO_NODES") {
           cin >> idx_i ;
           while (cin) {
               ai = vertex_array[idx_i-1];
               G[ai].status = 1;
               clear_in_edges(ai,G); // just in case
               ListZero.push_back(G[ai].name);
               cin >> idx_i ;
           }
           break;
        }

    }
    cin.clear();

    //  Reading number of nodes of original Boolean Network
    while (getline(cin,label) ) {
       if (label == "INITIAL_NUMBER_OF_NODES_OF_BN") {
          cin >> idx_i ;
          if ( ! cin ) {
            cerr << "Wrong initial number of nodes of BN exiting ......"<<endl;
            exit(0);
          }
          init_n_node_bn = idx_i ; // numbeor of nodes of BN
          break;
       }
       // cout << " ---------- " << label << endl;
    }
    cin.clear();


    //  Inverted nodes of original Boolean Network
    while (getline(cin,label) ) {
        if (label == "INVERTED_NODES_BN") {
           cin >> idx_i ;
           while (cin) {
               inverted_nodes_bn.push_back(idx_i);
               cin >> idx_i ;
           }
           break ;
        }
    }
    //cin.clear();

    //cin.close();
    ///////////////////////////////////////////////////////////////

    int old_n_nodes, old_n_edges, Nsteps = 1;
    do {
        OUT << "------------ Step number:  " << Nsteps++ << " -------------"<<endl;
        old_n_nodes = num_vertices(G)  ;
        old_n_edges = num_edges(G) ;


        OUT << "... removing following terminal vertices  : " << endl;
        RemoveTerminal(G, ListAEdges );

        OUT << "... removing vertices with one input  : " << endl;
        RemoveSingleInput( G, ListAEdges ); 
        ClearZeroAll ( G , ListZero  ) ;
        //InvertZeroAll(  G , ListZero  );
    
        OUT << "... finding constant nodes : " << endl;
        FindConstant( G , ListZero );

        OUT << "... removing constant nodes   : " << endl; 
        RemoveConstant(  G, ListConstant ) ;
        ClearZeroAll ( G , ListZero  ) ;
        //InvertZeroAll(  G, ListZero  );

        OUT << "... Removing nodes with only positive outputs ... " << endl;
        RemovePositiveOut( G, ListAEdges );
        ClearZeroAll ( G , ListZero  ) ;
        //InvertZeroAll(  G , ListZero );

        OUT << "... removing edges ... " << endl;
        RemoveEdge( G );

    } while ( old_n_nodes != num_vertices(G)  ||  old_n_edges != num_edges(G) ) ;

    PrintGraph<Graph>(G,OUT);


    cout<<endl<<"INITIAL_NUMBER_OF_NODES"<<endl;
    cout<<OriginalNNodes<<endl;

    cout<<endl<<"ZERO_NODES"<<endl;
    int Ninv= (int) ListZero.size();
    for (int i = 0 ; i < Ninv ; ++i) cout<< ListZero[i]<<endl;    

    cout<<endl<<"ONE_NODES"<<endl;
    int Nctes= (int) ListConstant.size();
    for (int i = 0 ; i < Nctes ; ++i) cout<< ListConstant[i]<<endl;    
    
    cout<<endl<<"ACYCLIC_GRAPH"<<endl;
    int Naedges = (int) ListAEdges.size();
    for (int i = 0 ; i < Naedges ; ++i) {
       cout<< ListAEdges[i].vj <<" "
          << ListAEdges[i].vi <<" " 
          << ListAEdges[i].sign << endl;    
    }

    if ( init_n_node_bn != -1 ) {
       cout<<endl<<"INITIAL_NUMBER_OF_NODES_OF_BN"<<endl;
       cout<<init_n_node_bn<<endl;

       cout<<endl<<"INVERTED_NODES_BN"<<endl;
       int tempsize = inverted_nodes_bn.size() ;
       for (int i= 0 ; i<tempsize ; i++)
           cout << inverted_nodes_bn[i] << endl;

    }
    

    OUT.close();


    return 0;
}

//////////////////////////////////////// print_graph
// Remove terminal nodes 
// of a graph
//
// Parameters:
// G          Graph (created with Boost)
// 
// Returns: none 
//
// Called by: main ()
// Calls: none
template <typename Graph>
void RemoveTerminal(Graph& G, vector<A_edge>& ListAEdges) {

    //  generate iteratos to loop over the nodes
    typename graph_traits<Graph>::vertex_iterator vi, vi_end, next;
    typename graph_traits<Graph>::vertex_descriptor  aj;
    typename graph_traits<Graph>::in_edge_iterator in_i, in_end;

    tie(vi , vi_end) = vertices(G);
    for ( next = vi ; vi != vi_end;  vi=next) { //loop over nodes
        ++next;

        if ( in_degree(*vi, G)==0 ) continue;

        if ( out_degree(*vi, G) < 1 ) { //--- This may be imporved by checking
                                        //--- the emptyness of the list
            OUT << G[*vi].name << endl;
            for (tie(in_i,in_end)=in_edges(*vi,G); in_i!=in_end; ++in_i) {
               A_edge  e ;
               aj = source(*in_i, G);
               e.vj = G[aj].name;
               e.vi = G[*vi].name;
               e.sign = G[*in_i].sign;
               ListAEdges.push_back(e);  
            }
            clear_vertex(*vi, G);
            remove_vertex(*vi, G);
        }
    }
}


//////////////////////////////////////// 
// Remove Nodes with only one input . RemoveSingleInput
// 
//
// Parameters:
// G          Graph
//
// Returns: none 
//
// Called by: main ()
// Calls: none
template <typename Graph>
void RemoveSingleInput( Graph& G, vector<A_edge>& ListAEdges) {

    // Declare Vertex descriptor
    typename graph_traits<Graph>::vertex_descriptor  aj, ak;
 
    // Declare edge descriptor 
    typename graph_traits<Graph>::edge_descriptor  e;
  
    //  generate iteratos to loop over the nodes
    typename graph_traits<Graph>::vertex_iterator vi, vi_end, next;
    
    // generate iterator to loop of out-edges and in-edges of a node
    typename graph_traits<Graph>::out_edge_iterator out_i, out_end, out_next;
    typename graph_traits<Graph>::in_edge_iterator in_i, in_end;
   
    bool inserted;

    tie(vi , vi_end) = vertices(G);
    for ( next = vi ; vi != vi_end;  vi=next) {
        ++next;

        if (G[*vi].status == 0)  continue ; //constant nodes ignored 
        // check if only one input edge not self edgde
        if ( in_degree(*vi, G) == 1 &&  !edge(*vi ,*vi, G ).second   ) {

            // display node name
            OUT << G[*vi].name << " ... constant nodes: ";

            tie(in_i, in_end) = in_edges(*vi, G); // get parent
            aj =  source( *in_i  , G) ;

            // put edge into a the acyclic graph
            A_edge  ae ;
            ae.vj = G[aj].name;
            ae.vi = G[*vi].name;
            ae.sign = G[*in_i].sign;
            ListAEdges.push_back(ae);

            // for each out edge of node i
            tie(out_i, out_end) = out_edges(*vi, G);
            for ( out_next = out_i ;  out_i != out_end; out_i=out_next ) {
                ++out_next;
                // geting the target nodes
                ak = target(*out_i, G);
                // s : total interactions from node j to node k 
                int s =  G[*in_i].sign * G[*out_i].sign;

                tie(e, inserted) = edge(aj, ak , G);
                if  ( inserted  ) {
                    s = s * G[e].sign;
                    // status of the node will be zero (constant)
                    if ( s < 0  )  {
                           OUT << " " << G[ak].name; 
                           //G[*vi].status = 0;
                           G[ak].status = 0;
                           //clear_in_edges(ak,G); // removing in edges
                    }

                }
                else { // add and edge (ajmak) with sing s
                    //cout << "adding ("<<G[aj.name] << "," << G[ak].name<<")"<< endl;
                    tie(e, inserted) = add_edge(aj, ak , G);
                    G[e].sign = s;
                }
            }
            clear_vertex(*vi, G);
            remove_vertex(*vi, G);
            OUT << endl;
        }
    }

}


//////////////////////////////////////// ChangeZero()
// If a node ai is  constant  with zero value
// then, change its value to 1. The sign of the
// output edges are also inverted
//
// Parameters:
// G            Graph
// ZeroNodes    Name of nodes with zero value
//
// Returns: none 
//
// Called by: main ()
// Calls: none
template <typename Graph, typename VertexDescriptor >
void InvertZero( VertexDescriptor ai,   Graph& G  ) {

    // generate iterator to loop of out-edges and in-edges of a node
    typename graph_traits<Graph>::out_edge_iterator out_i, out_end;

    if (G[ai].status == 0) {// constant nodes (this hould be in functions
        clear_in_edges(ai,G); // removing in edges
        G[ai].status = 1; // chagin from  0 to 1 constan nodes 
        // changing the sign of the out edges
        for (tie(out_i, out_end)=out_edges(ai, G); out_i != out_end; ++out_i)
            G[*out_i].sign = G[*out_i].sign * (-1);
    }
}


//////////////////////////////////////// InvertZeroAll(G)
// Seach for Zero nodes in  G and invert them 
// then, change its value to 1. The sign of the
// output edges are also inverted
//
// Parameters:
// G            Graph
//
// Returns: none 
//
// Called by: main ()
// Calls: none
template <typename Graph >
void InvertZeroAll( Graph& G, vector<int>& ListZero  ) {
    //  generate iteratos to loop over the nodes
    typename graph_traits<Graph>::vertex_iterator vi, vi_end, next;

    tie(vi , vi_end) = vertices(G);
    for ( next = vi ; vi != vi_end;  vi=next) {
        ++next;
        if (G[*vi].status == 0) {
            InvertZero( *vi, G  ) ;
            ListZero.push_back(G[*vi].name); 
        }
        
    }
}

//////////////////////////////////////// InvertZeroAll(G)
// Seach for Zero nodes in G and 
// change its value to 1. Clear input edges
//
// Parameters:
// G            Graph
//
// Returns: none 
//
// Called by: main ()
// Calls: none
template <typename Graph >
void ClearZeroAll( Graph& G, vector<int>& ListZero  ) {
    //  generate iteratos to loop over the nodes
    typename graph_traits<Graph>::vertex_iterator vi, vi_end, next;

    tie(vi , vi_end) = vertices(G);
    for ( next = vi ; vi != vi_end;  vi=next) {
        ++next;
        if (G[*vi].status == 0) {
            clear_in_edges(*vi,G); // removing in edges
            G[*vi].status = 1; // chagin from  0 to 1 constan nodes 
            // changing the sign of the out edges
            ListZero.push_back(G[*vi].name); 
        }
        
    }
}







//////////////////////////////////////// FindConstant()
// Find not obvious constant nodes 
// This nodes have inputs like: x AND NOT x
//
// Parameters:
// G            Graph
//
// Returns: none 
//
// Called by: main ()
// Calls: none
template <typename Graph >
void FindConstant( Graph& G , vector<int>& ListZero ) {

    // Declare Vertex descriptor
    typename graph_traits<Graph>::vertex_descriptor  aj, ak;

    // Declare edge descriptor 
    typename graph_traits<Graph>::edge_descriptor  e;

    //  generate iteratos to loop over the nodes
    typename graph_traits<Graph>::vertex_iterator vi, vi_end, next;

    // generate iterator to loop of out-edges and in-edges of a node
    typename graph_traits<Graph>::out_edge_iterator out_i, out_end;
    typename graph_traits<Graph>::in_edge_iterator in_i, in_end;

    bool inserted;
    bool ConstNode = false;

    tie(vi , vi_end) = vertices(G);
    for ( next = vi ; vi != vi_end;  vi=next) {
        ++next;
        ConstNode = false;
        // for each in  edge of node i
        for ( tie(in_i, in_end) = in_edges(*vi, G);  in_i != in_end; ++in_i) {
            aj = source(*in_i, G);

            //if ( G[aj].name ==  G[*vi].name ) continue; //selfloops not proccesed

            // for each out-edge  of node j 
            for ( tie(out_i,out_end)=out_edges(aj, G); out_i != out_end; ++out_i) {
                ak = target( *out_i,G);
                if ( G[ak].name == G[*vi].name ) continue;
                //if ( G[ak].name == G[aj].name ) continue;

                tie(e, inserted) = edge(ak, *vi , G); //check if edfe k -> i exist
                // checking for constant node
                if (inserted &&  G[e].sign==1 && G[*out_i].sign*G[*in_i].sign== -1){
                    // display node name
                    OUT << G[*vi].name ;
                    OUT << "     ...... : aj="<< G[aj].name ;
                    OUT << " ak=" << G[ak].name<< endl ;
                    ConstNode = true;
                    break;
                }
            }
            if (ConstNode) break;
        }
        if (ConstNode)  {
            clear_in_edges(*vi,G);
            G[*vi].status = 1;
            ListZero.push_back( G[*vi].name );
        }
    }

}

//////////////////////////////////////// RemoveConstant()
// Remove Constant nodes  
// find children constant nodes 
//
// Parameters:
// G            Graph
//
// Returns: none 
//
// Called by: main ()
// Calls: none
template <typename Graph >
void RemoveConstant( Graph& G , vector<int>& ListConstant ) {

    // Declare Vertex descriptor
    typename graph_traits<Graph>::vertex_descriptor aj, ak;

    //  generate iteratos to loop over the nodes
    typename graph_traits<Graph>::vertex_iterator vi, vi_end, next;

    // generate iterator to loop of out-edges and in-edges of a node
    typename graph_traits<Graph>::out_edge_iterator out_i, out_end;

    tie(vi , vi_end) = vertices(G);
    for ( next = vi ; vi != vi_end;  vi=next) {
        ++next;
        if ( in_degree(*vi, G) != 0) continue;

        if (G[*vi].status == 0) continue;

        OUT << G[*vi].name << "    Constant nodes generated: ";

        for ( tie(out_i,out_end)=out_edges(*vi,G); out_i != out_end; ++out_i ) {

            if (G[*out_i].sign == 1 && G[*vi].status == 1 )  {
                ak =  target(*out_i, G ) ;
                OUT << G[ak].name << " " ;
                G[ak].status = 0;
            } 
            else if ( G[*out_i].sign == -1 && G[*vi].status == 2 )  {
                ak =  target(*out_i, G ) ;
                OUT << G[ak].name << " " ;
                G[ak].status = 0;
            }
        }

        if ( G[*vi].status == 2 )
           ListConstant.push_back( G[*vi].name );

        clear_vertex(*vi, G);
        remove_vertex(*vi, G);
        OUT << endl;
    }
}

//////////////////////////////////////// RemovePositiveOut()
// Remove nodes with only positive outputs
// find children constant nodes 
//
// Parameters:
// G            Graph
//
// Returns: none 
//
// Called by: main ()
// Calls: none
template <typename Graph >
void RemovePositiveOut( Graph& G, vector<A_edge>& ListAEdges ) {

    // Declare Vertex descriptor
    typename graph_traits<Graph>::vertex_descriptor aj, ak;

    // Declare edge descriptor 
    typename graph_traits<Graph>::edge_descriptor  e;

    //  generate iteratos to loop over the nodes
    typename graph_traits<Graph>::vertex_iterator vi, vi_end, next;

    // generate iterator to loop of out-edges and in-edges of a node
    typename graph_traits<Graph>::out_edge_iterator out_i, out_end;
    typename graph_traits<Graph>::in_edge_iterator in_i, in_end;
    
    bool inserted;
    int sign;

    tie(vi , vi_end) = vertices(G);
    for ( next = vi ; vi != vi_end;  vi=next) {
        ++next;
        // if a selfloop, continue with the nex node
        tie(e, inserted) = edge(*vi , *vi , G);
        if (inserted) continue;

        // not analyzi recently constant nodes
        if ( G[*vi].status == 0 ) continue;
        if ( in_degree(*vi, G)==0 ) continue;

        // Cheking if all output edges are positive
        sign = 1;
        for (tie(out_i, out_end)=out_edges(*vi, G); out_i != out_end; ++out_i) {
            if ( G[*out_i].sign < 0  ) {
                sign = -1;
                break;
            }
        }
        if (sign == -1) continue; // next node if a negative edge found

        OUT << G[*vi].name << "  Constant nodes generated: ";
        // process the especial node
        // for each in edge
        for (tie(in_i, in_end)=in_edges(*vi, G); in_i != in_end; ++in_i) {
            aj = source(*in_i, G); // one parent
            // por each outedge
            for (tie(out_i, out_end)=out_edges(*vi, G); out_i != out_end; ++out_i) {
                ak = target(*out_i,G); //children
                int s = G[*in_i].sign * G[*out_i].sign ;
                tie(e,inserted) = edge(aj,ak,G);

                if (inserted) {
                    if ( G[e].sign * s  < 0 ) { //new constant nodes
                        G[ak].status=0;
                        OUT << G[ak].name << " ";
                    }
                }
                else { //adding a new edge (aj,ak) 
                    tie(e,inserted) = add_edge(aj,ak,G);
                    G[e].sign = s;
                }
            }
           
            // put edge into a the acyclic graph
            A_edge  ae;
            ae.vj = G[aj].name;
            ae.vi = G[*vi].name;
            ae.sign = G[*in_i].sign;
            ListAEdges.push_back(ae);
             
        }
        clear_vertex(*vi, G);
        remove_vertex(*vi, G); //deleting the node
        OUT << endl;
    }
}

//////////////////////////////////////// RemoveEdge()
// Remove redundant edges
//
// Parameters:
// G            Graph
//
// Returns: none 
//
// Called by: main ()
// Calls: none
template <typename Graph >
void RemoveEdge( Graph& G ) {

    // Declare Vertex descriptor
    typename graph_traits<Graph>::vertex_descriptor aj, ak;

    // Declare edge descriptor 
    typename graph_traits<Graph>::edge_descriptor  e, ej, ek;

    //  generate iteratos to loop over the nodes
    typename graph_traits<Graph>::vertex_iterator vi, vi_end, next;

    // generate iterator to loop of out-edges and in-edges of a node
    typename graph_traits<Graph>::in_edge_iterator in_i, in_end;

    bool inserted;

    tie(vi , vi_end) = vertices(G);
    for ( next = vi ; vi != vi_end;  vi=next) {
        ++next;
        
        // no selfloops allowed  
        if ( edge( *vi, *vi, G ).second ) continue;

        if ( in_degree(*vi, G) == 2) {

            // collecting the two in-edges
            tie(in_i, in_end)=in_edges(*vi, G);
            ej = *in_i , in_i++;
            ek = *in_i;
            // finding the parents of *vi
            aj = source(ej,G);
            ak = source(ek,G);

            if ( G[ej].sign == -1  &&  G[ek].sign == -1  ){

               tie(e,inserted) = edge( aj, ak, G );
               if (inserted && G[e].sign == 1 ) {
                   remove_edge( ek ,G);
                   OUT <<"("<<G[ak].name <<","<<G[*vi].name <<")"<< endl;
               }
               else{
                   tie(e,inserted) = edge( ak, aj, G );
                   if (inserted && G[e].sign == 1 ) {
                       remove_edge(ej,G);
                       OUT <<"("<<G[aj].name <<","<<G[*vi].name <<")"<< endl;
                   }
               }
            }            
        }
    }
}

//////////////////////////////////////// print_graph
// Print the number of nodes, number of edges and all .
// the edges of a graph
//
// Parameters:
// G          Graph
// OUT        ofstream of the file be printed
//
// Returns: none 
//
// Called by: main ()
// Calls: none
template <typename Graph> 
void PrintGraph( const Graph& G , ofstream& OUT ) {

    cout << num_vertices(G) << endl ;
    cout << num_edges(G) << endl; 
    typename graph_traits<Graph>::edge_iterator ei, ei_end;
    for (tie(ei, ei_end) = edges(G); ei != ei_end; ++ei) {
       cout << G[source(*ei, G)].name  << "  " ;
       cout << G[target(*ei, G)].name  << "  ";
       cout << G[*ei].sign << endl;
    }
}



