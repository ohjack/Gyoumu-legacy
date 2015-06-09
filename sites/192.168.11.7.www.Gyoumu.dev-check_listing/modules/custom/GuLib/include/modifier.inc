<?php
namespace Gulei;
include_once 'manipulator.inc';

abstract class AbstractModifier
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
      throw new \Exception();
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
    $RefVars = new \ManipulatorRefVar();
    $RefVars->addRefVar(array('rows' => &$recs));

    if(!$this->isModApplicable($RefVars, $ref))
      return false;

    $this->modifyRows($RefVars, $ref);
    $this->applyToRows($RefVars, $ref);
  }
}

class ModEscapeXml extends AbstractModifier
{
  protected function isModApplicable($RefVars, $ref)
  {
    return $ref->val('_param', 'escape');
  }

  protected function isCellApplicable($RefVars, $ref)
  {
    return true;
  }

  protected function modify($RefVars, $ref)
  {
    $cell = &$RefVars->refVar('cell');

    $mani = new \ManipulatorGuValue();
    $newVal = htmlspecialchars($mani->value($cell));
    
    $mani->replace($newVal, $cell);
  }
}

class ModWrapValue extends AbstractModifier
{
  protected function isModApplicable($RefVars, $ref)
  {
    return true;
  }

  protected function isCellApplicable($RefVars, $ref)
  {
    return true;
  }

  protected function modify($RefVars, $ref)
  {
    $cell = &$RefVars->refVar('cell');

    $mani = new \ManipulatorGuValue();
    $mani->wrap($cell);
  }
}

class ModPre extends AbstractModifier
{
  protected function isModApplicable($RefVars, $ref)
  {
    return true;
  }

  protected function isCellApplicable($RefVars, $ref)
  {
    if($ref->val('_param', 'pre'))
      return true;

    $key = $RefVars->val('key');

    if($ref->val($key, '_param', 'pre'))
      return true;
  }

  protected function modify($RefVars, $ref)
  {
    $cell = &$RefVars->refVar('cell');

    $mani = new \ManipulatorGuValue();
    $mani->wrap($cell);
    
    $m = $mani->match($cell);
    $cell = str_replace($m[0], "<pre>$m[0]</pre>", $cell);
  }
}

class ModNl2br extends AbstractModifier
{
  protected function isModApplicable($RefVars, $ref)
  {
    return true;
  }

  protected function isCellApplicable($RefVars, $ref)
  {
    $key = $RefVars->val('key');

    if($ref->val($key, '_param', 'no-nl2br'))
      return false;

    if($ref->val($key, '_param', 'nl2br'))
      return true;

    if($ref->val('_param', 'nl2br'))
      return true;
  }

  protected function modify($RefVars, $ref)
  {
    $cell = &$RefVars->refVar('cell');

    $mani = new \ManipulatorGuValue();
    $newVal = nl2br($mani->value($cell), true);
    
    $mani->replace($newVal, $cell);
   }
}

class ModTagize extends AbstractModifier
{
  protected function isModApplicable($RefVars, $ref)
  {
    return true;
  }

  protected function isCellApplicable($RefVars, $ref)
  {
    return true;
  }

  protected function modify($RefVars, $ref)
  {
    $cell = &$RefVars->refVar('cell');
    $key = $RefVars->val('key');

    $gu_name = $ref->val($key, '_attr', 'name');
    $gu_name = is_null($gu_name) ? $key : $gu_name;

    $cell = array(
      'data' => $cell,
      'gu_name' => $gu_name,
      'rowspan' => 1,
    );
  }

  protected function isRowApplicable($RefVars, $ref)
  {
    $row = $RefVars->val('row');
    return empty($row) ? false : true;
  }

  protected function modifyRow($RefVars, $ref)
  {
    $row = &$RefVars->refVar('row');
    $row = empty($row) ? array() : array('data' => $row);
  }

  protected function &getRowIterator(&$row)
  {
    return $row['data'];
  }
}

class ModAppendEmpty extends AbstractModifier
{
  protected function isModApplicable($RefVars, $ref)
  {
    return true;
  }

  protected function isCellApplicable($RefVars, $ref)
  {
    return false;
  }

  protected function modify($RefVars, $ref)
  {
    return null;
  }

  protected function isRowApplicable($RefVars, $ref)
  {
    $row = $RefVars->val('row');
    if(empty($row))
      return true;
  }

  protected function modifyRow($RefVars, $ref)
  {
    $row = &$RefVars->refVar('row');
    $row['empty'] = 1;
    $row['data'] = array();

    foreach($ref->val() as $name => $hd){
      if(strpos($name, '_') !== 0){
        if(!isset($hd['_param']['hide']) || !$hd['_param']['hide']){
          $row['data'][] = '';
        }
      }
    }
  }
}

abstract class AbstractTagModifier extends AbstractModifier
{
  function __construct($keys = null)
  {
    parent::__construct(null, 'data', $keys);
  }
}

class TagModHide extends AbstractTagModifier
{
  protected function isModApplicable($RefVars, $ref)
  {
    return true;
  }

  protected function isCellApplicable($RefVars, $ref)
  {
    $key = $RefVars->val('key');
    
    if($ref->val($key, '_param', 'hide'))
      return true;
  }

  protected function modify($RefVars, $ref)
  {
    $cell = &$RefVars->refVar('cell');
    $cell['style'] = 'display:none;';
  }
}

class TagModCopyDitto extends AbstractModifier
{
  function __construct($keys = null)
  {
    parent::__construct('data', 'data', $keys);
  }

  protected function isModApplicable($RefVars, $ref)
  {
    if($ref->val('_param', 'copy_method') == 'ditto')
      return true;
  }

  protected function isCellApplicable($RefVals, $ref)
  {
    return true;
  }

  protected function modifyRows($RefVars, $ref)
  {
    $cell_bg = array('merge-even', 'merge-odd');
    $row_num = 0;

    $rows = &$RefVars->refVar('rows');

    foreach($rows as $ind => $row){
      $hasDitto = false;

      foreach($row['data'] as $key => $val){
        if($val['data'] === '_DITTO_'){
          $hasDitto = true;

          for($c = 1; $c <= $ind; $c++){
            if(isset($rows[$ind - $c]['data'][$key]['rowspan'])){
              $rows[$ind - $c]['data'][$key]['rowspan']++;
              break;
            }
          }
          unset($rows[$ind]['data'][$key]);
        }
      }

      if($hasDitto){
        $rows[$ind]['merged'] = '1';
      }
      else{
        $row_num++;
      }

      $merge_class = $cell_bg[$row_num % 2];

      if(isset($rows[$ind]['class'])){
        $classes = explode(' ', $rows[$ind]['class']);
        $classes[] = $merge_class;
        $rows[$ind]['class'] = implode($classes, ' ');
      }
      else{
        $rows[$ind]['class'] = $merge_class;
      }
    }
  }

  protected function modify($RefVars, $ref)
  {
    return null;
  }
}

class ModCopyMerge extends AbstractModifier
{
  protected function isModApplicable($RefVars, $ref)
  {
    if($ref->val('_param', 'copy_method') == 'merge')
      return true;
  }

  protected function isCellApplicable($RefVals, $ref)
  {
    return true;
  }

  protected function modifyRows($RefVars, $ref)
  {
    $rows_send = array();
    $rows = &$RefVars->refVar('rows');

    foreach($rows as $row){
      if(in_array('_DITTO_', $row, true)){
        foreach($row as $name => $fld){
          if($fld !== '_DITTO_'){
            $rows_send[count($rows_send) - 1][$name] .= "\n" . $row[$name];
          }
        }
      }
      else{
        $rows_send[] = $row;
      }
    }
    $rows = $rows_send;
  }

  protected function modify($RefVars, $ref)
  {
    return null;
  }
}

class TagModAddCountColumn extends AbstractTagModifier
{
  protected function isModApplicable($RefVars, $ref)
  {
    if($ref->val('_param', 'count'))
      return true;
  }

  protected function isCellApplicable($RefVars, $ref)
  {
    return false;
  }

  protected function modifyRow($RefVars, $ref)
  {
    $row = &$RefVars->refVar('row');
    $row['data'] = array_merge(
      array(
           '%count' => array(
             'data' => '#',
             'gu_name' => '%count',
             'rowspan' => 1,
           ),
      ),
      $row['data']
    );
  }

  protected function modify($RefVars, $ref)
  {
    return null;
  }
}

abstract class AbstractModProcessRecords extends AbstractModifier
{
  abstract protected function getType();

  protected function searchFunction($param)
  {
    $type = $this->getType();
    $ObjTable = $param['ObjTable'];

    if(!$ObjTable->getArch()){
      return array();
    }

    $eles = array();
    $eles[] = $ObjTable;

    if(isset($param[$type]) && $param[$type]){
      $eles[] = new \GuArchElement('<root>' . $param[$type] . '</root>');
    }

    $funcs = array();
    foreach($eles as $ele){
      $funcNode = $ele->xp('./' . $type, $ele->getArch())->item(0);

      if($funcNode){
        foreach($ele->xp('./function | ./func', $funcNode) as $func_ele){
          $func = array();

          if($func_type = $func_ele->getAttribute('type')){
            $func['type'] = $func_type;
          }
          else{
            $func['type'] = 'error';
          }

          foreach($ele->xp('./source | ./src', $func_ele) as $node){
            $newNode = $ObjTable->getDoc()->importNode($node, true);
            $func['src'][] = $newNode;
          }

          $func['fld'] = array();
          foreach($ele->xp('./field | ./fld', $func_ele) as $node){
            $func['fld'][] = $node->nodeValue;
          }

          foreach($ele->xp('./message | ./msg', $func_ele) as $node){
            $func['msg'][] = trim($node->nodeValue);
          }

          $ref = array(
            'param' => $param,
            'func' => $func,
            'ObjTable' => $param['ObjTable'],
            'prefix' => $param['func_prefix'],
          );

          $classname = "\\Gulei\\Command\\" . ucfirst($type);
          $funcs[] = new $classname($ref);
        }
      }
    }
    return $funcs;
  }

  protected function modifyRows($RefVars, $ref)
  {
    $funcs = $this->searchFunction($ref->val('_param'));

    if(empty($funcs)){
      return;
    }

    $rows = &$RefVars->refVar('rows');

    foreach($funcs as $func){
      $refVar = new \ManipulatorRefVar();
      $refVar->addRefVar(array('recs' => &$rows));
      $func->execute($refVar);
    }
  }

  protected function isCellApplicable($RefVars, $ref)
  {
    return false;
  }

  protected function modify($RefVars, $ref)
  {
    return null;
  }
}

class ModTranslate extends AbstractModProcessRecords
{
  protected function getType()
  {
    return 'translate';
  }

  protected function isModApplicable($RefVars, $ref)
  {
    return true;
  }
}

class ModValidate extends AbstractModProcessRecords
{
  protected function getType()
  {
    return 'validate';
  }

  protected function isModApplicable($RefVars, $ref)
  {
    return !$ref->val('_param', 'no_validate');
  }
}

class ModFilter extends AbstractModProcessRecords
{
  protected function getType()
  {
    return 'filter';
  }

  protected function isModApplicable($RefVars, $ref)
  {
    return true;
  }
}

class ModUsort extends AbstractModifier
{
  protected function isModApplicable($RefVars, $ref)
  {
    return $ref->val('_param', 'sort');
  }

  protected function modifyRows($RefVars, $ref)
  {
    $rows = &$RefVars->refVar('rows');
    usort($rows, $ref->val('_param', 'sort'));
  }

  protected function isCellApplicable($RefVars, $ref)
  {
    return false;
  }

  protected function modify($RefVars, $ref)
  {
    return null;
  }
}

class ModAddCount extends AbstractModifier
{
  protected function isModApplicable($RefVars, $ref)
  {
    return $ref->val('_param', 'count');
  }

  protected function modifyRows($RefVars, $ref)
  {
    $rows = &$RefVars->refVar('rows');

    $c = 1;
    foreach($rows as &$row)
    {
      if($ref->val('_param', 'copy_method') == 'ditto'
        && in_array('_DITTO_', $row, true) ) {

        $row['%count'] = '_DITTO_';
      }
      else{
        $row['%count'] = $c;
        $c++;
      }
    }
  }

  protected function isCellApplicable($RefVars, $ref)
  {
    return false;
  }

  protected function modify($RefVars, $ref)
  {
    return null;
  }
}

abstract class AbstractModPack
{
  protected $items = array();

  public function __construct()
  {
    $this->init();
  }

  abstract protected function init();

  protected function append(array $mods)
  {
    $this->items = array_merge($this->items, $mods);
  }

  public function getItems()
  {
    return $this->items;
  }
}

class PackHeaderRawMods extends AbstractModPack
{
  protected function init()
  {
    $this->append(
      array(
           new ModTagize(),
           new TagModHide(),
      )
    );
  }
}

class PackHeaderAltMods extends AbstractModPack
{
  protected function init()
  {
    $this->append(
      array(
           new TagModAddCountColumn(),
      )
    );
  }
}

class PackRowRawMods extends AbstractModPack
{
  protected function init()
  {
    $this->append(
      array(
           new ModTagize(),
           new TagModHide(),
           new ModAppendEmpty(),
           new TagModCopyDitto(),
      )
    );
  }
}

class PackRecordAltMods extends AbstractModPack
{
  protected function init()
  {
    $this->append(
      array(
           new ModCopyMerge(),
           new ModTranslate(),
           new ModFilter(),
           new ModUsort(),
           new ModAddCount(),
           new ModValidate(),
           new ModEscapeXml(),
      )
    );
  }
}

class PackRowAltMods extends AbstractModPack
{
  protected function init()
  {
    $this->append(
      array(
           new ModPre('data', 'data'),
           new ModWrapValue('data', 'data', array('out' => array('id'))),
      )
    );
  }
}