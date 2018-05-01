(set-logic QF_SLRDI)
(declare-fun emptybag () BagInt)(assert (and (.|\n)*?\) )'
 
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
     test = ligne.split("(assert (not 
(and ")
     file_w.write("(assert )
(check-sat)
