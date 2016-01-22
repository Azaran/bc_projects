perl 7z2hashcat.pl ../testfiles/enc_readme.7z > hash.out 
sed -e 's:$:\n&:g' hash.out
