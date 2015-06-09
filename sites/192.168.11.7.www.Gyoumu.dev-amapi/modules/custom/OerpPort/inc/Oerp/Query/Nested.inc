<?php
namespace Oerp\Query;

class Nested extends Arch
{
  private function getFlatValue($flags)
  {
    $flags = array_merge(
      array(
           'limit' => $this->limit,
           'offset' => $this->offset,
      ),
      $flags
    );

    return $this->q->query($flags);
  }

  private function getInheritFlags($flags)
  {
    $inherit_mask =
        array_fill_keys(array('meta', 'id_only', 'props'), null);

    return array_intersect_key($flags, $inherit_mask);
  }

  private function processWrapperField($val, $eleFld, $flags)
  {
    $inherit_flags = $this->getInheritFlags($flags);
    $ttype = $eleFld->getAttribute('ttype');
    $name = $eleFld->getAttribute('name');

    $ids = array();
    for ($c = 0; $c < count($val); $c++) {
      switch ($ttype) {
        case 'one2many':
          $ids = array_merge($ids, $val[$c][$name]);
          break;

        case 'many2one':
          $ids[] = $val[$c][$name][0];
          break;
      }
    }

    $ids = array_filter($ids);
    $ids = array_values($ids);

    if ($ids) {
      $q = new \Oerp\Query\Nested($this->dump($eleFld), array('oerp:ids' => $ids));
      $ref = $q->sum($inherit_flags)->bunch('id', !!$flags['include_id']);
    }
    else {
      $ref = array();
    }

    switch ($ttype) {
      case 'one2many':
        $val->linkMany($name, $ref);
        break;
      case 'many2one':
        $val->link($name, $ref);
        break;
    }
  }

  private function processFlatField($val, $eleFld)
  {
    $name = $eleFld->getAttribute('name');
    $ttype = $eleFld->getAttribute('ttype');

    switch ($ttype) {
      case 'one2many':
      case 'many2many':
        foreach ($val as &$rec) {
          $rec[$name] = implode('<br />', $rec[$name]);
        }
        break;
      case 'many2one':
        foreach ($val as &$rec) {
          if (is_array($rec[$name])) {
            $rec[$name] = $rec[$name][1];
          }
        }
        break;
    }
  }

  private function prepareSelectionOptions($val)
  {
    $defs = \Oerp\Query\Fields::getDef($this->q->getModel());

    foreach ($val as &$rec) {
      foreach ($rec as $name => &$cell) {
        if ($defs[$name]['type'] == 'selection') {
          $cell = $defs[$name]['selection'][$cell];
        }
      }
    }
  }

  public function sum($flags = array())
  {
    $val = $this->getFlatValue($flags);

    if (!count($val)) {
      return new \Oerp\Util\_Array();
    }

    foreach ($this->getFieldEleList() as $eleFld) {
      if ($this->isWrapperField($eleFld)){
        $this->processWrapperField($val, $eleFld, $flags);
      }
      else {
        $this->processFlatField($val, $eleFld);
      }
    }

    $this->prepareSelectionOptions($val);
    return $val;
  }

  public function getCount()
  {
    return count($this->getFlatValue(array('id_only', 'limit' => false)));
  }
}