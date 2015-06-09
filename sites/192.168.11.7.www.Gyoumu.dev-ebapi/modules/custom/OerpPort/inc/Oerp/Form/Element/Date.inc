<?php
namespace Oerp\Form\Element;

class Date extends Abstract_DateLike
{
  protected function getPrepareSendJs()
  {
    return <<<JS
return {
  '#name' : fld['name'],
  '#type' : 'date',
  '#value' : fld['value']
};
JS;
  }

  protected function getEntryTemplate()
  {
    $ele['#type'] = 'textfield';
    $ele['#default_value'] = $this->args['default'];
    $ele['#attributes']['widget'] = 'datepicker';
    $ele['#attributes']['prepareSend'] = $this->getPrepareSendJs();
    return $ele;
  }
}
