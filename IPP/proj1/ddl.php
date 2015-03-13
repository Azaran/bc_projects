<?php
class ddl{

public function create($tablename){
	return "CREATE TABLE $tablename(";
}
public function primary($colname,$coltype){
	return "\n\t\t\tprk_$tablename"."_id INT PRIMARY KEY";
}
public function foreign($colname,$coltype){
	return ",\n\t\t\t$colname $coltype";
}
public function row($colname,$coltype){
	return ",\n\t\t\t$colname $coltype";
}
public function endtab(){
	return "\n);\n";
}

}
?>
