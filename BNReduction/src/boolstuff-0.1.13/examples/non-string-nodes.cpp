/*  $Id: non-string-nodes.cpp,v 1.1 2005/02/03 23:28:34 sarrazip Exp $
    Example where the expression tree nodes are not based on strings.

    Written by Pierre Sarrazin <http://sarrazip.com/>
    This file is in the public domain.
*/

#include <boolstuff/BoolExprParser.h>
#include <assert.h>
#include <stdlib.h>

using namespace std;
using namespace boolstuff;


class Node
{
public:
    Node(int n = 0) : value(n) {}
    int value;
};


ostream &operator << (ostream &out, const Node &n)
{
    return out << n.value;
}

bool operator == (Node a, Node b)
{
    return a.value == b.value;
}

bool operator < (Node a, Node b)
{
    return a.value < b.value;
}


int main(int argc, char *argv[])
{
    {
	try
	{
	    // Form the expression 11 & (22 | 33):
	    BoolExpr<Node> *a = new BoolExpr<Node>(11);
	    BoolExpr<Node> *b = new BoolExpr<Node>(22);
	    BoolExpr<Node> *c = new BoolExpr<Node>(33);
	    BoolExpr<Node> *orOper =
			    new BoolExpr<Node>(BoolExpr<Node>::OR, b, c);
	    BoolExpr<Node> *expr =
			    new BoolExpr<Node>(BoolExpr<Node>::AND, a, orOper);

	    cout << expr << "\n";

	    expr = BoolExpr<Node>::getDisjunctiveNormalForm(expr);
	    if (expr != NULL)
		cout << expr;
	    else
		cout << "FALSE";

	    cout << "\n";

	    delete expr;
	}
	catch (BoolExprParser::Error &err)
	{
	    cerr << "Column " << err.index + 1
		<< ": error #" << err.code << endl;
	}
    }

    return EXIT_SUCCESS;
}
