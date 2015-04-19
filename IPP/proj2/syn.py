

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
        elif mode == 'r':
            new_file = False
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
            if val != "":
 #               print (val)
                formatfile = my_fopen(val,'r')
                if formatfile != None:
                    args[arg] = True
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
#        checkFormatFile()
        format_s, format_o = getFormat()
        msg = applyFormat(format_s, format_o)
     
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
"""
def checkFormatFile():
    global formatfile
    text = formatfile.read()
    state = 'start'
    par = 0
    for i in range(0,len(text)):
        ch = text[i]
        if ord(ch) < 32:
            reterr(4)
        if state == 'start': 
            if ch == '.':
                state = '.'
            elif ch == '|':
                state = 'err'
            elif ch == '!':
                state = '!'
            elif ch == '*':
                state = 'err'
            elif ch == '+':
                state = 'err'
            elif ch == '(':
                state = '('
            elif ch == ')':
                state = 'err'
            elif ch == '%':
                state = '%'
            else:
                state = 'string'
        elif state == '.':
            if ch == '.':
                state = 'err'
            elif ch == '|':
                state = '|'
            elif ch == '!':
                state = '!'
            elif ch == '*':
                state = '*'
            elif ch == '+':
                state = '+'
            elif ch == '(':
                state = '('
            elif ch == ')' and par > 0:
                state = ')'
            elif ch == ')' and par == 0:
                state = 'err'
            elif ch == '%':
                state = '%'
            else:
                state = 'string'
        elif state == '|':
            if ch == '.':
                state = '.'
            elif ch == '|':
                state = 'err'
            elif ch == '!':
                state = '!'
            elif ch == '*':
                state = 'err'
            elif ch == '+':
                state = 'err'
            elif ch == '(':
                state = '('
            elif ch == ')':
                state = 'err'
            elif ch == '%':
                state = '%'
            else:
                state = 'string'
        elif state == '!':
            if ch == '.':
                state = '.'
            elif ch == '|':
                state = '|'
            elif ch == '!':
                state = '!'
            elif ch == '*':
                state = '*'
            elif ch == '+':
                state = '+'
            elif ch == '(':
                state = '('
            elif ch == ')':
                state = ')'
            elif ch == '%':
                state = '%'
            else:
                state = 'string'

        elif state == '*':
            if ch == '.':
                state = '.'
            elif ch == '|':
                state = '|'
            elif ch == '!':
                state = '!'
            elif ch == '*':
                state = '*'
            elif ch == '+':
                state = '+'
            elif ch == '(':
                state = '('
            elif ch == ')':
                state = ')'
            elif ch == '%':
                state = '%'
            else:
                state = 'string'

        elif state == '+':
            if ch == '.':
                state = '.'
            elif ch == '|':
                state = '|'
            elif ch == '!':
                state = '!'
            elif ch == '*':
                state = '*'
            elif ch == '+':
                state = '+'
            elif ch == '(':
                state = '('
            elif ch == ')':
                state = ')'
            elif ch == '%':
                state = '%'
            else:
                state = 'string'

        elif state == '(':
            if ch == '.':
                state = '.'
            elif ch == '|':
                state = '|'
            elif ch == '!':
                state = '!'
            elif ch == '*':
                state = '*'
            elif ch == '+':
                state = '+'
            elif ch == '(':
                state = '('
            elif ch == ')':
                state = ')'
            elif ch == '%':
                state = '%'
            else:
                state = 'string'

        elif state == ')':
            if ch == '.':
                state = '.'
            elif ch == '|':
                state = '|'
            elif ch == '!':
                state = '!'
            elif ch == '*':
                state = '*'
            elif ch == '+':
                state = '+'
            elif ch == '(':
                state = '('
            elif ch == ')':
                state = ')'
            elif ch == '%':
                state = '%'
            else:
                state = 'string'
               
        elif state == 'string':
            if ch == '.':
                state = '.'
            elif ch == '|':
                state = '|'
            elif ch == '!':
                state = '!'
            elif ch == '*':
                state = '*'
            elif ch == '+':
                state = '+'
            elif ch == '(':
                state = '('
            elif ch == ')':
                state = ')'
            elif ch == '%':
                state = '%'
            else:
                state = 'string'
        elif state == '%':
            if ch == 's':
                state = 'start'
            elif ch == 'a':
                state = 'start'
            elif ch == 'd':
                state = 'start'
            elif ch == 'l':
                state = 'start'
            elif ch == 'L':
                state = 'start'
            elif ch == 'w':
                state = 'start'
            elif ch == 'W':
                state = 'start'
            elif ch == 't':
                state = 'start'
            elif ch == 'n':
                state = 'start'
            elif ch == '.':
                state = 'start'
            elif ch == '|':
                state = 'start'
            elif ch == '!':
                state = 'start'
            elif ch == '*':
                state = 'start'
            elif ch == '+':
                state = 'start'
            elif ch == '(':
                state = 'start'
            elif ch == ')':
                state = 'start'
            elif ch == '%':
                state = 'start'
            else:
                reterr(4)
       """ 
def getFormat():
    global formatfile
    format_s = {}
    format_o = []
    i = 0
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
        if len(fmt_str) == 0:
            reterr(4)
#        print ("keywords = "+keywords_str+", format = "+fmt_str)
        fmt = parseFormat(fmt_str)
        format_s[keywords_r] = fmt
        format_o.append(keywords_r)
#        print(format_s)
        i += 1
    return format_s, format_o

def editKeyword(keyword):
    key = keyword
    keyword = re.sub('%s', '[\\t\\n\\r\\f\\v]', keyword)
    keyword = re.sub('%a', '.', keyword)
    keyword = re.sub('%d', '[0-9]', keyword)
    keyword = re.sub('%l', '[a-z]', keyword)
    keyword = re.sub('%L', '[A-Z]', keyword)
    keyword = re.sub('%w', '[a-zA-Z]', keyword)
    keyword = re.sub('%W', '[a-zA-Z0-9]', keyword)
    keyword = re.sub('%t', '\\t', keyword)
    keyword = re.sub('%n', '\\n', keyword)
    keyword = re.sub('%\.', '\\.', keyword)
    keyword = re.sub('%\|', '\\|', keyword)
    keyword = re.sub('%\!', '\\!', keyword)
    keyword = re.sub('%\*', '\\*', keyword)
    keyword = re.sub('%\+', '\\+', keyword)
    keyword = re.sub('%\(', '\\(', keyword)
    keyword = re.sub('%\)', '\\)', keyword)
    keyword = re.sub('%%', '\\%', keyword)
    keyword = re.sub('!(.)', '[^\\1]', keyword)
    keyword = re.sub('(.+)(\.){1}(.+)', '\\1\\3', keyword)
#    keyword = re.sub('%(\.|\||\!|\*|\+|\(|\)|%)', '\134\000\\1', keyword)
#    print (key+" -> "+keyword) 
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
#            else:
#                reterr(4)
#    print("list =",forml[0])    
#    print("pos =",forml[1])    
    return forml

def applyFormat(format_s, format_o):
    global infile
#    print(format_s)
    msg = ""
    for line in infile:
        added_l = []
        tags_l = []   # [[start_of_tag,end_of_tag]]
        rn = 0
        for regex in format_o:
#            print("regex = ",regex)
            rn += 1
            if len(format_o) == 0:
                continue
            shift = 0
            lookfor = re.compile(regex)
            kworditer = lookfor.finditer(line)
            for kword in kworditer:
                start = kword.start()+shift
                end = kword.end()+shift
                if kword.group() != '':
                    err = False
#                    print(kword)
                    for i in range(0, len(tags_l)):
                        if ((start > tags_l[i][0] and start < tags_l[i][1]) or \
                                (end > tags_l[i][0] and end < tags_l[i][1])):
                            err = True
                            break
                    for i in range(0, len(added_l)):
                        if added_l[i][0] == start and added_l[i][1] == end and \
                                added_l[i][2] == rn:
                            err = True
                            break
                    if not err:
                        len_prefix, len_surfix, line = addTags(line, kword.group(), \
                                start, end, format_s[regex])
                        end -= 1 
                        kw_len = end - start
                        tags_l.append([start,start+len_prefix])
                        tags_l.append([start+len_prefix+kw_len+1,start+len_prefix+kw_len+len_surfix])
                        added_l.append([start+len_prefix,start+len_prefix+kw_len,rn])
                        shift += len_prefix+len_surfix
#                    print("tags = ",tags_l,"\nadded = ",added_l) 

#<b><i><font color=#FFFFFF><font size=2><u><tt>if</tt></u></font></font></i></b>then
                        
                        
                        
                        
                        
                        
                        
                        
                        
                        
                        
                        
                        
                        
        msg += line
#    msg += "\n"
    return msg

def addTags(line, group, start, end, format_t):
    morder = max(format_t[1])
    prefix = ""
    surfix = ""
    for i in range(1,morder+1):
        for j in range(0,len(format_t[0])):
            if format_t[1][j] == i:
                if j == 0 and format_t[0][j]:
                    prefix += "<b>" 
                    surfix = "</b>" + surfix
                elif j == 1 and format_t[0][j]:
                    prefix += "<i>" 
                    surfix = "</i>" + surfix
                elif j == 2 and format_t[0][j]:
                    prefix += "<u>" 
                    surfix = "</u>" + surfix
                elif j == 3 and format_t[0][j]:
                    prefix += "<tt>" 
                    surfix = "</tt>" + surfix
                elif j == 4 and format_t[0][j] > 0:
                    prefix += "<font size=" + str(format_t[0][j]) + ">" 
                    surfix = "</font>" + surfix
                elif j == 5 and format_t[0][j] != "":
                    prefix += "<font color=#" + str(format_t[0][j]) + ">"
                    surfix = "</font>" + surfix
    line =  line[:start] + prefix + group + surfix + line[end:]
#    print ("line"+str(i)+" = "+line)
#    shift = len(prefix) + len(surfix)
    
    return len(prefix), len(surfix), line



"""
def applyFormat(format_s, format_o):
    global infile
#    print(format_s)
    msg = ""
    for line in infile:
        shift = -1
        kword_l = []
        for regex in format_o:
#            print("regex = ",regex)
            if len(format_o) > 0:
                lookfor = re.compile(regex)
                kworditer = lookfor.finditer(line)
                for kword in kworditer:
                    if kword.start() < kword.end():
                        kword_l.append([regex,kword.group(),kword.start(),kword.end(),0])    
#        print (kword_l)       
        for kword in kword_l:        
#            print(kword, str(shift))
            if shift == -1:
                shift = 0
            line, shift = addTags(line, kword[1], kword[2], kword[3], format_s[kword[0]])
            for i in range(0,len(kword_l)):
                if kword_l[i][2] > kword[2]:
                    kword_l[i][2] += shift
                    kword_l[i][3] += shift
        msg += line
#    msg += "\n"
    return msg

def addTags(line, group, start, end, format_t):
    morder = max(format_t[1])
    prefix = ""
    surfix = ""
    for i in range(1,morder+1):
        for j in range(0,len(format_t[0])):
            if format_t[1][j] == i:
                if j == 0 and format_t[0][j]:
                    prefix += "<b>" 
                    surfix = "</b>" + surfix
                elif j == 1 and format_t[0][j]:
                    prefix += "<i>" 
                    surfix = "</i>" + surfix
                elif j == 2 and format_t[0][j]:
                    prefix += "<u>" 
                    surfix = "</u>" + surfix
                elif j == 3 and format_t[0][j]:
                    prefix += "<tt>" 
                    surfix = "</tt>" + surfix
                elif j == 4 and format_t[0][j] > 0:
                    prefix += "<font size=" + str(format_t[0][j]) + ">" 
                    surfix = "</font>" + surfix
                elif j == 5 and format_t[0][j] != "":
                    prefix += "<font color=#" + str(format_t[0][j]) + ">"
                    surfix = "</font>" + surfix
    line =  line[:start] + prefix + group + surfix + line[end:]
#    print ("line"+str(i)+" = "+line)
    shift = len(prefix) + len(surfix)
    return line, shift
"""
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

