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

int main(int argc, char **argv)
{

    // trying to open the output file for writing
    ofstream OUT( "topolynomial.log" ) ;
    if (!OUT) {
        cerr << "Error creating(opening) output file ...exiting" << endl;
        exit(1) ;
    }

    // Read the number of nodes and edges    
    int NumberNodes, NumberEdges;
    cin >> NumberNodes;
    cin >> NumberEdges;
   
    // creating an array to store edges Aj Ai sign
    A_edge * Edges  = new A_edge [NumberEdges];


    // The number of inputs per node
    int * Ninputs  = new int [NumberNodes];
    for (int i =0 ; i < NumberNodes ; i++)  Ninputs[i] = 0;

    // Table containing nodes names                                             
    int * HASH  = new int [NumberNodes];
    for (int i=0 ; i < NumberNodes ; i++)  HASH[i] = -1;

   
    // reading the edges from the file and put them into the graph G
    int MaxNodeId=0, k=0;
    for (k = 0;  k<NumberEdges; k++) {
        cin >>  Edges[k].vj >> Edges[k].vi >> Edges[k].sign;
        if ( Edges[k].vj > MaxNodeId ) MaxNodeId = Edges[k].vj;
        if ( Edges[k].vi > MaxNodeId ) MaxNodeId = Edges[k].vi;
    }
    

    // Mapping names to 1 ... Nodes indexes , kind of dirty code    
    int * NAMES  = new int [MaxNodeId];
    int * STATES  = new int [MaxNodeId];
    for (int i = 0 ; i < MaxNodeId ; i++ ) {
          NAMES[i] = -2;
          STATES[i] = 2;
    }

    //////////////////////////////////////////////////////////

    // array with nodes with negative self loop (NSL)
    // Mark the nodes of AndNot that are present
    // in the reduced network    
    for (int i = 0 ; i < NumberEdges ; i++ ) {
        NAMES[ Edges[i].vi - 1 ] = 0;
        NAMES[ Edges[i].vj - 1 ] = 0;
    }


    // Asign HASH (1...nnodes) to Names (1....NAndNot)
    k=0;
    for (int i = 0 ; i < MaxNodeId ; i++ ) {
        if (NAMES[i] == 0)   {
            NAMES[i] = k;
            HASH[k] = i;
            k++;
        }
    }
 
    if (NumberNodes >0 ) OUT << "Doing the folling map : " << endl;
    for (int i = 0 ; i < NumberNodes ; i++ ) 
        OUT << HASH[i]+1 << " --> " << i+1 << endl;



    ////////////////////////////////////////////////////////
    // Counting the Number of inputs per node,
    for (int k  = 0 ; k < NumberEdges ; k++ ) {
        int j = Edges[k].vi;
        int i = NAMES[j-1] ; 
        Ninputs[i]=Ninputs[i] + 1; 
    }



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


    /// Print polynomial form
    for (int k = 0 ; k < NumberNodes ; k++ ) {

        if ( Ninputs[k] == 0 ) {
           cerr << " node " << HASH[k] + 1<< " has no inputs, check your reduced network " << endl; 
           return 0;
        }

        cout << "f" <<k+1; 
  
        for (int p = 1 ; p <= Ninputs[k] ; p++) {
            if (p == 1) cout <<"=" ;
            else cout << "*";

            int j =  AdjList[ IndexNet[k] + p];
            if ( j < 0 ) cout << "(1-x" << -j << ")" ;
            else cout << "x" << j ;

        }
        cout << endl;
    }

    delete [] Edges;
    delete [] NAMES;
    delete [] HASH;
    delete [] AdjList;
    delete [] Ninputs;
    delete [] IndexNet;
    delete [] STATES ;

    OUT.close();
    return 0;
}

