/*  $Id: example.cpp,v 1.4 2005/02/03 23:26:43 sarrazip Exp $
    Testing program for the library.

    Written by Pierre Sarrazin <http://sarrazip.com/>
    This file is in the public domain.
*/

#include <boolstuff/BoolExprParser.h>
#include <assert.h>
#include <stdlib.h>

using namespace std;
using namespace boolstuff;


static
ostream &
operator << (ostream &out, const set<string> &s)
{
    for (set<string>::const_iterator it = s.begin(); it != s.end(); it++)
	out << *it << " ";
    return out;
}


int main(int argc, char *argv[])
{
    BoolExprParser parser;
    string line;

    while (getline(cin, line))
    {
	try
	{
	    BoolExpr<string> *expr = parser.parse(line);
	    cout << "Original expression     : " << flush;
	    assert(expr != NULL);
	    expr->print(cout);
	    cout << "\n";

	    expr = BoolExpr<string>::getDisjunctiveNormalForm(expr);
	    cout << "Disjunctive normal form : ";
	    if (expr != NULL)
		cout << expr;
	    else
		cout << "FALSE";

	    cout << "\n";

	    if (expr != NULL)
	    {
		typedef vector<const BoolExpr<string> *> V;
		typedef V::const_iterator IT;

		V termRoots;
		expr->getDNFTermRoots(inserter(termRoots, termRoots.end()));
		for (IT it = termRoots.begin(); it != termRoots.end(); it++)
		{
		    const BoolExpr<string> *term = *it;
		    set<string> positives, negatives;
		    term->getTreeVariables(positives, negatives);

		    cout << "Term       : " << term << "\n";
		    cout << "  Positives: " << positives << "\n";
		    cout << "  Negatives: " << negatives << "\n";
		}
	    }

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
