/*  $Id: booldnf.cpp,v 1.12 2010/02/14 19:28:30 sarrazip Exp $
    booldnf.cpp - Program to convert a boolean expression to the DNF

    boolstuff - Disjunctive Normal Form boolean expression library
    Copyright (C) 2002-2010 Pierre Sarrazin <http://sarrazip.com/>

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

#include <boolstuff/BoolExprParser.h>
#include <assert.h>
#include <stdlib.h>
#include <cstdio>

#ifdef HAVE_GETOPT_LONG
#include <unistd.h>
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <getopt.h>
#endif

#include <iostream>

using namespace std;
using namespace boolstuff;


#ifdef HAVE_GETOPT_LONG
static struct option knownOptions[] =
{
    { "help",          no_argument,       NULL, 'h' },
    { "version",       no_argument,       NULL, 'v' },

    { NULL, 0, NULL, 0 }  // marks the end
};
#endif  /* HAVE_GETOPT_LONG */


static void
displayVersionNo()
{
    cout << COMMAND_NAME << ' ' << VERSION << '\n';
}


static void
displayHelp()
{
    cout << '\n';

    displayVersionNo();

    cout << "Part of " << PACKAGE << " " << VERSION << "\n";

    cout <<
"\n"
"Copyright (C) 2002-2010 Pierre Sarrazin <http://sarrazip.com/>\n"
"This program is free software; you may redistribute it under the terms of\n"
"the GNU General Public License.  This program has absolutely no warranty.\n"
    ;

    cout <<
"\n"
"Known options:\n"
"--help             Display this help page and exit\n"
"--version          Display this program's version number and exit\n"
"\n"
"See the " << COMMAND_NAME << "(1) manual page for details.\n"
"\n"
    ;
}


int
main(int argc, char *argv[])
{
    #ifdef HAVE_GETOPT_LONG

    /*  Interpret the command-line options:
    */
    int c;
    do
    {
	c = getopt_long(argc, argv, "", knownOptions, NULL);

	switch (c)
	{
	    case EOF:
		break;  // nothing to do

	    case 'v':
		displayVersionNo();
		return EXIT_FAILURE;

	    case 'h':
		displayHelp();
		return EXIT_SUCCESS;

	    default:
		displayHelp();
		return EXIT_FAILURE;
	}
    } while (c != EOF && c != '?');

    #else  /* ndef HAVE_GETOPT_LONG */

    if (argc != 1)
    {
	displayHelp();
	cout << "This version does not support command-line arguments.\n\n";
	return EXIT_FAILURE;
    }

    #endif  /* ndef HAVE_GETOPT_LONG */


    BoolExprParser parser;
    string line;
    bool success = true;

    while (getline(cin, line))
    {
	try
	{
	    BoolExpr<string> *expr = parser.parse(line);
	    assert(expr != NULL);

	    expr = BoolExpr<string>::getDisjunctiveNormalForm(expr);
	    if (expr != NULL)
		cout << expr;
	    else
	    {
		cout << COMMAND_NAME << ": result too large";
		success = false;
	    }

	    cout << endl;

	    delete expr;
	}
	catch (BoolExprParser::Error &err)
	{
	    cout << "? column " << err.index + 1
		    << " : ";
	    switch (err.code)
	    {
		case BoolExprParser::Error::GARBAGE_AT_END:
		    cout << "garbage at end";
		    break;
		case BoolExprParser::Error::RUNAWAY_PARENTHESIS:
		    cout << "runaway parenthesis";
		    break;
		case BoolExprParser::Error::IDENTIFIER_EXPECTED:
		    cout << "identifier expected";
		    break;
	    }
	    cout << endl;
	    success = false;
	}
    }

    return success ? EXIT_SUCCESS : EXIT_FAILURE;
}
