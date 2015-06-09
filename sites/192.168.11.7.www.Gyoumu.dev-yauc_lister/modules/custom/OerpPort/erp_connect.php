<?php
//_erp_connect();
_erp_connect(null, 'maolung_bak');

function _erp_connect($srv = null, $db = null, $usr = null, $pwd = null)
{
  $pack = array();
  $pack['srv'] = is_null($srv) ? variable_get('oerpport_server', '') : $srv;
  $pack['db'] = is_null($db) ? variable_get('oerpport_db', '') : $db;
  $pack['usr'] = is_null($usr) ? variable_get('oerpport_uname', '') : $usr;
  $pack['pwd'] = is_null($pwd) ? variable_get('oerpport_pass', '') : $pwd;

//  $tmp = new OerpObject('');
  $tmp = new \Oerp\Query\Basic('');
  $tmp->connect(
    $pack['srv'], $pack['db'], $pack['usr'], $pack['pwd']
  );
  unset($tmp);
}