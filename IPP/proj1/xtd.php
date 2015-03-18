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
    90 => "konflikt nazvu sloupcu z attributu nebo text. obsahu  a odkazu na tabulky",
    100 => "nespecifikovana chyba",
    150 => "nenaimplementovano",
  );

$shortopts = "abg";
$longopts = array (
  "help",
  "input:",
  "output:",
  "header:",
  "etc:",
);

$mode = array("a" => 0, "b" => 0, "g" => 0, "etc" => 0, "input" => 0, "output" => 0);
$tablist = new Elemlist;
$ddl = new DDL;
$surfix = array();
$output = "";

################ function declaration ##################

function findDupl($tabname, $path)
{
  global $surfix;

  if ($path == "/$tabname/")
    $surfix[$path][$tabname] = 1;      // prvni vyskyt, inicializuj index
  elseif (isset($surfix[$path][$tabname])) // pokud se uz prvek vyskytl navys
    $surfix[$path][$tabname]++;
  else 
    $surfix[$path][$tabname] = 1;      // prvni vyskyt, inicializuj index
}

function searchStruct($data, $tabname, $path)
{
  global $tablist,$ddl,$mode;
    $cpath = $path;
  for ($data->rewind();$data->valid(); $data->next()) // prochazej XML prvek po prvku
  {
    $key = strtolower($data->key());

    if (!eregi(".*/".$key."/",$cpath))  // neni posledni prvek v ceste stejny jak novy?
      $cpath = $path.$key."/";

    findDupl($key, $cpath);           // kolik je prvku v tabulce?
    $tablist->add($tabname, $key, $ddl->getT($data->current(), 0), $cpath);
      
    if ($data->hasChildren())                  // ma nejakej podelementy?
      searchStruct($data->current(),$key,$cpath);   // zanor se o uroven
    elseif (preg_match("/^[ \t\n\r\x0B]*(.*)[ \t\n\r\x0B]*$/s",$data->current(),$match))
      if ($match[1] != "")
	$tablist->add($key, "value", $ddl->getT($match[1], 0), $cpath);

    if ($mode["a"] == 0)
      foreach ($data->current()->attributes() as $attname => $attval) // projdi attributy
      {
	$attname = strtolower($attname);
	$tablist->add($key, $attname, $ddl->getT($attval, 1), $cpath);
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
  global $out_f;
  if (isset($out_f))
    fwrite($out_f, $text_out, strlen($text_out));  // pouze pro testovaci ucely
  else
    fwrite(STDOUT, $text_out, strlen($text_out));
}

function makeFilePath ($path, $io)
{
  if (!is_string($path))
    return 2;
  if ($io == 0)
  {
    preg_match('/^(.*\/)?(.+\.[0-9a-z]+)$/i',$path, $matches); // slozka kde bude soubor ulozen
    $rpath = realpath($matches[1]);
    if ($rpath)			      // overeni otevreni slozky
      $rpath .= "/".$matches[2];
  }
  else
    $rpath = realpath($path); // soubor pro cteni
  return $rpath; 
}

################# main function #########################
///////// overeni parametru a otevreni souboru ////////////////////


$opts = getopt($shortopts, $longopts);

if ($opts == false && $argc > 1)
  reterr(1);

foreach (array_keys($opts) as $opt)
  switch($opt)
  {
    case "help":  // volame help?
    {
      if (count($opts) > 1)  // $opt obsahuje vic jak jednu polozku
	reterr(1);
      reterr(0);
    }
    case 'b':
    {
      if ($mode[$opt]!= 0)
	reterr(1);
      if ($mode["etc"] > 0) // -b a --etc=n nemohou byt zaroven
        reterr(1);
      $mode["b"] = 1;
      break;
    }
    case 'a':
    {
      if ($mode[$opt]!= 0)
	reterr(1);
      $mode["a"] = 1;
      break;
    }
    case "etc":
    {
      reterr(150);
      if ($mode[$opt]!= 0)
	reterr(1);
      if ($mode["b"] > 0) // -b a --etc=n nemohou byt zaroven
        reterr(1);
      $mode["etc"] = intval($opts[$opt]);

      break;
    }    
    case "input":
    {
      $mode[$opt] = 1;
      $fpath = makeFilePath($opts["input"],1);
      if ($fpath == 2)
	reterr(1);
      elseif ($fpath == false)
	reterr(2);
      else
	$in_f = $opts[$opt];
      break;
    }
    case "output":
    {
      $fpath = makeFilePath($opts["output"],0);
      if ($fpath == 2)
	reterr(1);
      elseif ($fpath)
	$out_f = fopen($fpath, "w") or reterr(3); // overeni vystupniho souboru
      else
	reterr(3);
      break;
    }
    case "g":
    {
      if ($mode[$opt]!= 0)
	reterr(1);
      $mode[$opt] = 1;
      reterr(150);
      break;
    }
    case "header":
    {
      if ($mode[$opt])
	reterr(1);
      $mode[$opt] = 1;
      $output .="--".$opts["header"]."\n\n";
      break;
    }
    default:
      break;
  }


////////////////////////////////////////////////////////////////////
/////////// parsovani XML souboru //////////////////////////////////

$coltypes = array (
  -1 => "NTEXT",      // prazdny retezec
  "BIT",	// 0, 1, True, False - case-insensitive
  "INT",	// atribut celociselny
  "FLOAT",	// realne cislo dle C99
  "NVARCHAR",	// atribut obsahuje retezec <tag attr="nvarchar">
  "NTEXT"	// textovy obsah obsahuje textovy retezec <tag>texttovy retezec</tag>
);

#\\\\\\\\\\\\\\\\\\\\\\\\\\/////////////////////////////#
/////////////////// Pouziti Iteratoru \\\\\\\\\\\\\\\\\\\


if (isset($in_f))
  $fcont = file_get_contents($in_f);
else
  $in_f= "php://stdin";


$xml = new SimpleXMLIterator($in_f,0,true);  // vytvor z XML objekt
$xml->rewind();				   // skoc na zacatek objektu

$prev=1;

searchStruct($xml,"","/");

foreach ($tablist->table as $tabk => $tabv)  // pro kazdou tabulku
{ 
  if ($tabk == "" || $tabk == "0")    // z korene nedelej tabulku
    continue;

  $output .= $ddl->create($tabk).$ddl->primary($tabk); 

  foreach ($tabv as $colk => $colv) // pro kazdy sloupec
  {
    if (isset($tabv[$colk."_id"]))
      reterr(90);

    if (isset($tablist->table[$colk]))   // pokud se jedna o odkaz na jinou tabulku
    {  
      // vice prvku s stejnym nazvem a nemame zadany -b?
      if (preg_match("/(.*\/(.+)\/)".$colk."\/$/",$colv[1],$match))
      {
	if (isset($match) && isset($surfix[$match[1]][$match[2]]))
	  $prev = $surfix[$match[1]][$match[2]];
      }
      elseif (preg_match("/(\/)".$colk."\/$/",$colv[1],$match))
      {
	if (isset($match) && isset($surfix[$match[1]]))
	  $prev = $surfix[$match[1]][$match[1]];
      }

      if (isset($surfix[$colv[1]][$colk]))
	$k = ($surfix[$colv[1]][$colk]-$prev+1);
      
      if (isset($surfix[$colv[1]][$colk]))
      {
	$k = $surfix[$colv[1]][$colk];
	if ($k > $prev && $mode["b"]==0)
	  for ($j = 1; $j <= $k; $j++)
	    $output .= $ddl->foreign($colk.$j);
	else					    // mame -b
	  $output .= $ddl->foreign($colk);
      }
    }
    else					    // radek (sloupec) tabulky
      $output .= $ddl->row($colk, $coltypes[$colv[0]]);
  }
  $output .= $ddl->endtab();
}

//print_r($tablist);
//var_dump($surfix);
fileWrite($output);

reterr(0); // uspesny konec programu
?>
