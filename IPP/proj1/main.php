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
$surfix = array();
$output = "";
################ function declaration ##################
function findDupl($tabname, $path)
{
  global $surfix;
    if (isset($surfix[$path][$tabname])) // pokud se uz prvek vyskytl navys
      $surfix[$path][$tabname]++;
    else 
      $surfix[$path][$tabname] = 1;      // prvni vyskyt, inicializuj index
  // echo $tabname.$surfix[$path][$tabname]."\t\t$path\n";
}
function searchStruct($data, $tabname, $path)
{
  global $tablist,$ddl;
  for ($data->rewind();$data->valid(); $data->next()) // prochazej XML prvek po prvku
  {
    $tablist->add($tabname, $data->key(), $ddl->getT($data->current(), 0), $path);
    if ($data->hasChildren())                  // ma nejakej podelementy?
    {
      if (!eregi("/".$data->key()."/",$path))  // neni posledni prvek v ceste stejny jak novy?
	$path .= $data->key()."/";
      findDupl($data->key(), $path);           // kolik je prvek v tabulce?
      searchStruct($data->current(),$data->key(),$path);   // zanor se o uroven
    }
    elseif (eregi("^(! *$)?.+$",$data->current()))
    { 
      $tablist->add($data->key(), "value", $ddl->getT($data->current(), 0), $path);
  //    echo "current: ".$data->current()."\n";
    }
    foreach ($data->current()->attributes() as $attname => $attval) // projdi attributy
    {
      $tablist->add($data->key(), $attname, $ddl->getT($attval, 1), $path);
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
if (isset($opts["b"]) && isset($opts["etc"])) // -b a --etc=n nemohou byt zaroven
  reterr(12);

if (isset($opts["input"]))
{
  $in_f = fopen($opts["input"], "r") or reterr(2);  // overeni vstupniho souboru
  $read_in_f = fread($in_f, filesize($opts["input"]));
  //  echo "input= \\$opts[input]\\\n";
}
else
  $read_in_f = file_get_contents("php://stdin");

if (isset($opts["output"]))
  $out_f = fopen($opts["output"], "w") or reterr(3); // overeni vystupniho souboru
//    fwrite($out_f, $read_in_f);  // pouze pro testovaci ucely
else 
  $out_f = STDOUT;
if (isset($opts["g"]))
  reterr(0);
if (isset($opts["header"]))
  fileWrite("--".$opts["header"]."\n\n");
if (!isset($opts))
  reterr(1);
////////////////////////////////////////////////////////////////////
/////////// parsovani XML souboru //////////////////////////////////

$coltypes = array (
  "BIT",	// 0, 1, True, False - case-insensitive
  "INT",	// atribut celociselny
  "FLOAT",	// realne cislo dle C99
  "NVARCHAR",	// atribut obsahuje retezec <tag attr="nvarchar">
  "NTEXT"	// textovy obsah obsahuje textovy retezec <tag>texttovy retezec</tag>
);

#\\\\\\\\\\\\\\\\\\\\\\\\\\/////////////////////////////#
/////////////////// Pouziti Iteratoru \\\\\\\\\\\\\\\\\\\

$xml = new SimpleXMLIterator($read_in_f);  // vytvor z XML objekt
$xml->rewind();				   // skoc na zacatek objektu

// print_r($xml);
searchStruct($xml,"","/");
foreach ($tablist->table as $tabk => $tabv)
{ $i=0;
  if ($tabk == "" || $tabk == "0")
    continue;
  $output .= $ddl->create($tabk).$ddl->primary($tabk);
  foreach ($tabv as $colk => $colv)
  {
    if (isset($tablist->table[$colk]))
    {
      if (isset($surfix[$colv[1]][$colk]) && ($k = $surfix[$colv[1]][$colk]) > 1) // vice prvku s stejnym nazvem?
	for ($j = 1; $j <= $k; $j++)           
	  $output .= $ddl->foreign($colk.$j);
      else
	$output .= $ddl->foreign($colk);

    }
    else
      $output .= $ddl->row($colk, $coltypes[$colv[0]]);

   // echo $tabk.", ".$colk.", ".$colv[0]."\n";
  }
  $output .= $ddl->endtab();
}
fileWrite($output);
// print_r($tablist->table);

// fclose($out_f);

reterr(0); // uspesny konec programu
?>
