/*  $Id: checkc.c,v 1.14 2007/08/26 18:45:26 sarrazip Exp $

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
    Testing program for the library's C API.
*/

#include <boolstuff/c-api.h>
#include <stdlib.h>
#include <string.h>

#undef NDEBUG
#include <assert.h>

#define LEFT(x) boolstuff_get_left_subtree(x)
#define RIGHT(x) boolstuff_get_right_subtree(x)
#define TYPE(x) boolstuff_get_node_type(x)

int
main(int argc, char *argv[])
{
    boolexpr_t upper_and_node, lower_and_node, or_node, not_node,
    		a_node, b_node, c_node, d_node,
		dnf_root, dnf_clone,
		parsed_tree;
    int in_dnf;
    char *str;
    size_t error_index = 0;
    enum bool_error_t error_code = BOOLSTUFF_OK;

    setbuf(stdout, NULL);  /* turn off buffering; useful when asserts fail */

    printf("Checking the C API\n\n");

    /*
	Create a tree for the expression a & !(b | c & d).
	Some superfluous steps are inserted for testing purposes.

	The tree structure is supposed to be this:

              AND
             /   \
            a     NOT
                     \
                      OR
                     /  \
                    b    AND
                        /   \
                       c     d
    */
    a_node = boolstuff_create_value_node("x");
    assert(boolstuff_get_node_type(a_node) == BOOLSTUFF_VALUE);
    boolstuff_set_node_value(a_node, "a");
    assert(strcmp(boolstuff_get_node_value(a_node), "a") == 0);

    b_node = boolstuff_create_value_node("b");
    c_node = boolstuff_create_value_node("c");
    d_node = boolstuff_create_value_node("d");

    lower_and_node = boolstuff_create_operator_node(
					BOOLSTUFF_AND, c_node, d_node);

    or_node = boolstuff_create_operator_node(
					BOOLSTUFF_OR, b_node, lower_and_node);
    assert(boolstuff_get_node_type(or_node) == BOOLSTUFF_OR);
    assert(boolstuff_get_left_subtree(or_node) == b_node);
    assert(boolstuff_get_right_subtree(or_node) == lower_and_node);

    boolstuff_set_left_subtree(or_node, c_node);
    assert(boolstuff_get_left_subtree(or_node) == c_node);

    boolstuff_set_left_subtree(or_node, NULL);
    assert(boolstuff_get_left_subtree(or_node) == NULL);

    boolstuff_set_left_subtree(or_node, b_node);
    assert(boolstuff_get_left_subtree(or_node) == b_node);

    boolstuff_set_right_subtree(or_node, b_node);
    assert(boolstuff_get_right_subtree(or_node) == b_node);

    boolstuff_set_right_subtree(or_node, NULL);
    assert(boolstuff_get_right_subtree(or_node) == NULL);

    boolstuff_set_right_subtree(or_node, lower_and_node);
    assert(boolstuff_get_right_subtree(or_node) == lower_and_node);

    not_node = boolstuff_create_operator_node(BOOLSTUFF_NOT, NULL, or_node);

    upper_and_node = boolstuff_create_operator_node(
					BOOLSTUFF_AND, a_node, not_node);
    assert(boolstuff_get_node_type(upper_and_node) == BOOLSTUFF_AND);

    printf("Original expression: ");
    boolstuff_print_tree(stdout, upper_and_node);
    printf("\n");

    /*
	Test the structure of the tree, which is expected to be this:
    */
    assert(LEFT(upper_and_node) != NULL);
    assert(LEFT(LEFT(upper_and_node)) == NULL);
    assert(RIGHT(LEFT(upper_and_node)) == NULL);
    assert(RIGHT(upper_and_node) != NULL);
    assert(LEFT(RIGHT(upper_and_node)) == NULL);
    assert(RIGHT(RIGHT(upper_and_node)) != NULL);
    assert(LEFT(RIGHT(RIGHT(upper_and_node))) != NULL);
    assert(RIGHT(RIGHT(RIGHT(upper_and_node))) != NULL);
    assert(LEFT(RIGHT(RIGHT(RIGHT(upper_and_node)))) != NULL);
    assert(RIGHT(RIGHT(RIGHT(RIGHT(upper_and_node)))) != NULL);

    /*
	Test the types of the nodes:
    */
    assert(TYPE(upper_and_node) == BOOLSTUFF_AND);
    assert(TYPE(LEFT(upper_and_node)) == BOOLSTUFF_VALUE);
    assert(TYPE(RIGHT(upper_and_node)) == BOOLSTUFF_NOT);
    assert(TYPE(RIGHT(RIGHT(upper_and_node))) == BOOLSTUFF_OR);
    assert(TYPE(LEFT(RIGHT(RIGHT(upper_and_node)))) == BOOLSTUFF_VALUE);
    assert(TYPE(RIGHT(RIGHT(RIGHT(upper_and_node)))) == BOOLSTUFF_AND);

    str = boolstuff_print_tree_to_string(upper_and_node);
    printf("Again: %s\n", str);
    assert(strcmp(str, "a&!(b|c&d)") == 0);

    boolstuff_free_string(str);

    in_dnf = boolstuff_is_disjunctive_normal_form(upper_and_node);
    printf("In DNF? %s\n", in_dnf != 0 ? "yes" : "no");
    assert(in_dnf == 0);

    assert(boolstuff_get_disjunctive_normal_form(NULL) == NULL);

    dnf_root = boolstuff_get_disjunctive_normal_form(upper_and_node);
    assert(dnf_root != NULL);
		/* result cannot be null is this particular case */

    str = boolstuff_print_tree_to_string(dnf_root);
    printf("DNF: %s\n", str);
    assert(strcmp(str, "a&!b&!c|a&!b&!d") == 0);

    boolstuff_free_string(str);

    in_dnf = boolstuff_is_disjunctive_normal_form(dnf_root);
    printf("In DNF? %s\n", in_dnf != 0 ? "yes" : "no");
    assert(in_dnf != 0);


    /*
	Test DNF term roots:
    */
    {
	boolexpr_t *array;
	size_t array_size, i;

	array = boolstuff_get_dnf_term_roots(dnf_root, &array_size);
	assert(array != NULL);
	assert(array_size > 0);
	assert(array[array_size] == NULL);

	printf("DNF term roots: %u terms:\n{\n", (unsigned) array_size);
	for (i = 0; i < array_size; i++)
	{
	    char *s;
	    char **posArray, **negArray;
	    size_t j;

	    assert(array[i] != NULL);
	    s = boolstuff_print_tree_to_string(array[i]);
	    printf("Term at index %u: %s\n", (unsigned) i, s);
	    boolstuff_free_string(s);

	    boolstuff_get_tree_variables(array[i], &posArray, &negArray);

	    printf("\t""Positives: ");
	    for (j = 0; posArray[j] != NULL; j++)
		printf("'%s' ", posArray[j]);
	    printf("\n");
	    printf("\t""Negatives: ");
	    for (j = 0; negArray[j] != NULL; j++)
		printf("'%s' ", negArray[j]);
	    printf("\n");
	    
	    boolstuff_free_variables_sets(posArray, negArray);
	}
	assert(array[array_size] == NULL);
	printf("}\n");

	boolstuff_free_node_array(array);
    }


    /*
	Test tree cloning:
    */

    dnf_clone = boolstuff_clone_tree(dnf_root);

    boolstuff_destroy_tree(dnf_root);

    str = boolstuff_print_tree_to_string(dnf_clone);
    printf("Clone of DNF tree: %s\n", str);
    assert(strcmp(str, "a&!b&!c|a&!b&!d") == 0);
    boolstuff_free_string(str);

    in_dnf = boolstuff_is_disjunctive_normal_form(dnf_clone);
    printf("Clone in DNF? %s\n", in_dnf != 0 ? "yes" : "no");
    assert(in_dnf != 0);

    boolstuff_destroy_tree(dnf_clone);


    /*
	Test the parser:
    */
    const char *text = "!(a|b)&(c1_x|!d)";
    parsed_tree = boolstuff_parse(text, &error_index, &error_code);
    if (parsed_tree == NULL)
    {
	const char *msg = NULL;

	switch (error_code)
	{
	    case BOOLSTUFF_ERR_GARBAGE_AT_END:
		msg = "garbage at end";
		break;
	    case BOOLSTUFF_ERR_RUNAWAY_PARENTHESIS:
		msg = "runaway parenthesis";
		break;
	    case BOOLSTUFF_ERR_IDENTIFIER_EXPECTED:
		msg = "identifier expected";
		break;
	    default:
		assert(!"unexpected error code");
	}

	printf("Parse error #%d at column %u: %s\n",
		error_code, error_index + 1, msg);
	printf("Parsed text: \"%s\"\n", text);

	return EXIT_FAILURE;
    }
    printf("Parsed: ");
    boolstuff_print_tree(stdout, parsed_tree);
    printf("\n");

    assert(TYPE(parsed_tree) == BOOLSTUFF_AND);
    assert(TYPE(LEFT(parsed_tree)) == BOOLSTUFF_NOT);
    assert(TYPE(RIGHT(parsed_tree)) == BOOLSTUFF_OR);

    printf("\n""C API tested successfully.\n");
    return EXIT_SUCCESS;
}
