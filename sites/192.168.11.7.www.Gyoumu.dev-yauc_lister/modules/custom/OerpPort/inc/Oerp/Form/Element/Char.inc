<?php
namespace Oerp\Form\Element;

class Char extends Abstract_TextfieldLike
{
  protected function getPrepareSendJs()
  {
    return <<<JS
return {
  '#name' : fld['name'],
  '#type' : 'char',
  '#value' : fld['value']
};
JS;
  }
}