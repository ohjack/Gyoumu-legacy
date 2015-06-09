<?php
namespace Yauc\Lister\Page;

class Edit extends \Gulei\Page\Form\_Abstract
{
  static $IGNORES = array(
    'id', 'sellerId', 'status', 'msg', 'createtime', 'anchor', 'title_send', 'pic_path'
  );

  protected function getClassname()
  {
    return __CLASS__;
  }

  protected function getParam()
  {
    return array();
  }

  protected function hook_form_internal()
  {
    $q = new \Yauc\Lister\Query\Queue($_GET['rid']);
    $data = $q->getData();
    $form = array();

    if($data['msg']){
      $form['msg'] = array(
        '#type' => 'markup',
        '#value' => sprintf('<div class="messages error">%s</div>', $data['msg']),
      );
    }

    $form['data'] = array(
      '#type' => 'value',
      '#value' => $data,
    );

    foreach($data as $name => $val){
      if(in_array($name, self::$IGNORES, true)){
        continue;
      }

      if(is_string($val)){
        $type = strpos($val, "\n") !== false ? 'textarea' : 'textfield';

        $form['_' . $name] = array(
          '#title' => $name,
          '#type' => $type,
          '#default_value' => $val
        );
      }
    }

    return $form;
  }

  protected function getToolbarBottom()
  {
    $calls = array('Save', 'Delete');
    return $this->getLocalCalleeEle($calls);
  }
}