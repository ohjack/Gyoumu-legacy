<?php
namespace Oerp\Form\Element;

class Datetime extends Abstract_DateLike
{
  protected function getPrepareSendJs()
  {
    return <<<JS
return {
  '#name' : fld['name'],
  '#type' : 'datetime',
  '#value' : fld['value']
};
JS;
  }

  protected function getEntryTemplate()
  {
    $ele['#type'] = 'textfield';
    $ele['#default_value'] = $this->args['default'];
    $ele['#attributes']['widget'] = 'datetimepicker';
    $ele['#attributes']['prepareSend'] = $this->getPrepareSendJs();
    return $ele;
  }
}