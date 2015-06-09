<?php
namespace Oerp\Purchase\Installer\Form;

class PurchaseOrderLine extends \Oerp\Installer\View\Abstract_Form
{
  protected function getFilepath()
  {
    return __FILE__;
  }

  protected function getName()
  {
    return 'x_purchase.order.line.form';
  }

  protected function getModel()
  {
    return 'purchase.order.line';
  }
}