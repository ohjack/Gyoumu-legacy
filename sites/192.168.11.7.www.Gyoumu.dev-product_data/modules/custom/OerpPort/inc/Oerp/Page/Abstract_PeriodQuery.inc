<?php
namespace Oerp\Page;

abstract class Abstract_PeriodQuery extends \Gulei\Page\Form\_Abstract {
  protected $param;

  abstract protected function getProvider();
  abstract protected function getTreeArch();
  abstract protected function getTitle();

  public function getTableObj() {
    $param = array(
      'mark_translated' => FALSE,
    );
    return new \Gulei\Table\Arch($this->getTreeArch(), $this->getProvider(), $param);
  }

  protected function getStateValues() {
    $StateCtrl = new \Gulei\Form\State($this->form_state);
    return $StateCtrl->getVals();
  }

  protected function getSearchEle() {
    $vals = $this->getStateValues();

    $search = array(
      '#prefix' => '<h1>' . $this->getTitle() . '</h1><div id="search-panel">',
      '#suffix' => '</div>',
    );

    $search['date-from'] = array(
      '#title' => 'Date',
      '#type' => 'textfield',
      '#size' => '',
      '#default_value' => ($dval = $vals['date-from']) ? $dval : date('Y-m-d'),
      '#attributes' => array(
        'widget' => 'datepicker',
      ),
      '#weight' => 1,
    );

    $search['date-to'] = array(
      '#title' => 'To',
      '#type' => 'textfield',
      '#size' => '',
      '#default_value' => ($dval = $vals['date-to']) ? $dval : date('Y-m-d'),
      '#attributes' => array(
        'widget' => 'datepicker',
      ),
      '#weight' => 2,
    );

    $search['search'] = $this->getLocalCalleeEle(__CLASS__ . "\\Callee\\Search");

    $search['search']['#weight'] = 3;
    return $search;
  }

  protected function init() {
    parent::init();
    \Gulei\Includer\JS\Datepicker::inc();
    \Gulei\Util\Path::add_js(__DIR__ . '/Abstract_PeriodQuery/Abstract_PeriodQuery.js');
    \Gulei\Util\Path::add_css(__DIR__ . '/Abstract_PeriodQuery/Abstract_PeriodQuery.css');
  }

  protected function hook_form_internal() {
    $vals = $this->getStateValues();

    if (!$vals['date-from']) {
      $src = '';
      $this->param['hide'][] = 'toolbar-bottom';
    }
    else {
      $src = $this->getTableObj()->theme();
    }

    $form = array();

    $form['searchpanel'] = $this->getSearchEle();

    $form['src'] = array(
      '#type' => 'markup',
      '#value' => $src,
    );

    return $form;
  }

  protected function getToolbarBottom() {
    return $this->getLocalCalleeEle(array(__CLASS__ . "\\Callee\\Download_CSV"));
  }

  public function getParam() {
    return $this->param;
  }
}