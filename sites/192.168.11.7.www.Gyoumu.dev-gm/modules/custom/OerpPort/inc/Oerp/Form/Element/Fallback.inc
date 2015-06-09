<?php
namespace Oerp\Form\Element;

class Fallback extends _Abstract
{
  public function prepareSend()
  {
    return null;
  }

  protected function getPrepareSendJs()
  {
    return <<<JS
return {
  '#name' : fld['name'],
  '#type' : 'fallback',
  '#value' : fld['value']
};
JS;
  }

  protected function getEntryTemplate()
  {
    $ele['#type'] = 'textfield';
    $ele['#default_value'] = $this->args['field']['type'];
    $ele['#attributes']['readonly'] = 'readonly';
    return $ele;
  }
}