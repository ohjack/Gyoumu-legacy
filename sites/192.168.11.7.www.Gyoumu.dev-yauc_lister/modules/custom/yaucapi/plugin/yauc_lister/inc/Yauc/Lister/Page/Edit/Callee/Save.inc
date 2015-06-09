<?php
namespace Yauc\Lister\Page\Edit\Callee;

class Save extends \Gulei\Page\Form\Callee\_Abstract
{
  protected function getClassname()
  {
    return __CLASS__;
  }

  public function execute($form, &$form_state)
  {
    $vals = $form_state['values'];
    $send = array();

    foreach($vals as $name => $val){
      if(strpos($name, '_') !== 0){
        continue;
      }

      $fldname = substr($name, 1);
      $send[$fldname] = $val;
    }

    $send = array_merge($vals['data'], $send);
    $send['status'] = \Yauc\Lister\Provider\_Abstract::Q_NEW;
    $send['msg'] = '';
    unset($send['category']);

    $q = new \Yauc\Lister\Query\Queue($vals['data']['id']);
    $resp = $q->save($send);

    if($resp){
      drupal_set_message('Data saved.');
    }

    drupal_goto('yaucapi/auction/queue/edit', array('rid' => $vals['data']['id']));
  }
}