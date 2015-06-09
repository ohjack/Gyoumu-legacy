<?php

include_once('view.inc');

class OerpActWindow
{
  protected $menu_id;
  protected $menu_def;
  protected $view_def = array();
  protected $search;

  function __construct($menu_id, $search)
  {
    $resp = \Oerp\Query\Basic::_execute(
      array('ir.values', 'get', 'action', 'tree_but_open', array(array('ir.ui.menu', (int)$menu_id)), false)
    );

    $this->menu_def = $resp[0];

    $view = $this->menu_def[2]['views'][0];
    $vid = $view[0] ? (int)$view[0] : false;

    $this->view_def[] = \Oerp\Query\Basic::_execute(
      array(
           $this->menu_def[2]['res_model'],
           'fields_view_get', $vid, $view[1], array()
      )
    );
    dpr($this->view_def);
    $this->search = $search;
  }

  protected function getSearchOption()
  {
    $recp = \Oerp\Query\Basic::_execute(
      array(
           $this->menu_def[2]['res_model'],
           'fields_view_get', false, 'form', array()
      )
    );

    $arch = new \Gulei\Arch\Element($recp['arch']);

    $search = array();

    foreach($this->view_def[0]['fields'] as $name => $field){
      switch($field['select']){
        case '1':
          $search['basic'][$name] = $field;
        break;

        case '2':
          $search['advanced'][$name] = $field;
        break;
      }
    }

    foreach ($arch->xp('//field[@select="1"]') as $field) {
      $name = $field->getAttribute('name');
      $search['basic'][$name] = $recp['fields'][$name];
    }
    foreach ($arch->xp('//field[@select="2"]') as $field) {
      $name = $field->getAttribute('name');
      $search['advanced'][$name] = $recp['fields'][$name];
    }
    return $search;
  }

  public function getSearchEntry($ind)
  {
    $arch = new \Gulei\Arch\Element($this->view_def[$ind]['arch']);
    $search = $this->getSearchOption();

    $entry = array(
      '#type' => 'oerp_searchpanel',
    );

    $eles = array();
    foreach ($search['basic'] as $name => $fld_def) {
      $node = $arch->xp('/tree/field[@name="' . $name . '"][1]')->item(0);
      $node = $node ? $node : '<field name="' . $name . '"/>';
      
      $ele = \Oerp\Query\Fields::getSearchEle(
        $fld_def, $node, $this->search[$name]
      );
      
      $eles[$name] = $ele;
    }

    $entry['#fields'] = $search['basic'];
    $entry['#eles'] = $eles;
    return $entry;
  }

  protected function getCri($ind)
  {
    if (!is_array($this->search)) {
      return null;
    }

    $def = $this->view_def[$ind]['fields'];
    $cri = array();

    foreach ($this->search as $name => $kval) {
      if (strpos($name, 'func:') === 0) {
        continue;
      }

      switch ($def[$name]['type']) {
        case 'char':
          $cri[] = array($name, 'ilike', $kval);
          break;

        case 'int':
          $cri[] = array($name, '=', (int)$kval);
          break;

        case 'float':
          break;

        case 'boolean':
          $cri[] = array($name, '=', !!$kval);
          break;

        case 'many2one':
          if ($kval['rid']) {
            $cri[] = array($name, '=', (int)$kval);
          }
          break;
      }
    }
    return $cri;
  }

  public function getViewEntry($ind)
  {
    $param = $this->view_def[$ind];

    $entry = array();
    switch ($param['type']) {
      case 'tree':
        $param['oerp:op-type'] = 'edit';
        $param['criteria'] = $this->getCri($ind);
        $param['pager'] = true;
        $param['page'] = $_GET['page'];
        $param['items_per_page'] = $this->menu_def[2]['limit'];

        $entry['#type'] = 'oerp_treeview';
        $entry['#param'] = $param;
        break;
    }
    return $entry;
  }
}

function oerp_action_form(&$form_state, $menu_id)
{
  $act = new OerpActWindow($menu_id, $form_state['storage']['search']);

  $form = array();
  $form['#tree'] = true;
  $form['search'] = $act->getSearchEntry(0);
  $form['view'] = $act->getViewEntry(0);
  return $form;
}

function oerp_action_form_submit($form, &$form_state)
{
  $form_state['storage']['search'] = $form_state['values']['search'];
}