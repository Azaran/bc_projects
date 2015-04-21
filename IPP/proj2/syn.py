

#SYN:xvecer18

import sys
import os
import re
from operator import itemgetter

# vstup navratovy kod, vypis chyby na sterr na zaklade kodu a ukonreni programu s kodem
def reterr(err):
    errnum = {
            1 : "spatny format nebo kombinace parametru",
            2 : "neni zadany nebo se nepodarilo otevrit vstupni soubor",
            3 : "chyba pri pokusu o otevreni vystupniho souboru (spatna prava)",
            4 : "chybny format vstupniho souboru"
            }
    print(errnum[err],file=sys.stderr)
    sys.exit(err)

# vypisuje --help informace
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

# vstup nazev soubor, mode otevreni, vraci identifikator souboru pokud byl otevren
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

# kopirovani vstupu na vystup (pokud nebyl zadan validni soubor formatu)
def copyFile(in_f,out_f):
    r = in_f.read()
    out_f.write(str(r))

# vstup pocet aregumentu a jejich hodnoty, rozrad vstupni argumenty
def parseArg(argc,argv):
    i = 1
    global infile,outfile,formatfile
    if argc <= 1:           # pokud zadny argument
        return 0
    if argv[1] == "--help":     # pokud je zadan pouze --help
        if argc == 2:
#            printHelp();
            sys.exit(0)
        else:
            reterr(1)

    while i < argc:             # pokud nebyl zadan --help zpracuj parametry
        found = argv[i].find('=')       # najdi '=' at muzeme oddeli parametr a hodnotu
        if found > -1:                  # pokud jsi nasel
            arg = argv[i][:found]
            val = argv[i][found+1:]     
        else:                           # jinak neobsahuje hodnotu
            arg = argv[i]
            val = None
        if arg == "--input":            
            if args[arg]:                       # pokud uz by '--input' zadan
                reterr(1)                       # konci s return 1
            args[arg] = True                    # jinak uloz ze byl '--input' zadan
            if val != "":                       # pokud byl zadan souboru
                infile = my_fopen(val,'rt')     # pokus o otevreni
            else:                               # pokud nebyl zadan konci s chybou
                reterr(1)
        elif arg == "--output":                 # stejne jak '--input'
            if args[arg]:                   
                reterr(1)
            args[arg] = True
            if val != "":
                outfile = my_fopen(val,'wt')
            else:
                reterr(1)
        elif arg == "--format":                 # stejen jak '--input'
            if args[arg]:
                reterr(1)
            if val != "":
                formatfile = my_fopen(val,'r')  
                if formatfile != None:          # zde je zmena, neuspesne otereni se != chyba
                    args[arg] = True            # pouze pokud byl oteren soubor
            else:
                reterr(1)
        elif arg == "--br":                     # byl zadan '--br'
            if args[arg]:
                reterr(1)       
            args[arg] = True                    
        else:
            reterr(1)
        i+=1
    return argc-1                               

# zpracovani vstupniho souboru
def processFile():
    global infile,outfile,args
    msg = None
    
    # modifikuj sobour podle formatu ve formatfile
    if args['--format']:
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

    outfile.write(msg)                          # zapis na vystup

# ziskani formatu ze souboru s formatem
def getFormat():
    global formatfile
    format_s = [] 
    format_o = []
    for line in formatfile:                     # ber radek po radku
        reg_line = re.compile('^(.*?)([\t]+)(.+)[\s]*$')    # hledej regex, oddeleni, format 
        parts = reg_line.match(line)
        if parts == None:                           # pokud jsi na radku nic nenasel
            empty_line = re.compile('^([\s]*)$')    # kontrola zda je radek prazdny
            if empty_line.match(line) == None:      # pokud ne konci s chybou 4
                reterr(4)
        else:
            keyword = parts.group(1)                # jinak uloz regex
            fmt_str = parts.group(3)                #       uloz format

            # kdyby nalezeny regex nebo format nebo oddelovac byly prazdne volej chybu 4
            if len(keyword) == 0 or len(fmt_str) == 0 or len(parts.group(2)) == 0:
                reterr(4)
            checkFormatFile(keyword)                # zkontroluj zda regex odpovida abcecede
            keyword = editKeyword(keyword)          # preved regex do Python abecedy
            fmt = parseFormat(fmt_str)              # rozeber format
            format_s.append([keyword, fmt])         # uloz regex a format
            format_o.append(keyword)                # uloz poradi regexu
    
    return format_s, format_o

# vstup klicove slovo (regex), prevadi regex ze vstupni abecedy do abecedy Pythonu 3
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
    while i < len(keyword):                     # slozitejsi prevody pomoci prochaze po znaku
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

# vstup regex ve vstupni abecede, overeni zda se jedna o vyraz vstupni abecedy
# a za odpovida zadanim definovane gramatice
def checkFormatFile(keyword):
    i = 0
    special = False
    last = ''
    neg = False
    brac = 0
    klen = len(keyword)
    while i < klen:         # konecny automat
        ch = keyword[i]
#        print("\'"+last+"\' -> \'"+ch+"\'")
        if ord(ch) < 32:
            reterr(4)
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
    if brac != 0:           # pokud bylo vice otevrenych nez zavrenych zavorek
        reterr(4)
    if neg:                 # pokud nebyla ukoncena negace vyrazu
        reterr(4)

# vstup definice formatu, rozebrani formatu a zkontrolovani zda odpovida zadani
def parseFormat(fmt_str):
    tags = \
    re.compile('(bold){1}|(italic){1}|(underline){1}|(teletype){1}|(size)?:([1-7]{1}){1}|(color)?:([0-9A-Fa-f]{6}){1}')
    matches = re.finditer(tags, fmt_str)
    return makeForml(matches)

# vstup nalezena klicova slova formatu, prevod retezce na strukturu ulozenych a dat a poradi
def makeForml(matches):
    forml = [[False,False,False,False,0,""],[ 0, 0, 0, 0, 0, 0]]
    pos = 1                     # position in format string
    size = color = False
#    print (matches)
    ididsmth = False
    for match in matches:                   # pro kazdy nalez
        ididsmth = True
        i = 0
        for m in match.groups():            # pro kazdou zaznam v nalezu zjisti
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
            elif m == "size":               # pokud size tak dalsi bude velikost
                size = True            
            elif m == "color":              # pokud color tak dalsi bude barva
                color = True            
            elif m != None:
                if size:                    #pokud receno ze dalsi ma byt barva
                    if m != '':
                        forml[0][4] = int(m)    # uloz velikost
                        forml[1][4] = pos           
                        pos += 1
                        size = False
                    else:
                        reterr(4)               # pokud nenalezena velikost tak chyba 4
                elif color:
                    if m != '':
                        forml[0][5] = m         # ulozeni barvy
                        forml[1][5] = pos
                        pos += 1
                        color = False
                    else:
                        reterr(4)               # pokud nenalezena barva tak chyba 4
    if not ididsmth:                        # pokud nebylo neco nalezeno konci s chybou 4
        reterr(4)
    
    return forml

# vstup jsou struktury s formatem, zpracovava vstupni soubor a vklada fomatovaci prvky
def applyFormat(format_s, format_o):
    global infile
    msg = ""
    orig_msg = infile.read()                # nacti vstupni soubor
    scope = 0                               # zanoreni tagu
    prefixes =[]                            # seznam pocatecnich tagu
    surfixes = []                           # seznam ukoncovacich tagu
    new_msg = ""                            # vystupni zprava
    for r in range(0,len(format_o)):        # pro kazdy regulerni vyraz
#        if len(format_o) == 0:              
#            continue
        
        scope += 1                          # zvys zanoreni

        lookfor = re.compile(format_o[r])           
        kworditer = lookfor.finditer(orig_msg)      # najsi polozky odpovidajici rege
        for kword in kworditer:                     # pro kazdou polozku
#            print(kword,format_o[r])
            start = kword.start()           
            end = kword.end()
            if kword.group() != '':                 # pokud neni prazdna
                prefix, surfix = addTags(orig_msg, kword.group(), \
                        start, end, format_s[r][1])                     # dej mi tagy
                prefixes.append([kword.group(),scope,kword.start(),prefix]) # vloz do seznamu
                surfixes.append([kword.group(),scope,kword.end()-1,surfix]) # vloz do seznamu
    prefixes.sort(key=itemgetter(1))                # razeni seznamu
    prefixes.sort(key=itemgetter(2))                # pocatecnich tagu
    surfixes.sort(key=itemgetter(1), reverse=True)  # razeni seznamu
    surfixes.sort(key=itemgetter(2))                # ukoncovacich tagu
    
    for i in range(0,len(orig_msg)):            # projdi vstup znak po znaku
        p = 0
        s = 0
        while p < len(prefixes):                # pokud kazdy zaznam v seznamu
            if prefixes[p][2] == i:             # zjisti zda se zaznam vaze k pozici
                new_msg += prefixes[p][3]       # ano, vloz pocatcni tag
            p += 1                          

        new_msg += orig_msg[i]                  # pridej znak ze vstupu na vystup

        while s < len(prefixes):                # pokud kazdy zaznam v seznamu
            if surfixes[s][2] == i:             # zjisti zda se zaznam vaze k pozici
                new_msg += surfixes[s][3]       # ano, vloz ukoncovaci tag
            s += 1

    return new_msg 

# vstup informace o nalezu + format dane skupiny, z formatu vytvari posloupnost tagu
def addTags(line, group, start, end, format_t):
    morder = max(format_t[1])           # kolik skupin formatu je definovano
    prefix = ""
    surfix = ""
    for i in range(1,morder+1):                 # poradi od 1 do max
        for j in range(0,len(format_t[0])):     # pro kazdou polozku struktury
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
    
    return prefix, surfix               # vrat pocatecni a ukoncovaci tagy

# inicializace globalnich dat
infile = None
outfile = None
formatfile = None
#struktura pro ulozeni zadanych vstupnich argumentu
args = { 
    '--input': False,
    '--output': False,
    '--format': False,
    '--br': False
    }

# volani kontroly vstupni argumentu
chkarg = parseArg(len(sys.argv),sys.argv)

if not args['--input']:     # pokud neni definovan vstupni soubor, vstup = stdin
    infile = sys.stdin
if not args['--output']:    # pokud neni definovan vystupni soubor, vystup = stdout
    outfile = sys.stdout

if not args['--format'] and not args['--br']:   # pokud nijak neforamtuju text
    copyFile(infile,outfile)                    # zkopiruj vstup na vystup
else:
    processFile()                               # proved operace nad vstupem
    if infile!= sys.stdin:                      # pokud jsme necetli ze stdin zavri soubor
        infile.close()
    if outfile != sys.stdin:                    # pokud jsme necetli ze stdout zavri soubor
        outfile.close()
    if formatfile != None:              # pokud byl zadan soubor s formatem zavri soubor
        formatfile.close()

