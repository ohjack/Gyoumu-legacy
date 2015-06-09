<?php
namespace Oerp\Form\Element;

abstract class Abstract_TextfieldLike extends _Abstract
{
  public function prepareSend()
  {
    return (string)$this->args['value'];
  }

  protected function getEntryTemplate()
  {
    $ele['#type'] = 'textfield';
    $ele['#size'] = '';
    $ele['#default_value'] = (string)$this->args['default'];
    $ele['#attributes'] = array('prepareSend' => $this->getPrepareSendJs());
    return $ele;
  }
}