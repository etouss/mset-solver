(set-logic QF_SLRDI)
(declare-fun emptybag () BagInt)(assert (and (.|\n)*?\) )'
 
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
                 if tmp[0] not in already and tmp[0][0] != '?':
                     file_w.write("(declare-fun "+tmp[0]+" () "+tmp[1]+")\n")
                     already.add(tmp[0])
                 #print("(declare fun "+tmp[0]+" () "+tmp[1]+")")
         test = ligne.split("(assert (not 
(and ")
         file_w.write("(assert )
(check-sat)
