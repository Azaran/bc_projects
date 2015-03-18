<?php

#XTD:xvecer18

// ukladani elementu z XML

class Elemlist{
  public $table;   // string
  // inicializace tridy
  public function __construct()
  {
    $this->table = array(array());
  }

  // pridani polozky
  public function add($tabname,$elemname,$ddltype, $path){
    if (!isset($this->table[$tabname][$elemname]))
      $this->table[$tabname][$elemname] = array($ddltype, $path);
    elseif($this->table[$tabname][$elemname][0] < $ddltype)
      $this->table[$tabname][$elemname][0] = $ddltype;

  } 
} 
