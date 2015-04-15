

#SYN:xvecer18

import sys
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

def parseArg(argc,argv):
   # print ("argc = "+ str(argc))
    i = 1
    if argv[1] == "--help":
        if argc == 2:
            printHelp();
            sys.exit(0)
        else:
            reterr(1)
    while i < argc:
        print (str(i))
        if argv[i] == "--input":
            if args[argv[i]]:
                reterr(1)
            args[argv[i]] = True
            if i+1 < argc:
                print (argv[i])
                i+=1
#                infile = my_fopen(argv[++i],'r')
            else:
                reterr(1)
        elif argv[i] == "--output":
            if args[argv[i]]:
                reterr(1)
            args[argv[i]] = True
            if i+1 < argc:
                i+=1
#                outfile = my_fopen(argv[++i],'w')
            else:
                reterr(1)
        elif argv[i] == "--format":
            if args[argv[i]]:
                reterr(1)
            args[argv[i]] = True
            if i+1 < argc:
                i+=1
#                formatfile = my_fopen(argv[++i],'r')
            else:
                reterr(1)
        elif argv[i] == "--br":
            if args[argv[i]]:
                reterr(1)
            args[argv[i]] = True
        else:
            reterr(1)
        print (argv[i])
        i+=1

infile = sys.stdin
outfile = sys.stdout
formatfile = None
args = { 
    '--input': False,
    '--output': False,
    '--format': False,
    '--br': False
    }

parseArg(len(sys.argv),sys.argv)


