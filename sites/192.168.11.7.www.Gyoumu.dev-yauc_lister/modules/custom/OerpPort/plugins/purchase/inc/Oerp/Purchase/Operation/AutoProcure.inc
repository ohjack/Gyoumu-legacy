<?php
namespace Oerp\Purchase\Operation;

class AutoProcure implements \Gulei\Operation\_Interface {
  public function run() {
    $cri = array(array('supplier', '=', TRUE), array('active', '=', TRUE));
    $q = new \Oerp\Query('res.partner', $cri, array('name', 'address'));
    $partners = $q->raw();

    foreach ($partners as $partner) {
      if (!$partner['address']) {
        $q->setIds($partner['id']);
        $q->write(
          array(
            'address' => array(
              array(0, 0, array('name' => $partner['name']),
              ),
            ),
          )
        );
      }
    }

    $wid = \Oerp\Query\Basic::_execute(
      array('mrp.procurement.orderpoint.compute'),
      'create',
      'wizard'
    );

    $resp = \Oerp\Query\Basic::_execute(
      array(
        (int) $wid,
        array(
          'form' => array(
            'automatic' => 0,
          ),
        ),
        'compute',
      ),
      'execute',
      'wizard'
    );

    return $resp;
  }
}