<?php
namespace Oerp\Purchase\Installer\Form;

class PurchaseOrder extends \Oerp\Installer\View\Abstract_Form
{
  protected function getFilepath()
  {
    return __FILE__;
  }

  protected function getName()
  {
    return 'x_purchase.order.form';
  }

  protected function getModel()
  {
    return 'purchase.order';
  }
}