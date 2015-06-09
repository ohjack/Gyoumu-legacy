<?php
namespace Oerp\Table;

class Basic extends \Gulei\Table\Select
{
  private $q;

  function __construct($arch_src, $param = array()){
    $param['oerp:cri_ex'] = $_GET['cri'];

    parent::__construct($arch_src, null, $param);
    $this->q = new \Oerp\Query\Nested($arch_src, $param);
  }

  protected function getFuncPrefix()
  {
    return array_merge(array('OerpFunc_') , parent::getFuncPrefix());
  }

  function getDefaultParam(){
    return array(
      'merge' => true,
      'copy_method' => 'ditto',
      'mark_invalid' => true,
    );
  }

  public function refineQuery($args){
    $this->q->refineQuery($args);
  }

  protected function checkFuncName($src, $prefix = 'OerpFunc_'){
    $func_name = parent::checkFuncName($src, $prefix);

    if($func_name === false){
      return parent::checkFuncName($src);
    }
    else{
      return $func_name;
    }
  }

  public function getRecords($recs = null, $param = array()){
    $param = $this->getParam(null, $param);

    if($recs = parent::getRecords($recs, $param)){
      return $recs;
    }

    $sum = $this->q->sum();

    if($sum){
      $this->recs = $sum->supplement(
        array_keys($this->extractAttr('name')), $param['copy_method']
      );
    }else{
      $this->recs = array();
    }

    return $this->recs;
  }

  public function getHeaders($hds = null, $param = array()){
    $param = $this->getParam(null, $param);
    $hds = parent::getHeaders($hds, $param);
    return $hds;
  }

  public function getFormEles(){
    $param = $this->getParam();
    $eles = parent::getFormEles();

    if($param['search']){
      $defs = \Oerp\Query\Fields::getDef($param['oerp:model']);

      $eleSearch = array(
        '#type' => 'oerp_searchpanel',
      );

      $defaults = \Oerp\Form\Element\SearchPanel::decodeCri($param['oerp:cri_ex'], true);

      foreach($this->xp('./field[@select="1"]', $this->getArch()) as $select){
        $name = $select->getAttribute('name');

        $fld = array(
          'type' => $defs[$name]['type'],
          'string' => $defs[$name]['string'],
        );

        $eleSearch['#fields'][$name] = $fld;
        $eleSearch['#eles'][$name] = \Oerp\Query\Fields::getSearchEle(
          $fld, $this->dump($select), $defaults[$name]);
      }

      $eles['_search'] = $eleSearch;
    }
    return $eles;
  }

  protected function getSelectionValue($rec, $ind){
    if($rec['data']['id']['data']){
      return $rec['data']['id']['data'];
    }
    else{
      return $rec['id'];
    }
  }

  public function theme($eles = null, $hds = null, $rows = null, $param = array()){
    $param = array_merge(array('id' => true), $param);
    $param = $this->getParam(null, $param);

    $hds = $this->getHeaders($hds, $param);

    $param['attrs']['class'] .= ' oerp-treeview';

    $output = '';

    if($param['search']){
      $Src = new \Gulei\Arch\Element('<root>' . $eles['#children'] . '</root>');
      $output .= $Src->dump(
        $Src->xp('/root/div[@class="searchpanel"][1]')->item(0));
    }

    $output .= parent::theme($eles, $hds, $rows, $param);
    return $output;
  }

  public function getRecordsCount(){
    return $this->q->getCount();
  }

  protected function countRows()
  {
    $c = 0;
    foreach($this->rows as $row){
      if(!$row['merged']){
        $c++;
      }
    }
    return $c;
  }
}