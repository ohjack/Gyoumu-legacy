<?php
namespace Oerp\Page;

class TreeView extends \Gulei\Page\Form\_Abstract
{
  private $search_path = array('view', '_search');

  protected function getClassname()
  {
    return __CLASS__;
  }

  private function getViewProp()
  {
    $vid = $_GET['vid'];
    if(is_numeric($vid)){
      return $vid;
    }

    if(!$vid){
      return false;
    }

    $q = new \Oerp\Query\Basic('ir.ui.view', array(array('name', '=', $vid)), array('id'));
    $recs = (array)$q->raw();

    if(!$id = $recs[0]['id']){
      throw \Gulei\Exception\_Throw::msg('vid not found : ' . $vid);
    }

    $v = new \stdClass();
    $v->name = $vid;
    $v->id = $id;
    return $v;
  }

  protected function getParam()
  {
    list($vid, $param_over) = $this->args;
    $param = array('oerp:op-type' => 'edit');
    $param = is_array($param_over) ? array_merge($param, $param_over) : $param;
    return $param;
  }

  protected function hook_form_internal()
  {
    $this->redirect($this->form_state);
    $param = $this->getParam();

    $form = array();
    $form['#attributes'] = array('class' => 'oerp-formview');

    $v = $this->getViewProp();
    $param['vid'] = $v->id;
    $param['vname'] = $v->name;

    if ($param['vid'] && !$param['model']) {
      $q = new \Oerp\Query\Basic('ir.ui.view', null, array('model'), $param['vid']);
      $rec = $q->raw();
      $param['model'] = $rec[0]['model'];
    }

    $param['searchpanel'] = true;
    $param['cri_url'] = $param['cri_url'] ? $param['cri_url'] : $_GET['cri'];

    $search = $_GET;
    unset($search['q'], $search['page']);

    if (!isset($param['pager'])) {
      $param['pager'] = true;
    }

    $form['view'] = array(
      '#type' => 'oerp_treeview',
      '#param' => $param,
    );

    $hooks = module_invoke_all('oerp_treeview_process');
    if($procs = $hooks[$param['vname']]['pre']){
      foreach($procs as $proc) {
        $proc->execute($param);
      }
    }
    return $form;
  }

  protected function getToolbarBottom()
  {
    $calls = array('Save', 'Download_CSV');
    return $this->getLocalCalleeEle($calls);
  }

  protected function proc_view__search_func_find($form, &$form_state)
  {
    $search = $form_state['values']['view'];
    $eleSearch = $form['view'];

    if (!\Oerp\Form\Element\SearchPanel::submit(
      $form, $form_state, $eleSearch, $search)
    ) {
      return;
    }

    $this->goBack(false);
  }

  protected function proc_view__search_func_clear()
  {
    $query = array(
      'n' => $_GET['n'],
      'vid' => $_GET['vid']
    );
    return drupal_goto($_GET['q'], http_build_query($query, '', '&'));
  }
}