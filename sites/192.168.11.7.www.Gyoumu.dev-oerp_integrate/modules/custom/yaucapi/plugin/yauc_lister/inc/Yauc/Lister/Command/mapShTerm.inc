<?php
namespace Yauc\Lister\Command;

class mapShTerm extends \Gulei\Command\WithReference\_Abstract
{
  public function execute($refVar = null)
  {
    list($rec, $name) = $this->ref->vals('rec', 'name');

    switch($rec[$name]){
      case 'auc_shipping_buyer':
        return 'Buyer';

      case 'auc_shipping_seller':
        return 'Seller';

      default:
        return false;
    }
  }
}