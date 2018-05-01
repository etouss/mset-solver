result = ""
bench = open("benchmark.txt", "r")
file = ""
i = 0

for l in bench.readlines():
    s = l.split(":")
    if s[0] == "file":
        if s[1] != file:
            file = s[1]
            result = ""
    if result == "" and l == "unsat\n":
        result = l
        print(file+":"+result)
        i += 1
print(i)
bench.close()




