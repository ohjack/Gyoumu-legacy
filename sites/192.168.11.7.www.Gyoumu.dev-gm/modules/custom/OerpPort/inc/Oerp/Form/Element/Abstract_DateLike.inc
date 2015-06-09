<?php
namespace Oerp\Form\Element;

abstract class Abstract_DateLike extends _Abstract
{
  public function prepareSend()
  {
    $val = (string)$this->args['value'];
    return strlen(trim($val)) ? $val : false;
  }
}