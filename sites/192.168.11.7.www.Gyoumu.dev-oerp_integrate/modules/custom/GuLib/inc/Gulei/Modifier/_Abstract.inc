<?php
namespace Gulei\Modifier;

abstract class _Abstract
{
  protected $keys;
  protected $cell_ptr;
  protected $row_ptr;

  abstract protected function isModApplicable($RefVars, $ref);
  abstract protected function isCellApplicable($RefVars, $ref);
  abstract protected function modify($RefVars, $ref);

  function __construct($cell_ptr = null, $row_ptr = null, $keys = null)
  {
    $this->initArrayProperty('keys', $keys);
    $this->initArrayProperty('cell_ptr', $cell_ptr);
    $this->initArrayProperty('row_ptr', $row_ptr);
  }

  private function initArrayProperty($property, $input)
  {
    if(is_string($input)){
      $this->$property = array($input);
    }
    else if(is_array($input)){
      $this->$property = $input;
    }
    else{
      $this->$property = array();
    }
  }

  public function isInKeyDomain($key)
  {
    if(empty($this->keys))
      return true;

    if(isset($this->keys['out'])){
      if(in_array($key, $this->keys['out'], true))
        return false;

      if(!isset($this->keys['in']))
        return true;
    }

    if(isset($this->keys['in'])){
      if(in_array($key, $this->keys['in'], true)){
        return true;
      }
    }

    if(in_array($key, $this->keys, true)){
      return true;
    }
    return false;
  }

  protected function isRowApplicable($RefVars, $ref)
  {
    return true;
  }

  protected function modifyRows($RefVars, $ref)
  {
    return null;
  }

  protected function modifyRow($RefVars, $ref)
  {
    return null;
  }

  protected function &getVarRef(&$item, $ptrs)
  {
    foreach($ptrs as $ptr){
      unset($tmp);
      $tmp = &$item[$ptr];
      unset($item);
      $item = &$tmp;
    }
    return $item;
  }

  protected function &getCellReference(&$item)
  {
    return $this->getVarRef($item, $this->cell_ptr);
  }

  protected function &getRowIterator(&$row)
  {
    return $this->getVarRef($row, $this->row_ptr);
  }

  protected function applyToRows($RefVars, $ref)
  {
    $rows = &$RefVars->refVar('rows');

    foreach($rows as $ind => &$row){
      $RefVars->addRefVar(
        array(
             'row' => &$row,
             'ind' => $ind,
        )
      );

      if($this->isRowApplicable($RefVars, $ref)){
        $this->applyToRow($RefVars, $ref);
      }
    }
  }

  protected function applyToRow($RefVars, $ref)
  {
    $row = &$RefVars->refVar('row');

    $this->modifyRow($RefVars, $ref);
    $row_it = &$this->getRowIterator($row);

    if(!is_array($row_it)){
      echo '<pre>';
      throw new \Exception('Invalid row iterator');
      var_dump($this);
    }

    foreach($row_it as $key => &$cell){
      $RefVars->addRefVar(array('key' => $key));

      if(!$this->isInKeyDomain($key))
        continue;

      $refCell = &$this->getCellReference($cell);
      $RefVars->addRefVar(array('cell' => &$refCell));

      if(!$this->isCellApplicable($RefVars, $ref))
        continue;

      $this->modify($RefVars, $ref);
    }
  }

  public function apply(&$recs, $ref)
  {
    $RefVars = new \Gulei\Manipulator\RefVariable();
    $RefVars->addRefVar(array('rows' => &$recs));

    if(!$this->isModApplicable($RefVars, $ref))
      return false;

    $this->modifyRows($RefVars, $ref);
    $this->applyToRows($RefVars, $ref);
  }
}