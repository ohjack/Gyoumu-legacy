<?php
namespace Oerp\Form\Element;

class Selection extends _Abstract
{
  public function prepareSend()
  {
    return (string)$this->args['value'];
  }

  protected function getPrepareSendJs()
  {
    return <<<JS
var ele_txt = ele
  .children('option[value="' + fld['value'] + '"]').eq(0)
  .text();

return {
  '#name' : fld['name'],
  '#text' : ele_txt,
  '#value' : fld['value'],
  '#type' : 'selection'
};
JS;
  }

  protected function getEntryTemplate()
  {
    $ele['#type'] = 'select';
    $opts = array();

    foreach ($this->args['field']['selection'] as $key => $opt) {
      if (is_array($opt)) {
        $opts[$opt[0]] = $opt[1];
      }
      else {
        $opts[$key] = $opt;
      }
    }

    $ele['#options'] = $opts;
    $ele['#default_value'] = $this->args['default'];
    $ele['#attributes'] = array('prepareSend' => $this->getPrepareSendJs());
    return $ele;
  }
}
