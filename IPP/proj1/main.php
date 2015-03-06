<?php
################# variable declaration ##################
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
    0 => "", 
    1 => "spatny format nebo kombinace parametru",
    2 => "neni zadany nebo se nepodarilo otevrit vstupni soubor",
    3 => "chyba pri pokusu o otevreni vystupniho souboru (spatna prava)",
    4 => "chybny format vstupniho souboru",
    12 => "zadane parametry -b a --etc nemohou spolecne fungovat",
    100 => "nespecifikovana chyba",
  );
$shortopts = "abg";
$longopts = array (
  "help",
  "input::",
  "output::",
  "header::",
  "etc::",
);

################ function declaration ##################

function retnwri($err)   // vrat a zapis do stderr
{
  fwrite(STDERR, $GLOBALS["err_num"][$err]);
  exit($err);
}
function fileWrite($text_out)
{
  if (isset($out_f))
    fwrite($out_f, $text_out);  // pouze pro testovaci ucely
  else
    fwrite(STDOUT, $text_out);
  return(0);
}


################# main function #########################

///////// overeni parametru a otevreni souboru ////////////////////

$opts = getopt($shortopts, $longopts);
if (isset($opts["help"])){ // volame help?
  echo $help."\n";
  retnwri(0);
}

if (isset($opts["b"]) && isset($opts["etc"])) // -g a --etc=n nemohou byt zaroven
  retnwri(12);

if (isset($opts["input"]))
{
  $in_f = @fopen($opts["input"], "r") or retnwri(2);  // overeni vstupniho souboru
  $read_in_f = fread($in_f, filesize($opts["input"]));
  //  echo "input= \\$opts[input]\\\n";
}
else
  $read_in_f = file_get_contents("php://stdin");

if (isset($opts["output"]))
  $out_f = @fopen($opts["output"], "w") or retnwri(3); // overeni vystupniho souboru
//    echo "output= \\$opts[output]\\\n";
//    fwrite($out_f, $read_in_f);  // pouze pro testovaci ucely
else 
  $out_f = STDOUT;

if (isset($opts["header"]))
  fileWrite($opts["header"]);
// echo "header= \\$opts[header]\\\n";

////////////////////////////////////////////////////////////////////
/////////// parsovani XML souboru //////////////////////////////////

$coltypes = array (
  "BIT",	// 0, 1, True, False - case-insensitive
  "INT",	// atribut celociselny
  "FLOAT",	// realne cislo dle C99
  "NVARCHAR",	// atribut obsahuje retezec <tag art="nvarchar">
  "NTEXT"	// textovy obsah obsahuje textovy retezec <tag>texttovy retezec</tag>
);



$xml = simplexml_load_string($read_in_f);
//print_r($xml);
//
////////// vypis jednotlivych urovni XML ////////////////
// TODO: prevest na rekurzivni funkci a vytvorit strukturu pro ukladani dat
foreach ($xml->children()as $children)
{
  echo $children->getName()."\n";
  foreach ($children->children()as $childs)
  {   #   print_r($children);
    echo "\t".$childs->getName()."\n";
    foreach ($childs->children() as $child)
      echo "\t\t".$child->getName()." => ".$child->__toString()."\n";
  }
}

/////////////////////////////////////////////////////////
//fwrite($out_f, print_r($xml, TRUE));
/*
$create = "CREATE TABLE $tablename(";
$prk = "\n\t\t\tprk_$tablename_id INT PRIMARY KEY";
$row = ",\n\t\t\t$colname $coltype";
 */









/////////////////////////////////////////////////////////////////////
/////////// validni zruseni zdroju //////////////////////////////////
//fclose($out_f);
//print_r($opts);
var_dump($opts);

retnwri(0); // uspesny konec programu
?>
