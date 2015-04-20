# $Id: Makefile.am,v 1.9 2010/02/14 19:22:02 sarrazip Exp $

bin_PROGRAMS = booldnf

booldnf_SOURCES = booldnf.cpp

booldnf_CXXFLAGS = \
	-I$(top_srcdir)/src \
	-DCOMMAND_NAME=\"booldnf\"

booldnf_LDADD = ../boolstuff/libboolstuff-$(API).la

check: all
if HAVE_GETOPT_LONG
	test "`./booldnf --version`" = "booldnf $(VERSION)"
	@echo "Testing exit status when requesting help page:"
	./booldnf --help >/dev/null
	@echo "Testing exit status when passing unknown option:"
	./booldnf --unknown >/dev/null; test "$$?" = 1
endif
	test "`echo '!(!(a&m))' | ./booldnf`" = 'a&m'
	test "`echo '!(!a)&m' | ./booldnf`" = 'a&m'
	test "`echo '(!(!a)) & (!(!m))' | ./booldnf`" = 'a&m'
	test "`echo '(!!a)&(!!m)' | ./booldnf`" = 'a&m'
	test "`echo '!(!(a|m))' | ./booldnf`" = 'a|m'
	test "`echo 'a&(!b|c&!d)' | ./booldnf`" = 'a&c&!d|a&!b'
	test "`echo '!(a_b|c_d)' | ./booldnf`" = '!a_b&!c_d'
	test "`echo '' | ./booldnf`" = '? column 1 : identifier expected'
	test "`echo '(a' | ./booldnf`" = '? column 1 : runaway parenthesis'
	test "`echo '(a)b' | ./booldnf`" = '? column 4 : garbage at end'
	test "`echo '!(a&c&d|b&c&d)' | ./booldnf`" = '!a&!b|!a&!c|!a&!d|!c&!b|!c|!c&!d|!d&!b|!d&!c|!d'
	test "`echo 'a&!a' | ./booldnf`" = 'a&!a'
	test "`echo 'a&!a | b&!b' | ./booldnf`" = 'a&!a'
	test "`echo 'a&!b' | ./booldnf`" = 'a&!b'
	test "`echo 'a&a' | ./booldnf`" = 'a'
	test "`echo '(!a|b|c)&(a|b|!c)' | ./booldnf`" = 'a&b|a&c|b|b&!a|b&!c|c&b|!c&!a'
	test "_`./booldnf </dev/null`" = _


MAINTAINERCLEANFILES = Makefile.in
