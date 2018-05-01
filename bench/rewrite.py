import sys
file = open(sys.argv[1], "r")
file_w = open("../rZ3/"+sys.argv[1],"w")
s = file.read()

beg = s.find("[: [ nil:void,  ")


file.close()
file_w.close()


