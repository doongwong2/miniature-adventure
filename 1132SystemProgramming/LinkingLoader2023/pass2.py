import objreader
import common

def processTRecord(line, csaddr, progaddr, memorycontent): #handles Text Record(T record).
    addr = int("0x%s" % line[1:7], 16)
    addr += csaddr # gets the start address of control section relative to control section, converts to absolute memory address by adding CSADDR.

    memoryArrayIndex = addr - progaddr # converts memory address into an index in the memorycontent array.
    memoryArrayIndex *= 2
    
    len = int("0x%s" % line[7:9], 16) 
    
    for i in range(0, len*2): #extracts len bytes from the T record, loads them into memorycontent array.
        memorycontent[memoryArrayIndex] = line[9+i]
        memoryArrayIndex += 1

def processMRecord(line, csaddr, progaddr, memorycontent, ESTAB): #handles M Record(M record).
    addr = int("0x%s" % line[1:7], 16) #finds absolute address to modify.
    addr += csaddr

    memoryArrayIndex = addr - progaddr
    memoryArrayIndex *= 2
    
    ml = int("0x%s" % line[7:9], 16) #calculates how many half-bytes to read. ml = number of hex digits.
    
    if (ml == 5): #Special handling: If modifying 20 bits (5 hex digits), skip the first nibble.
        memoryArrayIndex += 1
        
    currentMemoryValue = "".join(memorycontent)[memoryArrayIndex:memoryArrayIndex+ml]
    # print(current)
    
    value = common.getDec("0x%s" % currentMemoryValue, ml*4) #converts hex string into decimal.
    token = line[10:len(line)-1] #modifies value using symbol address found in estab.
    if line[9] == '+':
        value += ESTAB[token]
    else:
        value -= ESTAB[token]
    
    result = common.hexToString(common.getHex(value, ml*4), ml) #converts result back into hex string and write into memory.
    
    for i in range(0, ml):
        memorycontent[memoryArrayIndex] = result[i]
        memoryArrayIndex += 1

def execute(argv, ESTAB, memorycontent):
    PROGADDR = int ("%s" % argv[1], 16) #reads starting program address and sets csaddr.
    CSADDR = PROGADDR
        
    for i in range(2, len(argv)): #iterates through each object as in pass1.
        lines = objreader.readfile(argv[i])
        # Header Record
        csname = objreader.getNameWithoutSpace(lines[0][1:7]) #reads the control section name and length from the header record.
        cslength = int("%s" % lines[0][13:19], 16)
        
        for l in range(1, len(lines)): #calls upon T record or M record processes.
            if lines[l][0:1] == 'T':
                processTRecord(lines[l], CSADDR, PROGADDR, memorycontent)
            if lines[l][0:1] == 'M':
                processMRecord(lines[l], CSADDR, PROGADDR, memorycontent, ESTAB)
                
        CSADDR += cslength #moves the control section base address for the next section. 
