<?php
namespace Oerp\Delivery\Provider\Tree;

class Draft extends \Oerp\Provider\TreeRecords
{
  public function __construct($param = array()){
    $param['arch'] = file_get_contents(__DIR__ . '/Draft.xml');
    parent::__construct($param);
  }
}