<?php
namespace Oerp\Purchase\Operation;

class MergeDraftOpOrder implements \Gulei\Operation\_Interface
{
  private function wiz($args, $action)
  {
    return \Oerp\Query\Wizard::_execute($args, $action);
  }

  public function run()
  {
    $prov = new \Oerp\Purchase\Provider\DraftOpOrder();
    $ids = $prov->getData()->collect('id');

    $wid = $this->wiz(
      array('purchase.order.merge'),
      'create'
    );

    $this->wiz(
      array(
        $wid,
        array(
          'model' => 'purchase.order',
          'ids' => $ids,
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
          'ids' => $ids,
        ),
        'merge',
      ),
      'execute'
    );
  }
}