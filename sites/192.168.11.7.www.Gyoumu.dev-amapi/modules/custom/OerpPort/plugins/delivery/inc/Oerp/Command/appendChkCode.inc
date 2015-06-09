<?php
namespace Oerp\Command;

class appendChkCode extends \Gulei\Command\WithReference\_Abstract
{
  function execute($refVar = null){
    list($rec, $name) = $this->ref->vals('rec', 'name');
    $val = $rec[$name];

    preg_match("/^PACK(\d*)/", $val, $m);

    if($m[1]){
      return $val . chr((int)$m[1] % 26 + 65);
    }
    return false;
  }
}

