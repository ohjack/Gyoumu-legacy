<?php
namespace Oerp\Installer\Field;

class Char extends \Oerp\Installer\Field\_Abstract
{
  protected function getParamMapping()
  {
    return array('name', 'field_description', 'model_id');
  }

  protected function getDefaultParam()
  {
    return array_merge(
      parent::getDefaultParam(),
      array('ttype' => 'char')
    );
  }
}