<?php
class DDL{

  public function create($tablename){
  	return "CREATE TABLE $tablename(";
  }
  public function primary($colname,$coltype){
  	return "\n\t\t\tprk_$tablename"."_id INT PRIMARY KEY";
  }
  public function foreign($colname,$coltype){
  	return ",\n\t\t\t$colname\_id $coltype";
  }
  public function row($colname,$coltype){
  	return ",\n\t\t\t$colname $coltype";
  }
  public function endtab(){
  	return "\n);\n";
  }
  public function getT($value,$attr){
    if ($attr == 1)
      return 3; // "NVARCHAR"
    elseif (eregi("^(0|1|true|false)$",$value))
      return 0; // "BIT"
    elseif (ereg("^[-+]{0,1}[0-9]*\.{0,1}[0-9]+([eE][-+]{0,1}[0-9]+)*$",$value))
      return 2; // "FLOAT"
    elseif (ereg("^[0-9]*$",$value))
      return 1; // "INT"
    else
      return 4; // "NTEXT"
  }
}
class Relations{

}
?>
