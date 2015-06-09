<?php
namespace Oerp\Delivery\Page;

class Order extends \Gulei\Page\Form\_Abstract
{
  protected function getClassname()
  {
    return __CLASS__;
  }

  protected function hook_form_internal()
  {
    $oid = $_GET['id'];
    $arch = file_get_contents(drupal_get_path('module', 'oerp_delivery') . '/xml/DeliveryOrderDetail.xml');
    $Rec = new \Oerp\Page\DeliveryOrderRecord($arch, '', array('oerp:ids' => '[' . $oid . ']'));
    return $Rec->getFormEntry();
  }

  protected function getToolbarBottom()
  {
    return array();
  }

  protected function getParam()
  {
    return array();
  }
}