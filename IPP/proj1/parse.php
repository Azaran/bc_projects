<?php
class Elemlist{
   var $table;   // string
   var $elem;    // string
   var $dtype;    // string
   // var $attr;    // boolean
   var $cnt;   // integer
   var $curr;    // integer
  
  public function __construct()
  {
    $this->table = array();
    $this->elem = array();
    $this->dtype = array();
    // $this->attr = array();
    $this->cnt = 0;
    $this->curr = -1;
  }
  public function add($tabname,$elemname,$ddltype,$attrib){
    $this->table[$this->cnt] = $tabname;
    $this->elem[$this->cnt] = $elemname;
    $this->dtype[$this->cnt] = $ddltype;
    // $this->attr[$this->cnt] = $attrib;
    $this->cnt++;
    $this->curr++;   
}
  public function size(){
  return $this->cnt;
  }
  public function getCurr(){
    if (!isempty())
    {    
      $ret = new elemlist();
      $ret->add($this->table[$this->curr],$this->elem[$this->curr],$this->attr[$this->curr],$this->dtype[$this->curr]);
      return $ret;
    }
    else
      return FALSE; 
  }
  public function getNext(){
      if ($this->curr+1 < $size)
      {
        $this->curr++;
        $ret = new elemlist();
        $ret = getCurr();
        return $ret;
      }
  } 
  public function getLast(){
      $this->curr = $size - 1;
      $ret = new elemlist();
      $ret = getCurr();
      return $ret;
  } 
  public function getFirst(){
      $this->curr = 0;
      $ret = new elemlist();
      $ret = getCurr();
      return $ret;
  }
  public function isempty(){
    if ($this->size == 0)
      return TRUE;
    else 
      return FALSE; 
  } 
} 
