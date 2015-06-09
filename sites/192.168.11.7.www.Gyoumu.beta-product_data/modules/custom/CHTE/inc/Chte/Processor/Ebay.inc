<?php
namespace Chte\Processor;

class Ebay extends Record
{
  public function getRecords()
  {
    foreach ($this->input as &$each_input) {
      $each_input['CustomLabel'] = $each_input['Custom Label'];
    }
    return parent::getRecords();
  }
}