import sys
import re

if __name__ == '__main__':

    regex_block = r'(?=\[: \[(.*)\]\])'
    regex_item = r'([\S]*?:(?:Int|BagInt))'
    regex_and = r'(\(and (.|\n)*?\) )'

    file = sys.argv[1]
    file_w = open("../z3r/"+file,"w")
    file_w.write("(set-logic QF_SLRDI)\n")

    file = open(file)
    lignes = file.readlines()
    ligne = ""
    for l in lignes:
        ligne += " "+l
    # print(ligne)


    m = re.findall(regex_block, ligne.split("(and")[0])
    for el in m:
        items = re.findall(regex_item, el)
        for it in items:
            tmp = it.split(":")
            file_w.write("(declare-fun "+tmp[0]+" () "+tmp[1]+")\n")
            #print("(declare fun "+tmp[0]+" () "+tmp[1]+")")
    test = ligne.split("(and ")
    file_w.write("(assert (and "+test[1].split("==>")[0])
    
    m = re.findall(regex_block, ligne.split("(and")[1])
    for el in m:
        items = re.findall(regex_item, el)
        for it in items:
            tmp = it.split(":")
            file_w.write("(declare-fun "+tmp[0]+" () "+tmp[1]+")\n")


    file_w.write("(assert (and "+test[2])
    file_w.write("(check-sat)\n")

    file.close()
    file_w.close()
#    m = re.findall(regex_and, ligne)
#    print(m)
#    for el in m:
#        print(el)

