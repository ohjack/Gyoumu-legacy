<?php 

define('OERP_WRITE_CREATE', 0);
define('OERP_WRITE_EDIT', 1);
define('OERP_WRITE_DEL', 2);

class OerpQueryWizard extends \Oerp\Query\Basic
{
  static public function _execute($args, $action = 'execute')
  {
    return parent::_execute($args, $action, 'wizard');
  }
}