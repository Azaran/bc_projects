<?php
$help = <<<EOF
./src/xtd.php
------------------------------------------------------------------------------
| --help                 - Zobrazeni napvedy k programu
| --input=filename.ext   - Vstupni soubor s XML (jinak STDIN)
| --output=filename.ext  - Vystupni soubor s XML (jinak STDOUT)
| --header='text'        - Hlavicka vystupniho souboru
| --etc=num              - Maximalni pocet sloupcu stejneho typu elementu
| -a                     - Z attributu se negeneruji sloupce
| -b                     - Vice  stejny elementu je reprezentovano jako jeden
| -g                     - Vystupni soubor bude pouze XML tvaru
------------------------------------------------------------------------------
EOF;
$err_num = array ( 
    1 => "spatny format nebo kombinace parametru",
    2 => "neni zadany nebo se nepodarilo otevrit vstupni soubor",
    3 => "chyba pri pokusu o otevreni vystupniho souboru (spatna prava)",
    4 => "chybny format vstupniho souboru",
    12 => "zadany parametry -b a --etc -> nemohou spolecne fungovat",
    100 => "dalsi chyby",
  );
$shortopts = "abg";

$longopts = array (
  "help",
  "input::",
  "output::",
  "header::",
  "etc::",
);
$opts = getopt($shortopts, $longopts);
if (isset($opts["help"])){ // volame help?
  echo $help."\n";
  return(0);
}
if (isset($opts["b"]) && isset($opts["etc"])) // -g a --etc=n nemohou byt zaroven
  return(12);

if (isset($opts["input"])){
  echo "input= \\$opts[input]\\\n";
  $in_f = @fopen($opts["input"], "r") or exit(2);  // overeni vstupniho souboru
  $read_in_f = fread($in_f, filesize($opts["input"]));
}
else
  $read_in_f = file_get_contents("php://stdin");
if (isset($opts["output"])){
  echo "output= \\$opts[output]\\\n";
  $out_f = @fopen($opts["output"], "w") or exit(3); // overeni vystupniho souboru
  fwrite($out_f, $read_in_f);
}
else 
  fwrite(STDOUT, $read_in_f);
if (isset($opts["header"]))
  echo "header= \\$opts[header]\\\n";


return(0);
//print_r($opts);
var_dump($opts);
?>
