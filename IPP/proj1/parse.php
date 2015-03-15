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
  public function add($tabname,$elemname,$ddltype){
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
  public function getCTable(){
    if (!$this->isempty())
    {    
      return $this->table[$this->curr];
    }
    else
      return FALSE; 
  }
  public function getCElem(){
    if (!$this->isempty())
    {    
      return $this->elem[$this->curr];
    }
    else
      return FALSE; 
  }
  public function getCType(){
    if (!$this->isempty())
    {    
      return $this->dtype[$this->curr];
    }
    else
      return FALSE; 
  }
  public function nxt(){
      if ($this->curr < $this->size())
	$this->curr++;
  }
  public function currnt(){
      return $this->curr;
  }
  public function getNext(){
      if ($this->curr+1 < $size)
      {
        $this->curr++;
        $ret = new elemlist();
        $ret = $this->getCurr();
        return $ret;
      }
  } 
  public function getLast(){
      $this->curr = $size - 1;
      //$ret = new elemlist();
      //$ret = $this->getCurr();
      //return $ret;
  } 
  public function setFirst(){
      $this->curr = 0;
      //$ret = new elemlist();
      //$ret = $this->getCurr();
      //return $ret;
  }
  public function isempty(){
    if (!$this->size())
      return TRUE;
    else 
      return FALSE; 
  } 
} 
