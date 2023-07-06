compile:
	gcc -Wall project2.c -o project2

run: test1  test2  test3  test4  test5  test6  test7  test8  test9  test10 

test1:
	cd project2tests/ ;\
	../project2 10 10 10 < a_in_1 > my_out_1 2>/dev/null;\
	diff a_out_1 my_out_1

test2:
	cd project2tests/ ;\
	../project2 10 10 10 < a_in_2 > my_out_2 2>/dev/null;\
	diff a_out_2 my_out_2

test3:
	cd project2tests/ ;\
	../project2 2 2 10 < a_in_3 > my_out_3 2>/dev/null;\
	diff a_out_3 my_out_3

test4:
	cd project2tests/ ;\
	../project2 2 2 10 < a_in_4 > my_out_4 2>/dev/null;\
	diff a_out_4 my_out_4

test5:
	cd project2tests/ ;\
	../project2 2 2 10 < a_in_5 > my_out_5 2>/dev/null;\
	diff a_out_5 my_out_5

test6:
	cd project2tests/ ;\
	../project2 3 3 3 < a_in_6 > my_out_6 2>/dev/null;\
	diff a_out_6 my_out_6

test7:
	cd project2tests/ ;\
	../project2 3 3 3 < a_in_7 > my_out_7 2>/dev/null;\
	diff a_out_7 my_out_7

test8:
	cd project2tests/ ;\
	../project2 1 1 1 < a_in_8 > my_out_8 2>/dev/null;\
	diff a_out_8 my_out_8

test9:
	cd project2tests/ ;\
	../project2 100 250 38 < a_in_9 > my_out_9 2>/dev/null;\
	diff a_out_9 my_out_9

test10:
	cd project2tests/ ;\
	../project2 250 250 250 < a_in_10 > my_out_10 2>/dev/null;\
	diff a_out_10 my_out_10

clean:
	rm project2tests/my*
