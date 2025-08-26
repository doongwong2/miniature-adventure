def hexToString(hexstr, length): #formats a hex number string(0x1A) into padded uppercase string of fixed length. Used for printing addresses and values in consistent hex format.
    hexstr = hexstr.upper()
    hexstr = hexstr[2:]
    n = length - len(hexstr)
    for i in range(0, n):
        hexstr = '0' + hexstr
    return hexstr
    
def getDec(hexstr, bits): # converts hex number string(FFF6) into a signed decimal number(-10). Used for handling modification records like signed offsets or displacement values.
    intval = int(hexstr, 16)
    if intval >= int(pow(2, bits-1)):
        intval -= int(pow(2, bits))
    return intval
    
def getHex(val, bits): # converts decimal number (-10) into a two's complement hex number string(FFF6). Used when modifying and writing back into memory.
    return hex((val + (1 << bits)) % (1 << bits))

    
def testMemoryContent(memorycontent): #For debugging purposes, to check if testMemoryContent is mutable. Lists, dicts and sets are mutable.
    memorycontent[5] = 'H'
