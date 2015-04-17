

#SYN:xvecer18

import sys
import os
import re

def reterr(err):
    errnum = {
            1 : "spatny format nebo kombinace parametru",
            2 : "neni zadany nebo se nepodarilo otevrit vstupni soubor",
            3 : "chyba pri pokusu o otevreni vystupniho souboru (spatna prava)",
            4 : "chybny format vstupniho souboru"
            }
    print(errnum[err],file=sys.stderr)
    sys.exit(err)

def printHelp():
    help_msg = " prog= " +sys.argv[0] +"\n\
 ------------------------------------------------------------------------------\n\
| --help                 - Zobrazeni napovedy k programu \n\
| --input=filename.ext   - Vstupni soubor s UTF-8 kodovanim \n\
| --output=filename.ext  - Vystupni soubor s UTF-8 kodovanim\n\
| --output=filename.ext  - Soubor s formatovacimi pravidly\n\
| --br                   - Prida element <br /> na konec radku puvodniho textu\n\
 ------------------------------------------------------------------------------"
    print (help_msg)

def my_fopen(filename,mode):
    path = os.path.abspath(filename)
#    print("path = "+path)
    try:
        new_file = open(path,mode)
    except IOError:
        if mode == 'rt':
            reterr(2)
        elif mode == 'wt':
            reterr(3)
        else:
            raise

    if new_file != False:
#        print("soubor uspesne otevren")
        return new_file
    else:
#        print("soubor se nepovedlo otevrit")
        return None

def copyFile(in_f,out_f):
    r = in_f.read()
    out_f.write(str(r))

def parseArg(argc,argv):
   # print ("argc = "+ str(argc))
    i = 1
    global infile,outfile,formatfile
#    print (argv[i])
    if argc <= 1:
        return 0
    if argv[1] == "--help":
        if argc == 2:
#            printHelp();
            sys.exit(0)
        else:
            reterr(1)
    while i < argc:
#        print (str(i))
        found = argv[i].find('=')
        if found > -1:
            arg = argv[i][:found]
            val = argv[i][found+1:]
        else:
            arg = argv[i]
            val = None
        if arg == "--input":
            if args[arg]:
                reterr(1)
            args[arg] = True
            if val != "":
#                print (val)
                infile = my_fopen(val,'rt')
            else:
                reterr(1)
        elif arg == "--output":
            if args[arg]:
                reterr(1)
            args[arg] = True
            if val != "":
  #              print (val)
                outfile = my_fopen(val,'wt')
            else:
                reterr(1)
        elif arg == "--format":
            if args[arg]:
                reterr(1)
            args[arg] = True
            if val != "":
 #               print (val)
                formatfile = my_fopen(val,'rt')
            else:
                reterr(1)
        elif arg == "--br":
            if args[arg]:
                reterr(1)
            args[arg] = True
        else:
            reterr(1)
        i+=1
    return argc-1

def processFile():
    global infile,outfile,args
    msg = None
    
    # modifikuj sobour podle formatu ve formatfile
    if args['--format']:
        msg = applyFormat(getFormat())
     
     # nahrad konce radku <br />   
    if args['--br']:
        nl = re.compile("\n")
        if msg == None:
            msg = ""
            fmsg = infile.read()
            for line in fmsg:
                line = nl.sub("<br />\n",line)
                msg = line
        else:
            msg = nl.sub("<br />\n",msg)

    outfile.write(msg)

def getFormat():
    global formatfile
    format_s = {}
    for line in formatfile:
        found = line.find("\t")
        keyword = line[:found]
        keywords_r = editKeyword(keyword)
        while line[found] == "\t":    # kdyby bylo vis tabulatoru za sebou
            found +=1
        nl = line.find("\n")
        if nl == -1:            #pokud nema zalomeni uloz az do konce radku, -1 odrizne posl znak
            nl = None
        fmt_str = line[found:nl]
#        print ("keywords = "+keywords_str+", format = "+fmt_str)
        fmt = parseFormat(fmt_str)
        format_s[keywords_r] = fmt
    return format_s

def editKeyword(keyword):
    keyword = re.sub('\%s', '[\\t\\n\\r\\f\\v]', keyword)
    keyword = re.sub('\%a', '.', keyword)
    keyword = re.sub('\%d', '[0-9]', keyword)
    keyword = re.sub('\%l', '[a-z]', keyword)
    keyword = re.sub('\%L', '[A-Z]', keyword)
    keyword = re.sub('\%w', '[a-zA-Z]', keyword)
    keyword = re.sub('\%W', '[a-zA-Z0-9]', keyword)
    keyword = re.sub('\%t', '\\t', keyword)
    keyword = re.sub('\%n', '\\n', keyword)
    keyword = re.sub('%(\.|\||\!|\*|\+|\(|\)|%)', '\\1', keyword)
    
    return keyword

def parseFormat(fmt_str):
    tags = \
    re.compile('(bold){1}|(italic){1}|(underline){1}|(teletype){1}|(size)?:([1-7]{1}){1}|(color)?:([0-9A-F]{6}){1}')
    matches = re.finditer(tags, fmt_str)
   # print (matches)
    return makeForml(matches)


def makeForml(matches):
    forml = [[False,False,False,False,0,""],[ 0, 0, 0, 0, 0, 0]]
    pos = 1                     # position in format string
    size = color = False
    for match in matches:
#        print ("match = ",match)
        i = 0
        for m in match.groups():
#            if m != None:                   # testing purpose
#                print (i," ",m ," starts at ",match.start())
            i+=1
            if m == "bold":
                forml[0][0] = True
                forml[1][0] = pos
                pos += 1
            elif m == "italic":
                forml[0][1] = True
                forml[1][1] = pos
                pos += 1
            elif m == "underline":
                forml[0][2] = True
                forml[1][2] = pos
                pos += 1
            elif m == "teletype":
                forml[0][3] = True
                forml[1][3] = pos
                pos += 1
            elif m == "size":
                size = True            
            elif m == "color":
                color = True            
            elif m != None:
                if size:
                    forml[0][4] = int(m)
                    forml[1][4] = pos
                    pos += 1
                    size = False
                elif color:
                    forml[0][5] = m
                    forml[1][5] = pos
                    pos += 1
                    color = False
#    print("list =",forml[0])    
#    print("pos =",forml[1])    
    return forml

def applyFormat(format_s):
    global infile
#    print(format_s)
    msg = ""
    for line in infile:
        for regex in format_s:
#            print("regex = ",regex)
            lookfor = re.compile(regex)
            for kword in lookfor.finditer(line):
                line = addTags(line, kword, format_s[regex])
        msg += line

    return msg

def addTags(line, kword, format_t):
    morder = max(format_t[1])
    start = kword.start()
    end = kword.end()
    for i in range(1,morder+1):
        for j in range(0,len(format_t[0])):
            if format_t[1][j] == i:
                if j == 0 and format_t[0][j]:
                    line = line[:start]+"<b>"+kword.group()+"</b>"+line[end:]
                    l = len("<b>")
                    start += l
                    end += l
                elif j == 1 and format_t[0][j]:
                    line = line[:start]+"<i>"+kword.group()+"</i>"+line[end:]
                    l = len("<i>")
                    start += l
                    end += l
                elif j == 2 and format_t[0][j]:
                    line = line[:start]+"<u>"+kword.group()+"</u>"+line[end:]
                    l = len("<u>")
                    start += l
                    end += l
                elif j == 3 and format_t[0][j]:
                    line = line[:start]+"<tt>"+kword.group()+"</tt>"+\
                            line[end:]
                    l = len("<tt>")
                    start += l
                    end += l
                elif j == 4 and format_t[0][j] > 0:
                    line = line[:start]+"<size="+str(format_t[0][j])+">"+kword.group()+\
                            "</size>"+line[end:]
                    l = len("<size=1>")         # size je 1 - 7
                    start += l
                    end += l
                elif j == 5 and format_t[0][j] != "":
                    line = line[:start]+"<color=#"+format_t[0][j]+">"+kword.group()+\
                            "</color>"+line[end:]
                    l = len("<color=#FFFFFF>")
                    start += l
                    end += l
#                print (line)
    return line

infile = None
outfile = None
formatfile = None
args = { 
    '--input': False,
    '--output': False,
    '--format': False,
    '--br': False
    }

chkarg = parseArg(len(sys.argv),sys.argv)

if not args['--input']:
    infile = sys.stdin
if not args['--output']:
    outfile = sys.stdout

if not args['--format'] and not args['--br']:
    copyFile(infile,outfile)  # zkopiruj vstup na vystup pokud 0 argumentu
else:
    processFile()               # proved operace nad vstupem
    if infile!= sys.stdin:      # pokud jsme necetli ze stdin zavri soubor
        infile.close()
    if outfile != sys.stdin:    # pokud jsme necetli ze stdout zavri soubor
        outfile.close()
    if formatfile != None:      # pokud byl zadan soubor s formatem zavri soubor
        formatfile.close()

