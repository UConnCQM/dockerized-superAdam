/*  Using the library boolstuff to parse a boolean
    expression into an binary evaluation tree.

    then create a new binary tree with only
    AND and OR functions assuming that the NOT
    operations are always  
    
*/

#include <boolstuff/BoolExprParser.h>
#include <assert.h>
#include <stdlib.h>
#include <vector>
#include <sstream>
#include <iostream>
#include <fstream>

using namespace std;
using namespace boolstuff;



// Node Kind, VAR is for variables
//            OR, AND for boolean operation 
enum Kind { OR, AND, VAR};

// Tree Node
struct bNode {
   bNode *left;   // left child
   bNode *right;  // right child
   Kind kind;     // OR, AND, or VAR
   string name;   // Name of variable, used only for VAR kind
   bool nega;     // if one then variables i ~x
   // Constructure for a variable node
   bNode (string Name, bool Nega) {
      kind = VAR;
      name =  Name;
      nega = Nega;
      left = NULL;
      right = NULL;
   }
   // Constructure for a Operation (OR AND) ndoe
   bNode (Kind Op , bNode * Left, bNode * Right){
      kind =  Op;
      this -> left = Left; 
      this -> right = Right;
      name = "";
      nega = false;
   } 

   ~bNode () {
      //cout << "destroy" <<     endl;
      delete left;
      delete right; 
   }
};

// Delete the tree
void destroy_tree(bNode *leaf)
{
  if(leaf!=NULL)
  {
    destroy_tree(leaf->left);
    destroy_tree(leaf->right);
    delete leaf;
  }
}

// Print the variable names 
// '~' means negative regulator
// otherwise positive regulator
void printTree(  std::ofstream& OUT ,   bNode * root) {
   if ( root == NULL )
       return;

   if ( root->kind ==  VAR ){
       if (root -> nega ) 
           OUT << "~";
       OUT << root->name ;
       return;
   }
   OUT <<"(";
   printTree( OUT ,   root -> left ) ;

   if (root -> kind == OR  )
      OUT << " | ";
   else 
      OUT << " & ";

   printTree( OUT ,  root -> right ) ;
   OUT <<")";
}

// Set an existing node to VAR kind
void settoVAR(bNode * leaf, string name, bool nega ) {
   leaf ->left = NULL;
   leaf ->right = NULL;
   leaf -> nega = nega ;
   leaf -> name = name;
   leaf -> kind = VAR;
}

bool CheckORonly( bNode*root ) {
   if (root == NULL) return false ;

   if (root->kind == OR ) {
      bool left =  CheckORonly(root->left) ;
      bool right =  CheckORonly(root->right) ;
      return  ( left && right );

   }
   else if ( root->kind == AND ) {
        return false;
   }
   else {
       return true;
   }

}


// Nagetae variables and change all OR to AND
bool  NegateAllVar( bNode*root  ) {
    if (root == NULL) return  false;

    if (root->kind == OR  ) {
        root->kind = AND;
        bool Left = NegateAllVar( root->left  ) ;
        bool Right = NegateAllVar( root->right ) ;
        return  (Left && Right )  ;
    }

    else if (root->kind == AND  ) { return false; }

    //if ( root -> kind == VAR) {

    root -> nega = !(root->nega) ;
         // create a new name 
    //}
    return true ;
}



bNode * CloneTree (BoolExpr<string> * expr){

   if ( expr == NULL )
       return NULL;

   BoolExpr<string>::Type type =  expr -> getType();

   if ( type ==  BoolExpr<string>::VALUE  ) 
       return new bNode(expr->getValue() , false );
   
   if ( type == BoolExpr<string>::NOT  ) {
       if ( expr->getRight() == NULL) {
           cerr << "A lonely NOT operation " << endl;
           assert(false);
       }
       if ( expr->getRight()-> getType() !=  BoolExpr<string>::VALUE ) {
           cout  << "There is not variable after NOT " << endl;
           assert(false);
       }
       return new bNode(expr->getRight()->getValue() , true );
   }

   assert(type == BoolExpr<string>::OR || type == BoolExpr<string>::AND);
   assert( (expr->getLeft()!=NULL) && (expr->getRight()!=NULL)) ;
   
   bNode *leftClone = CloneTree( expr->getLeft() );
   bNode *rightClone = CloneTree( expr->getRight() );

   if (type == BoolExpr<string>::OR ) 
      return new bNode (OR , leftClone, rightClone  ) ;
   else
      return new bNode (AND, leftClone, rightClone  ) ;
   
}


bool convertAndNot2( bNode* Fi, bool isroot, vector<bNode*> &Functions, vector<string> &Names ) {
    if ( Fi == NULL) return false;

    if (Fi->kind == VAR ) return false ;

    else if ( Fi->kind == AND ) {

       bool testORl =  convertAndNot2(Fi->left, false, Functions, Names);
       bool testORr = convertAndNot2(Fi->right, false, Functions, Names);
       return (testORl || testORr) ;
      

    }
    
    else { // OR
        
       bool onlyORs  = CheckORonly( Fi );
         
       if (isroot && onlyORs  ) {
               return false;
       }      
       else if  ( onlyORs ) {

          int N = Functions.size();

          if ( ! NegateAllVar( Fi  ) )  {
              cerr << " Probles in detecting  only ORs " << endl;
          }
              
          // create a new name 
          string newname;
          ostringstream convert;
          convert << ++N;
          newname = "x" + convert.str();

          // create a new Variable with newname
          Functions.push_back( new bNode(AND , Fi->left, Fi->right));
          Names.push_back( newname);

          // change this node to VAR
          settoVAR( Fi,newname , true );
          return false;           
             
       }



       if ( Fi->left->kind != VAR ) {

           int N = Functions.size();
           // create a new name 
           string newname;
           ostringstream convert;
           convert << ++N;
           newname = "x" + convert.str();

           // create a new Variable with newname
           Functions.push_back( new bNode(Fi->left->kind,
                                          Fi->left->left,
                                          Fi->left->right));
           Names.push_back(newname);

           // change this node to VAR
           settoVAR( Fi->left,newname , false );
       }

       if ( Fi->right->kind != VAR ) {

           int N = Functions.size();

           // create a new name 
           string newname;
           ostringstream convert;
           convert << ++N;
           newname = "x" + convert.str();

           // create a new Variable with newname
           Functions.push_back(new bNode(Fi->right->kind,
                                         Fi->right->left,
                                         Fi->right->right));
           Names.push_back(newname);

           // change this node to VAR
           settoVAR( Fi->right,newname ,false );
       }

       return true;
    }
   
}


bool convertAndNot( bNode* Fi, bool isroot, vector<bNode*> &Functions, vector<string> &Names) {
   
    if ( Fi == NULL) return false;

    if (Fi->kind == VAR ) return false ;

    else if ( Fi->kind == AND ) {

       bool testORl =  convertAndNot(Fi->left, false, Functions, Names);
       bool testORr = convertAndNot(Fi->right, false, Functions, Names);
       return (testORl || testORr) ;
    }
    else { // OR

       bNode * FLeft = Fi->left;
       bNode * FRight = Fi->right;

       if ( (FLeft->kind == VAR) && (FRight->kind == VAR) )  {

           if (isroot) {
               return false;
           }
  
           int N = Functions.size();

           // Negar los hijos
           Fi->left->nega = !(Fi->left->nega);
           Fi->right->nega = !(Fi->right->nega);
          
           // create a new name 
           string newname; 
           ostringstream convert; 
           convert << ++N;
           newname = "x" + convert.str(); 

           // create a new Variable with newname
           Functions.push_back( new bNode(AND , Fi->left, Fi->right)); 
           Names.push_back( newname); 
         
           // change this node to VAR
           settoVAR( Fi,newname , true );
           return false;

       }   

       if ( Fi->left->kind != VAR ) {

           int N = Functions.size();
           // create a new name 
           string newname;
           ostringstream convert;
           convert << ++N;
           newname = "x" + convert.str();

           // create a new Variable with newname
           Functions.push_back( new bNode(Fi->left->kind,
                                          Fi->left->left,
                                          Fi->left->right));
           Names.push_back(newname);

           // change this node to VAR
           settoVAR( Fi->left,newname , false );
       }

       if ( Fi->right->kind != VAR ) {

           int N = Functions.size();

           // create a new name 
           string newname;
           ostringstream convert;
           convert << ++N;
           newname = "x" + convert.str();

           // create a new Variable with newname
           Functions.push_back(new bNode(Fi->right->kind, 
                                         Fi->right->left, 
                                         Fi->right->right));
           Names.push_back(newname);

           // change this node to VAR
           settoVAR( Fi->right,newname ,false );
       }

       return true;
    // 
    }
}

// Negate one variable
void NegateVar(bNode*root , int index  ) {
    if (root == NULL) return ;

    if (root->kind == OR || root->kind == AND ) {
        NegateVar( root->left , index  ) ;
        NegateVar( root->right , index  ) ;
        return ;
    }
    
    if ( root -> kind == VAR) {
  
      string myString = root -> name   ;
      myString.erase (myString.begin());
      int value = atoi(myString.c_str()); 

      if ( value == index ) {
         root -> nega = !(root->nega) ;
         // create a new name 
         ostringstream convert;
         convert << index ;
         root -> name  = "s" + convert.str();  
       }
    }
    return ;
}


// Count the number of Edges (number of leafs)
int CountEdges(bNode*root   ) {
    if (root == NULL) return 0 ;

    if (root->name == "0" ) return 0 ; //constat nodes 
  
    if (root->kind == OR || root->kind == AND ) {
        int Nleft = CountEdges( root->left  ) ;
        int Nright =  CountEdges( root->right  ) ;
        return Nleft+Nright ;
    }
    else if ( root -> kind == VAR)  return 1;
    
    else return 0 ;
}

// print Edges
void PrintEdges(bNode*root, int NodeId   ) {
    if (root == NULL) return ;

    if (root->kind == OR || root->kind == AND ) {
        PrintEdges( root->left , NodeId ) ;
        PrintEdges( root->right, NodeId  ) ;
        return  ;
    }
    else if ( root -> kind == VAR) {

       string myString = root -> name   ;
       myString.erase (myString.begin());
       int value = atoi(myString.c_str());
       cout << endl <<  value << " " << NodeId << " " ;
       if (root -> nega)
          cout << "-" ;
       cout << "1";

    }
}

// Function that tryes to evaluate
// not expresion so that not operators
// are always near a value
bool NOTtest( BoolExpr<string> *expr ) {
  if ( expr  == NULL )
       return true;

  BoolExpr<string>::Type type =  expr -> getType();

  if ( type ==  BoolExpr<string>::VALUE  )
     return true;
  else if ( type ==  BoolExpr<string>::NOT ) {
     BoolExpr<string>::Type type2 =  expr->getRight()->getType() ;
     if ( type2 == BoolExpr<string>::VALUE )
        return true;
     else if ( type2 == BoolExpr<string>::NOT ) {

        expr->setType( expr->getRight()->getRight()->getType() );
        expr->setValue( expr->getRight()->getRight()->getValue() );
        BoolExpr<string> * newleft ;
        BoolExpr<string> * newright;
        newleft = expr->getRight()->getRight()->getLeft() ;
        newright = expr->getRight()->getRight()->getRight() ;
        expr->setRight( newright )  ;
        expr->setLeft( newleft  )  ;

        return false;
     }
     else {
        BoolExpr<string>::Type newtype ;

        if  ( type2 == BoolExpr<string>::AND )
            newtype = BoolExpr<string>::OR;
        else if ( type2 == BoolExpr<string>::OR )
            newtype = BoolExpr<string>::AND;
        else
           assert(false);


        BoolExpr<string> *left = new BoolExpr<string>( BoolExpr<string>::NOT, NULL, expr->getRight()->getLeft() );

        BoolExpr<string> *right = new BoolExpr<string>( BoolExpr<string>::NOT, NULL, expr->getRight()->getRight() );

        expr->setRight( right )  ;
        expr->setLeft( left  )  ;

        expr->setType( newtype );
        return false;
     }

  }

  return ( NOTtest(expr->getRight()) &&  NOTtest(expr->getLeft()) ) ;
}



int main(int argc, char *argv[])
{

//    if (argc != 2) {
//      cerr << "One argument is require (input file)" << endl;
//      exit(0);
//    }

//    string File = argv[1];

    BoolExprParser parser;
    string line;
    
    vector<bNode*> Functions;
    vector<string> Names ;

//    ifstream infile;
//    infile.open( File.c_str() );
//    if (!infile) {
//       cerr << "problems opening files  "<< File << endl ;
//       exit(0);
//    }

    // trying to open the output file for writing
    ofstream OUT( "BuildAndNot.log" ) ;
    if (!OUT) {
        cerr << "Error creating(opening) output file ...exiting" << endl;
        exit(1) ;
    }

    int counter = 0;
    while ( getline(cin,line) ) {
	try {

  	    BoolExpr<string> *expr = parser.parse(line) ;

            // Simpify NOT operators
            bool correctNOT = NOTtest( expr ) ;
            int ntries = 0; 
            while ( !correctNOT  && ntries < 1000 ) {
               correctNOT = NOTtest( expr );
               ntries++;
            }
            if (! NOTtest( expr ))  {
               cerr << " problems with NOT operators " << endl;
               return 0;
            }

            bNode* Fi =  CloneTree(expr);
            Functions.push_back(Fi) ;

            ostringstream convert;
            convert << ++counter;
            string name = "x" + convert.str();
            Names.push_back(name);
            
	    delete expr;
	}
	catch (BoolExprParser::Error &err) {
	    cerr << "Column " << err.index + 1
		<< ": error #" << err.code << endl;
	}
    }
    //infile.close();

    int nitial_n_nodes = Functions.size();

    for (unsigned int i=0 ; i< Functions.size() ; ++i){
       bool ORtest = true;
       while (ORtest)
           ORtest = convertAndNot2(Functions[i], true , Functions,Names);

    }

    // step 2
    // Chage OR functions to AND
    vector<bool> Negatives( Names.size(), false );
    for (unsigned int i=0 ; i< Functions.size() ; ++i){
        bNode * root = Functions[i];
        if ( root -> kind == OR ) {


           //if (root->left->kind == VAR && root->right->kind == VAR){
           //    root->kind = AND;
           //    root->left->nega = !(root->left->nega);
           //    root->right->nega = !(root->right->nega);
           //    Negatives[i] = true ;
          // }
           if (  CheckORonly( root ) ) {
              NegateAllVar(root) ;
              Negatives[i] = true ;   
           }
           else {
              cerr << " Problems with the first stage " << endl;
              exit(0);
           }
        }
    }
    // Handle  Constant Functions
    for (unsigned int i=0 ; i< Functions.size() ; ++i){
        string VarName = Functions[i]->name;
        if ( Functions[i]->kind==VAR  && VarName.size() == 1 ) {
           if (VarName == "0") {
              Functions[i]->nega = false;
              Negatives[i] = true;
           }
           else {
              Functions[i]->nega = false;
              Functions[i]->name = "0";
           }                       
        }
    }

    for (unsigned int i=0 ; i< Negatives.size() ; ++i){
       if ( Negatives[i] ) { 
          
           Names[i].replace(0,1,"s"); 
           for (unsigned int j=0 ; j< Functions.size() ; ++j)
               NegateVar( Functions[j] , i+1 ) ;
       }
    }
    
    int NumEdges = 0;
    for (unsigned int i=0 ; i< Functions.size() ; ++i)
          NumEdges = NumEdges + CountEdges(Functions[i]); 
    
    cout << Functions.size() << endl;
    cout << NumEdges ;
    
    for (unsigned int i=0 ; i< Functions.size() ; ++i){
         if (Functions[i]->name != "0") //cosntant nodes not printed
             PrintEdges(Functions[i], i+1 ); 
    }
    
    cout << endl;
    cout << endl;
    
    cout << "ZERO_NODES" << endl;
    cout << endl;

    cout << "INITIAL_NUMBER_OF_NODES_OF_BN" << endl;
    cout <<  nitial_n_nodes << endl << endl;

    cout << "INVERTED_NODES_BN"<<endl;
    for (unsigned int i=0 ; i< Negatives.size() ; ++i){
       if ( Negatives[i] ) cout << i+1 << endl;      
    }

    //cout << " TESSSTT " << endl;
    for (unsigned int i=0 ; i< Functions.size() ; ++i){
       printTree( OUT ,   Functions[i]  );
       OUT  << endl;
    }

         
    for (unsigned int i=0 ; i< Functions.size() ; ++i){
                 delete Functions[i] ;
    }
    OUT.close(); 
    return EXIT_SUCCESS;
}


