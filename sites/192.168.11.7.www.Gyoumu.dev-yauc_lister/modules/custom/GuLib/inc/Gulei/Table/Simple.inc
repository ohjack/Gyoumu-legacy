<?php
namespace Gulei\Table;

class Simple extends \Gulei\Table\_Abstract {
  public function __construct($hds, $recs, $param = array()) {
    $provHdr = new \Gulei\Provider\Lambda(array($hds));
    $provRec = new \Gulei\Provider\Lambda($recs);
    
    $arch = '';
    foreach($hds as $key => $hd){
      $arch .= sprintf('<field name="%s" string="%s"/>', $key, $hd);
    }
    $arch = '<tree>' . $arch . '</tree>';

    parent::__construct($arch, $provHdr, $provRec, $param);
  }

  protected function getDefaultParam() {
    return array();
  }

  protected function getFuncPrefix() {
    return array('GuFunc_');
  }
}