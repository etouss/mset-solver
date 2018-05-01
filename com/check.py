result = ""
bench = open("benchmark.txt", "r")
file = ""

for l in bench.readlines():
    s = l.split(":")
    if s[0] == "file":
        if s[1] != file:
            file = s[1]
            result = ""
    if l == "unsat\n" or l == "sat\n":
        if result == "":
            result = l
        elif result != l:
            print("error:"+file)

bench.close()



