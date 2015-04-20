/*  $Id: checkdnf.cpp,v 1.13 2007/08/26 18:45:26 sarrazip Exp $
    checkdnf.cpp - C++ API test program

    boolstuff - Disjunctive Normal Form boolean expression library
    Copyright (C) 2002-2005 Pierre Sarrazin <http://sarrazip.com/>

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation; either version 2
    of the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
    02111-1307, USA.
*/

/*
    Testing program for the library.
*/

#include <boolstuff/BoolExprParser.h>

#undef NDEBUG
#include <assert.h>

using namespace std;
using namespace boolstuff;


int
main(int argc, char *argv[])
{
    BoolExprParser parser;
    string line;
    while (getline(cin, line))
    {
	try
	{
	    BoolExpr<string> *expr = parser.parse(line);

	    assert(expr != NULL);
	    switch (expr->getType())
	    {
		case BoolExpr<string>::VALUE:
		    assert(expr->getLeft() == NULL);
		    assert(expr->getRight() == NULL);
		    break;
		case BoolExpr<string>::AND:
		case BoolExpr<string>::OR:
		    assert(expr->getLeft() != NULL);
		    assert(expr->getRight() != NULL);
		    break;
		case BoolExpr<string>::NOT:
		    assert(expr->getLeft() == NULL);
		    assert(expr->getRight() != NULL);
		    break;
		default:
		    assert(!"type of 'expr' is invalid");
	    }

	    expr = BoolExpr<string>::getDisjunctiveNormalForm(expr);

	    if (expr != NULL)
		expr->print(cout);
	    else
		cout << "FALSE";

	    cout << endl;
	    if (expr != NULL && !expr->isDisjunctiveNormalForm())
		cout << "BUG\n";
	    cout << "." << endl;

	    delete expr;
	}
	catch (BoolExprParser::Error &err)
	{
	    cerr << "Column " << err.index + 1 << ": ";
	    switch (err.code)
	    {
		case BoolExprParser::Error::GARBAGE_AT_END:
		    cerr << "garbage at end";
		    break;
		case BoolExprParser::Error::RUNAWAY_PARENTHESIS:
		    cerr << "runaway parenthesis";
		    break;
		case BoolExprParser::Error::IDENTIFIER_EXPECTED:
		    cerr << "identifier expected";
		    break;
		default:
		    assert(!"unexpected error code");
	    }
	    cerr << endl;
	}
    }
    return EXIT_SUCCESS;
}
