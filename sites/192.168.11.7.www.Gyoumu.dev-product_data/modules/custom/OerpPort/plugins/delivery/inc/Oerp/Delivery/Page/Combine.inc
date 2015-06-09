<?php
namespace Oerp\Delivery\Page;

class Combine extends \Gulei\Page\Form\_Abstract
{
  protected function getClassname()
  {
    return __CLASS__;
  }

  protected function getParam()
  {
    return array();
  }

  protected function hook_form_internal()
  {
    $param = array(
      'attrs' => array('id' => 'oerp-delivery-order-combine'),
      'copy_method' => 'merge',
    );
    $Prov = new \Oerp\Delivery\Provider\Tree\Draft();
    $Table = new \Oerp\Table\Arch($Prov->getArch(), $param);

    $item = array();
    $item['tree'] = array(
      '#type' => 'oerp_tabledrag',
      '#Obj' => $Table
    );

    return $item;
  }

  protected function getToolbarBottom()
  {
    $callees = array('Combine');
    return $this->getLocalCalleeEle($callees);
  }
}