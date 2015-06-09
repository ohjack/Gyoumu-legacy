<?php
namespace Oerp\Form\Element;

class Factory
{
  static public function createElement($args)
  {
    $type = $args['field']['type'];

    if(!$type){
      throw new \Exception('Unknown type: ' . $type);
    }

    $classname = "Oerp\\Form\\Element\\" . ucfirst($type);

    $Ele = new $classname($args);
    return $Ele;
  }

  static public function prepareSend($args)
  {
    $Ele = self::createElement($args);
    return $Ele->prepareSend();
  }
}