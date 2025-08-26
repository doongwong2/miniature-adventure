import sys

import pass1
import pass2
import common

ESTAB = {} #external symbol table to be inserted in pass 1, used in pass 2.

memoryspace = pass1.execute(sys.argv, ESTAB) # run pass 1.

for key in ESTAB:
    print("%s:%s" % (key, hex(ESTAB[key]))) # debug print of external symbols and their resolved addresses.

memorycontent = [] #prepare memory buffer.
for i in range(0, (2*memoryspace)): 
    memorycontent.append(".") # each byte is represented by 2 letters, hence size is 2* memoryspace.
    
pass2.execute(sys.argv, ESTAB, memorycontent) #run pass 2.

memorycontentstr = "".join(memorycontent)

PROGADDR = int ("%s" % sys.argv[1], 16) #first argument is to be assumed as program load address, used for calculate absolute address for printing.

for i in range(0, (2*memoryspace)):
    if (i%32 == 0):
        print("\n%s " % common.hexToString(hex(PROGADDR + int((i/2))), 4), end ="")
    print("%s" % memorycontent[i], end =" ")
print() #output the address labels.
    
        

