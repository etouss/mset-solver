import sys

for i in range(1,len(sys.argv)):
    f = open(sys.argv[i],"r")
    f2 = open("../src2/"+sys.argv[i].replace("bilia","bilia"),"w")
    for l in f.readlines():
        l = l.replace("BaTS","BaTS")
        l = l.replace("bats","bats")
        l = l.replace("bilia","bilia")
        l = l.replace("BILIA","BILIA")
        f2.write(l)
    f.close()
    f2.close()
