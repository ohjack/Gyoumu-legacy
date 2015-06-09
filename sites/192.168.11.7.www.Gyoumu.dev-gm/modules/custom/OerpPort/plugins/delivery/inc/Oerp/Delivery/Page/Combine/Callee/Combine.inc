<?php
namespace Oerp\Delivery\Page\Combine\Callee;

class Combine extends \Gulei\Page\Form\Calee\_Abstract
{
  protected function getClassname()
  {
    return __CLASS__;
  }

  private function internal_combine($sels, $main){
    $model = 'stock.picking';

    $q = new \Oerp\Query\Basic(
      $model, null, array('note', 'move_lines', 'x_cust_val'));

    $sels = array_filter($sels, create_function('$var', "
      return \$var !== $main;
    "));

//  prepare main rec
    $main_q = new \Oerp\Query\Basic(
      $model, null, array('x_cust_val'), $main);

    $main_rec = (array)$main_q->raw();
    $main_rec = $main_rec[0];
    unset($main_rec['id']);

//  combine move_lines
    foreach($sels as $sel){
      $q->setIds($sel);
      $rec = (array)$q->raw();
      $rec = $rec[0];
      unset($rec['id']);

      $main_rec['x_cust_val'] += $rec['x_cust_val'];

      foreach($rec['move_lines'] as &$line){
        $line = array(
          1,
          $line,
          array('picking_id' => $main),
        );
      }

      $q->write($rec);
    }

    $main_q->write($main_rec);

    foreach($sels as $sel){
      $q->setIds($sel);
      $q->unlink();
    }
  }

  public function execute($form, &$form_state)
  {
    $vals = $this->FormPage->getValues();
    $group = array();

    foreach($vals as $key => $pid){
      if(preg_match('/^pid-(.*)/', $key, $m) && $pid){
        if(isset($group[$pid])){
          $group[$pid][] = $m[1];
        }
        else{
          $group[$pid] = array($m[1]);
        }
      }
    }

    foreach($group as $pid => $childs){
      $this->internal_combine($childs, $pid);
    }
  }
}