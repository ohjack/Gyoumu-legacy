<?php
namespace Oerp\Form\Element;

class Text extends _Abstract
{
  public function prepareSend()
  {
    return (string)$this->args['value'];
  }

  protected function getPrepareSendJs()
  {
    return <<<JS
return {
  '#name' : fld['name'],
  '#value' : fld['value'],
  '#type' : 'text'
};
JS;
  }

  protected function getEntryTemplate()
  {
    $text = (string)$this->args['default'];
    $ele['#type'] = 'textarea';
    $ele['#default_value'] = $text;
    $ele['#attributes'] = array('prepareSend' => $this->getPrepareSendJs());
    return $ele;
  }
}