<?php
namespace Oerp\Purchase\Page;

class InputNote extends \Oerp\Page\Abstract_PeriodQuery {
  public function getClassname() {
    return __CLASS__;
  }

  protected function getTitle() {
    return 'Input Note';
  }

  protected function getProvider() {
    $param = $this->form_state['values'];
    return new \Oerp\Purchase\Provider\MoveHistoryByDay($param);
  }

  protected function getTreeArch() {
    $vals = $this->getStateValues();

    if ($vals['sum']) {
      $tree_arch_path = __DIR__ . '/InputNote/InputNote-sum.xml';
    }
    else {
      $tree_arch_path = __DIR__ . '/InputNote/InputNote.xml';
    }
    return file_get_contents($tree_arch_path);
  }

  protected function getSearchEle() {
    $search = parent::getSearchEle();
    $vals = $this->getStateValues();

    $search['date-type'] = array(
      '#title' => 'Date Type',
      '#type' => 'select',
      '#options' => array(
        'date_planned' => 'Pick Date',
        'date' => 'Input Date',
      ),
      '#default_value' => ($dval = $vals['date-type']) ? $dval : 'date',
      '#weight' => -1,
    );

    $search['supp'] = array(
      '#title' => 'Supplier',
      '#type' => 'textfield',
      '#default_value' => $vals['supp'],
      '#size' => '',
      '#weight' => 2.1,
    );

    $search['sum'] = array(
      '#title' => 'Show as accounting summary',
      '#type' => 'checkbox',
      '#default_value' => $vals['sum'],
      '#weight' => 2.2,
    );
    return $search;
  }
}