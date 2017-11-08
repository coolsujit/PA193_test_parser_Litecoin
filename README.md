# Parser-cryptocoin
Parser for Litecoin
[![Build Status](https://travis-ci.org/coolsujit/PA193_test_parser_Litecoin.svg?branch=master)](https://travis-ci.org/coolsujit/PA193_test_parser_Litecoin)


Liteparser.c is the program which is uses to parse the Litecoin block and display its information.
It also finds out whether the block is valid or not.
The block validity is ascertained as under :-
(a) The magic no is valid or not 
(b) The previous block hash mentioned is valid or not
(c) The transactions hash are used to calculate the merkel root hash and same is verified with the merkel root hash given in the block
(d) Whether the input transactions are valid or not.
(e) And finally whether the output transactions are valid or not.

Till now 

Part a is completed successfully.
