<?php
namespace Oerp\Hook\JS;

class Wizard extends \Oerp\Hook\JS\_Abstract {
  public function getWeight() {
    return self::WT_WIZARD;
  }

  public function applicable() {
    return $this->post['service'] == 'wizard';
  }

  public function exe() {
    $send = $this->send;

    $value = (array)$send->value;
    $value = $value['#value'];
    $value = \Oerp\Util\Helper::renderNestedInput($value);
    $datas = (array)json_decode($send->datas);

    $form = array_merge($datas, $value);

    $resp = \Oerp\Query\Wizard::_execute(
      array(
        (int)$send->wid,
        array(
          'model' => $send->model,
          'form' => $form,
          'id' => (int)$send->rid,
          'ids' => array(((int)$send->rid))
        ),
        $send->action
      )
    );
    return drupal_json($resp);
  }
}