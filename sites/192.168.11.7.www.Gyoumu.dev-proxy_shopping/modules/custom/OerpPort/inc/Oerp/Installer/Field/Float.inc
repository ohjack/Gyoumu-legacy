<?php
namespace Oerp\Installer\Field;

class Float extends \Oerp\Installer\Field\_Abstract
{
  protected function getParamMapping()
  {
    return array('name', 'field_description', 'model_id');
  }

  protected function getDefaultParam()
  {
    return array_merge(
      parent::getDefaultParam(),
      array('ttype' => 'float')
    );
  }
}