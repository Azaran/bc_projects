

#SYN:xvecer18

import sys
import os
import re
from operator import itemgetter

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

def my_fopen(filename,omode):

    path = os.path.abspath(filename)
#    print("path = "+path)
    try:
        new_file = open(path, mode = omode, encoding='utf-8')
    except IOError:
        if omode == 'rt':
            reterr(2)
        elif omode == 'wt':
            reterr(3)
        elif omode == 'r':
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

def getFormat():
    global formatfile
    format_s = [] 
    format_o = []
    for line in formatfile:
        reg_line = re.compile('^(.*?)([\t]+)(.+)[\s]*$')
        parts = reg_line.match(line)
        if parts == None:
            empty_line = re.compile('^([\s]*)$')
            if empty_line.match(line) == None:
                reterr(4)
        else:
            keyword = parts.group(1)
            fmt_str = parts.group(3)
            if len(keyword) == 0 or len(fmt_str) == 0 or len(parts.group(2)) == 0:
                reterr(4)
            checkFormatFile(keyword)
#            print("group1 = '"+keyword+"' group2 = "+fmt_str)
            keyword = editKeyword(keyword)
#            print ("keywords = "+keyword+", format = "+fmt_str)
            fmt = parseFormat(fmt_str)
            format_s.append([keyword, fmt])
            format_o.append(keyword)
#    print(format_s)
    return format_s, format_o

def editKeyword(keyword):
    key = keyword
    keyword = re.sub(r'\\', '\\\\\\\\', keyword)
    keyword = re.sub(r'\[', '\\[', keyword)
    keyword = re.sub(r'\]', '\\]', keyword)
    keyword = re.sub(r'\{', '\\{', keyword)
    keyword = re.sub(r'\}', '\\}', keyword)
    keyword = re.sub(r'\^', '\\^', keyword)
    keyword = re.sub(r'\$', '\\$', keyword)
    keyword = re.sub(r'\\n', '\\\\n', keyword)
    keyword = re.sub(r'\\t', '\\\\t', keyword)
    keyword = re.sub(r'\?', '\\\\?', keyword)
    keyword = re.sub('%s', '\\s', keyword)
    keyword = re.sub('%a', '[\s\S]', keyword)
    keyword = re.sub('%d', '\\d', keyword)
    keyword = re.sub('%l', '[a-z]', keyword)
    keyword = re.sub('%L', '[A-Z]', keyword)
    keyword = re.sub('%w', '[a-zA-Z]', keyword)
    keyword = re.sub('%W', '[a-zA-Z0-9]', keyword)
    keyword = re.sub('%t', '\\\\t', keyword)
    keyword = re.sub('%n', '\\\\n', keyword)
    keyword = re.sub('%\.', '\\.', keyword)
    keyword = re.sub('%\|', '\\|', keyword)
    keyword = re.sub('%\*', '\\*', keyword)
    keyword = re.sub('%\+', '\\+', keyword)
    keyword = re.sub('%\(', '\\(', keyword)
    keyword = re.sub('%\)', '\\)', keyword)
    keyword = re.sub('%%', '%', keyword)
    
    i = 0
    while i < len(keyword):
        if keyword[i] == '.' and keyword[i-1] != '\\':
            keyword = keyword[:i] + keyword[i+1:]
        elif keyword[i] == '!' and keyword[i-1] == '%':
            keyword = keyword[:i-1] + keyword[i:]
        elif keyword[i] == '!': 
            if keyword[i+1] == '[':
                msg = ''
                j = i+2
                while keyword[j] != ']' and j < len(keyword): 
                    msg += keyword[j]
                    j += 1
                keyword = keyword[:i] + "[^" + msg + "]" + keyword[j+1:]
                i = j
            elif keyword[i+1] == '\\':
                keyword = keyword[:i] + "[^\\" + keyword[i+2] + "]" + keyword[i+3:]
            else:
                keyword = keyword[:i] + "[^" + keyword[i+1] + "]" + keyword[i+2:]


        i += 1 
#    print (key+" -> "+keyword) 
    
    return keyword

def checkFormatFile(keyword):
    i = 0
    special = False
    last = ''
    neg = False
    brac = 0
    klen = len(keyword)
    while i < klen:
        ch = keyword[i]
#        print("\'"+last+"\' -> \'"+ch+"\'")
        if last == '%':
            if ch != 's'\
            and ch != 'a'\
            and ch != 'd'\
            and ch != 'l'\
            and ch != 'L'\
            and ch != 'w'\
            and ch != 'W'\
            and ch != 't'\
            and ch != 'n'\
            and ch != '['\
            and ch != ']'\
            and ch != '{'\
            and ch != '}'\
            and ch != '$'\
            and ch != '^'\
            and ch != '?'\
            and ch != '\\'\
            and ch != '/'\
            and ch != '%'\
            and ch != '.'\
            and ch != '|'\
            and ch != '!'\
            and ch != '+'\
            and ch != '('\
            and ch != ')'\
            and ch != '*':
                reterr(4)
        elif last == '.':
            if i == klen:
                reterr(4)
            elif ch == '.' or ch == '*' or ch == '+' or ch == '|':
                reterr(4)
        elif last == '+':
            if neg:
                neg = False
        elif last == '!':
            if i == klen:
                reterr(4)
            elif (ch == '!' or ch == '.' or ch == '|' or ch == '+' or ch == '*'):
                if prelast != '%':
                    reterr(4)
                elif i+1 >=klen:
                    reterr(4)
            else:
                neg = False
        elif last == ')':
            if prelast != '%' and brac < 1:
                reterr(4)
            elif prelast != '%':
                brac -= 1
            if neg:
                neg = False

        elif last == '(':
            if i+2 >= klen:
                reterr(4)
            if prelast != '%':
                brac += 1 

        elif last == '|':
            if i == klen:
                reterr(4)
            elif ch == '.' or ch == '|' or ch == '!':
                if prelast != '%':
                    reterr(4)
                elif i+1 >=klen:
                    reterr(4)

        elif last == '*':
            if neg:
                neg = False
        
        elif last == '':
            if i == klen:
                reterr(4)
            elif ch == '(' and i+2 >= klen:   # (x) ma velikost 3 cokoliv mensiho je chyba
                reterr(4)
            elif ch == '|' or ch == '*' or ch == '+' or ch == '.' or ch == ')':
                reterr(4)
            elif ch == '!':
                neg = True

        else:
#            print('anychar')
            if ch == '!':
                if i+1 >= klen:
                    reterr(4)
                else:
                    neg = True
            elif ch == '(' and i+2 >= klen: 
                reterr(4)
            elif (ch == '|' or ch == '.')  and i+1 >= klen: 
                reterr(4)
            elif ch == ')' and prelast != '%':
                brac -= 1

        prelast = last
        last = ch
#        print(i,klen,brac)
        i += 1
    if brac != 0:
        reterr(4)
    if neg:
        reterr(4)
#    print('prezil jsem checkFormat')

def parseFormat(fmt_str):
    tags = \
    re.compile('(bold){1}|(italic){1}|(underline){1}|(teletype){1}|(size)?:([1-7]{1}){1}|(color)?:([0-9A-Fa-f]{6}){1}')
    matches = re.finditer(tags, fmt_str)
    return makeForml(matches)


def makeForml(matches):
    forml = [[False,False,False,False,0,""],[ 0, 0, 0, 0, 0, 0]]
    pos = 1                     # position in format string
    size = color = False
#    print (matches)
    ididsmth = False
    for match in matches:
        ididsmth = True
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
                    if m != '':
                        forml[0][4] = int(m)
                        forml[1][4] = pos
                        pos += 1
                        size = False
                    else:
                        reterr(4)
                elif color:
                    if m != '':
                        forml[0][5] = m
                        forml[1][5] = pos
                        pos += 1
                        color = False
                    else:
                        reterr(4)
#    print("list =",forml[0])    
#    print("pos =",forml[1])    
    if not ididsmth:
        reterr(4)
    return forml

def applyFormat(format_s, format_o):
    global infile
#    print(format_s)
    msg = ""
#    for line in infile:
    orig_msg = infile.read()
    added_l = []
    tags_l = []   # [[start_of_tag,end_of_tag]]
    rn = 0
    scope = 0
    prefixes =[]
    surfixes = []
    new_msg = ""
    for r in range(0,len(format_o)):
        rn += 1
        if len(format_o) == 0:
            continue
        
        scope += 1

        lookfor = re.compile(format_o[r])
        kworditer = lookfor.finditer(orig_msg)
        for kword in kworditer:
#            print(kword,format_o[r])
            start = kword.start()
            end = kword.end()
            if kword.group() != '':
                prefix, surfix = addTags(orig_msg, kword.group(), \
                        start, end, format_s[r][1])
                prefixes.append([kword.group(),scope,kword.start(),prefix])
                surfixes.append([kword.group(),scope,kword.end()-1,surfix])
    prefixes.sort(key=itemgetter(1))                
    prefixes.sort(key=itemgetter(2))                
    surfixes.sort(key=itemgetter(1), reverse=True)                
    surfixes.sort(key=itemgetter(2))  
    
    for i in range(0,len(orig_msg)):
        p = 0
        s = 0
        while p < len(prefixes):
            if prefixes[p][2] == i:
                new_msg += prefixes[p][3]
            p += 1
        new_msg += orig_msg[i]
        while s < len(prefixes):
            if surfixes[s][2] == i:
                new_msg += surfixes[s][3]
            s += 1

#    print(new_msg) 
                
    return new_msg 

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
#    line =  line[:start] + prefix + group + surfix + line[end:]
#    print ("line"+str(i)+" = "+line)
#    shift = len(prefix) + len(surfix)
    
    return prefix, surfix

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
