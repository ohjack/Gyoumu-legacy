<?php
namespace Oerp\Product\Command;

class toRealState extends \Gulei\Command\WithReference\WithTable
{
  public function execute($refVar = null)
  {
    $rec = $this->ref->val('rec');

    switch($rec['product_tmpl_id.state']){
      case 'In Development':
        return '開發中';

      case 'End of Lifecycle':
        return 'EoL';

      case 'In Production':
        if($rec['product_tmpl_id.purchase_ok'] == 'No'){
          return 'Close';
        }
        return 'Ready';
    }
    return false;
  }
}