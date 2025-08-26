def readfile(srcfile): #reads .obj file, returns them as a list of strings, one per line.
    try:
        with open(srcfile, "r") as fp:
            return fp.readlines()
    except:
            return None
    
    
def getNameWithoutSpace(originalname): #splits string and returns the first part.
    list = originalname.split()
    return list[0]

    

