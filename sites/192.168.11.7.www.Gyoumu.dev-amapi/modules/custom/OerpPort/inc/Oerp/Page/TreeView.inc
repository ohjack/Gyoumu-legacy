<?php
namespace Oerp\Page;

class TreeView extends \Gulei\Page\Form\Basic
{
  private $search_path = array('view', '_search');

  public function hook_form(&$form_state, $vid, $param = array())
  {
    $this->redirect($form_state);

    $param = array_merge(
      array('oerp:op-type' => 'edit'),
      $param
    );

    $form = array();
    $form['#attributes'] = array('class' => 'oerp-formview');

    $param['vid'] = $vid;

    if ($param['vid'] && !$param['model']) {
      $q = new \Oerp\Query\Basic('ir.ui.view', null, array('model'), $vid);
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

    $form['func:submit'] = array(
      '#type' => 'submit',
      '#value' => 'Save',
      '#name' => 'func:submit',
    );

    $form['func:downloadCsv'] = array(
      '#type' => 'submit',
      '#value' => 'Download',
      '#name' => 'func:downloadCsv',
    );

    return $form;
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

    $this->goBack(false, $this->search_path);
  }

  protected function proc_view__search_func_clear()
  {
    $this->goBack();
  }

  protected function proc_func_submit($form, &$form_state)
  {
    $recs = \Oerp\Util\Helper::jsondecode(
      $form_state['clicked_button']['#post']['view']['json']);

    $recs = \Oerp\Util\Helper::renderNestedInput($recs);
    $param = $form['view']['#obj']->renew()->getParam();

    foreach ($recs as $rec) {
      $q = new \Oerp\Query\Basic($param['model'], null, null, array($rec[1]));

      switch ($rec[0]) {
        case 0: //create
          $q->create($rec[2]);
          break;

        case 1: //edit
          $q->write($rec[2]);
          break;

        case 2: //delete
          $q->unlink();
          break;
      }
    }
    $this->goBack();
  }

  protected function proc_func_downloadCsv($form, &$form_state)
  {
    $obj = $form['view']['#obj']->renew();
    $fname = 'Treeview-' . str_replace('.', '_', $obj->getParam('oerp:model'));

    $param = array(
      'oerp:op-type' => null,
    );

    $form_state['storage']['redirect'] = $obj->getCsvFile($param, $fname);
  }
}