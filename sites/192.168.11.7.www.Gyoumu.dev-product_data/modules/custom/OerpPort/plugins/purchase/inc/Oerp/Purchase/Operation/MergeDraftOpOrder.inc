<?php
namespace Oerp\Purchase\Operation;

class MergeDraftOpOrder implements \Gulei\Operation\_Interface
{
  private function wiz($args, $action)
  {
    return \Oerp\Query\Wizard::_execute($args, $action);
  }

  protected function cancelOrders($ids) {
    $q = new \Oerp\Query('purchase.order');
    foreach ($ids as $id) {
      $q->exec_workflow(array('purchase_cancel', $id));
    }
  }

  public function run()
  {
    $prov = new \Oerp\Purchase\Provider\DraftOpOrder();
    $recs = $prov->getData();

    $ids_to_merge = array();
    $ids_to_cancel = array();

    foreach ($recs as $rec) {
      if (count($rec['order_line']) == 1) {
        $prod = $rec['order_line'][0]['product_id'];

        if ($prod['state'] == 'sellable' && $prod['purchase_ok']) {
          $ids_to_merge[] = $rec['id'];
        }
        else {
          $ids_to_cancel[] = $rec['id'];
        }
      }
    }

    $this->cancelOrders($ids_to_cancel);

    $wid = $this->wiz(
      array('purchase.order.merge'),
      'create'
    );

    $this->wiz(
      array(
        $wid,
        array(
          'model' => 'purchase.order',
          'ids' => $ids_to_merge,
        ),
        'init'
      ),
      'execute'
    );

    $this->wiz(
      array(
        $wid,
        array(
          'model' => 'purchase.order',
          'ids' => $ids_to_merge,
        ),
        'merge',
      ),
      'execute'
    );
  }
}