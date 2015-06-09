<?php
namespace Oerp\Delivery\Provider\Tree;

class SPackCR extends \Oerp\Provider\TreeRecords
{
  public function __construct($param = array()){
    $param['arch'] = file_get_contents(__DIR__ . '/SPackCR.xml');
    parent::__construct($param);
  }
}