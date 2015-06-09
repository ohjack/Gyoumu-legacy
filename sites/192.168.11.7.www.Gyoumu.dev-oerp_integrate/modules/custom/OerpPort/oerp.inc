<?php
use Oerp as O;

require_once('include/func.inc');
require_once('include/view.inc');

class OerpObject extends \Oerp\Query\Basic
{
  function __construct($mod, $cri = array(), $flds = array(), $ids = null)
  {
    parent::__construct($mod, $cri, $flds, $ids);
  }

  public function defaults($GETNAME = false)
  {
    $q = new \Oerp\Query\Basic($this->getModel(), $this->getCriteria(), $this->getFields());
    $vals = $q->defaults();

    if ($GETNAME) {
      $defs = $this->getFieldsDef()->promote('name');

      foreach ($vals as $fname => &$val) {
        if ($val && in_array($defs[$fname]['ttype'],
                             array('one2many', 'many2one'))
        ) {

          $NameObj = new OerpObject(
            $defs[$fname]['relation'], null, array('name'), $val);

          $name = $NameObj->raw();
          $name = $name['name'];

          $val = $name;
        }
      }
    }
    return $vals;
  }

  public function getFields($pass = false)
  {
    $flds = parent::getFields();
    if (!empty($flds) || $pass) {
      return $flds;
    } else {
      return \Oerp\Query\Fields::getDef($this->getModel())->bunchKey('name');
    }
  }

  public function getIds($PASS = false)
  {
    $ids = parent::getIds();

    if ($PASS) {
      return $ids;
    }

    if (empty($ids)) {
      $Tmp = new \Oerp\Query\Basic(
        parent::getModel(),
        parent::getCriteria(),
        array('id')
      );

      return $Tmp->raw()->bunchKey('id');
    } else {
      return $ids;
    }
  }

  public function getModelDef()
  {
    $mod = new OerpObject(
      'ir.model', array(array('model', '=', $this->getModel()))
    );

    $val = $mod->raw();
    return new \Oerp\Util\_Array($val[0]);
  }

  public function getFieldsDef($FULL = false)
  {
    if (parent::getFields() == array() || $FULL == true) {
      return \Oerp\Query\Fields::getDef($this->getModel());
    } else {
      return \Oerp\Query\Fields::getDef($this->getModel(), $this->getFields());
    }
  }

  protected function getFieldDef($name)
  {
    return \Oerp\Query\Fields::getDef($this->getModel(), $name);
  }

  public function refer($name, $flds = array(), $RTNOBJ = false)
  {
    $def = $this->getFieldDef($name);
    if (!in_array($def['ttype'],
                  array('many2one', 'one2many', 'many2many'))
    ) {

      return null;
    }

    $mem_ids = $this->collectMemIds($name);
    if (empty($mem_ids)) {
      return null;
    }

    $Target = new OerpObject(
      $def['relation'], null, $flds, $mem_ids);

    if ($RTNOBJ) {
      return $Target;
    } else {
      return $Target->raw();
    }
  }

  private function collectMemIds($name)
  {
    $val = $this->raw();
    $def = $this->getFieldDef($name);

    switch ($def['ttype']) {
      case 'many2one':
        $ids = $val->bunch($name);
        $ids = array_keys($ids);
        return $ids;
        break;
      case 'one2many':
        $ids = $val->collect($name);
        return $ids;
        break;
    }
  }

  public function union($NewSet)
  {
    if ($this->getModel() != $NewSet->getModel()) {
      die('Cannot get union from two differient models');
    }

    $ids = array_merge($this->getIds(), $NewSet->getIds());
    $flds = array_merge($this->getFields(), $NewSet->getFields());

    $Union = new OerpObject(
      $this->getModel(), null, $flds, $ids);

    return $Union;
  }

  public function intersect($Parent)
  {
    $Flds = new \Oerp\Query\Fields($Parent->getModel());
    $rels = $Flds->getRels($this->getModel());
    $attr = $rels[0];
    //  TODO field to be selectable

    if (!empty($attr)) {
      if ($attr['mod'] == $this->getModel()) {
        $Tmp = new \Oerp\Query\Basic(
          $Parent->getModel(), $Parent->getCriteria(), array($attr['key']));

        if ($Parent->getIds() == array(0)) {
          return $attr;
        }

        $this_ids = $this->getIds(true);
        if (!empty($this_ids)) {
          $all_ids = $Tmp->raw()->bunchKey('id');
          $ids = array_intersect($this_ids, $all_ids);

          if (!empty($ids)) {
            $this->setIds($ids);
            return $attr;
          } else {
            return null;
          }
        }
        else {
          switch ($attr['ttype']) {
            case 'many2one':
              $this->setIds($Tmp->raw()->bunchKey($attr['key']));
              break;
            case 'one2many':
              $this->setIds($Tmp->raw()->collect($attr['key']));
              break;
          }
          return $attr;
        }
      }
    }
    return null;
  }
}

define('OERP_BTN_CHK', 1);
define('OERP_BTN_NEW', 2);
define('OERP_BTN_EDIT', 4);
define('OERP_BTN_DEL', 8);

class OerpButton
{
  private $btns;

  function __construct($btns)
  {
    $this->btns = $btns;
  }

  public function hasBtn($btn)
  {
    return $this->btns & $btn;
  }

  public function getHtml($btns = null)
  {
    $output = '';

    $chk = is_null($btns) ? $this->btns : $btns;

    if ($chk & OERP_BTN_CHK) {
      $output .= '
        <div class="oerp-button oerp-btn-chk ui-state-default ui-corner-all">
          <span class="oerp-icon ui-icon ui-icon-check"></span>
        </div>
      ';
    }

    if ($chk & OERP_BTN_EDIT) {
      $output .= '
        <div class="oerp-button oerp-btn-edit ui-state-default ui-corner-all">
          <span class="oerp-icon ui-icon ui-icon-pencil"></span>
        </div>
      ';
    }

    if ($chk & OERP_BTN_DEL) {
      $output .= '
        <div class="oerp-button oerp-btn-del ui-state-default ui-corner-all">
          <span class="oerp-icon ui-icon ui-icon-close"></span>
        </div>
      ';
    }

    return $output;
  }

  private function countBtn($EXC_NEW = false)
  {
    $bits = $this->btns;
    $c = 0;

    while ($bits) {
      $c += ($bits & 1) ? 1 : 0;
      $bits = $bits >> 1;
    }

    if ($EXC_NEW && $this->hasBtn(OERP_BTN_NEW)) {
      $c--;
    }

    return $c;
  }

  public function getWidthClass($EXC_NEW = false)
  {
    $c = $this->countBtn($EXC_NEW);
    $c = ($c < 1) ? 1 : $c;

    return 'oerp-op-' . $c;
  }
}

class OerpCsvBuffer extends GuCsvBuffer
{
  protected function getFuncPrefix()
  {
    return array_merge(array('OerpFunc_'), parent::getFuncPrefix());
  }
}

class OerpHookController
{
  public function getXML($name)
  {
    $sql = 'SELECT conf FROM {oerp_hook} WHERE name="%s"';
    return db_result(db_query($sql, $name));
  }

  public function getArray($name)
  {
    $xml = $this->getXML($name);

    if(!$xml){
      return array();
    }

    $Arch = new \Gulei\Arch\Param($xml);
    $ref = $Arch->getReference();

    $conf = array();
    foreach($ref as $fld_name => $val){
      $toks = explode('\\', $fld_name);
      $ptr = &$conf[array_shift($toks)];

      foreach($toks as $tok){
        unset($tmp);
        $tmp = &$ptr[$tok];
        unset($ptr);
        $ptr = &$tmp;
      }
      $ptr = $val['_param'];
    }
    return $conf;
  }
}