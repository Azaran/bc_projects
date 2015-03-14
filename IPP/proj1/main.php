<?php
require 'ddl.php';
require 'parse.php';
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
$tablist = new Elemlist;
$ddl = new DDL;
################ function declaration ##################

function searchStruct($data, $tabname, $indent)
{
  global $tablist,$ddl;
  $indent.="\t";
  for ($data->rewind();$data->valid(); $data->next())
  {
    //echo $data->current()."\t";
    if ($data->hasChildren())
    {
      // echo "Volame rekurzi > > >".$data->key()."\n";
      searchStruct($data->current(),$data->key(),$indent);
    }
      if (!empty($data->current()))
      {	
	// echo "Attribute nalezen: ";
	foreach ($data->current()->attributes() as $attname => $attval)
        {
	  $tablist->add($data->key(),$attname,$ddl->getT($attval,1));
	  // echo $data->key()." ($attname => $attval), ";
        }
	// echo "\n";
      }
      else
      {
        $tablist->add($tabname, $data->key(),$ddl->getT($data->current(),0)r);
        //echo "$tabname, ".$data->key().", 0\n";
      }
  }
  return 0; 
}
function reterr($err)   // vrat a zapis do stderr
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
function makeOutput ()
{
  global $tablist, $ddl;
  
}
################# main function #########################
///////// overeni parametru a otevreni souboru ////////////////////

$opts = getopt($shortopts, $longopts);
if (isset($opts["help"]))  // volame help?
{ 
  echo $help."\n";
  reterr(0);
}
if (isset($opts["b"]) && isset($opts["etc"])) // -g a --etc=n nemohou byt zaroven
  reterr(12);

if (isset($opts["input"]))
{
  $in_f = @fopen($opts["input"], "r") or reterr(2);  // overeni vstupniho souboru
  $read_in_f = fread($in_f, filesize($opts["input"]));
  //  echo "input= \\$opts[input]\\\n";
}
else
  $read_in_f = file_get_contents("php://stdin");

if (isset($opts["output"]))
  $out_f = @fopen($opts["output"], "w") or reterr(3); // overeni vystupniho souboru
//    fwrite($out_f, $read_in_f);  // pouze pro testovaci ucely
else 
  $out_f = STDOUT;

if (isset($opts["header"]))
  fileWrite($opts["header"]);

////////////////////////////////////////////////////////////////////
/////////// parsovani XML souboru //////////////////////////////////

$coltypes = array (
  "BIT",	// 0, 1, True, False - case-insensitive
  "INT",	// atribut celociselny
  "FLOAT",	// realne cislo dle C99
  "NVARCHAR",	// atribut obsahuje retezec <tag art="nvarchar">
  "NTEXT"	// textovy obsah obsahuje textovy retezec <tag>texttovy retezec</tag>
);

/////////////////////////////////////////////////////////
/////////////////// PouyitiIiteratoru \\\\\\\\\\\\\\\\\\

$xml = new SimpleXMLIterator ($read_in_f);
$xml->rewind();

print_r($xml);
searchStruct($xml,"","");
print_r($tablist);



//fwrite($out_f, print_r($xml, TRUE));

//fclose($out_f);
var_dump($opts);

reterr(0); // uspesny konec programu
?>
