<?php
namespace Oerp\View;

class Tree extends \Gulei\Table\_Abstract
{
  protected function initParam($eles)
  {
    //assign passed values
    $param = $eles['#param'];

    //query db if any needed value is missing
    if( !$param['vid'] && !$param['model'] && $param['arch']) {
      $param = $this->initParamNoVid($param);
    }
    else {
      $param = $this->initParamHasVid($param);
    }

    $param['key'] = ($param['key']) ? $param['key'] : $eles['#name'];

    $param['criteria'] = is_string($param['criteria'])
        ? json_decode($param['criteria']) : $param['criteria'];

    $param['criteria'] = is_array($param['criteria'])
        ? $param['criteria'] : array();

    if ($param['cri_url']) {
      $cri = \Oerp\Form\Element\SearchPanel::decodeCri($param['cri_url']);
      $cri = is_array($cri) ? $cri : array();

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

    if ($OerpParam['oerp:criteria']) {
      $param['criteria'] = $OerpParam['oerp:criteria'];
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
    return array(
      'mark_translated' => true,
      'mark_invalid' => true,
      'mark_notice' => true,
      'mark_missing' => true,
      'copy_method' => 'ditto',
      'nl2br' => true,
      'items_per_page' => 80,
      'sticky_header' => true,
      'mark_translated' => false,
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
      $pat, '', $param['key'],
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
    /*
    foreach($arch->xp('//field[@context]') as $field){
    $name = $field->getAttribute('name');
    $search['basic'][$name] = $def['fields'][$name];
    }

    foreach($def['fields'] as $name => $field){
    if($field['select'] == 1){
    $search['basic'][$name] = $field;
    }
    }*/

    foreach ($arch->xp('//field[@select="1"]') as $field) {
      $name = $field->getAttribute('name');
      $search['basic'][$name] = $def['fields'][$name];
    }

    foreach ($arch->xp('//field[@select="2"]') as $field) {
      $name = $field->getAttribute('name');
      $search['advanced'][$name] = $def['fields'][$name];
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

    foreach ($search['basic'] as $name => $fld_def) {
      $node = $arch->xp('/tree/field[@name="' . $name . '"][1]')->item(0);
      $node = $node ? $node : '<field name="' . $name . '"/>';

      $ele = \Oerp\Query\Fields::getSearchEle($fld_def, $node, $default[$name]);
      $eles[$name] = $ele;
    }

    $entry['#type'] = 'oerp_searchpanel';
    $entry['#fields'] = $search['basic'];
    $entry['#eles'] = $eles;
    return $entry;
  }
}