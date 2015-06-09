<?php
namespace Oerp\View;

class Tree extends \Gulei\Table\_Abstract
{
  protected function parseCri($cri){
    if(is_string($cri)){
      $cri = \Oerp\Form\Element\SearchPanel::decodeCri($cri);
    }

    $cri = is_array($cri) ? $cri : array();
    return $cri;
  }

  protected function initParam($eles)
  {
    //assign passed values
    $param = $eles['#param'];
    $param['criteria'] = $this->parseCri($param['criteria']);

    //query db if any needed value is missing
    if( !$param['vid'] && !$param['model'] && $param['arch']) {
      $param = $this->initParamNoVid($param);
    }
    else {
      $param = $this->initParamHasVid($param);
    }

    $param['key'] = ($param['key']) ? $param['key'] : $eles['#name'];

    if ($param['cri_url']) {
      $cri = $this->parseCri($param['cri_url']);
      $param['criteria'] = array_merge($param['criteria'], $cri);
    }
    return $param;
  }

  function __construct($eles)
  {
    $param = array_merge(
      $this->getDefaultParam(),
      $this->initParam($eles)
    );

    if(!$param['pager']){
      unset($param['items_per_page'], $param['page']);
    }

    parent::__construct(
      $param['arch'],
      new \Gulei\Provider\Arch($param['arch']),
      new \Oerp\Provider\TreeRecords($param),
      $param
    );

    $this->mods['hd']['raw'] = array(new \Oerp\Modifier\PackHeaderRaw());
    $this->mods['row']['raw'] = array(new \Oerp\Modifier\PackRowRaw());
  }

  protected function getFuncPrefix()
  {
    return array('GuFunc_', 'OerpFunc_');
  }

  protected function initParamHasVid($param)
  {
    $model = $param['oerp:model'] ? $param['oerp:model'] : $param['model'];

    if (!$param['fields'] || !$param['arch']) {
      $q = new \Oerp\Query\Basic($model);
      $view = $q->fields_view_get($param['vid'], 'tree');

      if (is_array($param['fields'])) {
        $param['fields'] = array_intersect($view['fields'], $param['fields']);
      }
      else {
        $param['fields'] = $view['fields'];
      }

      $param['arch'] = $view['arch'];
    }
    return $param;
  }

  protected function initParamNoVid($param)
  {
    $Arch = new \Gulei\Arch\Element($param['arch']);
    $OerpParam = $Arch->getNodeValues('/tree/param[@name]');

    if ($OerpParam['oerp:model']) {
      $param['model'] = $OerpParam['oerp:model'];
    }

    if ($cri = $OerpParam['oerp:criteria']) {
      $cri = $this->parseCri($cri);
      $param['criteria'] = array_merge($param['criteria'], $cri);
    }

    if ($Arch->xp('/tree/field/field[1]')->length) {
      $param['nested'] = true;
    }
    return $param;
  }

  protected function getFields()
  {
    $flds = array();
    foreach ($this->xp('/tree/field') as $fld) {
      $flds[] = $fld->getAttribute('name');
    }
    return $flds;
  }

  public function processElement($eles)
  {
    $eles['#obj'] = $this->save();
    $eles['#tree'] = true;
    $eles['json'] = array(
      '#type' => 'hidden',
      '#value' => '',
    );

    return $eles;
  }

  protected function getDefaultParam()
  {
    $param = array(
      'mark_translated' => true,
      'mark_invalid' => true,
      'mark_notice' => true,
      'mark_missing' => true,
      'copy_method' => 'ditto',
      'nl2br' => true,
      'items_per_page' => 80,
      'sticky_header' => true,
      'mark_translated' => false,
      'oerp:op-type' => 'CUD',
      'no-cache' => TRUE,
    );

    return array_merge(
      parent::getDefaultParam(),
      $param
    );
  }

  public function theme($eles, $param = null)
  {
    $param = $this->getParam($param);

    $rows = $this->getRows();
    $tb_class = 'gu-table oerp-treeview ' . $param['oerp:op-type'];
    $tb_class .= $this->getRowCount() > 1 ? '' : ' empty';
    $param['attrs']['class'] .= $tb_class;

    $pat = '
      <div class="oerp-view oerp-treeview" oerp_vid="%s" oerp_key="%s"
        oerp_model="%s" oerp_name="%s" cache_id="%s">

        <div class="caption">%s</div>
        %s
      </div>
    ';

    $model = ($param['oerp:model']) ? $param['oerp:model'] : $param['model'];

    $output = sprintf(
      $pat,
      '', $param['key'],
      $model, $param['name'], $param['cache_id'],
      $this->getArch()->getAttribute('string'),
      $eles['#children'] . parent::theme(null, $rows, $param)
    );

    $inc = new \Oerp\Includer\View();
    $inc->includeCss();
    $inc->includeJs();
    return $output;
  }

  protected function getSearchOption()
  {
    $q = new \Oerp\Query\Basic($this->getParam('model'));
    $def = $q->exec(
      array('fields_view_get', false, 'form', array())
    );

    $arch = new \Gulei\Arch\Element($def['arch']);
    $search = array();

    foreach ($arch->xp('//field[@select="1"]') as $field) {
      $name = $field->getAttribute('name');
      $search['basic'][] = $name;
    }

    foreach ($arch->xp('//field[@select="2"]') as $field) {
      $name = $field->getAttribute('name');
      $search['advanced'][] = $name;
    }
    return $search;
  }

  public function getSearchEntry()
  {
    $param = $this->getParam();
    $arch = new \Gulei\Arch\Element($param['arch']);
    $search = $this->getSearchOption();

    if (!$search['basic']) {
      return false;
    }

    $eles = array();
    $default = \Oerp\Form\Element\SearchPanel::decodeCri($param['cri_url'], true);

    if(isset($param['search-custom'])){
      $modes = array('custom');
      $search['custom'] = $param['search-custom'];
    }
    else if($param['search-advanced']){
      $modes = array('basic', 'advanced');
    }
    else{
      $modes = array('basic');
    }

    $defs = \Oerp\Query\Fields::getDef($this->getParam('model'));
    $fields_send = array();

    foreach($modes as $mode){
      foreach ($search[$mode] as $name) {
        if(isset($eles[$name])){
          continue;
        }

        $node = $arch->xp('/tree/field[@name="' . $name . '"][1]')->item(0);
        $node = $node ? $node : '<field name="' . $name . '"/>';

        if($sels = $defs[$name]['selection']){
          $sels_send = array();
          foreach($sels as $key => $sel){
            $sels_send[] = array($key, $sel);
          }
          $defs[$name]['selection'] = $sels_send;
        }

        $ele = \Oerp\Query\Fields::getSearchEle($defs[$name], $node, $default[$name]);
        $eles[$name] = $ele;

        $fields_send[$name] = $defs[$name];
      }
    }

    $entry['#type'] = 'oerp_searchpanel';
    $entry['#fields'] = $fields_send;
    $entry['#eles'] = $eles;
    return $entry;
  }
}