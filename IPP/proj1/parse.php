<?php
class Elemlist{
  //public $path;    // string
  public $table;   // string
  //public $elem;    // string
  //public $dtype;   // string
  //public $cnt;     // integer
  //public $curr;    // integer
  
  public function __construct()
  {
    //$this->path = array();
    $this->table = array(array());
    //$this->elem = array();
    //$this->dtype = array();
    //$this->cnt = 0;
    //$this->curr = -1;
  }
  public function add($tabname,$elemname,$ddltype, $path){
    //$this->path[$this->cnt] = $path;
    $this->table[$tabname][$elemname] = array($ddltype, $path);
    //$this->elem[$this->cnt] = $elemname;
    //$this->dtype[$this->cnt] = $ddltype;
    //$this->cnt++;
    //$this->curr++;   
  } 
  /*
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
  public function getCPath(){
    if (!$this->isempty())
    {    
      return $this->path[$this->curr];
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
/*
  public function getNext(){
      if ($this->curr+1 < $size)
      {
        $this->curr++;
        $ret = new elemlist();
        $ret = $this->getCurr();
        return $ret;
      }
  } 

  public function setLast(){
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
   */
  public function isempty(){
    if (!$this->size())
      return TRUE;
    else 
      return FALSE; 
  } 
} 
