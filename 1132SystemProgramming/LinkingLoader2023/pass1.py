import objreader

def execute(argv, ESTAB):
    PROGADDR = int ("%s" % argv[1], 16)
    CSADDR = PROGADDR #the program's starting address. PROGADDR is first control section, CSADDR keeps track of the address where each control section begins.
        
    memoryspacelen = 0 #keeps track of total memory size used by all control sections.
    
    for i in range(2, len(argv)): #read contents
        lines = objreader.readfile(argv[i]) 
        # Header Record
        csname = objreader.getNameWithoutSpace(lines[0][1:7]) #get name of the control section.
        cslength = int("%s" % lines[0][13:19], 16) #length of the control section, in the last 6 digits.
        
        memoryspacelen += cslength #add length to total memory.
        
        ESTAB[csname] = CSADDR #adds control section and its starting address to ESTAB.
        
        for l in range(1, len(lines)):
            if lines[l][0:1] != 'D': #skips to Define Records(D record) only.
                continue
            # D Record
            n = int((len(lines[l])-2)/12) 
            for j in range(0, n): #each D can define multiple symbols, each symbol is 12 characters long(6-char name) + (6-char relative address).
                name = objreader.getNameWithoutSpace(lines[l][1+(12*j):7+(12*j)])
                addr = int("%s" % lines[l][7+(12*j):13+(12*j)], 16)
                ESTAB[name] = CSADDR + addr
                
        CSADDR += cslength #after processing one .obj file, the loader shifts by length of control section for the next section can be placed immediately after this section.
        
    return memoryspacelen #tells the main program how much memory space to allocate.

        
    
    
    
    
    
    
    
