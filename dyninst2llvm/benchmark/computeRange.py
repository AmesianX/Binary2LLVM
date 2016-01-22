def compareListRange(name, singleRange, rangeList):
    # do the comparison to find out if they overlap
    # also print out the number of comparisons made
    start = False
    singleRangeMin = singleRange[0]
    singleRangeMax = singleRange[1]
    numComparison = 0
    for curName, curRange in rangeList: 
        if curName == name:
            start = True
            continue
        if not start:
            continue
        curRangeMin = curRange[0]
        curRangeMax = curRange[1]
        violated = False
        numComparison = numComparison+2
        if not (curRangeMin > singleRangeMax or curRangeMax < singleRangeMin):
            print name, ' and ', curName, ' overlaps'
    return numComparison    
            
        
def compareRanges(rangeName2RangeDic):
    rangeName2Range = rangeName2RangeDic.items()
    totalNumComparison = 0
    for name, curRange in rangeName2Range:
        totalNumComparison+=compareListRange(name, curRange, rangeName2Range)
    print totalNumComparison, " comparison is performed "



def convert2dec(numraw):
    num = str(numraw)
    if num.startswith('0x') or num.startswith('-0x'):
        return int(num,16)
    else:
        return int(num,10)


# return (min, max)
# coList is the list of coefficient, maybe in hex or dec
# varminList is the list of lower bound
def computeRange(coList, varminList, varmaxList, offset):
    assert(len(coList) == len(varminList) == len(varmaxList))
    #make everything dec int
    coListInt = map(convert2dec, coList)
    varminListInt = map(convert2dec, varminList)
    varmaxListInt = map(convert2dec, varmaxList)
    offsetInt = convert2dec(offset)
    minlist = map(lambda x,y:x*y, coListInt,varminListInt)
    globalmin = reduce(lambda x,y:x+y, minlist)+offsetInt
    maxlist = map(lambda x,y:x*y, coListInt,varmaxListInt)   
    globalmax = reduce(lambda x,y:x+y, maxlist)+offsetInt
    return (globalmin, globalmax)


def main():
    # we want to compute a range by specifying a set of coefficients and the range 
    # each variable can take ( one variable is associate with one coefficient)
    # e.g. polynomial coefficient 1, 0x758, 0x888888
    parseFile = open('range.txt','r')
    # the format is like name: colist1,colist2,...: minlist1, minlist2,...: maxlist1, maxlist2...: offset
    
    
    colist = (1,2,0x10)
    minlist = ('-0xf','-0x1',1)    
    maxlist = ('0','0x3',1)
    s = computeRange(colist,minlist,maxlist,8)
    print s
    allNamedRange={}
    allNamedRange['first']= s
    colist2 = (1,2,0x10)
    minlist2 = ('-0xf','-0x1',1)    
    maxlist2 = ('0','0x3',1)
    s2 = computeRange(colist2,minlist2,maxlist2,1008)
    allNamedRange['second']= s2
    print s2
    compareRanges(allNamedRange)
    
    

if __name__ == "__main__":
    main()
