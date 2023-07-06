#!/usr/bin/python3
#outputs in the std output the tests of the makefile

#create run that runs all tests 
print("run: ",end="")
for i in range(1,11):
    print("test" + str(i) + " ",end=" ")
print()


#create argument list from files
argsList = []
for i in range(1,11):
    f = open("project2tests/a_args_" + str(i),mode="r")
    args = f.readline(-1).split()
    argsList = argsList + args
    f.close

# write tests
for i in range(1,11):
    print("test" +str(i) +":")
    print("\tcd project2tests/ ;\\")
    print("\t../project2 " + argsList[3*(i-1)] + " " + argsList[3*(i-1) + 1]+ " " +argsList[3*(i-1) + 2] +  " < a_in_" + str(i) +" > my_out_" + str(i) + " 2>/dev/null;\\")
    print("\tdiff a_out_" + str(i) + " my_out_" + str(i))
    print()

