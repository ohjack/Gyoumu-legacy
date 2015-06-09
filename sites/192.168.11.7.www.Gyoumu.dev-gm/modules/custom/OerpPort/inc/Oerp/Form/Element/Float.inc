<?php
namespace Oerp\Form\Element;

class Float extends Abstract_TextfieldLike {
  protected function getPrepareSendJs()
  {
    return <<<JS
return {
  '#name' : fld['name'],
  '#type' : 'float',
  '#value' : fld['value']
};
JS;
  }
}