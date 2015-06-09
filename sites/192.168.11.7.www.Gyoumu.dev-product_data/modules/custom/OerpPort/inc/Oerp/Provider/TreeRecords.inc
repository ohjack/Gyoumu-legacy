<?php
namespace Oerp\Provider;

class TreeRecords extends \Gulei\Provider\_Abstract
{
  public function getReference()
  {
    return array();
  }

  public function overrideParam($param) {
    $this->param = array_merge($this->param, $param);
  }

  public function getData()
  {
    $param = $this->param;

    if ($param['nested'] || $param['arch']) {
      $recs = $this->_getRecordsNested($param);
    }
    else {
      $recs = $this->_getRecords($param);
    }

    if(!$param['no-blank-tail']){
      $fld_names = $this->getArchFieldNames($param);
      $rec = array_fill_keys($fld_names, ' ');
      $rec['id'] = 0;
      $recs[] = $rec;
    }
    return $recs;
  }

  public function count()
  {
    $q = new \Oerp\Query\Basic(
      $this->param['model']
    );

    return $q->exec(array('search_count', $this->param['criteria']));
  }

  private function _getRecords($param)
  {
    $def = $param['fields'];
    foreach ($def as &$each) {
      if ($each['type'] == 'selection') {
        $opts = $each['selection'];
        $each['selection'] = array();

        foreach ($opts as $opt) {
          $each['selection'][$opt[0]] = $opt[1];
        }
      }
    }

    $param['page'] = $_GET['page'] ? $_GET['page'] : 0;

    if ($param['ids'] == array(0)) {
      $recs = array();
    }
    else {
      $q = new \Oerp\Query\Basic(
        $param['model'],
        $param['criteria'],
        array_keys($param['fields']),
        $param['ids']
      );

      $recs = (array)$q->query(
        array(
             'offset' => $param['page'] * $param['items_per_page'],
             'limit' => $param['items_per_page'],
        )
      );

      foreach ($recs as &$rec) {
        foreach ($rec as $name => &$cell) {
          switch ($def[$name]['type']) {
            case 'selection':
              $cell = $def[$name]['selection'][$cell];
              break;

            case 'many2one':
              $cell = $cell[1];
              break;

            case 'one2many':
            case 'many2many':
              $cell = implode("\n", $cell);
              break;
          }
        }
      }
    }

    return $recs;
  }

  protected function getArchFieldNames($param) {
    $Ele = new \Gulei\Arch\Element($param['arch']);
    return array_keys($Ele->extractAttr('name'));
  }

  private function _getRecordsNested($param)
  {
    $q = new \Oerp\Query\Nested($param['arch'], $param);
    $flags = is_array($param['flag']) ? $param['flag'] : array();
    $sum = $q->sum($flags);

    if($sum){
      $this->recs = $sum->supplement(
        $this->getArchFieldNames($param), $param['copy_method']);
    }
    else{
      $this->recs = array();
    }

    return $this->recs;
  }

  public function getArch()
  {
    return $this->param['arch'];
  }
}