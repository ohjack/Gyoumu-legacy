<?php
namespace Oerp\Form\Element;

class Integer extends Abstract_TextfieldLike {
  protected function getPrepareSendJs()
  {
    return <<<JS
return {
  '#name' : fld['name'],
  '#type' : 'integer',
  '#value' : fld['value']
};
JS;
  }
}