<?php

#XTD:xvecer18

// trida pro praci s DDL
class DDL{
  
  // vytvoreni tabulky
  public function create($tablename){
  	return "CREATE TABLE $tablename(";
  }

  // vytvoreni primarniho klice v tabulce
  public function primary($tablename){
  	return "\n   prk_$tablename"."_id INT PRIMARY KEY";
  }

  // vytvoreni ciziho klice v tabulce
  public function foreign($colname){
  	return ",\n   ".$colname."_id INT";
  }

  // vytvoreni polozky v tabulce  
  public function row($colname,$coltype){
  	return ",\n   $colname $coltype";
  }

  // ukonceni tabulky
  public function endtab(){
  	return "\n);\n\n";
  }

  // zjisteni datoveho typu hodnoty dane polozky
  public function getT($value,$attr){
    if (eregi("^(0|1|true|false)$",$value))
      return 0; // "BIT"
    elseif (ereg("^[0-9]*$",$value))
      return 1; // "INT"
    elseif (ereg("^[ ]*[-+]{0,1}([0-9]*\.[0-9]+)|([0-9]+[eE]{1}[-+]{0,1}[0-9]+)[ ]*$",$value))
      return 2; // "FLOAT"
    else
    {
      if ($attr == 1)
	return 3; // "NVARCHAR"
      elseif ($value == "")
	return -1; // "NTEXT" prazdny retezec (moznost prepisu)
      else
  	return 4; // "NTEXT"
    }
  }
}
?>
