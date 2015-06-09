<?php
namespace Oerp\Query;

class Wizard extends \Oerp\Query\Basic
{
  static public function _execute($args, $action = 'execute')
  {
    return parent::_execute($args, $action, 'wizard');
  }
}