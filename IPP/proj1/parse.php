<?php
class elemlist{
private $table;   // string
private $element; // string
private $attr;    // boolean
private $count;   // integer
function _construct()
{
  $table = array();
  $element = array();
  $attr = array();
  $count = 0;
}
function add($tabname,$elemname,$attrib){
  $table[$count] = $tabname;
  $element[$count] = $elemname;
  $attrib[$count] = $attr;
  $count++;
}
function size(){
  return $count;
}
function 

?>
