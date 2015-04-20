#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstring>
#include <utility>
#include <algorithm>
#include <vector>


using namespace std;

// Structure for Acyclic edges;
struct A_edge {
  int vj;
  int vi;
  int sign;
};


bool FindAllStates(int  sizeA, vector<A_edge> &Aedges, int MaxNodeId , int * STATES ) ;
void PrintState( ostream &OUT,   int MaxNodeId , int * STATES ) ;

int main(int argc, char **argv)
{


    int init_n_node_bn = -1; // number of nodes of BN
    vector<int> inverted_nodes_bn ;
    vector<A_edge> Aedges; // array to store edges of reduced network;

    // trying to open the output file for writing
    ofstream OUT( "fixedpoints.log" ) ;
    if (!OUT) {
        cerr << "Error creating(opening) output file ...exiting" << endl;
        exit(1) ;
    }

    // Read the number of nodes and edges    
    int NumberNodes, NumberEdges;
    cin >> NumberNodes;
    cin >> NumberEdges;

    //int * STATES_REDUCED = new int [NumberNodes] ;
   
    // creating an array of arrays to store edges Aj Ai sign
    A_edge * Edges  = new A_edge [NumberEdges];

    // The number of inputs per node
    int * Ninputs  = new int [NumberNodes];
    for (int i =0 ; i < NumberNodes ; i++)  Ninputs[i] = 0;

    // Table containing nodes names                                             
    int * HASH  = new int [NumberNodes];
    for (int i=0 ; i < NumberNodes ; i++)  HASH[i] = -1;

   
    // reading the edges from the file and put them into the graph G
    int MaxNodeId=0, k=0;
    for (k = 0;  k<NumberEdges; k++) 
        cin >>  Edges[k].vj >> Edges[k].vi >> Edges[k].sign;
   
    // Reading the number of nodes of the AND_NOT network    
    string label;
    while (getline(cin,label) ) {
        if (label == "INITIAL_NUMBER_OF_NODES") {
             cin >> MaxNodeId ;
             if ( !cin ) {
                cerr << "Check initial number of nodes, exiting ...." <<endl;
                exit(0);
             }
             break;
        }
    }

    // Mapping names to 1 ... Nodes indexes , kind of dirty code    
    int * NAMES  = new int [MaxNodeId];
    int * STATES  = new int [MaxNodeId];
    for (int i = 0 ; i < MaxNodeId ; i++ ) {
          NAMES[i] = -2;
          STATES[i] = 2;
    }

    // Reading Zero Nodes
    int ZeroNode = -1;
    while (getline(cin,label) ) { 
        if (label == "ZERO_NODES") { 
           cin >> ZeroNode;
           while (cin) {
              STATES[ZeroNode-1] = 0;
              cin >> ZeroNode;
           }
           break ;
        }
    }
    cin.clear();

    // Reading OneNodes
    int OneNode = -1;
    while (getline(cin,label) ) { 
        if (label == "ONE_NODES")  {
           cin >> OneNode;
           while (cin) {
              STATES[OneNode-1] = 1;
              cin >> OneNode;
           }
           break ;
        }
    }
    cin.clear();

 
    // Reading The number of Edges in the Acyclick graph
    int vvj, vvi, vvsign;
    int sizeA = 0;
    while (getline(cin,label) ){ 
        if (label == "ACYCLIC_GRAPH")  {
           cin >> vvj >> vvi >> vvsign ;
           while (cin ) {
               A_edge  ae;
               ae.vj = vvj;
               ae.vi = vvi;
               ae.sign = vvsign;
               Aedges.push_back(ae);    
               cin >> vvj >> vvi >> vvsign ;
               sizeA++;
           }
           break;
        }
    }
    cin.clear();


    // Reading the # nodes of ORIGINAL boolean network  
    while (getline(cin,label) ) {
        if (label == "INITIAL_NUMBER_OF_NODES_OF_BN") {
             cin >> init_n_node_bn  ;
             if ( !cin ) {
                cerr << "Check initial number of nodes, exiting ...." <<endl;
                exit(0);
             }


             if (init_n_node_bn >  MaxNodeId ) {
                cerr << "How come # of nodes of Original NB" ;
                cerr << " is greater than that of AND-NOT ? ..exiting"<<endl;
                exit(0);
             }
             break;
        }
    }
    cin.clear();

    //// Inverted nodes of original node
    int idx_i = -1;
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
    cin.clear();


    //END of reading
    //////////////////////////////////////////////////////////

    // array with nodes with negative self loop (NSL)
    bool * SelfLoops = new bool[MaxNodeId];
    for (int i = 0 ; i < MaxNodeId ; i++) 
         SelfLoops[i] = false;
 
    // find negative self loops
    //for (int i = 0 ; i < NumberEdges ; i++ ) {
    //     if ((Edges[i].vi == Edges[i].vj) && (Edges[i].sign == -1 ) ) 
    //          SelfLoops[ Edges[i].vi - 1] = true; 
    //}

    // Number of nodes without nodes NSL
    int NewNumberNodes = NumberNodes ;
    //for (int i = 0 ;i < MaxNodeId ; i++) 
    //    if ( SelfLoops[i] )  NewNumberNodes-- ;
 
    // Mark the nodes of AndNot that are present
    // in the reduced network    
    for (int i = 0 ; i < NumberEdges ; i++ ) {
        NAMES[ Edges[i].vi - 1 ] = 0;
        NAMES[ Edges[i].vj - 1 ] = 0;
    }


    // Asign HASH (1...nnodes) to Names (1....NAndNot)
    k=0;
    for (int i = 0 ; i < MaxNodeId ; i++ ) {
        if ( (NAMES[i] == 0) &&  !SelfLoops[i]  ) {
           NAMES[i] = k;
           HASH[k] = i;
           k++;
        }
    }
    // check correctnes of my algorithm
    if ( k != NewNumberNodes )  {
       cerr << "Some constant nodes detected, check your network " << endl;
       exit(0);
    }
 
    // Asign HAS for nodes with negative self loops
    //for (int i = 0 ; i < MaxNodeId ; i++ ) {
    //    if ( (NAMES[i] == 0) &&  SelfLoops[i]  ) {
    //       NAMES[i] = k;
    //      HASH[k] = i;
    //       k++;
    //    }
    //}

    // Chek if k == NNodes, if no then reduction is necessary
    //if ( k != NumberNodes )  {
    //   cerr << "Some constant nodes detected, check your network " << endl;
    //   exit(0);
    //}
    
    if (NumberNodes >0 ) OUT << "Doing the folling map : " << endl;
    for (int i = 0 ; i < NumberNodes ; i++ ) 
        OUT << HASH[i]+1 << " --> " << i+1 << endl;

    if (NumberNodes != NewNumberNodes ) { 
        OUT << "Nodes of Reduce Network with negative self loop" << endl;
        for (int i = NewNumberNodes ; i < NumberNodes ; i++ )
           OUT << HASH[i]+1 <<  endl;
    }
    ////////////////////////////////////////////////////////


    ////////////////////////////////////////////////////////
    // Counting the Number of inputs per node,
    for (int k  = 0 ; k < NumberEdges ; k++ ) {
        int j = Edges[k].vi;
        int i = NAMES[j-1] ; 
        Ninputs[i]=Ninputs[i] + 1; 
    }

    /////////////////////////////////////////////////////
    //// check for  F(xi) = not xi 
    //for (int k = 0 ; k < NumberNodes ; k++ ) {

    //    if ( Ninputs[k] != 1 ) continue;

    //    if ( SelfLoops[ HASH[k] ] ) {
    //       cerr << " No fixed point found "<< endl;
    //       OUT.close();
    //       exit(0);
    //    }
    //}
    ///////////////////////////////////////////////////////////

    ///////////////////////////////////////////////////////
    // Array with the adjan Matrix
    int * AdjList  = new int [NumberEdges+NumberNodes];
    // The number of inputs per node
    int * IndexNet  = new int [NumberNodes]; // stor the indexes per node
    
    int counter =0;
    for (int k  = 0 ; k < NumberNodes ; k++ ) {
        IndexNet[k] = counter;
        AdjList[counter] = k+1;
        counter = counter + Ninputs[k] + 1 ;
        Ninputs[k] = 0;
        
    }
    
    //Fillin the Adjancensy List
     // Counting the Number of inputs per node,
    for (int k  = 0 ; k < NumberEdges ; k++ ) {
        int j = Edges[k].vj;
        int jj = NAMES[j-1] ;

        int i = Edges[k].vi;
        int ii = NAMES[i-1] ;
    
        Ninputs[ii]=Ninputs[ii] + 1;
        AdjList[ IndexNet[ii] + Ninputs[ii] ] = Edges[k].sign * (jj + 1);
    }
 
    delete [] Edges;


    ///////////////////////////////////////////////////////////
    /// If Number of Nodes equal to zero just use acyclic graph
    if (NumberNodes == 0 ) {
       OUT << " Since there are no nodes, we use acyclic graph only" << endl;
       if (! FindAllStates( sizeA,Aedges, MaxNodeId , STATES ) )
           exit(0);

       int nnodes_orig = MaxNodeId;

       if (  init_n_node_bn != -1   ) {
           nnodes_orig = init_n_node_bn;
           int tempn = inverted_nodes_bn.size();
           for (int i =0 ; i < tempn ; i++)  {
               int jj =  inverted_nodes_bn[i] - 1;
               if (STATES[jj] > 0)
                   STATES[jj] = 0;
               else
                   STATES[jj] = 1;
           }
       }

       PrintState(OUT,  nnodes_orig  , STATES );

       OUT.close();
       return 0;
    }
    /////////////////////////////////////////////////////////////

   
    bool * BinState = new bool [NumberNodes];
    //bool * BinNext = new bool [NumberNodes];

    // If there are fixed points, negative selfloops are zero
    //for ( int i=NewNumberNodes ; i < NumberNodes ; i++ ) 
    //    BinState[i] = 0;
  
     /// read fixed points of reduced fixed points
    int Nfix_red  = -1;
    while (getline(cin,label) ) {
       
       if (label == "FIXED_POINTS_REDUCED") {
          cin >>  Nfix_red  ;
          if ( Nfix_red == 0 ) {
             OUT.close();
             return 0; 
          }
          for (int state = 0 ; state < Nfix_red ; state ++ ) { 
             for  ( int i =0 ; i < NumberNodes ; i ++ ) {
                cin >> BinState[i]  ;
                if ( !cin ) {
                //if ( (BinState[i] != 0)  ||  ( BinState[i] != 1 ) ) {
                //   cerr << " error reading fixed points of reduced network " << endl;
                   return 0;
                }
             }
             
             /// Map to states of original    
             for (int i =  0 ; i < NumberNodes ; i++) {
                if ( BinState[i] ) 
                   STATES[ HASH[i] ] = 1;                   
                else 
                   STATES[ HASH[i] ] = 0;
             }
             
             OUT << "FP of reduced graph : ";
             for (int i = 0 ; i < NumberNodes  ; i++ ) {
                if ( BinState[i] ) OUT << 1;
                else OUT << 0;
             }  OUT << endl; 

             if (! FindAllStates( sizeA,Aedges, MaxNodeId , STATES ) ) {
                exit(0);
             }

             int nnodes_orig = MaxNodeId;

             if (  init_n_node_bn != -1   ) {
                nnodes_orig = init_n_node_bn;
                int tempn = inverted_nodes_bn.size();
                for (int i =0 ; i < tempn ; i++)  {
                   int jj =  inverted_nodes_bn[i] - 1;                  
                   if (STATES[jj] > 0) 
                       STATES[jj] = 0;
                   else 
                       STATES[jj] = 1;
       
                }
             }

             PrintState(OUT,  nnodes_orig  , STATES );

             if (  init_n_node_bn != -1   ) {
                 int tempn = inverted_nodes_bn.size();
                 for (int i =0 ; i < tempn ; i++)  {
                    int jj =  inverted_nodes_bn[i] - 1;                  
                    if (STATES[jj] > 0) 
                        STATES[jj] = 0;
                    else 
                        STATES[jj] = 1;
       
                 }
             }
 
          }
       }
       break;
    }

    delete [] NAMES;
    delete [] HASH;
    delete [] AdjList;
    delete [] Ninputs;
    delete [] IndexNet;
    delete [] BinState;
    delete [] STATES ;

    OUT.close();
    return 0;
}

//////////////////////////////////////// FindAllStates
// Find all the values of the Original Boolean Network
// using the acyclick Grpah
//
// Parameters:
// int          sizeA , Number of edges of Acyclic grapfh
// A_edge       Aedges, Structure to store a Acyclic Edge 
// int          MaxNodeId, Number of Nodes of Original Network
// int          STATES , array that stores the states of genes
//                       -1 Zero node
//                        0 
//                        1
//                        2 not assigned value 
//
// Returns: true if everything is ok
//
// Called by: main ()
// Calls: none
bool FindAllStates(int  sizeA, vector<A_edge> &Aedges, int MaxNodeId , int * STATES ) {

    /// use the acyclicj graph to find states of all nodes
    // reseting the vi nodes;
    for (int j  = 0  ;  j < sizeA   ; j++ ) 
         STATES[ Aedges[j].vi -1 ] = 2;
     

    for (int j  = sizeA-1 ; j >= 0  ; j-- ) {
        int jj = Aedges[j].vj ;
        int ii = Aedges[j].vi ;

        if ( STATES[jj-1]  == 2 ) {
           cerr << " Problems with node " << jj << endl;
           return false ;
        }

        int temp = 0;
        if ( Aedges[j].sign == -1) {
           if ((STATES[jj-1] == 0 ) &&  (STATES[ii-1] > 0))
              temp = 1;
        }
        else {
           if ((STATES[jj-1] != 0 ) &&  (STATES[ii-1] > 0))
              temp  = 1;
        }
        STATES[ii-1] = temp;

     }
     return true;
}

//////////////////////////////////////// FindAllStates
// Prin the fixed point of a the Original Network
// if one state was not correclt obtained the function prints "?"
//
// Parameters:
// ostream       OUT,  Handler of Output file 
// int          MaxNodeId, Number of Nodes of Original Network
// int          STATES , array that stores the states of genes
void PrintState( ostream & OUT,   int MaxNodeId , int * STATES ) {
     for (int i=0 ; i < MaxNodeId ; i++) {
        if (STATES[i] == -1 ) cout << 0 ;
        else if (STATES[i] == 0 ) cout << 0;
        else if (STATES[i] == 1 ) cout << 1;
        else cout << "?";
     }
     cout << endl;
}
