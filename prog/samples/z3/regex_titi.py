import sys
import re

if __name__ == '__main__':



    regex_block = r'(?=\[: \[(.*)\]\])'
    regex_item = r'([\S]*?:(?:Int|BagInt))'
    regex_and = r'(\(and (.|\n)*?\) )'

    for i in range(1,len(sys.argv)):

        file = sys.argv[i]
        file_w = open("../z3r/"+file,"w")
        file_w.write("(set-logic QF_SLRDI)\n")

        file = open(file)
        lignes = file.readlines()
        ligne = ""
        for l in lignes:
            ligne += " "+l
        # print(ligne)

        already = set()
        file_w.write("(declare-fun emptybag () BagInt)")

        m = re.findall(regex_block, ligne)
        for el in m:
            items = re.findall(regex_item, el)
            for it in items:
                tmp = it.split(":")
                if tmp[0] not in already and ("?" not in tmp[0]):
                    file_w.write("(declare-fun "+tmp[0]+" () "+tmp[1]+")\n")
                    already.add(tmp[0])
                if tmp[0] not in already and ("?" in tmp[0]):
                    file_w.write("(declare-fun "+tmp[0].replace("??","?")+" () "+tmp[1]+")\n")
                    already.add(tmp[0])
                #print("(declare fun "+tmp[0]+" () "+tmp[1]+")")
        test = ligne.split("(and ")
        file_w.write("(assert (and "+test[1].split("==>")[0])

        file_w.write("(assert (not \n(and "+test[2]+")\n")
        file_w.write("(check-sat)\n")

        file.close()
        file_w.close()
#    m = re.findall(regex_and, ligne)
#    print(m)
#    for el in m:
#        print(el)
