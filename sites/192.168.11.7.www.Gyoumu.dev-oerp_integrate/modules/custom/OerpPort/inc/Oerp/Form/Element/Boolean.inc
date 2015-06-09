<?php
namespace Oerp\Form\Element;

class Boolean extends _Abstract
{
  public function prepareSend()
  {
    return (bool)$this->args['value'];
  }

  protected function getPrepareSendJs()
  {
    return <<<JS
return {
  '#name' : fld['name'],
  '#type' : 'boolean',
  '#value' : fld['value']
};
JS;
  }

  protected function getEntryTemplate()
  {
    $ele['#type'] = 'checkbox';
    $ele['#default_value'] = $this->args['default'];
    $ele['#attributes'] = array('prepareSend' => $this->getPrepareSendJs());
    return $ele;
  }
}